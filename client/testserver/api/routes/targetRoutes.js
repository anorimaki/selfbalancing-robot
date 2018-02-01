'use strict';

module.exports = function(app, speedController) {
	app.route('/targets').
		put((req, res) => {
			speedController.target = req.body.speed;
		});
};
