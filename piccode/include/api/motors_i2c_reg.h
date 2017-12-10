#ifndef MOTORS_SYSTEM_REGISTERS_H
#define	MOTORS_SYSTEM_REGISTERS_H

#include "motors_i2c_model.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
/*
 *  General system registers
 */
// (R/W)
#define MOTORSREG_STATUS                    0
#define MOTORSREGBIT_STATUS_RUNNING         0x10
#define MOTORSREGBIT_STATUS_RUN_REQUEST     0x01
#define MOTORSREGBIT_STATUS_PAUSE_REQUEST   0x02
    
#define MOTORSREG_SYSTEM_END            (MOTORSREG_STATUS+sizeof(int8_t))

/*
 *  PID pitch registers
 */
#define MOTORSREG_PITCH_PID_BEGIN       MOTORSREG_SYSTEM_END			//1
    
//PIDFifo (R)
#define MOTORSREG_PITCH_FIFO_CURRENT    (MOTORSREG_PITCH_PID_BEGIN)		//1
#define MOTORSREG_PITCH_FIFO_SIZE       (MOTORSREG_PITCH_FIFO_CURRENT+sizeof(PIDStateEntry))	//15
    
//PIDSettings (R/W)
#define MOTORSREG_PITCH_PID	         	(MOTORSREG_PITCH_FIFO_SIZE + sizeof(int8_t))	//16
#define MOTORSREG_PITCH_PID_KP          MOTORSREG_PITCH_PID								//16
#define MOTORSREG_PITCH_PID_KD          (MOTORSREG_PITCH_PID_KP + sizeof(int8_t))		//17
#define MOTORSREG_PITCH_PID_KI          (MOTORSREG_PITCH_PID_KD + sizeof(int8_t))  		//18
  
#define MOTORSREG_PITCH_PID_END         (MOTORSREG_PITCH_PID_KI+sizeof(int8_t))			//19
    
/*
 *  PID speed registers
 */
#define MOTORSREG_SPEED_PID_BEGIN       MOTORSREG_PITCH_PID_END			//1
    
//PIDFifo (R)
#define MOTORSREG_SPEED_FIFO_CURRENT    (MOTORSREG_SPEED_PID_BEGIN)		//1
#define MOTORSREG_SPEED_FIFO_SIZE       (MOTORSREG_SPEED_FIFO_CURRENT+sizeof(PIDStateEntry))	//15
    
//PIDSettings (R/W)
#define MOTORSREG_SPEED_PID	         	(MOTORSREG_SPEED_FIFO_SIZE + sizeof(int8_t))	//16
#define MOTORSREG_SPEED_PID_KP          MOTORSREG_SPEED_PID								//16
#define MOTORSREG_SPEED_PID_KD          (MOTORSREG_SPEED_PID_KP + sizeof(int8_t))		//17
#define MOTORSREG_SPEED_PID_KI          (MOTORSREG_SPEED_PID_KD + sizeof(int8_t))  		//18
  
#define MOTORSREG_SPEED_PID_END         (MOTORSREG_SPEED_PID_KI+sizeof(int8_t))			//19

    
   

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_REGISTERS_H */

