var express = require('express'),
  app = express(),
  port = process.env.PORT || 3000;

var bodyParser = require('body-parser')

app.use(function(req, res, next) {
	  res.header("Access-Control-Allow-Origin", "*");
	  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
	  next();
	});

app.use( bodyParser.json() );

var routes = require('./api/routes/pitchRoutes');
routes(app);

var pitchController = require('./api/controllers/pitchController');
pitchController.init();

app.listen(port);

console.log('todo list RESTful API server started on: ' + port);