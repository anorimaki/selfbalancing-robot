import { NgModule } from '@angular/core';

import { WifiRoutingModule } from './wifi-routing.module'
import { WifiComponent } from './wifi.component';


@NgModule( {
	imports: [
		WifiRoutingModule
	],
	declarations: [
		WifiComponent
	]
} )
export class WifiModule { }
