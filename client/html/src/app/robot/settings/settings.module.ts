import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { FlexLayoutModule } from '@angular/flex-layout';
import { MatInputModule, MatListModule, MatButtonModule } from '@angular/material';

import { RbSettingsComponent } from './settings.component';
import { SettingsService } from './settings.service';
import { RbPidComponent } from './pid/pid.component';
import { MpuCalibrationComponent } from './mpu/calibration.component';
import { MpuSettingsComponent } from './mpu/settings.component';


@NgModule( {
	imports: [
		FormsModule,
		MatInputModule,
		MatButtonModule,
		MatListModule,
		FlexLayoutModule,
		CommonModule
	],
	declarations: [
		RbSettingsComponent,
		RbPidComponent,
		MpuSettingsComponent,
		MpuCalibrationComponent
	],
	exports: [
		RbSettingsComponent
	],
	providers: [
		SettingsService
	]
} )
export class SettingsModule {}
