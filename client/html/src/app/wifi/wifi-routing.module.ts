import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { WifiComponent } from './wifi.component';


const routes: Routes = [
	{ path: '', component: WifiComponent }
];

@NgModule({
	imports: [RouterModule.forChild(routes)],
	exports: [RouterModule]
})
export class WifiRoutingModule { }
