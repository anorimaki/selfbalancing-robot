import { Component, ViewChild } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { DataService } from '../data.service';
import { NotificationService } from "app/core/notification.service";
import { PidDataComponent } from '../pid/pid.component';

@Component( {
	templateUrl: './speed.component.html',
	styleUrls: ['./speed.component.css']
} )
export class SpeedDataComponent {
	@ViewChild("pidView") 
	private pidView: PidDataComponent;

	constructor( private dataService: DataService,
		private notificationService: NotificationService ) {
	}

	ngAfterViewInit() {
		let data = this.dataService.speed.getData().map( pidSteps => {
			pidSteps.forEach( pidStep => {
				pidStep.index = pidStep.index/250000,	// index are steps of 4us. Convert to seconds
				pidStep.output = pidStep.output * (180 / (3.1415 * 65535))
			});
			return pidSteps;
		});

		this.pidView.init( 300, data );
	}
}