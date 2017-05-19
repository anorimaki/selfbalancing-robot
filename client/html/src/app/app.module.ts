import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import { DxChartModule, DxRangeSelectorModule } from 'devextreme-angular';

import { AppComponent } from './app.component';
import { RobotComponent } from './robot/robot.component';
import { RobotService } from './robot.service';
import { RbChartComponent } from './robot/chart/chart.component';

@NgModule({
  declarations: [
    AppComponent,
    RobotComponent,
    RbChartComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule,
    DxChartModule,
    DxRangeSelectorModule
  ],
  providers: [RobotService],
  bootstrap: [AppComponent]
})
export class AppModule { }
