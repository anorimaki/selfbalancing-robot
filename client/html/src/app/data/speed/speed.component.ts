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
		this.pidView.init( this.dataService.speed.getData() );
	}
}