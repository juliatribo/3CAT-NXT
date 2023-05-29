/*
 * flash2.h
 *
 *  Created on: 17 ene. 2023
 *      Author: NilRi
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include <stdint.h>
#include <stdbool.h>

#include "definitions.h"
#include "events.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"

#define PHOTO_ADDR 					0x08040000

//#define PAYLOAD_STATE_ADDR 		0x08008000
//#define COMMS_STATE_ADDR 			0x08008001
//#define DETUMBLE_STATE_ADDR 		0x08008004
#define CURRENT_STATE_ADDR			0x08030000
#define PREVIOUS_STATE_ADDR			0x08030001
#define DEPLOYMENT_STATE_ADDR 		0x08030002
#define DEPLOYMENTRF_STATE_ADDR 	0x08030003
#define NOMINAL_ADDR 				0x08030004
#define LOW_ADDR 					0x08030005
#define CRITICAL_ADDR 				0x08030006
#define EXIT_LOW_ADDR 				0x08030007
#define SET_TIME_ADDR				0x08030008 	//4 bytes (GS -> RTC)
#define RTC_TIME_ADDR				0x0803000C	//4 bytes (RTC -> Unix) Get from the RTC

//CONFIGURATION ADDRESSES
#define CONFIG_ADDR 				0x08030010
#define KP_ADDR 					0x08030010
#define GYRO_RES_ADDR 				0x08030011
#define SF_ADDR 					0x08030012
#define CRC_ADDR 					0x08030013
#define PHOTO_RESOL_ADDR 			0x08030014
#define PHOTO_COMPRESSION_ADDR 		0x08030015
#define F_MIN_ADDR 					0x08030016
#define F_MAX_ADDR 					0x08030018
#define DELTA_F_ADDR 				0x0803001A
#define INTEGRATION_TIME_ADDR 		0x0803001C

#define TLE_ADDR 					0x08030020 // 138 bytes
//#define EXIT_LOW_POWER_FLAG_ADDR 	0x080080AA

//CALIBRATION ADDRESSES
#define CALIBRATION_ADDR			0x080300AB
#define MAGNETO_MATRIX_ADDR			0x080300AB // 35 bytes
#define MAGNETO_OFFSET_ADDR			0x080300CF // 11 bytes
#define GYRO_POLYN_ADDR 			0x080300DB // 23 bytes
#define PHOTODIODES_OFFSET_ADDR 	0x080300F3 // 12 bytes

//TELEMETRY ADDRESSES
#define TELEMETRY_ADDR				0x08030100 // 34 bytes in total
#define TEMPLAT_ADDR 				0x08030100		// 6
#define BATT_TEMP_ADDR 			    0x08030106		// 1
#define MCU_TEMP_ADDR 			    0x08030107		// 1
#define BATT_CAP_ADDR 			    0x08030108		// 3
//      CURRENT_STATE_ADDR                             1
#define GYRO_ADDR 			        0x08030109		// 6
#define MAGNETOMETER_ADDR 			0x0803010F      // 8
#define PHOTODIODES_ADDR 			0x08030117      // 8


//TIME ADDR
#define PL_TIME_ADDR 				0x08030111 	//4 bytes (GS -> PAYLOAD CAMERA)
#define PL_RF_TIME_ADDR				0x0803011C	//8 bytes (GS -> PAYLOAD RF)

//COMMS CONFIGURATION ADDRESSES
//TODO: How many bytes are needed for each one ?
#define COUNT_PACKET_ADDR 			0x08030200
#define COUNT_WINDOW_ADDR 			0x08030201
#define COUNT_RTX_ADDR 				0x08030202

#define TIMEOUT_ADDR 				0x08030204 // 2 bytes

#define ANTENNA_DEPLOYED_ADDR       0x08030205


/**********OTHER ADDR****************************/
#define DATA_ADDR					0x08030000
#define TLE_ADDR1 					0x08038020 // 138 bytes
#define TLE_ADDR2 					0x08038065 // 138 bytes
#define TIMEOUT_ADDR				0x0803E860 // Time between packets
#define PHOTOTIME_ADDR					0x08038008 	//4 bytes (GS -> RTC)
/************************************************/

extern EventGroupHandle_t xEventGroup;
extern QueueHandle_t FLASH_Queue;
extern SemaphoreHandle_t xMutex;

void Write_Flash(uint32_t data_addr, uint8_t *data,uint16_t n_bytes);
void Read_Flash(uint32_t data_addr, uint8_t *RxBuf, uint16_t n_bytes);
void Send_to_WFQueue(uint8_t* pointer,uint32_t arrayLength,uint32_t addr,DataSource_t DataSource);


#endif /* INC_FLASH_H_ */
