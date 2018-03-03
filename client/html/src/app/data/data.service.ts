import { Injectable } from '@angular/core'
import { Observable } from 'rxjs/Observable';
import { Subscription } from 'rxjs/Subscription';
import { Subscriber } from 'rxjs/Subscriber';
import { ReplaySubject } from 'rxjs/ReplaySubject';
import 'rxjs/add/operator/expand';
import 'rxjs/add/operator/do'
import 'rxjs/add/operator/retry';
import 'rxjs/add/operator/concatMap';
import 'rxjs/add/observable/empty';
import 'rxjs/add/observable/timer';
import 'rxjs/add/operator/catch';
import { RobotService, PidService } from 'app/core/robot.service';
import { PidStep } from "app/shared/pid-step";
import { PidSettings } from "app/core/pid-settings";
import { SettingsService, PidSettingsService } from 'app/settings/settings.service';
import { NotificationService } from "app/core/notification.service";
import { PidState } from 'app/core/pid-state';
import { SpeedStep } from 'app/data/speed/speed-step';
import { PitchStep } from 'app/data/pitch/pitch-step';
import { HeadingStep } from 'app/data/heading/heading-step';

let l;

export class PidDataService<T extends PidStep> {
	private static BUFFER_SIZE = 300;

	private rxData: Observable<T[]>;
	private rxDataBufer:  ReplaySubject<T[]>;
	private dataSuscription?: Subscription;
	isPolling: boolean;

	constructor( private settingsService: PidSettingsService,
				private notificationService: NotificationService,
				private pidSerivce: PidService,
				private pidStateFactory: (settings: PidSettings, state: PidState) => T, 
				private period: number ) {
		this.isPolling = false;			
		let baseRxData = () => this.settingsService.get().
			concatMap( settings =>
				this.pidSerivce.getState().
					map( pitches => pitches.map( state => pidStateFactory(settings, state) ) ).
					catch( (err, caught) => {
						this.notificationService.error( "Error getting PID pitch data" );
						return caught;
					}));
		this.rxData = baseRxData().expand( () => {
								if ( this.isPolling ) {
									return Observable.timer(period).concatMap( baseRxData ) 
								}
								return Observable.empty();
							});
		this.rxDataBufer = new ReplaySubject<T[]>(PidDataService.BUFFER_SIZE);
	}

	startPolling(): void {
		this.isPolling = true;
		this.dataSuscription = this.rxData.subscribe( this.rxDataBufer );
	}
	
	stopPolling(): void {
		this.isPolling = false;
		if ( this.dataSuscription ) {
			this.dataSuscription.unsubscribe();
		} 
	}

	getData(): Observable<T[]> {
		return this.rxDataBufer;
	}
}


@Injectable()
export class DataService {
	speed: PidDataService<SpeedStep>;
	pitch: PidDataService<PitchStep>;
	heading: PidDataService<HeadingStep>;
		
	constructor( private robotService : RobotService,
				private settingsService: SettingsService,
				private notificationService: NotificationService ) { 
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
		this.pitch.startPolling();
		this.heading.startPolling();
	}
	
	stopPolling(): void {
		this.speed.stopPolling();
		this.pitch.stopPolling();
		this.heading.stopPolling();
	}
 }
