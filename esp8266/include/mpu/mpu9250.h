#ifndef MPU9250_H
#define MPU9250_H

#include "mpu/quaternation.h"
#include "util/optional.h"


namespace mpu
{


class MpuData
{
public:
	MpuData() {}
	MpuData( const Quaternation& q, unsigned long ts ): m_quaternation(q), m_timestamp(ts) {}

	const Optional<Quaternation>& quaternation() const
		{ return m_quaternation; }

	unsigned long timestamp() const
			{ return m_timestamp; }

	void setQuaternation( const Quaternation& q )
		{ m_quaternation = q; }

private:
	Optional<Quaternation> m_quaternation;
	unsigned long m_timestamp;

};


class Mpu9250
{
public:
	typedef void(*ErrorHandler)();

public:
	void init( ErrorHandler errorHandler );

	Optional<MpuData> getData();

private:
	void handleError();

private:
	ErrorHandler m_errorHandler;
};


}

#endif
