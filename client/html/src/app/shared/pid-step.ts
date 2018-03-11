import { PidState } from 'app/core/pid-state';
import { PidSettings } from 'app/core/pid-settings';

export class PidStep {
			// Take this values from motor controller code
	static readonly PID_MAX_OUTPUT = 0x7FFF;
	static readonly PID_MIN_OUTPUT = -PidStep.PID_MAX_OUTPUT;
	static readonly PID_MAX_INTEGRAL_ERROR = 0x0003FFF;
	static readonly PID_MIN_INTEGRAL_ERROR = -PidStep.PID_MAX_INTEGRAL_ERROR;

	static readonly PID_INPUT_BIT_SIZE = 15;
	static readonly PID_CONSTANT_BIT_SIZE = 13
	static readonly PID_OUTPUT_BIT_SIZE = 16;
	static readonly OUTPUT_SHIFT =
			(PidStep.PID_INPUT_BIT_SIZE + (PidStep.PID_CONSTANT_BIT_SIZE - 5)) -
				PidStep.PID_OUTPUT_BIT_SIZE ;

	public index: number;
	public current: number;
	public target: number;
	public output: number;
	public integralOutput: number;
	public derivativeOutput: number;
	public proportionalOutput: number;

	constructor( state: PidState, settings: PidSettings ) {
		this.current = state.current;
		this.target = state.target;
		this.index = state.index;

		this.calculateOutputs( state, settings );
	}

	private calculateOutputs( state: PidState, settings: PidSettings ): void {
		const error = state.target - state.current;

		let integralError = state.integralError + error;
		if ( integralError > PidStep.PID_MAX_INTEGRAL_ERROR ) {
			integralError = PidStep.PID_MAX_INTEGRAL_ERROR;
		}
		if ( integralError < PidStep.PID_MIN_INTEGRAL_ERROR ) {
			integralError = PidStep.PID_MIN_INTEGRAL_ERROR;
		}
		this.integralOutput = settings.integral * integralError;

		const derivativeError = error - state.previousError;
		this.derivativeOutput = settings.derivative * derivativeError;

		this.proportionalOutput = settings.proportional * error;

		this.output = this.integralOutput + this.proportionalOutput +
						this.derivativeOutput ;

						// Scale value as algorithm in PIC
		this.output = this.output >> PidStep.OUTPUT_SHIFT;

		if ( this.output > PidStep.PID_MAX_OUTPUT ) {
			this.output = PidStep.PID_MAX_OUTPUT;
		}
		if ( this.output < PidStep.PID_MIN_OUTPUT ) {
			this.output = PidStep.PID_MIN_OUTPUT;
		}
	}
}

