import { Component, ViewChild } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { DataService } from '../data.service';
import { NotificationService } from "app/core/notification.service";
import { PidDataComponent } from '../pid/pid.component';
import { PidStep } from 'app/shared/pid-step';

@Component( {
	templateUrl: './pitch.component.html',
	styleUrls: ['./pitch.component.css']
} )
export class PitchDataComponent {
	@ViewChild("pidView") 
	private pidView: PidDataComponent;

	constructor( private dataService: DataService,
		private notificationService: NotificationService ) {
	}

	ngAfterViewInit() {
		let data = this.dataService.pitch.getData().map( pidSteps => {
			pidSteps.forEach( pidStep => {
				pidStep.index = pidStep.index/250000,	// index are steps of 4us. Convert to seconds
				pidStep.current = pidStep.current * (180 / (3.1415 * 65535)),
				pidStep.output = (pidStep.output * 100) / PidStep.PID_MAX_OUTPUT
			});
			return pidSteps;
		});

		this.pidView.init( 1500, data );
	}
}