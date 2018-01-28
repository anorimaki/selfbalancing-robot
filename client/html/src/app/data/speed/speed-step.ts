import { PidStep } from "app/shared/pid-step";
import { PidState } from "app/core/pid-state";
import { PidSettings } from "app/core/pid-settings";

export class SpeedStep extends PidStep {
	constructor( state: PidState, settings: PidSettings ) {
		super( state, settings );

		this.index = this.index/250000;	// index are steps of 4us. Convert to seconds
		this.output = this.output * (100/PidStep.PID_MAX_OUTPUT);
	}
}