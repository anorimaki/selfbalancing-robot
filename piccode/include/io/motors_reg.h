#ifndef MOTORS_SYSTEM_REGISTERS_H
#define	MOTORS_SYSTEM_REGISTERS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MOTORSREG_STATUS                0
#define MOTORSREGBIT_STATUS_ON          0x01

#define MOTORSREG_PITCH_PID_KP          0x02
#define MOTORSREG_PITCH_PID_KD          (MOTORSREG_PITCH_PID_KP + sizeof(uint8_t))
#define MOTORSREG_PITCH_PID_KI          (MOTORSREG_PITCH_PID_KD + sizeof(uint8_t))
#define MOTORSREG_PITCH_PID_PREV_ERR    (MOTORSREG_PITCH_PID_KI + sizeof(uint8_t))
#define MOTORSREG_PITCH_TARGET          (MOTORSREG_PITCH_PID_PREV_ERR + sizeof(uint16_t))
#define MOTORSREG_PITCH_CURRENT         (MOTORSREG_PITCH_TARGET + sizeof(uint16_t))

    
#define MOTORSREG_LEFT_VELOCITY_SET     (MOTORSREG_PITCH_CURRENT + sizeof(uint16_t))
    
#define MOTORS_REGISTERS_SIZE           (MOTORSREG_LEFT_VELOCITY_SET+1)


#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_REGISTERS_H */

