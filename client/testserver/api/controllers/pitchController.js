'use strict';


function random(low, high) {
    return Math.random() * (high - low) + low;
}

var index = 0;
var toSend = [];

function generate() {
	toSend.push( { 
		index: index++,
		previous_error: random( -57, 57 ),
		integral_error: random( -1000000, 1000000 ),
		target: random( -57, 57 ),
		current: random( -57, 57 )
	} ); 
}


exports.init = function() {
	setInterval( generate, 100 );
}

exports.read_state = function(req, res) {
	res.json( toSend );
	toSend = [];
};