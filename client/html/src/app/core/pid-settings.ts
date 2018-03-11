export class PidSettings {
	public proportional?: number;
	public derivative?: number;
	public integral?: number;

	constructor(proportional?: number, derivative?: number, integral?: number) {
		this.proportional = proportional;
		this.derivative = derivative;
		this.integral = integral;
	}

	merge( o: any ): void {
		if (o.proportional !== undefined) {
			this.proportional = o.proportional;
		}
		if (o.derivative !== undefined) {
			this.derivative = o.derivative;
		}
		if (o.integral !== undefined) {
			this.integral = o.integral;
		}
	}

	static fromJson( json: any ): PidSettings {
		return new PidSettings( json.proportional, json.derivative, json.integral );
	}
}
