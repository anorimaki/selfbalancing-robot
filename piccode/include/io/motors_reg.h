#ifndef MOTORS_SYSTEM_REGISTERS_H
#define	MOTORS_SYSTEM_REGISTERS_H

#ifdef	__cplusplus
extern "C" {
#endif


#define MOTORSREG_STATUS                    0
#define MOTORSREGBIT_STATUS_ON              0x01   
    
#define MOTORSREG_LEFT_VELOCITY_SET         1
    
#define MOTORS_REGISTERS_SIZE               MOTORSREG_LEFT_VELOCITY_SET+1 


#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_REGISTERS_H */

