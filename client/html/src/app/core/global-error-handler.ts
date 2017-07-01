import { ErrorHandler, Injectable, Injector } from '@angular/core';
import { MdSnackBar } from '@angular/material';

@Injectable()
export class GlobalErrorHandler implements ErrorHandler {

    constructor( private injector: Injector ) { }

    handleError( error ) {
        const snackBar = this.injector.get( MdSnackBar );
        const message = error.message ? error.message : error.toString();
        
        snackBar.open( message, null, { duration: 2000 } );
        
        // IMPORTANT: Rethrow the error otherwise it gets swallowed
        throw error;
    }

}