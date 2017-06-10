import { Injectable } from '@angular/core';
import { Http } from '@angular/http';
import { Observable }     from 'rxjs/Observable';
import { PitchState }     from './pitch-state';
import 'rxjs/add/operator/map';
import { environment } from '../environments/environment';

@Injectable()
export class RobotService {
    
    private pitchStateUrl = `${environment.robotUrl}/motors/pitch/state`;
    
    constructor( private http : Http ) { }
    
    pitchState() : Observable<PitchState[]> {
        return this.http.get( this.pitchStateUrl ).map( response => response.json() as PitchState[] );
    }
}
