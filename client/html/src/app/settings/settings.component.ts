import { Component, ViewChild, OnInit, OnDestroy } from '@angular/core';
import { RbPidComponent } from './pid/pid.component';
import { NotificationService } from "app/core/notification.service";
import { SettingsService, PidSettingsService } from './settings.service';
import { Subscription } from 'rxjs/Subscription';



class PidComponent {
	private setSubscription: Subscription; 
	private getSubscription: Subscription;

	constructor( private view: RbPidComponent,
				private service: PidSettingsService,
				private notificationService: NotificationService ) {
	}

	destroy(): void {
		if ( this.getSubscription ) {
			this.getSubscription.unsubscribe();
		}
		if ( this.setSubscription ) {
			this.setSubscription.unsubscribe();
		}
	}

	toView(): void {
		this.getSubscription = this.service.get().subscribe( 
			settings => {
				this.view.set( settings );
			},
			err => {
				this.notificationService.error( "Error reading PID settings" );
			});
	}

	toService(): void {
		this.setSubscription = this.service.set( this.view.get() ).subscribe( 
            () => {
				this.notificationService.success( "PID settings saved" );
            },
            err => {
                this.notificationService.error( "Error saving PID settings" );
			});
	}
}


@Component( {
    selector: 'rb-settings',
    templateUrl: './settings.component.html',
    styleUrls: ['./settings.component.css']
} )
export class RbSettingsComponent implements OnInit, OnDestroy {
	@ViewChild("pitchPid") 
	private pitchPidView: RbPidComponent;
	
	@ViewChild("speedPid") 
	private speedPidView: RbPidComponent;

	private pitchPid: PidComponent;
	private speedPid: PidComponent;

	constructor( private notificationService: NotificationService,
				private settingsService: SettingsService ) {}

	ngOnInit() {
        this.pitchPid = new PidComponent( this.pitchPidView, 
								this.settingsService.pitchPid,
								this.notificationService );
		this.speedPid = new PidComponent( this.speedPidView, 
									this.settingsService.speedPid,
									this.notificationService );
		this.readPidSettings();
    }

	ngOnDestroy() {
		this.pitchPid.destroy();
		this.speedPid.destroy();
	}

	private applyAvailable(): boolean {
		return this.pitchPidView.applyAvailable() || 
				this.speedPidView.applyAvailable();
	}

	private readPidSettings(): void {
		this.pitchPid.toView();
		this.speedPid.toView();
	}
	
	private apply(): void {
		this.pitchPid.toService();
		this.speedPid.toService();
	}
}