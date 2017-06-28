export class PitchState {
    private i: number;
    private p_err: number;
    private i_err: number;
    private tar: number;
    private cur: number;

    constructor();
    constructor(other: PitchState);
    constructor(other?: PitchState) {
        this.i = other ? other.i : 0;
        this.p_err = other ? other.p_err : 0;
        this.i_err = other ? other.i_err : 0;
        this.tar = other ? other.tar : 0;
        this.cur = other ? other.cur : 0;
    }
    
    get index(): number {
        return this.i;
    }
    
    get previous_error(): number {
        return this.p_err;
    }
    
    get integral_error(): number {
        return this.i_err;
    }
    
    set integral_error(err: number) {
        this.i_err = err;
    }
    
    get target(): number {
        return this.tar;
    }
    
    get current(): number {
        return this.cur;
    }
}
