import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';

import { FlexLayoutModule } from "@angular/flex-layout";

import {NoopAnimationsModule} from '@angular/platform-browser/animations';
import {MdInputModule} from '@angular/material';
import {MdButtonModule} from '@angular/material';
import {MdCheckboxModule} from '@angular/material';

import { AmChartsModule } from "@amcharts/amcharts3-angular";

import { AppComponent } from './app.component';
import { RobotComponent } from './robot/robot.component';
import { RobotService } from './robot.service';
import { RbChartComponent } from './robot/chart/chart.component';
import { PidComponent } from './robot/pid/pid.component';

@NgModule({
  declarations: [
    AppComponent,
    RobotComponent,
    RbChartComponent,
    PidComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule,
    FlexLayoutModule,
    AmChartsModule,
    NoopAnimationsModule,
    MdInputModule,
    MdButtonModule,
    MdCheckboxModule
  ],
  providers: [RobotService],
  bootstrap: [AppComponent]
})
export class AppModule { }
