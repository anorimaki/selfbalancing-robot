#ifndef SRC_MOTORCTRL_H_
#define SRC_MOTORCTRL_H_

#include "pid.h"

namespace motion
{

class Motors
{
public:
	typedef control::PID Pitch;

public:
	bool init();

	bool pitchState( Pitch::State& state );
};

}

#endif /* SRC_PWM_H_ */
