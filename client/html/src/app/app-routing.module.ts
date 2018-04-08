import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';


const routes: Routes = [
	{ path: 'wifi', loadChildren: 'app/wifi/wifi.module#WifiModule' },
	{ path: 'robot', loadChildren: './robot/robot.module#RobotModule' },
	{ path: '', redirectTo: '/wifi', pathMatch: 'full' },
];

@NgModule({
	imports: [
		RouterModule.forRoot(routes)
	],
	exports: [RouterModule],
	providers: []
})
export class AppRoutingModule { }
