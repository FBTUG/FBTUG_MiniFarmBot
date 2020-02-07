/*
	FBTUG miniFarmbot	
	Scratch Ext 1.0.0.0
	2018/08/08
*/

var lastDataReceived = null;

var http = require('http');
var fs = require('fs');
var url = require('url');

var PORT = 8900;
var HOST = '192.168.1.155';

var dgram = require('dgram');
var client = dgram.createSocket('udp4');

var command = '';
var dis = '';
var message = '';

http.createServer( function (request, response) {  
	var pathname = url.parse(request.url).pathname;       
    var url_params = request.url.split('/');

    if (url_params.length < 2)
        return;

    command = decodeURI(url_params[1]);
	
	//var message = command;
	//dis = decodeURI(url_params[2]);
	
	//message = new Buffer( command + ' ' + dis );
	//console.log('> ' + message);
	//<Buffer 70 6f 6c 6c 20> = poll 輪詢	
	//Scratch 發出 'GOO Y' '10' => G00 Y 10
	//組合成 G00Y10
	//poll 10

	//client.on('message', function(msg, info){
	//	console.log('Data received from server : ' + msg.toString());
	//	console.log('Received %d bytes from %s:%d\n',msg.length, info.address, info.port);
	//});	
		
	switch (command){
		case 'poll':			   
            respondToPoll(response);	//-輪詢資料- From Scratch 發送來的
            break;
        case 'G00 X':
			dis = (url_params.length >= 3) ? url_params[2] : 0;
			console.log('G00 X' + dis);
			var message = new Buffer( 'G00 X' + dis );
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});
			break;
        case 'G00 Y':
			dis = (url_params.length >= 3) ? url_params[2] : 0;
			console.log('G00 Y' + dis);
			var message = new Buffer( 'G00 Y' + dis );
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;
        case 'G00 Z':
			dis = (url_params.length >= 3) ? url_params[2] : 0;
			console.log('G00 Z' + dis);
			var message = new Buffer( 'G00 Z' + dis );
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});
			break;
		case 'G00':
			dis = (url_params.length >= 3) ? url_params[2] : 0;
			console.log('G00 ' + decodeURI(dis) );
			var message = new Buffer( 'G00 '+ decodeURI(dis) );
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;	
		case 'F41 P8 V1 M0':
			console.log('F41 P8 V1 M0');
			var message = new Buffer('F41 P8 V1 M0');
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;		
		case 'F41 P8 V0 M0':
			console.log('F41 P8 V0 M0');
			var message = new Buffer('F41 P8 V0 M0');
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;	
		case 'F41 P9 V1 M0':
			console.log('F41 P9 V1 M0');
			var message = new Buffer('F41 P9 V1 M0');
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;		
		case 'F41 P9 V0 M0':
			console.log('F41 P9 V0 M0');
			var message = new Buffer('F41 P9 V0 M0');
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;
		case 'F41 P10 V1 M0':
			console.log('F41 P10 V1 M0');
			var message = new Buffer('F41 P10 V1 M0');
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;		
		case 'F41 P10 V0 M0':
			console.log('F41 P10 V0 M0');
			var message = new Buffer('F41 P10 V0 M0');
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;			
		case 'Stop':
			console.log('Stop');
			var message = new Buffer('S');
			client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
				if (err) throw err;
			});			
			break;							
	}
	
	response.end('Hello FarmBot.\n');
   
}).listen(9000);


console.log('Scratch for miniFarmbot Ext running at http://192.168.1.155:8900/');

function respondToPoll(response){
    var noDataReceived = false;
    var resp = "";

    response.end(resp);
}