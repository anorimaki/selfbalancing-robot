import { Component, ViewChild } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { DataService } from '../data.service';
import { PidDataComponent } from '../pid/pid.component';
import { PidStep } from 'app/shared/pid-step';

@Component( {
	templateUrl: './pitch.component.html',
	styleUrls: ['./pitch.component.css']
} )
export class PitchDataComponent {
	@ViewChild("pidView") 
	private pidView: PidDataComponent;

	constructor( private dataService: DataService ) {
	}

	ngAfterViewInit() {
		// Wait a tick because the component's view has already been checked
		setTimeout( () => this.pidView.init( 1200, this.dataService.pitch ), 0 );
	}
}