import { Component, Inject } from '@angular/core';

import { NetConfig } from 'wifi/core/net-config'
import { MAT_DIALOG_DATA } from '@angular/material';

@Component({
	selector: 'wifi-network-settings-dialog',
	templateUrl: './network-settings-dialog.component.html',
	styleUrls: ['./network-settings-dialog.component.css']
})
export class NetworkSettingsDialogComponent {
	settings: NetConfig;

	constructor( @Inject(MAT_DIALOG_DATA) data: NetConfig ) {
		this.settings = Object.assign( {}, data );
	}
}
