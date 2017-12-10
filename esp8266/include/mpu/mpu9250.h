#ifndef MPU9250_H
#define MPU9250_H

#include "mpu/quaternation.h"
#include "util/optional.h"


namespace mpu
{


class Gyro
{
public:
	Gyro() {}

	Gyro( short* data ) {
		m_data[0] = data[0];
		m_data[1] = data[1];
		m_data[2] = data[2];
	}

	short x() const { return m_data[0]; }
	short y() const { return m_data[1]; }
	short z() const { return m_data[2]; }

private:
	short m_data[3];
};


typedef Gyro Accel;


class MpuData
{
public:
	MpuData(): m_timestamp(-1) {}
	MpuData( unsigned long ts ): m_timestamp(ts) {}

	const Optional<Quaternation>& quaternation() const {
		return m_quaternation;
	}

	void setQuaternation( const Quaternation& q ) {
		m_quaternation = q;
	}

	const Optional<Accel>& accel() const {
		return m_accel;
	}

	void setAccel( const Accel& a ) {
		m_accel = a;
	}

	const Optional<Gyro>& gyro() const {
		return m_gyro;
	}

	void setGyro( const Gyro& g ) {
		m_gyro = g;
	}

	unsigned long timestamp() const {
		return m_timestamp;
	}

private:
	Optional<Quaternation> m_quaternation;
	Optional<Accel> m_accel;
	Optional<Gyro> m_gyro;
	unsigned long m_timestamp;

};


class Mpu9250
{
public:
	bool init();
	bool end();

	bool enableDmp(bool enbale);

	bool configure();
	bool calibrate();
	bool storedCalibration() ;

	bool getData( Optional<MpuData>& data );

	void test();

private:
	void handleError();
};


}

#endif
