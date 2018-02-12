import { PidStep } from "app/shared/pid-step";
import { PidState } from "app/core/pid-state";
import { PidSettings } from "app/core/pid-settings";

export class SpeedStep extends PidStep {
	static readonly MOTORS_SPEED_BITS = 10;

	constructor( state: PidState, settings: PidSettings ) {
		super( state, settings );

		this.index = this.index/250000;	// index are steps of 4us. Convert to seconds
		this.current >>= (PidStep.PID_INPUT_BIT_SIZE-SpeedStep.MOTORS_SPEED_BITS);
		this.target >>= (PidStep.PID_INPUT_BIT_SIZE-SpeedStep.MOTORS_SPEED_BITS);
		this.output = this.output * (100/PidStep.PID_MAX_OUTPUT);
	}
}