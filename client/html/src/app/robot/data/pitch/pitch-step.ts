import { PidStep } from 'robot/shared/pid-step';
import { PidState } from 'robot/core/pid-state';
import { PidSettings } from 'robot/core/pid-settings';

const MAX_PITCH_ANGLE_BITS = 16;
const ORIGINAL_ANGLE_QBITS = 16;
const ANGLE_QBITS = ORIGINAL_ANGLE_QBITS - (MAX_PITCH_ANGLE_BITS - PidStep.PID_INPUT_BIT_SIZE);
const ANGLE_FRACTIONAL_FACTOR = (1 << ANGLE_QBITS) - 1;
const ANGLE_TO_DEGREES_FACTOR = (180 / (3.1415 * ANGLE_FRACTIONAL_FACTOR)); 	// Radinas in q(ANGLE_BITS) to degress

export class PitchStep extends PidStep {
	constructor( state: PidState, settings: PidSettings ) {
		super( state, settings );

		this.index = this.index / 250000;		// index are steps of 4us. Convert to seconds
		this.current = this.current * ANGLE_TO_DEGREES_FACTOR;
		this.target = this.target * ANGLE_TO_DEGREES_FACTOR;
		this.output = (this.output * 100) / PidStep.PID_MAX_OUTPUT;
	}
}
