import { Component, ViewChild, AfterViewInit } from '@angular/core';
import { DataService } from '../data.service';
import { PidDataComponent } from '../pid/pid.component';

@Component( {
	templateUrl: './heading.component.html',
	styleUrls: ['./heading.component.css']
} )
export class HeadingDataComponent implements AfterViewInit {
	@ViewChild('pidView')
	private pidView: PidDataComponent;

	constructor( private dataService: DataService ) {
	}

	ngAfterViewInit() {
		// Wait a tick because the component's view has already been checked
		setTimeout( () => this.pidView.init( 100, this.dataService.heading ), 0 );
	}
}
