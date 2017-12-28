import { PidState } from "app/core/pid-state";
import { PidSettings } from "app/core/pid-settings";

export class PidStep {
                //Take this values from motor controller code
    static readonly PID_MAX_OUTPUT = 0x7FFF;
    static readonly PID_MIN_OUTPUT = -PidStep.PID_MAX_OUTPUT;
    static readonly PID_MAX_INTEGRAL_ERROR = 0x3FFF;
    static readonly PID_MIN_INTEGRAL_ERROR = -PidStep.PID_MAX_OUTPUT;
    
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
        let error = state.target - state.current;
        
        let integralError = state.integralError + error;
        if ( integralError > PidStep.PID_MAX_INTEGRAL_ERROR )
            integralError = PidStep.PID_MAX_INTEGRAL_ERROR;
        if ( integralError < PidStep.PID_MIN_INTEGRAL_ERROR )
            integralError = PidStep.PID_MIN_INTEGRAL_ERROR;
        this.integralOutput = settings.integral * integralError;
       
        let derivativeError = error - state.previousError;
        this.derivativeOutput = settings.derivative * derivativeError;
        
        this.proportionalOutput = settings.proportional * error;
        
		this.output = this.integralOutput + this.proportionalOutput + 
						this.derivativeOutput ;
        
		this.output = this.output >> 7;     //Scale value as algorithm in PIC
		
        
        if ( this.output > PidStep.PID_MAX_OUTPUT )
            this.output = PidStep.PID_MAX_OUTPUT;
        if ( this.output < PidStep.PID_MIN_OUTPUT )
			this.output = PidStep.PID_MIN_OUTPUT;
    }
}
