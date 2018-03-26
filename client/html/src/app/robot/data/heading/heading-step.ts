import { PidStep } from 'robot/shared/pid-step';
import { PidState } from 'robot/core/pid-state';
import { PidSettings } from 'robot/core/pid-settings';
import { SpeedStep } from 'robot/data/speed/speed-step';

export class HeadingStep extends PidStep {
	constructor( state: PidState, settings: PidSettings ) {
		super( state, settings );

		this.index = this.index / 250000;		// index are steps of 4us. Convert to seconds
		this.current >>= (PidStep.PID_INPUT_BIT_SIZE - SpeedStep.MOTORS_SPEED_BITS);
		this.target >>= (PidStep.PID_INPUT_BIT_SIZE - SpeedStep.MOTORS_SPEED_BITS);
		this.output = this.output * (100 / PidStep.PID_MAX_OUTPUT);
	}
}
