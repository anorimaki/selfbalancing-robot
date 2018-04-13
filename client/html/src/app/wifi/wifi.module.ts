import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { MatToolbarModule } from '@angular/material/toolbar';
import { MatSlideToggleModule } from '@angular/material';
import { MatInputModule } from '@angular/material';
import { MatTabsModule } from '@angular/material/tabs';
import { MatDividerModule } from '@angular/material/divider';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatDialogModule } from '@angular/material/dialog';
import { MatTableModule } from '@angular/material/table';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { FlexLayoutModule } from '@angular/flex-layout';

import { WifiRoutingModule } from './wifi-routing.module'
import { WifiComponent } from './wifi.component';
import { WifiStationComponent } from './station/station.component';
import { WifiSoftApComponent } from './softap/softap.component';
import { WifiCoreModule } from './core/core.module';
import { NetworkSettingsDialogComponent } from './network/network-settings-dialog.component';
import { BssSettingsDialogComponent } from './bss/bss-settings-dialog.component';
import { SsidScanDialogComponent } from './bss/ssid-scan-dialog.component';


@NgModule( {
	imports: [
		WifiRoutingModule,
		WifiCoreModule,
		CommonModule,
		FormsModule,
		MatInputModule,
		MatSlideToggleModule,
		MatToolbarModule,
		MatTabsModule,
		MatCheckboxModule,
		MatDividerModule,
		MatDialogModule,
		MatTableModule,
		MatProgressSpinnerModule,
		FlexLayoutModule,
	],
	declarations: [
		WifiComponent,
		WifiStationComponent,
		WifiSoftApComponent,
		NetworkSettingsDialogComponent,
		BssSettingsDialogComponent,
		SsidScanDialogComponent
	],
	entryComponents: [
		NetworkSettingsDialogComponent,
		BssSettingsDialogComponent,
		SsidScanDialogComponent
	]
} )
export class WifiModule { }
