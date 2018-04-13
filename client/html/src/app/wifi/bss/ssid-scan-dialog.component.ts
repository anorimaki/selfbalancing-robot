import { Component, OnInit } from '@angular/core';
import { MatDialogRef } from '@angular/material';
import { MatTableDataSource } from '@angular/material';

import { BssInfo } from 'wifi/core/bss-info';
import { NotificationService } from 'app/core/notification.service';
import { WifiService } from 'wifi/core/wifi.service';

@Component({
	selector: 'wifi-ssid-scan-dialog',
	templateUrl: './ssid-scan-dialog.component.html',
	styleUrls: ['./ssid-scan-dialog.component.css']
})
export class SsidScanDialogComponent implements OnInit {
	displayedColumns = ['ssid', 'channel', 'auth', 'signal_strength'];
	dataSource = new MatTableDataSource();
	isLoadingResults = true;
	selectedSsid?: string;

	constructor( private dialogRef: MatDialogRef<SsidScanDialogComponent>,
				private wifiService: WifiService,
				private notificationService: NotificationService ) {
	}

	ngOnInit() {
		this.fetch();
	}

	onSelected( item: BssInfo ) {
	//	this.selectedRow = selectedRow;
		this.selectedSsid = item.ssid;
		this.dialogRef.close( item.ssid );
	}

	private fetch() {
		this.isLoadingResults = true;
		this.wifiService.scan().subscribe(
			bssInfo => {
				this.dataSource.data = bssInfo;
				this.isLoadingResults = false;
			},
			err => {
				this.notificationService.error( 'Error scan networks', err );
				this.isLoadingResults = false;
			}
		);
	}
}
