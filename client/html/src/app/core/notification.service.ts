import { Injectable } from '@angular/core';
import { MatSnackBar } from '@angular/material';

@Injectable()
export class NotificationService {
	constructor( private snackBar: MatSnackBar ) { }

	error( message: string, ex?: Error ) {
		this.snackBar.open( message, null, { duration: 2000, panelClass: ['notification-error'] } );
		if ( ex !== undefined ) {
			console.log(ex);
		}
	}

	success( message: string ) {
		this.snackBar.open( message, null, { duration: 2000, panelClass: ['notification-success'] } );
	}
}
