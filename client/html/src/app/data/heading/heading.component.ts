import { Component, ViewChild } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { DataService } from '../data.service';
import { PidDataComponent } from '../pid/pid.component';
import { PidStep } from 'app/shared/pid-step';

@Component( {
	templateUrl: './heading.component.html',
	styleUrls: ['./heading.component.css']
} )
export class HeadingDataComponent {
	@ViewChild("pidView") 
	private pidView: PidDataComponent;

	constructor( private dataService: DataService ) {
	}

	ngAfterViewInit() {
		this.pidView.init( 1500, this.dataService.heading.getData() );
	}
}