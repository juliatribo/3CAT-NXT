/*
 * telecommands.h
 *
 *  Created on: 16 feb. 2023
 *      Author: NilRi
 */

#ifndef INC_TELECOMMANDS_H_
#define INC_TELECOMMANDS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stm32l4xx_hal.h>

/******TELECOMMANDS*****/

/*OBC*/
#define RESET2  				01	/*The PQ might take a reset*/
#define NOMINAL					02
#define LOW						03
#define CRITICAL				04
#define EXIT_LOW_POWER			05
#define EXIT_CONTINGENCY		06
#define EXIT_SUNSAFE			07
#define SET_TIME				0x08

//#define NACKDATA  			08	/*If it is received if the GS do not receive all the segments of the data.
 	 	 	 	 	 	 	 	 //*The PQ will send since the last segment received correctly.*/
/*ADCS*/
#define SET_CONSTANT_KP			10
#define TLE  					11 /*Packet from GS with the new TLE, update it inside memory
 	 	 	 	 	 	 	  	  	  the SPG4 uses it to propagate the orbit*/
#define SET_GYRO_RES			12


/*COMMS*/
#define SEND_DATA  				20	/*If the acquired photo or spectogram is needed to be send to GS*/
#define SEND_TELEMETRY 			21
#define STOP_SENDING_DATA  		22
#define ACK_DATA  				23	/*It is received when all the data is received correctly*/
#define SET_SF_CR				24


#define ASK_DATA 				60	//This command is sent from GS asking the satellite to ask other satellites for data
#define ASK_TELEMETRY			61	//This command is sent from GS asking the satellite to ask other satellites for telemetry
#define ASK_STOP_SENDING_DATA  	62	//This command is sent from GS asking the satellite to ask the other satellite to stop sending data
#define ASK_ACK_DATA  			63	//This command is sent from GS asking the satellite to inform the other satellite everything is received correctly
#define ASK_SET_SF_CR			64	//This command is sent from GS asking the satellite to ask other satellites to set sf cr

//#define SET_CRC				25
#define SEND_CALIBRATION		25
#define CHANGE_TIMEOUT			26

/*CAMARA*/
#define TAKE_PHOTO 				30	/*Might rotate the PQ into the right position +
								wait until it is in the position where the picture is wanted to be taken.*/
//#define SET_PHOTO_RESOL	31	//Photo Resolution
//#define PHOTO_COMPRESSION 32

/*PAYLOAD 2: ELECTROSMOG ANTENNA*/
#define TAKE_RF  				40
//#define F_MIN					41
//#define F_MAX					42
//#define DELTA_F				43
//#define INTEGRATION_TIME		44


#define SEND_CONFIG				50	//Send all configuration

#define ACK						60
#define ERROR					65
#define BEACON					68



#endif /* INC_TELECOMMANDS_H_ */
