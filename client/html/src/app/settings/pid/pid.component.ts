import { Component, ChangeDetectionStrategy, ChangeDetectorRef } from '@angular/core';
import { PidSettings } from "app/core/pid-settings";
import { Observable } from "rxjs/Observable";

@Component({
	selector: 'rb-pid',
	templateUrl: './pid.component.html',
	changeDetection: ChangeDetectionStrategy.OnPush,
	styleUrls: ['./pid.component.css']
})
export class RbPidComponent {
	private current: PidSettings;
	private newVal: PidSettings;

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
		return (this.newVal.proportional != undefined) ||
			(this.newVal.integral != undefined) ||
			(this.newVal.derivative != undefined);
	}
}
