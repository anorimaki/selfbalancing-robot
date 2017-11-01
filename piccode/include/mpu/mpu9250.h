#ifndef MPU9250_H
#define	MPU9250_H

#include "math/quaternation.h"
#include <stdbool.h>

typedef struct  {
    Quaternation quaternation;
} MpuData;

typedef enum {
    MPU_OK,
    MPU_NO_DATA,
    MPU_ERROR,
    MPU_OVERUN,
    MPU_DATA_CORRUPTION
} MpuStatus;

bool mpu9250_init();

MpuStatus mpu9250_get_data( MpuData* data );

#endif	/* MPU9250_H */

