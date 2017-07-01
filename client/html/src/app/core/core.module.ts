import { NgModule, ErrorHandler } from '@angular/core';

import { GlobalErrorHandler } from './global-error-handler';
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
