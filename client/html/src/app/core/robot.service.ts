import { Injectable } from '@angular/core';
import { Http } from '@angular/http';
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/operator/map';
import { MpuCalibrationData, MpuSettings } from './mpu-data';
import { PidState } from './pid-state';
import { PidSettings } from './pid-settings';
import { environment } from 'environments/environment';

export class PidService {
	private stateUrl;
	private settingsUrl;

	constructor( urlPath: string, private http: Http  ) {
		this.stateUrl = `${environment.robotUrl}${urlPath}/state`;
		this.settingsUrl = `${environment.robotUrl}${urlPath}/settings`;
	}

	getState(): Observable<PidState[]> {
		return this.http.get( this.stateUrl ).
			map( response => (<any[]>response.json()).
				map( item => new PidState(item) )
			);
	}

	getSettings(): Observable<PidSettings> {
		return this.http.get( this.settingsUrl ).
			map( response => PidSettings.fromJson( response.json() ) );
	}

	setSettings( settings: PidSettings ): Observable<void> {
		return this.http.put( this.settingsUrl, settings ).
			map( response => null );
	}
}


export class MpuService {
	private calibrationUrl;
	private settingsUrl;

	constructor( private http: Http  ) {
		this.calibrationUrl = `${environment.robotUrl}mpu/calibration`;
		this.settingsUrl = `${environment.robotUrl}mpu/settings`;
	}

	getCalibration(): Observable<MpuCalibrationData> {
		return this.http.get( this.calibrationUrl ).
			map( response => response.json() );
	}

	doCalibration(): Observable<MpuCalibrationData> {
		return this.http.put( this.calibrationUrl, undefined ).
			map( response => response.json() );
	}

	getSettings(): Observable<MpuSettings> {
		return this.http.get( this.settingsUrl ).
			map( response => response.json() );
	}

	setSettings( settings: MpuSettings ): Observable<void> {
		return this.http.put( this.settingsUrl, settings ).
			map( response => null );
	}
}

@Injectable()
export class RobotService {
	pitch: PidService;
	speed: PidService;
	heading: PidService;
	mpu: MpuService;

	constructor( http: Http ) {
		this.speed = new PidService( 'speed', http );
		this.pitch = new PidService( 'pitch', http );
		this.heading = new PidService( 'heading', http );
		this.mpu = new MpuService( http );
	}
}
