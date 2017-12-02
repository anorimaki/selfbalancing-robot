import { Injectable } from '@angular/core'
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/observable/of';

import { RobotService, PidService } from 'app/core/robot.service';
import { PidSettings } from "app/core/pid-settings";

export class PidSettingsService {
	private _settings: PidSettings;

	constructor( private pidService: PidService ) {
	}

	get(): Observable<PidSettings> {
		if ( this._settings ) {
			return Observable.of( this._settings );
		}

		return this.pidService.getSettings().
			map( settings => {
				this._settings = settings;
				return settings;
			});
	}
	
	set( settings: PidSettings ): Observable<void> {
		this._settings = settings;
		return this.pidService.setSettings(settings);
	}
}

@Injectable()
export class SettingsService {
	speedPid: PidSettingsService;
	pitchPid: PidSettingsService;

	constructor( private robotService: RobotService ) {
		this.speedPid = new PidSettingsService( robotService.speed );
		this.pitchPid = new PidSettingsService( robotService.pitch );
	}
}
