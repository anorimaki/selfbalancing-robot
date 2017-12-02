import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { MatButtonModule } from '@angular/material/button';

import { SharedModule } from 'app/shared/shared.module'
import { RbDataComponent } from './data.component';
import { GeneralDataComponent } from './general/general.component';
import { PitchDataComponent } from './pitch/pitch.component';
import { SpeedDataComponent } from './speed/speed.component';
import { PidDataComponent } from './pid/pid.component';
import { DataService } from './data.service';

const routes: Routes = [{
	path: 'data', component: RbDataComponent,
	children: [
		{ path: '', redirectTo: './general', pathMatch: 'full' },
		{ path: 'general', component: GeneralDataComponent },
		{ path: 'pitch', component: PitchDataComponent },
		{ path: 'speed', component: SpeedDataComponent }
	]
}];

@NgModule({
	imports: [
		MatButtonModule,
		SharedModule,
		RouterModule.forChild(routes),
	],
	declarations: [
		RbDataComponent,
		GeneralDataComponent,
		PitchDataComponent,
		SpeedDataComponent,
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

