import { Injectable } from '@angular/core';
import { MatSnackBar } from '@angular/material';

@Injectable()
export class NotificationService {

	constructor( private snackBar: MatSnackBar ) { }

	error( message: string ) {
		this.snackBar.open( message, null, { duration: 2000, extraClasses: ['notification-error'] } );
	}

	success( message: string ) {
		this.snackBar.open( message, null, { duration: 2000, extraClasses: ['notification-success'] } );
	}
}
