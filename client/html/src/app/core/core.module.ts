import { NgModule } from '@angular/core';
import { MatSnackBarModule } from '@angular/material';

import { NotificationService } from './notification.service';

@NgModule( {
	imports: [
		MatSnackBarModule
	],
	declarations: [
	],
	providers: [
		NotificationService
	],
} )
export class CoreModule { }
