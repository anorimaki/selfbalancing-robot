import { Component, ViewChild } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { DataService } from '../data.service';
import { PidDataComponent } from '../pid/pid.component';

@Component( {
	templateUrl: './speed.component.html',
	styleUrls: ['./speed.component.css']
} )
export class SpeedDataComponent {
	@ViewChild("pidView") 
	private pidView: PidDataComponent;

	constructor( private dataService: DataService ) {
	}

	ngAfterViewInit() {
		// Wait a tick because the component's view has already been checked
		setTimeout( () => this.pidView.init( 100, this.dataService.speed ), 0 );
	}
}