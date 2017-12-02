import { Injectable } from '@angular/core';
import { Http } from '@angular/http';
import { Observable }     from 'rxjs/Observable';
import 'rxjs/add/operator/map';

import { PidState } from './pid-state';
import { PidSettings } from './pid-settings';
import { environment } from 'environments/environment';

export class PidService {
    private stateUrl;
	private stettingsUrl;

	constructor( private urlPath: string, private http: Http  ) {
		this.stateUrl = `${environment.robotUrl}/${urlPath}/state`;
		this.stettingsUrl = `${environment.robotUrl}/${urlPath}/settings`;
	}

	getState() : Observable<PidState[]> {
        return this.http.get( this.stateUrl ).map( response => 
            (<any[]>response.json()).map( item => new PidState(item) ) );
    }
    
    getSettings(): Observable<PidSettings> {
		return this.http.get( this.stettingsUrl ).
					map( response => PidSettings.fromJson( response.json() ) );
    } 
 
    setSettings( settings: PidSettings ): Observable<void> {
        return this.http.put( this.stettingsUrl, settings ).map( response => null );
	}
}

@Injectable()
export class RobotService {
	pitch: PidService;
	speed: PidService;

    constructor( private http : Http ) { 
		this.speed = new PidService( 'speed', http );
		this.pitch = new PidService( 'pitch', http );
	}
}
