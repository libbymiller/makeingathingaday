//https://stackoverflow.com/questions/16333790/node-js-quick-file-server-static-files-over-http
var http = require('http');
var fs = require('fs');
var path = require('path');


var app = http.createServer(function (request, response) {
    console.log('request starting...');

    var filePath = '.' + request.url;
    if (filePath == './')
        filePath = './index.html';

    var extname = path.extname(filePath);
    var contentType = 'text/html';
    switch (extname) {
        case '.js':
            contentType = 'text/javascript';
            break;
        case '.css':
            contentType = 'text/css';
            break;
        case '.json':
            contentType = 'application/json';
            break;
        case '.png':
            contentType = 'image/png';
            break;      
        case '.jpg':
            contentType = 'image/jpg';
            break;
        case '.mp3':
            contentType = 'audio/mp3';
            break;
        case '.wav':
            contentType = 'audio/wav';
            break;
    }

    fs.readFile(filePath, function(error, content) {
        if (error) {
            if(error.code == 'ENOENT'){
                fs.readFile('./404.html', function(error, content) {
                    response.writeHead(200, { 'Content-Type': contentType });
                    response.end(content, 'utf-8');
                });
            }
            else {
                response.writeHead(500);
                response.end('Sorry, check with the site admin for error: '+error.code+' ..\n');
                response.end(); 
            }
        }
        else {
            response.writeHead(200, { 'Content-Type': contentType });
            response.end(content, 'utf-8');
        }
    });

}).listen(3000);



var io = require('socket.io').listen(app);

// Emit welcome message on connection
io.on('connection', function(socket) {
    // Use socket to communicate with this particular client only, sending it it's own id
    socket.emit('welcome', { message: 'WWWelcome!', id: socket.id });

    socket.on('morse', function(data){
       console.log("got a morse");
       console.log(data);
      io.sockets.emit('broadcast',data);
//      socket.emit('welcome', { message: 'WWWwwwwwwelcome!', id: socket.id });
//      socket.emit('welcome', { message: 'WWWwwwwwwelcome!'});
  //    socket.emit('welcome', { message: 'WWWwwwwwwelcome!', id: socket.id });
//      socket.emit('morse', { message: data});
    });

});


