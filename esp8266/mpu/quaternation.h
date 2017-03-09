#ifndef MPU_QUATERNATION_H_
#define MPU_QUATERNATION_H_

namespace mpu
{

class Quaternation
{
public:
	Quaternation( long lw, long lx, long ly, long lz ) {
		m_w = lw;
		m_x = lx;
		m_y = ly;
		m_z = lz;
		normalize();
	}

	float w() const { return m_w; }
	float x() const { return m_x; }
	float y() const { return m_y; }
	float z() const { return m_z; }

private:
	void normalize() {
		float sum = m_w*m_w + m_x*m_x + m_y*m_y + m_z*m_z;
		float qlen=sqrt(sum);

		m_w /= qlen;
		m_x /= qlen;
		m_y /= qlen;
		m_z /= qlen;
	}

private:
	float m_w;
	float m_x;
	float m_y;
	float m_z;
};

}

#endif /* MPU_QUATERNATION_H_ */
