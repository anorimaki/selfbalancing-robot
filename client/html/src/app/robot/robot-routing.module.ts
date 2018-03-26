import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { RobotComponent } from './robot.component';
import { RbControlComponent } from './control/control.component';
import { RbSettingsComponent } from './settings/settings.component';


const routes: Routes = [
	{
		path: '', component: RobotComponent,
		children: [
			{ path: 'control', component: RbControlComponent },
			{ path: 'data', loadChildren: './data/data.module#DataModule' },
			{ path: 'settings', component: RbSettingsComponent }
		]
	}
];

@NgModule({
	imports: [RouterModule.forChild(routes)],
	exports: [RouterModule]
})
export class RobotRoutingModule { }
