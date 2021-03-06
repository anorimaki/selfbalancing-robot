'use strict';

module.exports = function(app, urlPath, controller) {
	app.route('/' + urlPath + '/state').
		get((req, res) => controller.readState(req, res));
	app.route('/' + urlPath + '/settings').
		get((req, res) => controller.readSettings(req, res)).
		put((req, res) => controller.writeSettings(req, res));
};
