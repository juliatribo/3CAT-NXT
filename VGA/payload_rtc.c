/*!
 * \file      configuration.c
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
 *
 * \modified on: 01/03/2022
 */

#include "payload_rtc.h"

void PAYLOAD_RTC_GetTime(RTC_HandleTypeDef *hrtc) {
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BIN);

	//Make the conversion from RTC Type to struct tm
	//January and February are counted as months 13 and 14 of the previous year
	time_t timestamp;
	struct tm currTime;
	currTime.tm_year = sDate.Year + 100;
    currTime.tm_mon = sDate.Month-1;
    currTime.tm_mday = sDate.Date;
	currTime.tm_hour = sTime.Hours-1;
    currTime.tm_min = sTime.Minutes;
    currTime.tm_sec = sTime.Seconds;

    timestamp = mktime(&currTime); // Convert from struct tm to time_t

    uint32_t time32;
    time32 = (uint32_t)timestamp; // Convert from time_t to uint32_t
    Send_to_WFQueue(&time32, sizeof(time32), RTC_TIME_ADDR, OBCsender);
}

void PAYLOAD_RTC_SetTime(RTC_HandleTypeDef *hrtc) {

	uint32_t settime;
	Read_Flash(SET_TIME_ADDR, &settime, 4);

	time_t unix = settime;
	struct tm currTime;
	localtime_r(&unix, &currTime);
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	// Converts tm struct to RTC_TimeTypeDef, RTC_DateTypeDef
	sDate.Year = currTime.tm_year - 100;
	sDate.Month = currTime.tm_mon +1;
	sDate.Date = currTime.tm_mday;
	sTime.Hours= currTime.tm_hour + 1;
	sTime.Minutes = currTime.tm_min;
	sTime.Seconds = currTime.tm_sec;

    HAL_RTC_SetTime(hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(hrtc, &sDate, RTC_FORMAT_BIN);

    Send_to_WFQueue(&settime, sizeof(settime), RTC_TIME_ADDR, OBCsender);
}

