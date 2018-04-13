import { Component, OnInit, OnDestroy } from '@angular/core';
import { MatSlideToggleChange } from '@angular/material/material';
import { MatCheckboxChange } from '@angular/material/material';
import { MatDialog } from '@angular/material';
import { Observable } from 'rxjs/Observable';
import { Subscription } from 'rxjs/Subscription';
import 'rxjs/add/operator/mergeMap';
import 'rxjs/add/observable/timer';

import { NotificationService } from 'app/core/notification.service';
import { WifiService } from 'wifi/core/wifi.service';
import { NetworkSettingsDialogComponent } from 'wifi/network/network-settings-dialog.component';
import { BssSettingsDialogComponent } from 'wifi/bss/bss-settings-dialog.component';
import { StationConfig } from 'wifi/core/station-config';

@Component({
	selector: 'wifi-station',
	templateUrl: './station.component.html',
	styleUrls: ['./station.component.css']
})
export class WifiStationComponent implements OnInit, OnDestroy {
	static STATUS_CONNECTED = 'Got IP';

	public config: StationConfig;

	private refreshSubscription: Subscription;

	constructor( private wifiService: WifiService,
				private notificationService: NotificationService,
				private dialog: MatDialog ) {
	}

	ngOnInit() {
		this.refreshSubscription = Observable.timer(0, 1000).
			flatMap( () => this.wifiService.station.get() ).
			subscribe(
				info => this.config = info,
				err => this.notificationService.error( 'Error getting station config mode', err ) );
	}

	ngOnDestroy() {
		if ( this.refreshSubscription ) {
			this.refreshSubscription.unsubscribe();
		}
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
					err => this.notificationService.error( 'Error setting network in station mode', err ) );
			}
		});
	}

	onChangeBss(): void {
		const dialogRef = this.dialog.open( BssSettingsDialogComponent, {
			data: this.config.bss
		});
		dialogRef.afterClosed().subscribe(result => {
			if ( result ) {
				this.wifiService.station.setBss( result ).subscribe(
					info => this.config = info,
					err => this.notificationService.error( 'Error setting BSS in station mode', err ) );
			}
		});

	}

	get connected() {
		return this.config.status === WifiStationComponent.STATUS_CONNECTED;
	}
}
