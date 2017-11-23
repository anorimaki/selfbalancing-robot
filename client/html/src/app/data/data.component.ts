import { Component, OnInit, OnDestroy } from '@angular/core';


import { NotificationService } from "app/core/notification.service";
import { DataService } from "./data.service";


@Component( {
    templateUrl: './data.component.html',
    styleUrls: ['./data.component.css']
} )
export class RbDataComponent implements OnInit, OnDestroy {
	constructor( private dataService: DataService  ) {}

	ngOnInit() {
		this.dataService.startPolling();
	}

	ngOnDestroy() {
		this.dataService.stopPolling();
	}
}