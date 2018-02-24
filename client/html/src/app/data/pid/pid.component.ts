import { Component, ViewChild, Input, Output, EventEmitter } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { Subscription } from 'rxjs/Subscription';
import { RbChartComponent, RbChartSerie } from 'app/shared/chart/chart.component';
import { PidDataService } from '../data.service';
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

	private service: PidDataService<PidStep>;
	private dataSubscription: Subscription;

	init( dataSize: number, service: PidDataService<PidStep> ): void {
		this.service = service;
		this.chart.dataSize = dataSize;
		this.dataSubscription = service.getData().subscribe( state  => {
			this.chart.insert( state );
		});
	}

	ngOnDestroy() {
		if ( this.dataSubscription ) {
			this.dataSubscription.unsubscribe();
		}
	}

	private isEnabled(): boolean {
		return this.service && this.service.isPolling;
	}

	private onEnable() {
		this.service.startPolling();
	}

	private onDisable() {
		this.service.stopPolling();
	}
}