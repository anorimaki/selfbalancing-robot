import { Component, OnInit } from '@angular/core';
import { RobotService } from '../robot.service';
import { PitchState } from "app/pitch-state";
import { Observable } from 'rxjs/Observable';
import DataSource from 'devextreme/data/data_source';
import ArrayStore from 'devextreme/data/array_store';
import 'rxjs/add/operator/expand';
import 'rxjs/add/operator/concatMap';
import 'rxjs/add/observable/timer';

@Component( {
    selector: 'app-robot',
    templateUrl: './robot.component.html',
    styleUrls: ['./robot.component.css']
} )
export class RobotComponent implements OnInit {
    pitchStateStore: ArrayStore;
    pitchStateDataSource: DataSource;

    constructor( private robotService: RobotService ) { }

    ngOnInit() {
        this.pitchStateStore = new ArrayStore([]);
        this.pitchStateDataSource = new DataSource(this.pitchStateStore);
        
        let pollData = this.robotService.pitchState().map( pitch => {
                this.pitchStateStore.insert( pitch ).done( (values, keys) => {
                    this.pitchStateDataSource.load();
                } ); 
            })
        
        pollData.expand( () => Observable.timer(2000).concatMap( () => pollData ) ).subscribe();
    }
}
