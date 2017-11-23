import { Component, ViewChild } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { RbChartComponent } from 'app/shared/chart/chart.component';
import { DataService } from '../data.service';
import { NotificationService } from "app/core/notification.service";

@Component( {
	templateUrl: './pitch.component.html',
	styleUrls: ['./pitch.component.css']
} )
export class PitchDataComponent {
	@ViewChild("pitchChar") 
	private pitchChar: RbChartComponent;

	private dataSubscription: Subscription;
	
	constructor( private dataService: DataService,
				private notificationService: NotificationService ) {
	}

	ngAfterViewInit() {
		this.dataSubscription = this.dataService.getData().subscribe( pitches  => {
			this.pitchChar.insert( pitches );
		});
	}

	ngOnDestroy() {
		this.dataSubscription.unsubscribe();
	}

	private pollingEnable( enable: boolean ) {
		if ( enable ) {
			this.dataService.startPolling();
		}
		else {
			this.dataService.stopPolling();
		}
	}
}