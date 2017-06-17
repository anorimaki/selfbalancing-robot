import { Component, OnInit, ViewChild } from '@angular/core';
import { RobotService } from '../robot.service';
import { RbChartComponent } from './chart/chart.component';
import { PitchState } from "app/pitch-state";
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/operator/expand';
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
    
    constructor( private robotService: RobotService ) { }

    ngOnInit() {
        let pollData = this.robotService.pitchState().map( pitches => {
                return pitches.map( item => {
                        let scaledPitch = new PitchState( <PitchState>item );
                        scaledPitch.integral_error /= 5000; 
                        return scaledPitch;
                    } );
            }).
            retry();
        
        pollData.expand( () => Observable.timer(50).concatMap( () => pollData ) ).subscribe(
                    pitches => {
                        this.pitchChar.insert( pitches );
                    }
                );
    }
}
