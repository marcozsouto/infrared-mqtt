const aedes = require('aedes')();
const server = require('net').createServer(aedes.handle);

const port = 1883;

server.listen(port, function () {
  console.log('MQTT broker started and listening on port', port);
});

aedes.on('client', function (client) {
  console.log('Client connected:', client.id);
});

aedes.on('publish', function (packet, client) {
  if (client) {
    console.log('Message from client', client.id, 'on topic', packet.topic, ':', packet.payload.toString());
  }
});
