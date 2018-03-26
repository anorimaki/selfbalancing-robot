import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { NoopAnimationsModule } from '@angular/platform-browser/animations';
import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';


@NgModule( {
	imports: [
		BrowserModule,
		AppRoutingModule,
		NoopAnimationsModule
	],
	declarations: [
		AppComponent
	],

	bootstrap: [AppComponent]
} )
export class AppModule { }
