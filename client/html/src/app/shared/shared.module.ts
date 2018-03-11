import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import { FlexLayoutModule } from '@angular/flex-layout';

import { NoopAnimationsModule } from '@angular/platform-browser/animations';
import { MatInputModule } from '@angular/material';
import { MatButtonModule } from '@angular/material';
import { MatCheckboxModule } from '@angular/material';
import { MatSnackBarModule } from '@angular/material';
import { MatSlideToggleModule } from '@angular/material';

import { RbChartComponent } from './chart/chart.component';


@NgModule( {
	imports: [
		BrowserModule,
		FormsModule,
		HttpModule,
		FlexLayoutModule,
		NoopAnimationsModule,
		MatInputModule,
		MatButtonModule,
		MatCheckboxModule,
		MatSnackBarModule,
		MatSlideToggleModule
	],
	declarations: [
		RbChartComponent
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

		RbChartComponent
	]
} )
export class SharedModule { }
