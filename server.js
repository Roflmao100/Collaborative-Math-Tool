var express = require('express');
var app = express();
var server = require('http').createServer(app);
var io = require('socket.io').listen(server);
users = [];
connections = [];

server.listen(process.env.PORT || 3000);
console.log('server running...');
app.get('/', function(req, res){
	res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket){
	connections.push(socket);
	initUsers();
	console.log('Connected: %s sockets connected', connections.length);
	
	//Disconnect
	socket.on('disconnect', function(data){
		var deletedUser = socket.username;
		users.splice(getIndexOf(socket.username), 1);
		connections.splice(connections.indexOf(socket), 1);
		updateUsers(deletedUser, false);
		console.log('Disconnect: %s sockets connected', connections.length);
	});

	function getIndexOf(name){
		for(var i = 0; i < users.length; i++){
			if(users[i].name == name){
				return i;
			}
		}
		return -1;
	}
	
	socket.on('send data', function(data){
		io.sockets.emit('update', { drawing: data, user: socket.username });
	});

	socket.on('new user', function(data, color, callback){
		var check = checkUsername(data);
		callback(check);
		if(check == 0){
			socket.username = data;
			users.push({ name: socket.username, col: color });
			updateUsers(data, true);
			console.log(socket.username);
		}
	});

	socket.on('send message', function(data){
		io.sockets.emit('new message', { msg: data.input, user: socket.username, col: data.col });
	});

	socket.on('new background color', function(newColor){
		io.sockets.emit('change background color', newColor);
	});

	function checkUsername(text){
		if(text.length < 5){
			return 1;
		}else if(text != escape(text)){
			return 2;
		}else{
			for(var i = 0; i < users.length; i++){
				if(users[i].name == text){
					return 3;
				}
			}
		}
		return 0;
	}

	function updateUsers(newUser, isConnecting){
		io.sockets.emit('update users', users, newUser, isConnecting);
	}

	function initUsers(){
		io.sockets.emit('init users', users);
	}
});