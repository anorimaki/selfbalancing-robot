import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import { FlexLayoutModule } from "@angular/flex-layout";

import { NoopAnimationsModule } from '@angular/platform-browser/animations';
import { MatInputModule } from '@angular/material';
import { MatButtonModule } from '@angular/material';
import { MatCheckboxModule } from '@angular/material';
import { MatSnackBarModule } from '@angular/material';
import { MatSlideToggleModule } from '@angular/material';

import { AmChartsModule } from "@amcharts/amcharts3-angular";

import { RbChartComponent } from './chart/chart.component';
import { PidComponent } from './pid/pid.component';



@NgModule( {
    imports: [
        BrowserModule,
        FormsModule,
        HttpModule,
        FlexLayoutModule,
        AmChartsModule,
        NoopAnimationsModule,
        MatInputModule,
        MatButtonModule,
        MatCheckboxModule,
        MatSnackBarModule,
        MatSlideToggleModule
    ],
    declarations: [
        RbChartComponent,
        PidComponent
    ],
    exports: [
        CommonModule,
        FormsModule,
        HttpModule,
        FlexLayoutModule,
        MatInputModule,
        MatButtonModule,
        MatCheckboxModule,
        MatSnackBarModule,
        MatSlideToggleModule,

        RbChartComponent,
        PidComponent
    ]
} )
export class SharedModule { }
