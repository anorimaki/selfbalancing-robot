#ifndef DISPLAY_H
#define	DISPLAY_H

#include "mpu/mpu9250.h"


void display_system_initialization();
void display_system_running();
void display_system_paused();
void display_mpu_error();
void display_mpu_result( MpuStatus status );


#endif	/* DISPLAY_H */

