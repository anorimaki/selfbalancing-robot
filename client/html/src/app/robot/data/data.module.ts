import { NgModule } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { RouterModule } from '@angular/router';

import { SharedModule } from 'robot/shared/shared.module'
import { DataRoutingModule } from './data-routing.module'
import { RbDataComponent } from './data.component';
import { GeneralDataComponent } from './general/general.component';
import { PitchDataComponent } from './pitch/pitch.component';
import { SpeedDataComponent } from './speed/speed.component';
import { PidDataComponent } from './pid/pid.component';
import { HeadingDataComponent } from './heading/heading.component';
import { DataService } from './data.service';



@NgModule({
	imports: [
		MatButtonModule,
		SharedModule,
		RouterModule,
		DataRoutingModule
	],
	declarations: [
		RbDataComponent,
		GeneralDataComponent,
		PitchDataComponent,
		SpeedDataComponent,
		HeadingDataComponent,
		PidDataComponent
	],
	exports: [
		RbDataComponent,
	],
	providers: [
		DataService
	]
})
export class DataModule { }

