import { Component, OnInit } from '@angular/core';
import { RobotService } from 'robot/core/robot.service';
import { MpuSettings } from 'robot/core/mpu-data';
import { NotificationService } from 'robot/core/notification.service';


@Component({
	selector: 'mpu-settings',
	templateUrl: './settings.component.html',
	styleUrls: ['./settings.component.css']
})
export class MpuSettingsComponent implements OnInit {
	// Accessed from HTML template
	public settings: MpuSettings;

	// Accessed from HTML template
	public newValue: MpuSettings;

	constructor( private robotService: RobotService,
				private notificationService: NotificationService ) {}

	ngOnInit() {
		this.read();
	}

	// Accessed from HTML template
	public apply() {
		this.robotService.mpu.setSettings({
			pitchOffset: this.newValue.pitchOffset * 3.1415 / 180
		}).subscribe(
			() => {
				this.read();
			},
			err => {
				this.notificationService.error( 'Error setting MPU settings' );
			});
	}

	private read() {
		this.robotService.mpu.getSettings().subscribe(
			settings => {
				this.settings = {
					pitchOffset: settings.pitchOffset * 180 / 3.1415
				}
				this.newValue = Object.assign( {}, this.settings );
			},
			err => {
				this.notificationService.error( 'Error reading MPU settings' );
			});
	}
}
