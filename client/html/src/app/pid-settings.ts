export class PidSettings {
    public proportional: number;
    public derivative: number;
    public integral: number;

    constructor(proportional?: number, derivative?: number, integral?: number) {
        this.proportional = proportional ? proportional : null;
        this.derivative = derivative ? derivative : null;
        this.integral = integral ? integral : null;
    }
}
