const PidController = require('./api/controllers/PidController');
const pidRoutes = require('./api/routes/pidRoutes');
const targetRoutes = require('./api/routes/targetRoutes');
var express = require('express'),
  app = express(),
  port = process.env.PORT || 3000;
const bodyParser = require('body-parser')

speedController = new PidController(10);
pitchController = new PidController(10);


app.use(function(req, res, next) {
	  res.header("Access-Control-Allow-Origin", "*");
	  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
	  res.header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
	  next();
	});

app.use( bodyParser.json() );

pidRoutes(app, 'pitch', pitchController);
pidRoutes(app, 'speed', speedController);
targetRoutes(app, speedController);

app.listen(port);

console.log('RESTful API server started on: ' + port);