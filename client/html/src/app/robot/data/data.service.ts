
import {empty as observableEmpty, timer as observableTimer,  Observable ,  Subscription ,  ReplaySubject } from 'rxjs';
import {concatMap, map, catchError, expand} from 'rxjs/operators';
import { Injectable } from '@angular/core'







import { RobotService, PidService } from 'robot/core/robot.service';
import { PidStep } from 'robot/shared/pid-step';
import { PidSettings } from 'robot/core/pid-settings';
import { SettingsService, PidSettingsService } from 'robot/settings/settings.service';
import { NotificationService } from 'app/core/notification.service';
import { PidState } from 'robot/core/pid-state';
import { SpeedStep } from 'robot/data/speed/speed-step';
import { PitchStep } from 'robot/data/pitch/pitch-step';
import { HeadingStep } from 'robot/data/heading/heading-step';


export class PidDataService<T extends PidStep> {
	private static BUFFER_SIZE = 300;

	isPolling: boolean;
	private rxData: Observable<T[]>;
	private rxDataBuffer:  ReplaySubject<T[]>;
	private dataSubscription?: Subscription;

	constructor( private settingsService: PidSettingsService,
				private notificationService: NotificationService,
				private pidService: PidService,
				pidStateFactory: (settings: PidSettings, state: PidState) => T,
				period: number ) {
		this.isPolling = false;
		const baseRxData = () => this.settingsService.get().pipe(
			concatMap( settings =>
				this.pidService.getState().pipe(
					map( pitches => pitches.map( state => pidStateFactory(settings, state) ) ),
					catchError( (err, caught) => {
						this.notificationService.error( 'Error getting PID data', err );
						return caught;
					}),)));
		this.rxData = baseRxData().pipe(expand( () => {
								if ( this.isPolling ) {
									return observableTimer(period).pipe(concatMap( baseRxData ))
								}
								return observableEmpty();
							}));
		this.rxDataBuffer = new ReplaySubject<T[]>(PidDataService.BUFFER_SIZE);
	}

	startPolling(): void {
		this.isPolling = true;
		this.dataSubscription = this.rxData.subscribe( this.rxDataBuffer );
	}

	stopPolling(): void {
		this.isPolling = false;
		if ( this.dataSubscription ) {
			this.dataSubscription.unsubscribe();
		}
	}

	getData(): Observable<T[]> {
		return this.rxDataBuffer;
	}
}


@Injectable()
export class DataService {
	speed: PidDataService<SpeedStep>;
	pitch: PidDataService<PitchStep>;
	heading: PidDataService<HeadingStep>;

	constructor( robotService: RobotService,
				settingsService: SettingsService,
				notificationService: NotificationService ) {
		this.speed = new PidDataService( settingsService.speedPid,
						notificationService, robotService.speed,
						(settings: PidSettings, state: PidState) => new SpeedStep(state, settings),
						700 );
		this.pitch = new PidDataService( settingsService.pitchPid,
						notificationService, robotService.pitch,
						(settings: PidSettings, state: PidState) => new PitchStep(state, settings),
						150 );
		this.heading = new PidDataService( settingsService.headingPid,
						notificationService, robotService.heading,
						(settings: PidSettings, state: PidState) => new HeadingStep(state, settings),
						700 );
	}

	startPolling(): void {
		this.speed.startPolling();
		// this.pitch.startPolling();
		setTimeout( () => this.pitch.startPolling(), 200 );
		// this.heading.startPolling();
		setTimeout( () => this.heading.startPolling(), 400 );
	}

	stopPolling(): void {
		this.speed.stopPolling();
		this.pitch.stopPolling();
		this.heading.stopPolling();
	}
}
