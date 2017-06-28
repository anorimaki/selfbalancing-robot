import { Component, OnInit, ViewChild } from '@angular/core';
import { RobotService } from '../robot.service';
import { RbChartComponent } from './chart/chart.component';
import { PidComponent } from './pid/pid.component';
import { PitchState } from "app/pitch-state";
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/operator/expand';
import 'rxjs/add/operator/do'
import 'rxjs/add/operator/retry';
import 'rxjs/add/operator/concatMap';
import 'rxjs/add/observable/timer';

@Component( {
    selector: 'app-robot',
    templateUrl: './robot.component.html',
    styleUrls: ['./robot.component.css']
} )
export class RobotComponent implements OnInit {
    @ViewChild("pitchChar") 
    private pitchChar: RbChartComponent;
    
    @ViewChild("pitchPid") 
    private pitchPid: PidComponent;
    
    private errorMessage: string;
    
    constructor( private robotService: RobotService ) { }

    ngOnInit() {
        this.schedulePollPitch();
        this.readPidSettings();
    }
    
    
    private schedulePollPitch(): void  {
        let pollData = this.robotService.getPitchState().map( pitches => {
            return pitches.map( item => {
                    let scaledPitch = new PitchState( <PitchState>item );
                    scaledPitch.integral_error /= 5000; 
                    return scaledPitch;
                } );
        }).
        do( () => { this.errorMessage = null; },
            err => { this.errorMessage = err; }
        ).
        retry();
    
        pollData.expand( () => Observable.timer(30000).concatMap( () => pollData ) ).subscribe(
                pitches => {
                    this.pitchChar.insert( pitches );
                }
            );
    }
    
    private readPidSettings(): void {
        this.pitchPid.init( () => this.robotService.getPitchPidSettings(), 
                           (settings) => this.robotService.setPitchPidSettings(settings) );
   }
}
