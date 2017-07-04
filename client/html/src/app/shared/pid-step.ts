import { PidState } from "app/core/pid-state";
import { PidSettings } from "app/core/pid-settings";

export class PidStep {
                //Take this values from motor controller code
    static readonly PID_MAX_OUTPUT = 0x7FFF;
    static readonly PID_MIN_OUTPUT = -PidStep.PID_MAX_OUTPUT;
    static readonly PID_MAX_INTEGRAL_ERROR = PidStep.PID_MAX_OUTPUT*14;
    static readonly PID_MIN_INTEGRAL_ERROR = PidStep.PID_MIN_OUTPUT*14;
    
    public readonly result: number;
    public readonly current: number;
    public readonly target: number;
    public readonly integralError: number;
    public readonly index: number;

    constructor( state: PidState, settings: PidSettings ) {
        this.result = this.calculateResult( state, settings );
        
        this.current = state.current; 
        this.integralError = state.integralError;
        this.target = state.target;
        this.index = state.index;
    }
        
    private calculateResult( state: PidState, settings: PidSettings ): number {
        let error = state.current - state.target;
        let derivative_error = error - state.previousError;
        let integralError = state.integralError + 
                            error * settings.integral;
        
        if ( integralError > PidStep.PID_MAX_INTEGRAL_ERROR )
            integralError = PidStep.PID_MAX_INTEGRAL_ERROR;
        if ( integralError < PidStep.PID_MIN_INTEGRAL_ERROR )
            integralError = PidStep.PID_MIN_INTEGRAL_ERROR;
        
        let result = (settings.derivative * derivative_error) +
                        (settings.proportional * error) +
                        integralError;
        
        if ( result > PidStep.PID_MAX_OUTPUT )
            result = PidStep.PID_MAX_OUTPUT;
        if ( result < PidStep.PID_MIN_OUTPUT )
            result = PidStep.PID_MIN_OUTPUT;
        
        return result;
    }
}
