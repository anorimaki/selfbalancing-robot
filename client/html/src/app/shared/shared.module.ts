import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import { FlexLayoutModule } from "@angular/flex-layout";

import { NoopAnimationsModule } from '@angular/platform-browser/animations';
import { MdInputModule } from '@angular/material';
import { MdButtonModule } from '@angular/material';
import { MdCheckboxModule } from '@angular/material';
import { MdSnackBarModule } from '@angular/material';
import {MdSlideToggleModule} from '@angular/material';

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
        MdInputModule,
        MdButtonModule,
        MdCheckboxModule,
        MdSnackBarModule,
        MdSlideToggleModule
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
        MdInputModule,
        MdButtonModule,
        MdCheckboxModule,
        MdSnackBarModule,
        MdSlideToggleModule,

        RbChartComponent,
        PidComponent
    ]
} )
export class SharedModule { }
