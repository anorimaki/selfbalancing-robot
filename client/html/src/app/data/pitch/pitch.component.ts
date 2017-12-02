import { Component, ViewChild } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { DataService } from '../data.service';
import { NotificationService } from "app/core/notification.service";
import { PidDataComponent } from '../pid/pid.component';

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
		this.pidView.init( this.dataService.pitch.getData() );
	}
}