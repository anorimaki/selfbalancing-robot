import { Component, Inject } from '@angular/core';
import { MatDialog, MAT_DIALOG_DATA } from '@angular/material';

import { BssConfig } from 'wifi/core/bss-config'
import { SsidScanDialogComponent } from './ssid-scan-dialog.component'


@Component({
	selector: 'wifi-bss-settings-dialog',
	templateUrl: './bss-settings-dialog.component.html',
	styleUrls: ['./bss-settings-dialog.component.css']
})
export class BssSettingsDialogComponent {
	settings: BssConfig;

	constructor( private dialog: MatDialog, @Inject(MAT_DIALOG_DATA) data: BssConfig ) {
		this.settings = Object.assign( {}, data );
	}

	onScan() {
		const dialogRef = this.dialog.open( SsidScanDialogComponent );
		dialogRef.afterClosed().subscribe( result => {
			if ( result ) {
				this.settings.ssid = result;
			}
		});
	}
}
