import { Injectable } from '@angular/core'
import { Observable, AsyncSubject, Subscription } from 'rxjs';


import { RobotService, PidService } from 'robot/core/robot.service';
import { PidSettings } from 'robot/core/pid-settings';

export class PidSettingsService {
	private subject: AsyncSubject<PidSettings>;
	private serviceSubscription?: Subscription;

	constructor( private pidService: PidService ) {
		this.subject = new AsyncSubject();
	}

	get(): Observable<PidSettings> {
		if ( !this.serviceSubscription ) {
			this.serviceSubscription = this.pidService.getSettings().subscribe(this.subject);
		}
		return this.subject;
	}

	set( settings: PidSettings ): Observable<void> {
		if ( this.serviceSubscription ) {
			this.serviceSubscription.unsubscribe();
			this.serviceSubscription = undefined;
		}
		return this.pidService.setSettings(settings);
	}
}


@Injectable()
export class SettingsService {
	speedPid: PidSettingsService;
	pitchPid: PidSettingsService;
	headingPid: PidSettingsService;

	constructor( robotService: RobotService ) {
		this.speedPid = new PidSettingsService( robotService.speed );
		this.pitchPid = new PidSettingsService( robotService.pitch );
		this.headingPid = new PidSettingsService( robotService.heading );
	}
}
