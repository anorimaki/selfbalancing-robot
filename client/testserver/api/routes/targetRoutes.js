'use strict';

module.exports = function(app, speedController) {
	app.route('/targets').
		put((req, res) => {
			console.log( "s: "+ req.body.speed );
			speedController.target = req.body.speed;
			res.end();
		});
};
