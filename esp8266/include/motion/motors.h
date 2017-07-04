#ifndef SRC_MOTORCTRL_H_
#define SRC_MOTORCTRL_H_

#include "motors_i2c_model.h"
#include "util/arduino_stl_support.h"
#include <vector>

namespace motion
{

class Motors
{
public:
	typedef ::PIDStateEntry PitchState;
	typedef ::PIDSettings PIDSettings;

public:
	bool init();

	bool pitchState( std::vector<PitchState>& state );

	bool pitchPIDSettins( PIDSettings& settings );
	bool setPitchPIDSettins( const PIDSettings& settings );
};

}

#endif /* SRC_PWM_H_ */
