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


class PidDataService {
	private static BUFFER_SIZE = 400;

	private rxData: Observable<PidStep[]>;
	private rxDataBufer:  ReplaySubject<PidStep[]>;
	private dataSuscription?: Subscription;
	private polling: boolean;

	constructor( private settingsService: PidSettingsService,
				private notificationService: NotificationService,
				private pidSerivce: PidService,
				private period: number ) {
		this.polling = false;			
		let baseRxData = () => this.settingsService.get().
			concatMap( settings =>
				this.pidSerivce.getState().
					map( pitches => pitches.map( item => new PidStep( item, settings ) ) ).
					catch( (err, caught) => {
						this.notificationService.error( "Error getting PID pitch data" );
						return caught;
					}));
		this.rxData = baseRxData().expand( () => {
								if ( this.polling ) {
									return Observable.timer(period).concatMap( baseRxData ) 
								}
								return Observable.empty();
							});
		this.rxDataBufer = new ReplaySubject<PidStep[]>(PidDataService.BUFFER_SIZE);
	}

	startPolling(): void {
		this.polling = true;
	//	this.rxData.subscribe( this.rxDataBufer );
		this.dataSuscription = this.rxData.subscribe( this.rxDataBufer );
	}
	
	stopPolling(): void {
		this.polling = false;
		if ( this.dataSuscription ) {
			this.dataSuscription.unsubscribe();
		} 
	}

	getData(): Observable<PidStep[]> {
		return this.rxDataBufer;
	}
}


@Injectable()
export class DataService {
	private static BUFFER_SIZE = 300;

	speed: PidDataService;
	pitch: PidDataService;
		
	constructor( private robotService : RobotService,
				private settingsService: SettingsService,
				private notificationService: NotificationService ) { 
		this.speed = new PidDataService( settingsService.speedPid, 
							notificationService, robotService.speed, 1000 );
		this.pitch = new PidDataService( settingsService.pitchPid,
							notificationService, robotService.pitch, 20 );
	}

	startPolling(): void {
		this.speed.startPolling();
		this.pitch.startPolling();
	}
	
	stopPolling(): void {
		this.speed.stopPolling();
		this.pitch.stopPolling();
	}
 }
