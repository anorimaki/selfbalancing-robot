import { NgModule } from '@angular/core';
import { MatToolbarModule } from '@angular/material/toolbar';
import { MatButtonModule } from '@angular/material/button';

import { ControlModule } from './control/control.module';
import { SettingsModule } from './settings/settings.module';
import { RobotRoutingModule } from './robot-routing.module';
import { RobotCoreModule } from './core/core.module';
import { SharedModule } from './shared/shared.module';

import { RobotComponent } from './robot.component';


@NgModule( {
	imports: [
		MatToolbarModule,
		MatButtonModule,

		RobotCoreModule,
		ControlModule,
		SettingsModule,
		SharedModule,
		RobotRoutingModule
	],
	declarations: [
		RobotComponent
	]
} )
export class RobotModule { }
