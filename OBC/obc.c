/*
 * state_machine.c
 *
 *  Created on: Nov 9, 2022
 *      Author: NilRi
 */
#include "obc.h"


TimerHandle_t xTimerObc;
TimerHandle_t xTimerComms;
TimerHandle_t xTimerAdcs;
TimerHandle_t xTimerEps;
TimerHandle_t xTimerPayload;

TimerHandle_t xTimerPhoto;

void OBC_Init()
{
	uint8_t state;
	bool deploy = false;
	// Create Software timers for limiting the time we want to spend in each task.
	// As of now, we have assigned an arbitrary value of 3s to all.
	// However, the time needs to be set according to the priority of each task.
			// The lower the priority, the lesser the time period.
			// The higher the priority, the higher the time period.
	xTimerCreate(xTimerObc, pdMS_TO_TICKS(3000), false, NULL, ObcTimerCallback);
	xTimerCreate(xTimerComms, pdMS_TO_TICKS(3000), false, NULL, CommsTimerCallback);
	xTimerCreate(xTimerAdcs, pdMS_TO_TICKS(3000), false, NULL, AdcsTimerCallback);
	xTimerCreate(xTimerEps, pdMS_TO_TICKS(3000), false, NULL, EpsTimerCallback);
	xTimerCreate(xTimerPayload, pdMS_TO_TICKS(3000), false, NULL, PayloadTimerCallback);
	xTimerCreate(xTimerPhoto, pdMS_TO_TICKS(3000), false, NULL, PhotoTimerCallback);

	xTaskNotify(ADCS_Handle, DETUMBLING_NOTI, eSetBits);

	xTaskNotify(EPS_Handle, EPS_BATTERY_NOTI, eSetBits); // Read battery in order to know if we have enough capacity for antenna deployment

	Read_Flash(CURRENT_STATE_ADDR, &state, 1);

	if(state == _NOMINAL) // Deploy antennas only if the next state is NOMINAL
	{
		xTaskNotify(COMMS_Handle, ANTENNA_DEPLOYMENT_NOTI, eSetBits);
		xTaskNotify(PAYLOAD_Handle, ANTENNA_DEPLOYMENT_NOTI, eSetBits); // If RFI
		deploy = true;
	}

	Send_to_WFQueue(&deploy, 1, ANTENNA_DEPLOYED_ADDR, OBCsender);

	xTaskNotify(ADCS_Handle, CHECKROTATE_NOTI, eSetBits);

	xTaskNotify(EPS_Handle, EPS_BATTERY_NOTI, eSetBits);
}

void OBC_Nominal()
{
	uint32_t CPUFreq;
	uint8_t state, prev_state;
	bool deploy;

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
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

		xTaskResumeAll(); // Resume the Scheduler

		// The function pdMS_TO_TICKS return ticks according to the CPU frequency.
		// We need to update the timer period each time we change the state
		xTimerChangePeriod(xTimerObc,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerAdcs,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerComms,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerEps,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerPayload,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerPhoto,pdMS_TO_TICKS(3000),0);
	}

	Read_Flash(ANTENNA_DEPLOYED_ADDR, &deploy, 1);

	if(!deploy)
	{
		xTaskNotify(COMMS_Handle, ANTENNA_DEPLOYMENT_NOTI, eSetBits);
		xTaskNotify(PAYLOAD_Handle, ANTENNA_DEPLOYMENT_NOTI, eSetBits); // If RFI
		deploy = true;
		Send_to_WFQueue(&deploy, 1, ANTENNA_DEPLOYED_ADDR, OBCsender);
	}

	EventBits_t EventBits;
	bool telecommand_process = false;

	for(;;)
	{
		if(!telecommand_process)
		{
			EventBits = xEventGroupWaitBits(xEventGroup, COMMS_RXIRQFlag_EVENT | COMMS_RXNOTI_EVENT, 0, false, pdMS_TO_TICKS(500));
		}

		if( ((EventBits & COMMS_RXIRQFlag_EVENT)==COMMS_RXIRQFlag_EVENT) || ((EventBits & COMMS_RXNOTI_EVENT)==COMMS_RXNOTI_EVENT))
		{

			EventBits = xEventGroupWaitBits(xEventGroup, COMMS_TELECOMMAND_EVENT | COMMS_WRONGPACKET_EVENT, 0, false, pdMS_TO_TICKS(3000));

			if((EventBits & COMMS_TELECOMMAND_EVENT)==COMMS_TELECOMMAND_EVENT)
			{
				OBC_COMMS_RXFlags();
				xEventGroupClearBits(xEventGroup, COMMS_RXIRQFlag_EVENT | COMMS_TELECOMMAND_EVENT | COMMS_WRONGPACKET_EVENT);
				telecommand_process = false;
			}

			else if (((EventBits & COMMS_TELECOMMAND_EVENT)!=COMMS_TELECOMMAND_EVENT) && ((EventBits & COMMS_WRONGPACKET_EVENT)!=COMMS_WRONGPACKET_EVENT))
			{
				telecommand_process = true;
			}
		}

		xTaskNotify(ADCS_Handle, CHECKROTATE_NOTI, eSetBits);
		xTaskNotify(EPS_Handle, EPS_HEATER_NOTI, eSetBits);
		xTaskNotify(EPS_Handle, EPS_BATTERY_NOTI, eSetBits);

		// System Reset in a Software timer
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

		xTimerChangePeriod(xTimerObc,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerAdcs,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerPayload,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerComms,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerEps,pdMS_TO_TICKS(3000),0);
	}

	Read_Flash(PREVIOUS_STATE_ADDR,&previousState,1);

	 if (previousState ==_SUNSAFE)
	 {
		 vTaskResume(ADCS_Handle   ); // Resume ADCS task
		 vTaskResume(PAYLOAD_Handle); // Resume PAYLOAD task
	 }

	xTaskNotify(COMMS_Handle, CONTINGENCY_NOTI, eSetBits); // Notify COMMS that we have entered the CONTINGENCY state
	xTaskNotify(ADCS_Handle,  CONTINGENCY_NOTI, eSetBits); // Notify ADCS that we have entered the CONTINGENCY state

	EventBits_t EventBits;
	bool telecommand_process = false;

	for(;;)
	{
		if(!telecommand_process)
		{
			EventBits = xEventGroupWaitBits(xEventGroup, COMMS_RXIRQFlag_EVENT | COMMS_RXNOTI_EVENT, 0, false, pdMS_TO_TICKS(500));
		}

		if( ((EventBits & COMMS_RXIRQFlag_EVENT)==COMMS_RXIRQFlag_EVENT) || ((EventBits & COMMS_RXNOTI_EVENT)==COMMS_RXNOTI_EVENT))
		{

			EventBits = xEventGroupWaitBits(xEventGroup, COMMS_TELECOMMAND_EVENT | COMMS_WRONGPACKET_EVENT, 0, false, pdMS_TO_TICKS(3000));

			if((EventBits & COMMS_TELECOMMAND_EVENT)==COMMS_TELECOMMAND_EVENT)
			{
				OBC_COMMS_RXFlags();
				xEventGroupClearBits(xEventGroup, COMMS_RXIRQFlag_EVENT | COMMS_TELECOMMAND_EVENT | COMMS_WRONGPACKET_EVENT);
				telecommand_process = false;
			}

			else if (((EventBits & COMMS_TELECOMMAND_EVENT)!=COMMS_TELECOMMAND_EVENT) && ((EventBits & COMMS_WRONGPACKET_EVENT)!=COMMS_WRONGPACKET_EVENT))
			{
				telecommand_process = true;
			}
		}

		xTaskNotify(EPS_Handle, EPS_HEATER_NOTI, eSetBits);
		xTaskNotify(EPS_Handle, EPS_BATTERY_NOTI, eSetBits);
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

		xTimerChangePeriod(xTimerObc,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerAdcs,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerPayload,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerComms,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerEps,pdMS_TO_TICKS(3000),0);
	}

	Read_Flash(PREVIOUS_STATE_ADDR,&previousState,1);

	xTaskNotify(COMMS_Handle, SUNSAFE_NOTI, eSetBits);
	xTaskNotify(ADCS_Handle, SUNSAFE_NOTI, eSetBits);

	switch(previousState)
	{
		case _CONTINGENCY:
			vTaskSuspend(ADCS_Handle);    // Suspend the ADCS task
			vTaskSuspend(PAYLOAD_Handle); // Suspend the PAYLOAD task
			break;
		default:
			break;
	}

	EventBits_t EventBits;
	bool telecommand_process = false;

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

		if(!telecommand_process)
		{
			EventBits = xEventGroupWaitBits(xEventGroup, COMMS_RXIRQFlag_EVENT | COMMS_RXNOTI_EVENT, 0, false, pdMS_TO_TICKS(500));
		}

		if( ((EventBits & COMMS_RXIRQFlag_EVENT)==COMMS_RXIRQFlag_EVENT) || ((EventBits & COMMS_RXNOTI_EVENT)==COMMS_RXNOTI_EVENT))
		{

			EventBits = xEventGroupWaitBits(xEventGroup, COMMS_TELECOMMAND_EVENT | COMMS_WRONGPACKET_EVENT, 0, false, pdMS_TO_TICKS(3000));

			if((EventBits & COMMS_TELECOMMAND_EVENT)==COMMS_TELECOMMAND_EVENT)
			{
				OBC_COMMS_RXFlags();
				xEventGroupClearBits(xEventGroup, COMMS_RXIRQFlag_EVENT | COMMS_TELECOMMAND_EVENT | COMMS_WRONGPACKET_EVENT);
				telecommand_process = false;
			}

			else if (((EventBits & COMMS_TELECOMMAND_EVENT)!=COMMS_TELECOMMAND_EVENT) && ((EventBits & COMMS_WRONGPACKET_EVENT)!=COMMS_WRONGPACKET_EVENT))
			{
				telecommand_process = true;
			}
		}

		xTaskNotify(EPS_Handle, EPS_HEATER_NOTI, eSetBits);
		xTaskNotify(EPS_Handle, EPS_BATTERY_NOTI, eSetBits);
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

		xTimerChangePeriod(xTimerObc,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerAdcs,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerPayload,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerComms,pdMS_TO_TICKS(3000),0);
		xTimerChangePeriod(xTimerEps,pdMS_TO_TICKS(3000),0);
	}

	HAL_PWREx_EnableLowPowerRunMode(); // Enter into LOW POWER RUN MODE

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

		HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI); // Enter in SLEEP MODE with the WFI (Wait For Interrupt) instruction

		/* SLEEPING ~ 10s */

		HAL_TIM_Base_Stop_IT(&htim7);
		SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
		HAL_ResumeTick();

		xTaskNotify(EPS_Handle, EPS_HEATER_NOTI, eSetBits);
		xTaskNotify(EPS_Handle, EPS_BATTERY_NOTI, eSetBits);
	}

	HAL_PWREx_DisableLowPowerRunMode();
}




