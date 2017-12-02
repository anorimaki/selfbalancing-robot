#ifndef SRC_MOTORCTRL_H_
#define SRC_MOTORCTRL_H_

#include "motors_i2c_model.h"
#include "util/arduino_stl_support.h"
#include "io/display.h"
#include <vector>

namespace motion
{


class PidEngine {
public:
	typedef ::PIDStateEntry PitchState;
	typedef ::PIDSettings PIDSettings;

public:
	PidEngine( io::Display* display, uint8_t settingsReg, uint8_t fifoSizeReg, uint8_t fifoCurrentReg ):
					m_display( display ),
					m_settingsReg( settingsReg ),
					m_fifoSizeReg( fifoSizeReg ),
					m_fifoCurrentReg( fifoCurrentReg ) {}

	bool state( std::vector<PitchState>& state );

	bool settins( PIDSettings& settings );
	bool setSettins( const PIDSettings& settings );

private:
	io::Display* m_display;
	const uint8_t m_settingsReg;
	const uint8_t m_fifoSizeReg;
	const uint8_t m_fifoCurrentReg;
};


class Motors
{
public:
	typedef ::PIDStateEntry PitchState;
	typedef ::PIDSettings PIDSettings;

public:
	bool init( io::Display* display );

	PidEngine& speed() { return *m_speed; }
	PidEngine& pitch() { return *m_pitch; }

private:
	PidEngine* m_speed;
	PidEngine* m_pitch;
};

}

#endif /* SRC_PWM_H_ */
