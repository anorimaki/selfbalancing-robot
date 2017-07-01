import { NgModule, ErrorHandler } from '@angular/core';

import { AppComponent } from './app.component';
import { CoreModule } from './core/core.module'
import { PitchModule } from './pitch/pitch.module';


@NgModule( {
    imports: [
        CoreModule,
        PitchModule
    ],
    declarations: [
        AppComponent
    ],

    bootstrap: [AppComponent]
} )
export class AppModule { }
