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
	PidEngine( io::Display* display, uint8_t settingsReg, uint8_t fifoSizeReg, uint8_t fifoCurrentReg, uint8_t targetReg ):
					m_display( display ),
					m_settingsReg( settingsReg ),
					m_fifoSizeReg( fifoSizeReg ),
					m_fifoCurrentReg( fifoCurrentReg ),
					m_targetReg( targetReg ) {}

	bool state( std::vector<PitchState>& state );

	bool settins( PIDSettings& settings );
	bool setSettins( const PIDSettings& settings );

	bool setTarget( int16_t target );

private:
	io::Display* m_display;
	const uint8_t m_settingsReg;
	const uint8_t m_fifoSizeReg;
	const uint8_t m_fifoCurrentReg;
	const uint8_t m_targetReg;
};


class Motors
{
public:
	typedef ::PIDStateEntry PitchState;
	typedef ::PIDSettings PIDSettings;

public:
	void init( io::Display* display );

	bool isRunning();
	bool pause();
	bool resume();

	PidEngine& speed() { return *m_speed; }
	PidEngine& pitch() { return *m_pitch; }
	PidEngine& heading() { return *m_heading; }

	bool setMpuOffset( int32_t pitchOffset );
	bool getMpuOffset( int32_t* pitchOffset );

private:
	PidEngine* m_speed;
	PidEngine* m_pitch;
	PidEngine* m_heading;
	io::Display* m_display;
};

}

#endif /* SRC_PWM_H_ */
