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


import { RobotService } from 'app/core/robot.service';
import { PidStep } from "app/shared/pid-step";
import { PidSettings } from "app/core/pid-settings";
import { SettingsService } from 'app/settings/settings.service';

@Injectable()
export class DataService {
	private static BUFFER_SIZE = 300;

	private rxPitchData: Observable<PidStep[]>;
	private rxPitchDataBufer:  ReplaySubject<PidStep[]>;
	private pitchDataSuscription: Subscription;
		
	constructor( private robotService : RobotService,
				private settingsService: SettingsService  ) { 
		let baseRxPitchData = this.settingsService.pitchPid.
			concatMap( pitchPidSettings => 
				this.robotService.getPitchState().
					map( pitches => 
						pitches.map( item => new PidStep( item, pitchPidSettings ) )
					)
				);
		this.rxPitchData = baseRxPitchData.expand( () => 
								Observable.timer(200).concatMap( () => baseRxPitchData ) );
		this.rxPitchDataBufer = new ReplaySubject<PidStep[]>(DataService.BUFFER_SIZE);
	}

    startPolling(): void {
		this.pitchDataSuscription = this.rxPitchData.subscribe( this.rxPitchDataBufer );
	}
	
	stopPolling(): void {
		this.pitchDataSuscription.unsubscribe();
	}

	getData(): Observable<PidStep[]> {
		return this.rxPitchDataBufer;
	}
}
