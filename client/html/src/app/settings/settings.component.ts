import { Component, ViewChild, OnInit } from '@angular/core';
import { RbPidComponent } from './pid/pid.component';
import { NotificationService } from "app/core/notification.service";
import { SettingsService } from './settings.service';

@Component( {
    selector: 'rb-settings',
    templateUrl: './settings.component.html',
    styleUrls: ['./settings.component.css']
} )
export class RbSettingsComponent implements OnInit {
	@ViewChild("pitchPid") 
	private pitchPid: RbPidComponent;
	
	@ViewChild("speedPid") 
	private speedPid: RbPidComponent;

	constructor( private notificationService: NotificationService,
				private settingsService: SettingsService ) {}

	ngOnInit() {
        this.readPidSettings();
    }

	private applyAvailable(): boolean {
		//return this.pitchPid.applyAvailable() || this.speedPid.applyAvailable();
		return this.pitchPid.applyAvailable();
	}

	private readPidSettings(): void {
        this.settingsService.pitchPid.subscribe( 
			settings => {
				this.pitchPid.set( settings );
			},
			err => {
				this.notificationService.error( "Error reading PID settings" );
			});
	}
	
	private apply(): void {
		this.settingsService.setPitchPid( this.pitchPid.get() ).subscribe( 
            () => {
				this.notificationService.success( "PID settings saved" );
            },
            err => {
                this.notificationService.error( "Error saving PID settings" );
            });
	}
}