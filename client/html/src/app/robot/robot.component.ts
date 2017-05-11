import { Component, OnInit } from '@angular/core';
import { RobotService } from '../robot.service';
import { PitchState } from "app/pitch-state";

@Component( {
    selector: 'app-robot',
    templateUrl: './robot.component.html',
    styleUrls: ['./robot.component.css']
} )
export class RobotComponent implements OnInit {

    pitchState: PitchState;

    constructor( private robotService: RobotService ) { }

    ngOnInit() {
        this.robotService.pitchState().subscribe(
            pitch => {
                this.pitchState = pitch;
                console.log( JSON.stringify(pitch) )
            },
            err => console.log( "Something went wrong: ${err.message}" ) );
    }
}
