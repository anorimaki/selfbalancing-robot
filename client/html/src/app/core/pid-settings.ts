export class PidSettings {
    public proportional: number;
    public derivative: number;
    public integral: number;

    constructor(proportional?: number, derivative?: number, integral?: number) {
        this.proportional = proportional ? proportional : null;
        this.derivative = derivative ? derivative : null;
        this.integral = integral ? integral : null;
    }
    
    merge( o: PidSettings ): void {
        if (o.proportional) {
            this.proportional = o.proportional;
        }
        if (o.derivative) {
            this.derivative = o.derivative;
        }
        if (o.integral) {
            this.integral = o.integral;
        }
	}
	
	static fromJson( json: any ): PidSettings {
		return new PidSettings( json.proportional, json.derivative, json.integral );
	}
}
