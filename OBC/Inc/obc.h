/*
 * obc.h
 *
 *  Created on: Nov 9, 2022
 *      Author: NilRi
 */

#ifndef INC_OBC_H_
#define INC_OBC_H_

#include <softTim.h>
#include "main.h"
#include <stdbool.h>
#include "clock.h"
#include "periph.h"

#include "obc_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "events.h"

#include "flash.h"
#include "notifications.h"

#include "definitions.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_tim.h"
#include "stm32l4xx_hal_adc.h"
#include "stm32l4xx_hal_dac.h"
#include "stm32l4xx_hal_spi.h"
#include "stm32l4xx_hal_uart.h"
#include "stm32l4xx_hal_i2c.h"

#include "board.h"
#include "gpio-board.h"



/****PERIPHERAL VARIABLES****/
extern ADC_HandleTypeDef hadc1;
extern DAC_HandleTypeDef hdac1;
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
extern UART_HandleTypeDef huart4;

/****TASK HANDLE VARIABLES****/
extern TaskHandle_t FLASH_Handle;
extern TaskHandle_t COMMS_Handle;
extern TaskHandle_t ADCS_Handle;
extern TaskHandle_t PAYLOAD_Handle;
extern TaskHandle_t EPS_Handle;

/****FREERTOS VARIABLES****/
extern EventGroupHandle_t xEventGroup;

void OBC_Init();
void OBC_Nominal();
void OBC_Contingency();
void OBC_Sunsafe();
void OBC_Survival();

void OBC_COMMSFlags();


#endif /* INC_OBC_H_ */
