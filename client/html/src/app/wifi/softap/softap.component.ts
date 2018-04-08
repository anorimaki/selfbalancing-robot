import { Component, OnInit } from '@angular/core';
import { MatSlideToggleChange } from '@angular/material/material';

import { NotificationService } from 'app/core/notification.service';
import { WifiService } from 'wifi/core/wifi.service';
import { SoftApConfig } from 'wifi/core/softap-config';


@Component({
	selector: 'wifi-softap',
	templateUrl: './softap.component.html',
	styleUrls: ['./softap.component.css']
})
export class WifiSoftApComponent implements OnInit {
	public config: SoftApConfig;

	constructor( private wifiService: WifiService,
				private notificationService: NotificationService ) {
	}

	ngOnInit() {
		this.fetchInfo();
	}

	onEnabledChanged( event: MatSlideToggleChange ): void {
		this.wifiService.softAp.setEnabled( event.checked ).subscribe(
			info => this.setInfo( info ),
			err => this.notificationService.error( 'Error enabling SoftAP', err ) );

	}

	private fetchInfo() {
		this.wifiService.softAp.get().subscribe(
			info => this.setInfo( info ),
			err => this.notificationService.error( 'Error getting SoftAP config', err ) );
	}

	private setInfo( info: SoftApConfig ) {
		this.config = info;
	}
}
