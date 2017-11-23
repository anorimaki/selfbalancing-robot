import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { FlexLayoutModule } from "@angular/flex-layout";
import { MatInputModule } from '@angular/material';
import { MatButtonModule } from '@angular/material';

import { RbSettingsComponent } from './settings.component';
import { SettingsService } from './settings.service';
import { RbPidComponent } from './pid/pid.component';



@NgModule( {
	imports: [
        FormsModule,
        MatInputModule,
		MatButtonModule,
		FlexLayoutModule
    ],
    declarations: [
		RbSettingsComponent,
		RbPidComponent
    ],
    exports: [
        RbSettingsComponent
	],
	providers: [
        SettingsService
    ]
} )
export class SettingsModule { }
