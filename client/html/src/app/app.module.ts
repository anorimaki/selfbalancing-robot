import { NgModule, ErrorHandler } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { MatToolbarModule } from '@angular/material/toolbar';
import { MatButtonModule } from '@angular/material/button';
import { AppComponent } from './app.component';
import { CoreModule } from './core/core.module';
import { ControlModule } from './control/control.module';
import { RbControlComponent } from './control/control.component';
import { DataModule } from './data/data.module';
import { RbDataComponent } from './data/data.component';
import { SettingsModule } from './settings/settings.module';
import { RbSettingsComponent } from './settings/settings.component';
import { SharedModule } from './shared/shared.module';


const routes: Routes = [
    { path: '', redirectTo: '/control', pathMatch: 'full' },
    { path: 'control', component: RbControlComponent },
    { path: 'data', component: RbDataComponent },
    { path: 'settings', component: RbSettingsComponent }
];

@NgModule( {
    imports: [
        CoreModule,
        MatToolbarModule,
        MatButtonModule,
        ControlModule,
        DataModule,
        SettingsModule,
        SharedModule,
        RouterModule.forRoot(routes),
    ],
    exports: [
        RouterModule 
    ],
    declarations: [
        AppComponent
    ],

    bootstrap: [AppComponent]
} )
export class AppModule { }
