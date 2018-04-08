import { NgModule } from '@angular/core';

import { RobotService } from './robot.service';
import { CoreModule } from 'app/core/core.module';

@NgModule( {
	imports: [
		CoreModule
	],
	declarations: [
	],
	exports: [
		CoreModule
	],
	providers: [
		RobotService,
	],
} )
export class RobotCoreModule { }
