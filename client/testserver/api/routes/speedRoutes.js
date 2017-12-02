'use strict';

module.exports = function (app) {
	const PidController = require('../controllers/PidController');
	const controller = new PidController(100);

	app.route('/speed/state').
		get((req, res) => controller.readState(req, res));
	app.route('/speed/pid').
		get((req, res) => controller.readSettings(req, res)).
		put((req, res) => controller.writeSettings(req, res));
};
