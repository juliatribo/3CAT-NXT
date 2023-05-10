/*
 * obc_task.c
 *
 *  Created on: Apr 5, 2023
 *      Author: NilRi
 */

#include "obc_task.h"

void OBC_COMMS_RXFlags()
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
