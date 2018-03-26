import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { HttpClientModule } from '@angular/common/http';
import { FlexLayoutModule } from '@angular/flex-layout';

import { MatInputModule } from '@angular/material';
import { MatButtonModule } from '@angular/material';
import { MatCheckboxModule } from '@angular/material';
import { MatSnackBarModule } from '@angular/material';
import { MatSlideToggleModule } from '@angular/material';

import { RbChartComponent } from './chart/chart.component';


@NgModule( {
	imports: [
		FormsModule,
		CommonModule,
		HttpClientModule,
		FlexLayoutModule,
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
		HttpClientModule,
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
