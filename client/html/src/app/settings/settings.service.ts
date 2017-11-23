import { Injectable } from '@angular/core'
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/observable/of';

import { RobotService } from 'app/core/robot.service';
import { PidSettings } from "app/core/pid-settings";

@Injectable()
export class SettingsService {
	private _pitchPid: PidSettings;

	constructor( private robotService: RobotService ) {}

	get pitchPid(): Observable<PidSettings> {
		if ( this._pitchPid ) {
			return Observable.of( this._pitchPid );
		}

		return this.robotService.getPitchPidSettings().
			map( settings => {
				this._pitchPid = settings;
				return settings;
			});
	}

	setPitchPid( settings: PidSettings ): Observable<void> {
		this._pitchPid = settings;
		return this.robotService.setPitchPidSettings(settings);
	}
}
