'use strict';


function random(low, high) {
    return Math.random() * (high - low) + low;
}

var index = 0;
var toSend = [];

function generate() {
	if ( toSend.length == 200 ) {
		toSend.shift(); 
	}
	toSend.push( { 
		i: index++,
		p_err: random( -57, 57 ),
		i_err: random( 50000, 100000 ),
		tar: random( -57, 57 ),
		cur: random( -5, 5 )
	} );
}


exports.init = function() {
	setInterval( generate, 10 );
}

var date = 0;

exports.read_state = function(req, res) {
	var current = Date.now();
	console.log( "Elapsed " + (current-date) + ". size: " + toSend.length );
	date = current;
	
	res.json( toSend );
	toSend = [];
};


var pid = {
	integral: 1.333,
	proportional: 2.333,
	derivative: 2
}

exports.read_pid = function(req, res) {
	res.json( pid );
};

exports.write_pid = function(req, res) {
	pid = req.body
};