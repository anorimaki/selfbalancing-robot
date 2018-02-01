'use strict';

function random(low, high) {
    return Math.random() * (high - low) + low;
}

class PidController {
	constructor( period ) {
		this.index = 0;
		this.target = 0;
		this.toSend = [];
		this.settings = {
			integral: 1.333,
			proportional: 2.333,
			derivative: 2
		};
		this.date = 0;

		setInterval( () => this.generate(), period );
	}

	readSettings(req, res) {
		res.json( this.settings );
	};
	
	writeSettings(req, res) {
		this.settings = req.body;
		res.end();
		console.log( "write: " + JSON.stringify(req.body) ); 
	};

	readState(req, res) {
		var current = Date.now();
		console.log( "Elapsed " + (current-this.date) + ". size: " + this.toSend.length );
		this.date = current;
		
		res.json( this.toSend );
		this.toSend = [];
	};

	generate() {
		if ( this.toSend.length == 200 ) {
			this.toSend.shift(); 
		}
		this.toSend.push( { 
			i: this.index++,
			p_err: random( -57, 57 ),
			i_err: random( 50000, 100000 ),
			tar: this.target,
			cur: random( -5, 5 )
		} );
	}
}

module.exports = PidController;