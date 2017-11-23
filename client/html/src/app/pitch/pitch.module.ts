import { NgModule } from '@angular/core';

import { SharedModule } from 'app/shared/shared.module'
import { RbPitchComponent } from './pitch.component';

@NgModule( {
    imports: [
        SharedModule
    ],
    declarations: [
        RbPitchComponent
    ],
    exports: [
       RbPitchComponent
	]
} )
export class PitchModule { }
