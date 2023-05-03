/*!
 * \file      configuration.h
 *
 * \brief     It contains the initializing functions and all the functions that check
 * 			  the satellite state
 *
 *
 * \created on: 01/10/2021
 *
 * \author    Pol Simon
 *
 * \author    David Reiss
 */

#ifndef INC_PAYLOAD_RTC_H_
#define INC_PAYLOAD_RTC_H_

//#include "stm32l4xx_hal_rtc.h"
#include "definitions.h"
#include "flash.h"
#include <time.h>
#include <string.h>

void OBC_RTC_GetTime(RTC_HandleTypeDef *hrtc);
void OBC_RTC_SetTime(RTC_HandleTypeDef *hrtc);


#endif /* INC_PAYLOAD_RTC_H_ */
