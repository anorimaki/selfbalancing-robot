import { NgModule } from '@angular/core';

import { RobotService } from './robot.service';
import { NotificationService } from './notification.service';

@NgModule( {
	imports: [],
	declarations: [
	],
	providers: [
		RobotService,
		NotificationService
	],
} )
export class CoreModule { }
