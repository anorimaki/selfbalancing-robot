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
#define MOTORSREG_PITCH_FIFO_SIZE       (MOTORSREG_PITCH_FIFO_CURRENT+sizeof(PIDStateEntry))//15
    
//PIDSettings (R/W)
#define MOTORSREG_PITCH_PID	         	(MOTORSREG_PITCH_FIFO_SIZE + sizeof(int8_t))		//16
#define MOTORSREG_PITCH_PID_KP          MOTORSREG_PITCH_PID									//16
#define MOTORSREG_PITCH_PID_KD          (MOTORSREG_PITCH_PID_KP + sizeof(PIDConstant))		//18
#define MOTORSREG_PITCH_PID_KI          (MOTORSREG_PITCH_PID_KD + sizeof(PIDConstant))  	//20
    
//PID target (R/W)
#define MOTORSREG_PITCH_PID_TARGET      (MOTORSREG_PITCH_PID_KI + sizeof(PIDConstant))      //22
    
#define MOTORSREG_PITCH_PID_END         (MOTORSREG_PITCH_PID_TARGET + sizeof(int16_t))		//24
    
/*
 *  PID speed registers
 */
#define MOTORSREG_SPEED_PID_BEGIN       MOTORSREG_PITCH_PID_END			//24
    
//PIDFifo (R)
#define MOTORSREG_SPEED_FIFO_CURRENT    (MOTORSREG_SPEED_PID_BEGIN)		//24
#define MOTORSREG_SPEED_FIFO_SIZE       (MOTORSREG_SPEED_FIFO_CURRENT+sizeof(PIDStateEntry))//38
    
//PIDSettings (R/W)
#define MOTORSREG_SPEED_PID	         	(MOTORSREG_SPEED_FIFO_SIZE + sizeof(int8_t))		//39
#define MOTORSREG_SPEED_PID_KP          MOTORSREG_SPEED_PID									//39
#define MOTORSREG_SPEED_PID_KD          (MOTORSREG_SPEED_PID_KP + sizeof(PIDConstant))		//41
#define MOTORSREG_SPEED_PID_KI          (MOTORSREG_SPEED_PID_KD + sizeof(PIDConstant))  	//43
  
//PID target (R/W)
#define MOTORSREG_SPEED_PID_TARGET      (MOTORSREG_SPEED_PID_KI + sizeof(PIDConstant))		//45

#define MOTORSREG_SPEED_PID_END         (MOTORSREG_SPEED_PID_TARGET + sizeof(int16_t))		//47
    
/*
 *  PID heading registers
 */
#define MOTORSREG_HEADING_PID_BEGIN      MOTORSREG_SPEED_PID_END			//47
    
//PIDFifo (R)
#define MOTORSREG_HEADING_FIFO_CURRENT  (MOTORSREG_HEADING_PID_BEGIN)		//47
#define MOTORSREG_HEADING_FIFO_SIZE     (MOTORSREG_HEADING_FIFO_CURRENT+sizeof(PIDStateEntry))//61
    
//PIDSettings (R/W)
#define MOTORSREG_HEADING_PID           (MOTORSREG_HEADING_FIFO_SIZE + sizeof(int8_t))		//62
#define MOTORSREG_HEADING_PID_KP        MOTORSREG_HEADING_PID								//62
#define MOTORSREG_HEADING_PID_KD        (MOTORSREG_HEADING_PID_KP + sizeof(PIDConstant))	//64
#define MOTORSREG_HEADING_PID_KI        (MOTORSREG_HEADING_PID_KD + sizeof(PIDConstant))  	//66
  
//PID target (R/W)
#define MOTORSREG_HEADING_PID_TARGET    (MOTORSREG_HEADING_PID_KI + sizeof(PIDConstant))	//68

#define MOTORSREG_HEADING_PID_END       (MOTORSREG_HEADING_PID_TARGET + sizeof(int16_t))	//70
    
/*
 *  MPU
 */
#define MOTORSREG_MPU_BEGIN             MOTORSREG_HEADING_PID_END
#define MOTORSREG_MPU_OFFSET            MOTORSREG_MPU_BEGIN
#define MOTORSREG_MPU_END               (MOTORSREG_MPU_OFFSET + sizeof(MpuConfiguration))
   

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_REGISTERS_H */

