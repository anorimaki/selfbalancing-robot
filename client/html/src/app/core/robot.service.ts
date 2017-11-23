import { Injectable } from '@angular/core';
import { Http } from '@angular/http';
import { Observable }     from 'rxjs/Observable';
import 'rxjs/add/operator/map';

import { PidState }     from './pid-state';
import { PidSettings }     from './pid-settings';
import { environment } from 'environments/environment';

@Injectable()
export class RobotService {
    private static pitchStateUrl = `${environment.robotUrl}/motors/pitch/state`;
    private static pitchPidUrl = `${environment.robotUrl}/motors/pitch/pid`;
    
    constructor( private http : Http ) { }
    
    getPitchState() : Observable<PidState[]> {
        return this.http.get( RobotService.pitchStateUrl ).map( response => 
            (<any[]>response.json()).map( item => new PidState(item) ) );
    }
    
    getPitchPidSettings(): Observable<PidSettings> {
		return this.http.get( RobotService.pitchPidUrl ).
					map( response => PidSettings.fromJson( response.json() ) );
    } 
 
    setPitchPidSettings( settings: PidSettings ): Observable<void> {
        return this.http.put( RobotService.pitchPidUrl, settings ).map( response => null );
    }
}
