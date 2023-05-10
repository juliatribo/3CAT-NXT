/*
 * software_timers.h
 *
 *  Created on: Mar 26, 2023
 *      Author: NilRi
 */

#ifndef INC_SOFTWARE_TIMERS_H_
#define INC_SOFTWARE_TIMERS_H_

#include "FreeRTOS.h"
#include "event_groups.h"
#include "timers.h"

#include "events.h"

extern EventGroupHandle_t xEventGroup;

void CommsTimerCallback(TimerHandle_t xTimer);

#endif /* INC_SOFTWARE_TIMERS_H_ */
