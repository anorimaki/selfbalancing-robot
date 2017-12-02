import { Component, ViewChild, Input, Output, EventEmitter } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { Subscription } from 'rxjs/Subscription';
import { RbChartComponent, RbChartSerie } from 'app/shared/chart/chart.component';
import { DataService } from '../data.service';
import { NotificationService } from "app/core/notification.service";
import { PidStep } from "app/shared/pid-step";

@Component( {
	selector: 'rb-pid-data',
	templateUrl: './pid.component.html',
	styleUrls: ['./pid.component.css']
} )
export class PidDataComponent {
	@ViewChild("chart") 
	private chart: RbChartComponent;

	@Input()
	private variableName: string;

	private dataSubscription: Subscription;
	
	constructor( private dataService: DataService,
				private notificationService: NotificationService ) {
	}

	init( data: Observable<PidStep[]> ): void {
	/*	this.chart.init( 'index', [{ 
				label: this.variableName, 
				field: 'current',
				type: 0 
			}, { 
				label: 'Proportional output', 
				field: 'proportionalOutput',
				type: 1 
			}, { 
				label: 'Derivative output', 
				field: 'derivativeOutput',
				type: 1 
			}, { 
				label: 'Integral output', 
				field: 'integralOutput',
				type: 1 
			}, { 
				label: 'Output', 
				field: 'output',
				type: 2 
			}]); */

		this.dataSubscription = data.subscribe( state  => {
			this.chart.insert( state );
		});
	}

	ngOnDestroy() {
		if ( this.dataSubscription ) {
			this.dataSubscription.unsubscribe();
		}
	}

	private onEnable() {
		this.dataService.startPolling();
	}

	private onDisable() {
		this.dataService.stopPolling();
	}
}