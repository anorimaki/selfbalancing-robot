#ifndef MPU_ANGLES_H_
#define MPU_ANGLES_H_

#include "mpu/quaternation.h"
#include <math.h>

namespace mpu
{

class Angles
{
public:
	Angles( const Accel& a )
	{
		float x = a.x();
		float y = a.y();
		float z = a.z();

		m_roll = atan2( y, sqrt( x * x + z * z ) );
		m_pitch = atan2( -x, sqrt( y * y + z * z ) );
		m_yaw = 0;
	}

	Angles( const Quaternation& q )
	{		//See https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		float ysqr = q.y() * q.y();

		// roll (x-axis rotation)
		float t0 = +2.0 * (q.w() * q.x() + q.y() * q.z());
		float t1 = +1.0 - 2.0 * (q.x() * q.x() + ysqr);
		m_roll = atan2(t0, t1);

		// pitch (y-axis rotation)
		float t2 = +2.0 * (q.w() * q.y() - q.z() * q.x());
		t2 = t2 > 1.0 ? 1.0 : t2;
		t2 = t2 < -1.0 ? -1.0 : t2;
		m_pitch = asin(t2);

		// yaw (z-axis rotation)
		float t3 = +2.0 * (q.w() * q.z() + q.x() * q.y());
		float t4 = +1.0 - 2.0 * (ysqr + q.z() * q.z());
		m_yaw = atan2(t3, t4);
	}

	float yaw() const { return m_yaw; }
	float pitch() const { return m_pitch; }
	float roll() const { return m_roll; }

	void toDegress() {
		m_yaw *= (180.0 / PI);
		m_pitch *= (180.0 / PI);
		m_roll *= (180.0 / PI);
	}

private:
	float m_yaw;
	float m_pitch;
	float m_roll;
};

}

#endif /* MPU_ANGLES_H_ */
