import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs/Observable';

import { StationConfig } from './station-config';
import { SoftApConfig } from './softap-config';
import { BssConfig } from './bss-config';
import { NetConfig } from './net-config';

import { environment } from 'environments/environment';


export class StationService {
	constructor( private http: HttpClient, private baseUrl: string ) {
	}

	get(): Observable<StationConfig> {
		return this.http.get<StationConfig>( this.baseUrl );
	}

	setEnabled( enabled: boolean ): Observable<StationConfig> {
		return this.http.put<StationConfig>( this.baseUrl + '/state', {enabled: enabled} )
	}

	setBss( bss: BssConfig ): Observable<StationConfig> {
		return this.http.put<StationConfig>( this.baseUrl + '/bss', bss )
	}

	setDhcp( enabled: boolean ): Observable<StationConfig> {
		return this.http.put<StationConfig>( this.baseUrl + '/dhcp', {enabled: enabled} )
	}

	setNetwork( network: NetConfig ): Observable<StationConfig> {
		return this.http.put<StationConfig>( this.baseUrl + '/network', network )
	}
}

export class SoftApService {
	constructor( private http: HttpClient, private baseUrl: string ) {
	}

	get(): Observable<SoftApConfig> {
		return this.http.get<SoftApConfig>( this.baseUrl );
	}

	setEnabled( enabled: boolean ): Observable<SoftApConfig> {
		return this.http.put<SoftApConfig>( this.baseUrl + '/state', {enabled: enabled} )
	}

	setBss( bss: BssConfig ): Observable<SoftApConfig> {
		return this.http.put<SoftApConfig>( this.baseUrl + '/bss', bss )
	}
}

@Injectable()
export class WifiService {
	softAp: SoftApService;
	station: StationService;

	constructor( http: HttpClient ) {
		const baseUrl = environment.robotUrl + 'wifi';
		this.softAp = new SoftApService( http, baseUrl + '/softap' );
		this.station = new StationService( http, baseUrl + '/station' );
	}
}
