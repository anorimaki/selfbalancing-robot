import { Component, ChangeDetectionStrategy, ChangeDetectorRef } from '@angular/core';
import { PidSettings } from 'robot/core/pid-settings';

@Component({
	selector: 'rb-pid',
	templateUrl: './pid.component.html',
	changeDetection: ChangeDetectionStrategy.OnPush,
	styleUrls: ['./pid.component.css']
})
export class RbPidComponent {
	// Accessed from HTML template
	public  current: PidSettings;

	// Accessed from HTML template
	public newVal: PidSettings;

	constructor(private ref: ChangeDetectorRef) {
		this.current = new PidSettings();
		this.newVal = new PidSettings();
	}

	set(s: PidSettings) {
		this.current = s;
		this.ref.detectChanges();
	}

	get(): PidSettings {
		this.current.merge( this.newVal );
		this.newVal = new PidSettings();
		this.ref.detectChanges();
		return this.current;
	}

	applyAvailable(): boolean {
		return (this.newVal.proportional !== undefined) ||
			(this.newVal.integral !== undefined) ||
			(this.newVal.derivative !== undefined);
	}
}
