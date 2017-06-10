export class PitchState {
    index: number;
    previous_error: number;
    integral_error: number;
    target: number;
    current: number;

    constructor();
    constructor(other: PitchState);
    constructor(other?: PitchState) {
        this.index = other ? other.index : 0;
        this.previous_error = other ? other.previous_error : 0;
        this.integral_error = other ? other.integral_error : 0;
        this.target = other ? other.target : 0;
        this.current = other ? other.current : 0;
    }
}
