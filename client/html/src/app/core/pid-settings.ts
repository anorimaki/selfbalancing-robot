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
        if (this.proportional==null) {
            this.proportional = o.proportional;
        }
        if (this.derivative==null) {
            this.derivative = o.derivative;
        }
        if (this.integral==null) {
            this.integral = o.integral;
        }
    }
}
