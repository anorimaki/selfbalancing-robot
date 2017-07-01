import { Component, OnInit, ViewChild } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/operator/expand';
import 'rxjs/add/operator/do'
import 'rxjs/add/operator/retry';
import 'rxjs/add/operator/concatMap';
import 'rxjs/add/observable/empty';
import 'rxjs/add/observable/timer';

import { RobotService } from 'app/core/robot.service';
import { PitchState } from "app/core/pitch-state";
import { PidSettings } from "app/core/pid-settings";
import { RbChartComponent } from 'app/shared/chart/chart.component';
import { PidComponent } from 'app/shared/pid/pid.component';
import { NotificationService } from "app/core/notification.service";


@Component( {
    selector: 'rb-pitch',
    templateUrl: './pitch.component.html',
    styleUrls: ['./pitch.component.css']
} )
export class RbPitchComponent implements OnInit {
    @ViewChild("pitchChar") 
    private pitchChar: RbChartComponent;
    
    @ViewChild("pitchPid") 
    private pitchPid: PidComponent;
    
    private pollData: Observable<PitchState[]>;
    
    constructor( private robotService: RobotService,
            private notificationService: NotificationService ) { }

    
    ngOnInit() {
        this.schedulePollPitch();
        this.readPidSettings();
    }
    
    
    private schedulePollPitch(): void  {
        this.pollData = this.robotService.getPitchState().map( pitches => {
                return pitches.map( item => {
                        let scaledPitch = new PitchState( <PitchState>item );
                        scaledPitch.integral_error /= 5000; 
                        return scaledPitch;
                    } );
            }).
            catch( (err, caught) => {
                this.notificationService.error( "Error getting PID pitch data" );
                return caught;
            });
        this.pollingEnable( true );
    }
    
    private readPidSettings(): void {
        this.robotService.getPitchPidSettings().subscribe( 
                settings => {
                    this.pitchPid.set( settings );
                },
                err => {
                    this.notificationService.error( "Error reading PID settings" );
                });
    }
    
    private pollingEnable( enable: boolean ) {
        if ( enable ) {
            this.pollData.expand( () =>
                this.pitchChar.enabled ? 
                    Observable.timer(200).concatMap( () => this.pollData ) :
                    Observable.empty() ).
            subscribe( pitches => {
                this.pitchChar.insert( pitches );
            });
        }
    }
    
    private saveSettings( settings: PidSettings ) {
        this.robotService.setPitchPidSettings(settings).subscribe( 
            () => {
                this.pitchPid.set( settings );
                this.notificationService.success( "PID settings saved" );
            },
            err => {
                this.notificationService.error( "Error saving PID settings" );
            });
    }
}
