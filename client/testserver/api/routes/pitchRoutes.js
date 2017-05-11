'use strict';

module.exports = function(app) {
  var pitch = require('../controllers/pitchController');

  app.route('/motors/pitch/state')
    	.get(pitch.read_state);
};
