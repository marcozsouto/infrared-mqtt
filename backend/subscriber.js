const express = require('express');
const mqtt = require('mqtt');
const bodyParser = require('body-parser');

const app = express();
const port = 3075;

const topic = "esp32/led";

// MQTT broker connection
const mqttClient = mqtt.connect('mqtt://localhost:1883'); // replace with your broker

mqttClient.on('connect', () => {
  console.log('Connected to MQTT broker');
});

mqttClient.on('error', (err) => {
  console.error('MQTT connection error:', err);
});

// Middleware to parse JSON request body
app.use(bodyParser.json());

app.get('/publish', (req, res) => {

  let object = {
    on: true,
    fan: 2,
    mode: 1,
    temp: 21,
    swing: true,
  };

  mqttClient.publish(topic, JSON.stringify(object), (err) => {
    if (err) {
      return res.status(500).json({ error: 'Failed to publish message' });
    }
    res.status(200).json({ success: true, message: `Message published to topic ${topic}` });
  });
});

// Start the server
app.listen(port, () => {
  console.log(`Server running on http://localhost:${port}`);
});
