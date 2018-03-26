import { Component, OnInit } from '@angular/core';
import { MpuCalibrationData } from 'robot/core/mpu-data';
import { NotificationService } from 'robot/core/notification.service';
import { RobotService } from 'robot/core/robot.service';

@Component({
	selector: 'mpu-calibration',
	templateUrl: './calibration.component.html',
	styleUrls: ['./calibration.component.css']
})
export class MpuCalibrationComponent implements OnInit {
	// Accessed from HTML template
	public data: MpuCalibrationData;

	constructor( private robotService: RobotService,
				private notificationService: NotificationService ) {}

	ngOnInit() {
		this.robotService.mpu.getCalibration().subscribe(
			calibrationData => {
				this.data = calibrationData;
			},
			err => {
				this.notificationService.error( 'Error reading MPU calibration' );
			});
	}

	doCalibration() {
		this.robotService.mpu.doCalibration().subscribe(
			calibrationData => {
				this.data = calibrationData;
			},
			err => {
				this.notificationService.error( 'Error doing MPU calibration' );
			});
	}
}
