import { Component, OnInit } from '@angular/core';
import { PidSettings } from "app/pid-settings";
import { Observable } from "rxjs/Observable";

@Component( {
    selector: 'rb-pid',
    templateUrl: './pid.component.html',
    styleUrls: ['./pid.component.css']
} )
export class PidComponent implements OnInit {
    private current: PidSettings;
    private newVal: PidSettings;
    private load: () => Observable<PidSettings>;
    private save: (settings: PidSettings) => Observable<void>;
    
    constructor() { 
        this.current = new PidSettings();
        this.newVal = new PidSettings();
    }
    
    ngOnInit() {
        this.current = new PidSettings();
        this.newVal = new PidSettings();
    }
    
    init( load: () => Observable<PidSettings>, save: (settings: PidSettings) => Observable<void> ): void {
        this.load = load;
        this.save = save;
        
        load().subscribe( settings => {
                this.current = settings;
                this.newVal = new PidSettings(); 
            });
    } 
}
