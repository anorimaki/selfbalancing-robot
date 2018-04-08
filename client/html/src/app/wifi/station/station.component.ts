import { Component, OnInit } from '@angular/core';
import { MatSlideToggleChange } from '@angular/material/material';
import { MatCheckboxChange } from '@angular/material/material';
import { MatDialog } from '@angular/material';

import { NotificationService } from 'app/core/notification.service';
import { WifiService } from 'wifi/core/wifi.service';
import { NetworkSettingsDialogComponent } from 'wifi/network/network-settings-dialog.component';
import { StationConfig } from 'wifi/core/station-config';

@Component({
	selector: 'wifi-station',
	templateUrl: './station.component.html',
	styleUrls: ['./station.component.css']
})
export class WifiStationComponent implements OnInit {
	static STATUS_CONNECTED = 'Got IP';

	public config: StationConfig;

	constructor( private wifiService: WifiService,
				private notificationService: NotificationService,
				private dialog: MatDialog ) {
	}

	ngOnInit() {
		this.fetchInfo();
	}

	onEnabledChanged( event: MatSlideToggleChange ): void {
		this.wifiService.station.setEnabled( event.checked ).subscribe(
			info => this.config = info,
			err => this.notificationService.error( 'Error enabling/disabling station mode', err ) );

	}

	onDhcpChanged( event: MatCheckboxChange ): void {
		this.wifiService.station.setDhcp( event.checked ).subscribe(
			info => this.config = info,
			err => this.notificationService.error( 'Error setting DHCP', err ) );

	}

	onChangeNetwork(): void {
		const dialogRef = this.dialog.open( NetworkSettingsDialogComponent, {
				data: this.config.network
			});
		dialogRef.afterClosed().subscribe(result => {
			if ( result ) {
				this.wifiService.station.setNetwork( result ).subscribe(
					info => this.config = info,
					err => this.notificationService.error( 'Error settings network in station mode', err ) );
			}
		});
	}

	get connected() {
		return this.config.status === WifiStationComponent.STATUS_CONNECTED;
	}

	private fetchInfo() {
		this.wifiService.station.get().subscribe(
			info => this.config = info,
			err => this.notificationService.error( 'Error getting station config mode', err ) );
	}
}
