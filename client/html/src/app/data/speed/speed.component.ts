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
		this.pidView.init( 300, this.dataService.speed.getData() );
	}
}