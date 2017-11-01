#ifndef DISPLAY_H
#define	DISPLAY_H

#include "mpu/mpu9250.h"


void display_system_initialization();
void display_system_ready();
void display_system_wait();
void display_mpu_init_error();
void display_mpu_result( MpuStatus status );


#endif	/* DISPLAY_H */

