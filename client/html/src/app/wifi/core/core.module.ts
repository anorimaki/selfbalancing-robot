import { NgModule } from '@angular/core';
import { HttpClientModule } from '@angular/common/http';

import { CoreModule } from 'app/core/core.module';

import { WifiService } from './wifi.service';

@NgModule( {
	imports: [
		CoreModule,
		HttpClientModule
	],
	declarations: [
	],
	exports: [
		CoreModule
	],
	providers: [
		WifiService
	],
} )
export class WifiCoreModule {}
