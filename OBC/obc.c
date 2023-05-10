/*
 * state_machine.c
 *
 *  Created on: Nov 9, 2022
 *      Author: NilRi
 */
#include "obc.h"

TimerHandle_t COMMSTimer;

void OBC_Init()
{
	uint8_t currentState, previousState;

	previousState = _INIT;
	Send_to_WFQueue( &previousState, sizeof(previousState), PREVIOUS_STATE_ADDR, OBCsender);
	currentState = _NOMINAL;
	Send_to_WFQueue(&currentState, sizeof(currentState), CURRENT_STATE_ADDR, OBCsender);

}

void OBC_Nominal()
{
	uint32_t CPUFreq;
	uint8_t currentState, previousState;


	CPUFreq = HAL_RCC_GetHCLKFreq();
	if(CPUFreq!=26000000)
	{
		vTaskSuspendAll(); // Suspend the Scheduler

		__disable_irq();        // Disable all interrupts: Peripherals, Systick...
		Periph_Disable_Clock(); // Disable the Peripherals Clocks
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // Stop the Systick timer

		Clock_26MHz(); // Clock configuration whose CPU frequency is 26 MHz

		CPUFreq = HAL_RCC_GetHCLKFreq(); // Update the SystemCoreClock
		SysTick_Config(SystemCoreClock / 1000); // Initialize the Systick and restart
		Periph_Enable_Clock();  // Enable the Peripherals Clocks
		__enable_irq();         // Enable all interrupts: Peripherals, Systick...
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); // FOR TESTING PURPOSES

		xTaskResumeAll(); // Resume the Scheduler
		COMMSTimer = xTimerCreate("COMMSTimer", pdMS_TO_TICKS(5000), pdFALSE, NULL, CommsTimerCallback);
	}

	for(;;)
	{
		OBC_Flags();
		xTimerStart(COMMSTimer,0);
		xEventGroupWaitBits(xEventGroup, COMMS_DONE_EVENT, COMMS_DONE_EVENT, true, portMAX_DELAY);
	}
}

void OBC_Contingency()
{
	uint32_t CPUFreq;
	uint8_t currentState, previousState;

	CPUFreq = HAL_RCC_GetHCLKFreq();
	if(CPUFreq != 8000000)
	{
		vTaskSuspendAll(); // Suspend the Scheduler

		__disable_irq();        // Disable all interrupts: Peripherals, Systick...
		Periph_Disable_Clock(); // Disable the Peripherals Clocks
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // Stop the Systick timer

		Clock_8MHz(); // Clock configuration whose CPU frequency is 8 MHz

		CPUFreq = HAL_RCC_GetHCLKFreq();        // Update the System Core Clock
		SysTick_Config(SystemCoreClock / 1000); // Initialize the Systick and restart
		Periph_Enable_Clock();  // Enable the Peripherals Clocks
		__enable_irq();         // Enable all interrupts: Peripherals, Systick...

		xTaskResumeAll(); // Resume the Scheduler
	}

	Read_Flash(PREVIOUS_STATE_ADDR,&previousState,1);

	 if (previousState ==_SUNSAFE)
	 {
		 vTaskResume(ADCS_Handle   ); // Resume ADCS task
		 vTaskResume(PAYLOAD_Handle); // Resume PAYLOAD task
	 }

	xTaskNotify(COMMS_Handle, CONTINGENCY_NOTI, eSetBits); // Notify COMMS that we have entered the CONTINGENCY state
	xTaskNotify(ADCS_Handle,  CONTINGENCY_NOTI, eSetBits); // Notify ADCS that we have entered the CONTINGENCY state

	for(;;)
	{
		OBC_Flags();
	}
}

void OBC_Sunsafe()
{
	uint32_t CPUFreq;
	uint8_t currentState, previousState;

	if(CPUFreq != 8000000)
	{
		vTaskSuspendAll(); // Suspend the Scheduler

		__disable_irq();        // Disable all interrupts: Peripherals, Systick...
		Periph_Disable_Clock(); // Disable the Peripherals Clocks
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // Stop the Systick timer

		Clock_8MHz(); // Clock configuration whose CPU frequency is 8 MHz

		CPUFreq = HAL_RCC_GetHCLKFreq();        // Update the System Core Clock
		SysTick_Config(SystemCoreClock / 1000); // Initialize the Systick and restart
		Periph_Enable_Clock();  // Enable the Peripherals Clocks
		__enable_irq();         // Enable all interrupts: Peripherals, Systick...

		xTaskResumeAll(); // Resume the Scheduler
	}

	Read_Flash(PREVIOUS_STATE_ADDR,&previousState,1);

	switch(previousState)
	{
		case _CONTINGENCY:
			xTaskNotify(COMMS_Handle, SUNSAFE_NOTI, eSetBits); // Notify COMMS that we have entered the SUNSAFE state
			xTaskNotify(ADCS_Handle, SUNSAFE_NOTI, eSetBits); // Notify ADCS that we have entered the SUNSAFE state
			vTaskSuspend(ADCS_Handle);    // Suspend the ADCS task
			vTaskSuspend(PAYLOAD_Handle); // Suspend the PAYLOAD task
			break;

		case _SURVIVAL:
			vTaskResume(COMMS_Handle);
			xTaskNotify(COMMS_Handle, SUNSAFE_NOTI, eSetBits); // Notify COMMS that we have entered the SUNSAFE state
			break;
		default:
			break;
	}

	/****TIMER 7 CONFIGURATION FOR A 10s SLEEP*****/
	TIM7->SR&=~(1<<0); // Clear update interrupt flag
	TIM7->PSC = 20000; // 16-bit Prescaler : TIM2 frequency = CPU_F/PSC = 2.000.000/20.000 = 100 Hz.
	TIM7->ARR = 1000;  // 16-bit Auto Reload Register : ARR = Sleep_time * TIM7_F = 10*100=1000.

	for(;;)
	{
		TIM7->SR&=~(1<<0); // Clear update interrupt flag
		HAL_TIM_Base_Start_IT(&htim7);

		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; // Suspend the scheduler (systick timer) interrupt to not wake up the MCU
		HAL_SuspendTick();                          // Suspend the tim6 interrupts to not wake up the MCU

		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); // Enter in SLEEP MODE with the WFI (Wait For Interrupt) instruction

		/* SLEEPING ~ 10s */

		HAL_TIM_Base_Stop_IT(&htim7);

		SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk; // Enable the scheduler (systick timer) interrupt
		HAL_ResumeTick();                           // Enable the tim7 interrupt

		OBC_Flags();
	}
}

void OBC_Survival()
{
	uint32_t CPUFreq;
	uint8_t currentState, previousState;

	CPUFreq = HAL_RCC_GetHCLKFreq();

	if(CPUFreq != 2000000)
	{
		vTaskSuspendAll(); // Suspend the Scheduler

		__disable_irq();        // Disable all interrupts: Peripherals, Systick...
		Periph_Disable_Clock(); // Disable the Peripherals Clocks
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // Stop the Systick timer

		Clock_2MHz(); // Clock configuration whose CPU frequency is 2 MHz

		CPUFreq = HAL_RCC_GetHCLKFreq();        // Update the System Core Clock
		SysTick_Config(SystemCoreClock / 1000); // Initialize the Systick and restart
		Periph_Enable_Clock();  // Enable the Peripherals Clocks
		__enable_irq();         // Enable all interrupts: Peripherals, Systick...

		xTaskResumeAll(); // Resume the Scheduler
	}

	Read_Flash(PREVIOUS_STATE_ADDR,&previousState,1);

	if(previousState == _SUNSAFE)
		{
			xTaskNotify(COMMS_Handle, SURVIVAL_NOTI, eSetBits); // Notify COMMS that we have entered the SURVIVAL state
			vTaskSuspend(COMMS_Handle); // Suspend the COMMS task
		 }

	HAL_PWREx_EnableLowPowerRunMode(); // Enter into LOW POWER RUN MODE

	/****TIMER 7 CONFIGURATION FOR A 10s SLEEP*****/
	TIM7->SR&=~(1<<0); // Clear update interrupt flag
	TIM7->PSC = 20000; // 16-bit Prescaler : TIM2 frequency = CPU_F/PSC = 2.000.000/20.000 = 100 Hz.
	TIM7->ARR = 1000;  // 16-bit Auto Reload Register : ARR = Sleep_time * TIM7_F = 10*100=1000.

	bool battery = true;
	for(;;)
	{
		TIM7->SR&=~(1<<0); // Clear update interrupt flag
		HAL_TIM_Base_Start_IT(&htim7);

		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; // Suspend the scheduler (systick timer) interrupt to not wake up the MCU
		HAL_SuspendTick();                          // Suspend the tim6 interrupts to not wake up the MCU

		HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI); // Enter in SLEEP MODE with the WFI (Wait For Interrupt) instruction

		/* SLEEPING ~ 10s */

		HAL_TIM_Base_Stop_IT(&htim7);
		SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
		HAL_ResumeTick();

		OBC_Flags();
	}

	HAL_PWREx_DisableLowPowerRunMode();
}

void OBC_Flags()
{
	uint8_t currentState, previousState, nominal, low, critical;
	uint32_t RX_OBC_NOTIS;

	 if (xTaskNotifyWait(0, 0xFFFFFFFF, &RX_OBC_NOTIS, 0)==pdPASS)
		 {
			if((RX_OBC_NOTIS & EXIT_CONTINGENCY_NOTI)==EXIT_CONTINGENCY_NOTI)
				{
					currentState = _NOMINAL;
					Send_to_WFQueue(&currentState,sizeof(currentState),CURRENT_STATE_ADDR,OBCsender);
				}

			if((RX_OBC_NOTIS & EXIT_SUNSAFE_NOTI)==EXIT_SUNSAFE_NOTI)
				{
					currentState = _CONTINGENCY;
					Send_to_WFQueue(&currentState,sizeof(currentState),CURRENT_STATE_ADDR,OBCsender);
				}

			if((RX_OBC_NOTIS & NOMINAL_NOTI)==NOMINAL_NOTI)
				{
					//Read_Flash(NOMINAL_ADDR, &nominal, sizeof(nominal));
					nominal = 19;
					Send_to_WFQueue(&nominal,sizeof(nominal),NOMINAL_ADDR,OBCsender);
				}

			if((RX_OBC_NOTIS & LOW_NOTI)==LOW_NOTI)
				{
					//Read_Flash(LOW_ADDR, &low, sizeof(low));
					low = 10;
					Send_to_WFQueue(&low,sizeof(low),LOW_ADDR,OBCsender);
				}

			if((RX_OBC_NOTIS & CRITICAL_NOTI)==CRITICAL_NOTI)
				{
					//Read_Flash(CRITICAL_ADDR, &critical, sizeof(critical));
					critical = 5;
					Send_to_WFQueue(&critical,sizeof(critical),LOW_ADDR,OBCsender);
				}

			if((RX_OBC_NOTIS & SET_TIME_NOTI)==SET_TIME_NOTI)
				{

				}
		 }

}



