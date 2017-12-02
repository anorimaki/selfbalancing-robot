var express = require('express'),
  app = express(),
  port = process.env.PORT || 3000;

var bodyParser = require('body-parser')

app.use(function(req, res, next) {
	  res.header("Access-Control-Allow-Origin", "*");
	  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
	  res.header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
	  next();
	});

app.use( bodyParser.json() );

var pidRoutes = require('./api/routes/pidRoutes');
pidRoutes(app, 'pitch');
pidRoutes(app, 'speed');

app.listen(port);

console.log('todo list RESTful API server started on: ' + port);