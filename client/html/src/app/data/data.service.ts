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


class PidDataService {
	private static BUFFER_SIZE = 300;

	private rxData: Observable<PidStep[]>;
	private rxDataBufer:  ReplaySubject<PidStep[]>;
	private dataSuscription?: Subscription;

	constructor( private settingsService: PidSettingsService,
				private pidSerivce: PidService,
				private period: number ) {
		let baseRxData = this.settingsService.get().
			concatMap( settings => 
				this.pidSerivce.getState().
					map( pitches => 
						pitches.map( item => new PidStep( item, settings ) )
					)
				);
		this.rxData = baseRxData.expand( () => 
								Observable.timer(period).concatMap( () => baseRxData ) );
		this.rxDataBufer = new ReplaySubject<PidStep[]>(PidDataService.BUFFER_SIZE);
	}

	startPolling(): void {
		this.dataSuscription = this.rxData.subscribe( this.rxDataBufer );
	}
	
	stopPolling(): void {
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
				private settingsService: SettingsService  ) { 
		this.speed = new PidDataService( settingsService.speedPid, 
									robotService.speed, 1000 );
		this.pitch = new PidDataService( settingsService.pitchPid,
									robotService.pitch, 200 );
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
