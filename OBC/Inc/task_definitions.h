/*
 * taskDefinitions.h
 *
 *  Created on: 20 feb. 2023
 *      Author: NilRi
 */

#ifndef INC_TASK_DEFINITIONS_H_
#define INC_TASK_DEFINITIONS_H_

/********TASK STACK SIZES********/
#define sTIM_STACK_SIZE         1000
//      DAEMON_STACK_SIZE       500    Defined at CubeMX
#define FLASH_STACK_SIZE		8500
#define OBC_STACK_SIZE			2000
#define COMMS_STACK_SIZE		3000
#define EPS_STACK_SIZE		    1000
#define PAYLOAD_STACK_SIZE		1000
#define ADCS_STACK_SIZE			1000


/********TASK PRIORITIES********/
#define sTIM_PRIORITY           8
//      DAEMON_PRIORITY         7      Defined at CubeMX
#define FLASH_PRIORITY			6
#define PAYLOAD_PRIORITY		5
#define ADCS_PRIORITY			4
#define EPS_PRIORITY			3
#define OBC_PRIORITY			2
#define COMMS_PRIORITY			1




#endif /* INC_TASK_DEFINITIONS_H_ */
