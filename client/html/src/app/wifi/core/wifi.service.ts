import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs/Observable';

import { StationConfig } from './station-config';
import { SoftApConfig } from './softap-config';
import { BssConfig } from './bss-config';
import { NetConfig } from './net-config';
import { BssInfo } from './bss-info';

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
	baseUrl: string;

	constructor( private http: HttpClient ) {
		this.baseUrl = environment.robotUrl + 'wifi';
		this.softAp = new SoftApService( http, this.baseUrl + '/softap' );
		this.station = new StationService( http, this.baseUrl + '/station' );
	}
	
	scan(): Observable<BssInfo[]> {
		return this.http.get<BssInfo[]>(this.baseUrl + '/ssids');
	}
}
