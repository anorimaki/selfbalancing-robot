'use strict';

exports.read_state = function(req, res) {
	res.json({ 
			previous_error: 23,
			integral_error: 23,
			target: 23,
			current: 23
		});
	console.log( 'Called read state' );
};