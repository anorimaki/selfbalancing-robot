import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/operator/map';
import { MpuCalibrationData, MpuSettings } from './mpu-data';
import { PidState } from './pid-state';
import { PidSettings } from './pid-settings';
import { environment } from 'environments/environment';

export class PidService {
	private stateUrl;
	private settingsUrl;

	constructor( urlPath: string, private http: HttpClient  ) {
		this.stateUrl = `${environment.robotUrl}${urlPath}/state`;
		this.settingsUrl = `${environment.robotUrl}${urlPath}/settings`;
	}

	getState(): Observable<PidState[]> {
		return this.http.get<PidState[]>( this.stateUrl ).
			map( states => states.map( state => new PidState(state) ) );
	}

	getSettings(): Observable<PidSettings> {
		return this.http.get<PidSettings>( this.settingsUrl ).
			map( settings => PidSettings.fromJson( settings ) );
	}

	setSettings( settings: PidSettings ): Observable<void> {
		return this.http.put( this.settingsUrl, settings ).
			map( response => null );
	}
}


export class MpuService {
	private calibrationUrl;
	private settingsUrl;

	constructor( private http: HttpClient  ) {
		this.calibrationUrl = `${environment.robotUrl}mpu/calibration`;
		this.settingsUrl = `${environment.robotUrl}mpu/settings`;
	}

	getCalibration(): Observable<MpuCalibrationData> {
		return this.http.get<MpuCalibrationData>( this.calibrationUrl );
	}

	doCalibration(): Observable<MpuCalibrationData> {
		return this.http.put<MpuCalibrationData>( this.calibrationUrl, undefined );
	}

	getSettings(): Observable<MpuSettings> {
		return this.http.get<MpuSettings>( this.settingsUrl );
	}

	setSettings( settings: MpuSettings ): Observable<void> {
		return this.http.put<any>( this.settingsUrl, settings );
	}
}

@Injectable()
export class RobotService {
	pitch: PidService;
	speed: PidService;
	heading: PidService;
	mpu: MpuService;

	constructor( http: HttpClient ) {
		this.speed = new PidService( 'speed', http );
		this.pitch = new PidService( 'pitch', http );
		this.heading = new PidService( 'heading', http );
		this.mpu = new MpuService( http );
	}
}
