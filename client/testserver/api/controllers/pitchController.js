'use strict';


function random(low, high) {
    return Math.random() * (high - low) + low;
}

var index = 0;

exports.read_state = function(req, res) {
	res.json({ 
			index: index,
			previous_error: random( -57, 57 ),
			integral_error: random( -1000000, 1000000 ),
			target: random( -57, 57 ),
			current: random( -57, 57 )
		});
	index = index + 1;
	console.log( 'Called read state' );
};