#ifndef MPU9250_H
#define MPU9250_H

#include "mpu/mpu9250.h"

namespace mpu
{


class MpuData
{
private:
	Quaternation quaternation;
};


class Mpu9250
{
public:
	void init();

};


}

#endif
