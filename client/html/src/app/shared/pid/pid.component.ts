import { Component, OnInit, Output, EventEmitter } from '@angular/core';
import { PidSettings } from "app/core/pid-settings";
import { Observable } from "rxjs/Observable";

@Component( {
    selector: 'rb-pid',
    templateUrl: './pid.component.html',
    styleUrls: ['./pid.component.css']
} )
export class PidComponent implements OnInit {
    private current: PidSettings;
    private newVal: PidSettings;
    private loadFunction: () => Observable<PidSettings>;
    @Output() saveRequest = new EventEmitter<PidSettings>();
    
    constructor() { 
        this.current = new PidSettings();
        this.newVal = new PidSettings();
    }
    
    ngOnInit() {
        this.current = new PidSettings();
        this.newVal = new PidSettings();
    }
    
    set( s: PidSettings ): void {
       this.current = s;
       this.newVal = new PidSettings();
    }
    
    private sendingAvailable(): boolean {
        return (this.newVal.proportional != null) ||
                    (this.newVal.integral != null) ||
                    (this.newVal.derivative != null) ? true : null;
    }
    
    private save(): void {
        this.newVal.merge( this.current );
        this.saveRequest.emit( this.newVal );
    }
}
