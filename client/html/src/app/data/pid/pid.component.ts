import { Component, ViewChild, Input, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { RbChartComponent } from 'app/shared/chart/chart.component';
import { PidDataService } from '../data.service';
import { PidStep } from 'app/shared/pid-step';

@Component( {
	selector: 'rb-pid-data',
	templateUrl: './pid.component.html',
	styleUrls: ['./pid.component.css']
} )
export class PidDataComponent implements OnDestroy {
	@Input()
	public variableName: string;

	@ViewChild('chart')
	private chart: RbChartComponent;

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

	// Accessed from HTML template
	public isEnabled(): boolean {
		return this.service && this.service.isPolling;
	}

	// Accessed from HTML template
	public onEnable() {
		this.service.startPolling();
	}

	// Accessed from HTML template
	public onDisable() {
		this.service.stopPolling();
	}
}
