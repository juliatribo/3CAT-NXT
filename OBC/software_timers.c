/*
 * software_timers.c
 *
 *  Created on: Mar 26, 2023
 *      Author: NilRi
 */
#include "software_timers.h"

void CommsTimerCallback(TimerHandle_t xTimer)
{
	xEventGroupSetBits(xEventGroup, COMMS_DONE_EVENT);
}
