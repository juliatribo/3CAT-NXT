/*
 * events.h
 *
 *  Created on: Feb 15, 2023
 *      Author: NilRi
 */

#ifndef INC_EVENTS_H_
#define INC_EVENTS_H_

/******COMMS EVENTS******/
#define COMMS_RXIRQFlag_EVENT 				1<<0   // IRQ flag pending on the transceiver
#define COMMS_RXNOTI_EVENT                  1<<1   // Task notifications pending on COMMS

#define COMMS_TELECOMMAND_EVENT				1<<2   // Telecommand has been processed succesfully
#define COMMS_WRONGPACKET_EVENT				1<<3   // The packet received is not ours

/******ADCS EVENTS******/
#define ADCS_DONE_EVENT               		1<<4   // Dangerous temperature gradient on lateral boards

/******EPS EVENTS******/
#define EPS_DONE_EVENT						1<<5

/******PAYLOAD EVENTS******/
#define PAYLOAD_POINTINGDONE_EVENT          1<<8
#define PAYLOAD_TIMEFORPHOTO_EVENT          1<<9

#endif /* INC_EVENTS_H_ */
