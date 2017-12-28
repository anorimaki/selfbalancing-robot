#ifndef MPU9250_H
#define	MPU9250_H

#include "math/quaternation.h"
#include "api/motors_i2c_model.h"
#include <stdbool.h>

#define MPU_DATA_RATE   200      //In Hz

extern MpuConfiguration mpu9250_configuration;

static inline uint8_t mpu9250_i2c_read( uint8_t address )
{
    return *byte_ptr(&mpu9250_configuration.pitch_offset, address);
}

static inline void mpu9250_i2c_write( uint8_t address, uint8_t value )
{
    *byte_ptr(&mpu9250_configuration.pitch_offset, address) = value;
}

        
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

void mpu9250_init();
bool mpu9250_start();
bool mpu9250_end();

MpuStatus mpu9250_get_data( MpuData* data );
MpuStatus mpu9250_get_pitch( fix16_t* pitch );

#endif	/* MPU9250_H */

