/*
 * events.h
 *
 *  Created on: Feb 15, 2023
 *      Author: NilRi
 */

#ifndef INC_EVENTS_H_
#define INC_EVENTS_H_


#define COMMS_START_EVENT						1<<0
#define PAYLOAD_START_EVENT   					1<<1
#define ADCS_START_EVENT						1<<2
#define FLASH_START_EVENT		    			1<<3

#define COMMS_DONE_EVENT						1<<4
#define PAYLOAD_DONE_EVENT   					1<<5
#define ADCS_DONE_EVENT							1<<6
#define FLASH_DONE_EVENT		    			1<<7

#endif /* INC_EVENTS_H_ */
