/*
 * utils.h
 *
 *  Created on: 23 nov. 2022
 *      Author: NilRi
 */

#ifndef INC_NOTIFICATIONS_H_
#define INC_NOTIFICATIONS_H_

/****************************
 * GENERAL NOTIFICATIONS	*
 ****************************/


/****************************
 * GENERAL NOTIFICATIONS	*
 ****************************/
// Notification: Contingency state
// To: COMMS, ADCS
// From: OBC
#define CONTINGENCY_NOTI	1<<0
// Notification: SUNSAFE state = KILL THE THREAD
// To: COMMS, ADCS, PAYLOAD
// From:OBC
#define SUNSAFE_NOTI		1<<25
// Notification: Wake up a task that is suspended (sleep)
// To: COMMS, ADCS, PAYLOAD
// From:OBC
#define WAKEUP_NOTI			1<<1
// Notification: Reset the system
// To: OBC
// From:COMMS
#define RESET_NOTI			1<<22
// Notification: We are in the contact region of the GS
// To: COMMS
// From:ADCS
#define GS_NOTI				1<<2
// Notification: We are NOT in the contact region of the GS
// To: COMMS
// From:ADCS
#define NOTGS_NOTI			1<<3


/************************
 * PAYLOAD NOTIFICATIONS	*
 ************************/
// Notification: Take photo
// To: PAYLOAD_CAMERA
// From: COMMS
#define TAKEPHOTO_NOTI		1<<4
// Notification: Take RF
// To: PAYLOAD_RF
// From: COMMS
#define TAKERF_NOTI			1<<5
// Notification: Photo taken
// To: COMMS
// From: PAYLOAD_CAMERA
#define DONEPHOTO_NOTI		1<<6
// Notification: RF taken
// To: COMMS
// From: PAYLOAD_RF
#define DONERF_NOTI			1<<7
// Notification: The Payload camera is not working
// To: OBC -> COMMS
// From: PAYLOAD_CAMERA
#define PAYLOAD_ERROR_NOTI  1<<21


/************************
 * OBC NOTIFICATIONS	*
 ************************/
// Notification: Exit CONTINGENCY State
// To: OBC
// From: COMMS
#define EXIT_CONTINGENCY_NOTI	1<<8

// Notification: Exit SUNSAFE state
// To: OBC
// From: COMMS
#define EXIT_SUNSAFE_NOTI       1<<24

// Notification: Set time from GS
// To: OBC
// From: COMMS
#define SET_TIME_NOTI 		1<<9

// Notification: New NOMINAL level
// To: OBC
// From: COMMS
#define NOMINAL_NOTI 		1<<10

// Notification: New LOW level
// To: OBC
// From: COMMS
#define LOW_NOTI			1<<11

// Notification: New CRITICAL level
// To: OBC
// From: COMMS
#define CRITICAL_NOTI 		1<<12



/************************
 * ADCS NOTIFICATIONS	*
 ************************/
// Notification: CTEKP available
// To: ADCS
// From: COMMS -> OBC
#define CTEKP_NOTI			1<<13

// Notification: TLE available
// To: ADCS
// From: COMMS-> OBC
#define TLE_NOTI			1<<14

// Notification: GYRO RESOLUTION available
// To: ADCS
// From: COMMS-> OBC
#define GYRORES_NOTI		1<<15

// Notification: CALIBRATION available
// To: ADCS
// From: COMMS-> OBC
#define CALIBRATION_NOTI	1<<16

// Notification: Start pointing to the Earth
// To: ADCS
// From: COMMS-> OBC
#define POINTING_NOTI		1<<17

// Notification: Detumble the satellite
// To: ADCS
// From: COMMS-> OBC
#define DETUMBLING_NOTI		1<<18

// Notification: Rotate the satellite
// To: ADCS
// From: COMMS-> OBC
#define ROTATE_NOTI			1<<19

// Notification: Pointing done
// To: PL
// From: ADCS
#define POINTING_DONE_NOTI		1<<20

// Notification: Stop pointing
// To: ADCS
// From: OBC
#define STOP_POINTING_NOTI		1<<23

// To: EPS
// From: OBC
#define READ_BATTERY_NOTI       1<<26

// To: COMMS
// From: OBC
#define SURVIVAL_NOTI       1<<27

#endif /* INC_NOTIFICATIONS_H_ */
