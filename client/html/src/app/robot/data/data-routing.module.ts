import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { RbDataComponent } from './data.component';
import { GeneralDataComponent } from './general/general.component';
import { PitchDataComponent } from './pitch/pitch.component';
import { SpeedDataComponent } from './speed/speed.component';
import { HeadingDataComponent } from './heading/heading.component';

const routes: Routes = [{
	path: '', component: RbDataComponent,
	children: [
		{ path: 'general', component: GeneralDataComponent },
		{ path: 'pitch', component: PitchDataComponent },
		{ path: 'speed', component: SpeedDataComponent },
		{ path: 'heading', component: HeadingDataComponent }
	]
}];

@NgModule({
	imports: [RouterModule.forChild(routes)],
	exports: [RouterModule]
})
export class DataRoutingModule { }
