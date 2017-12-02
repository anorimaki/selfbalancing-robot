'use strict';

module.exports = function(app, urlPath) {
	const PidController = require('../controllers/PidController');
	const controller = new PidController(10);

	app.route('/' + urlPath + '/state').
		get((req, res) => controller.readState(req, res));
	app.route('/' + urlPath + '/settings').
		get((req, res) => controller.readSettings(req, res)).
		put((req, res) => controller.writeSettings(req, res));
};
