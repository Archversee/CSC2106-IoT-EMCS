const express = require("express");
const http = require("http");
const socketIo = require("socket.io");
const mqtt = require("mqtt");
const cors = require("cors");

const app = express();
app.use(cors());

// serve frontend
app.use(express.static("../frontend"));

const server = http.createServer(app);

const io = socketIo(server, {
    cors: {
        origin: "*"
    }
});

// MQTT connection
const MQTT_BROKER = "mqtt://localhost:1883";
const mqttClient = mqtt.connect(MQTT_BROKER);

mqttClient.on("connect", () => {

    console.log("Connected to MQTT broker");

    mqttClient.subscribe(["sensors/#"], (err) => {
        if (!err) {
            console.log("Subscribed to sensors/#");
        }
    });

});

// incoming MQTT messages
mqttClient.on("message", (topic, message) => {

    const payload = message.toString();

    console.log(topic + " : " + payload);

    io.emit("mqtt_message", {
        topic: topic,
        message: payload
    });

});

// web client connection
io.on("connection", (socket) => {

    console.log("Web client connected");

    socket.on("disconnect", () => {
        console.log("Web client disconnected");
    });

});

// command endpoint
app.get("/cmd/:msg", (req, res) => {

    const msg = req.params.msg;

    mqttClient.publish("sensors/cmd", msg);

    console.log("Command sent:", msg);

    res.send("Command sent");

});

// start server
server.listen(3000, () => {
    console.log("Dashboard server running on port 3000");
});