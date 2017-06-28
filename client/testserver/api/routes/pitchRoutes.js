'use strict';

module.exports = function(app) {
  var pitch = require('../controllers/pitchController');

  app.route('/motors/pitch/state').
    	get(pitch.read_state);
  app.route('/motors/pitch/pid').
		get(pitch.read_pid).
		put(pitch.write_pid);
};
