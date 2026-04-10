// js/mqttclient.js
//.import "mqtt.min.js" as MqttLib
//.import "mqttclient.js" as MqttLib
.import "mqtt.js" as MqttLib

var client = null

// transceiver is your MqttTransceiver QObject,
// passed in from QML on init
var transceiver = null

function init(txrx) {
    transceiver = txrx
}

function connectToBroker(host, port) {
    var url = "ws://" + host + ":" + port + "/mqtt"

    client = MqttLib.connect(url, {
        clientId: "SkyDebuggerClient_"
            + Math.random().toString(16).slice(2),
        connectTimeout: 5000,
        reconnectPeriod: 0  // no auto-reconnect, handle manually
    })

    client.on("connect", function () {
        transceiver.setConnected(true)
        transceiver.handleIncomingMessage("SYSTEM", "Connected!")
    })

    client.on("error", function (err) {
        transceiver.handleIncomingMessage(
            "SYSTEM", "Error: " + err.message)
    })

    client.on("close", function () {
        transceiver.setConnected(false)
        transceiver.handleIncomingMessage(
            "SYSTEM", "Disconnected")
    })

    client.on("message", function (topic, payload) {
        transceiver.handleIncomingMessage(
            topic, payload.toString())
    })
}

function subscribe(topic, qos) {
    if (!client) return
    client.subscribe(topic, { qos: qos }, function (err) {
        if (!err) {
            transceiver.handleIncomingMessage(
                "SYSTEM", "Subscribed to: " + topic)
        } else {
            transceiver.handleIncomingMessage(
                "SYSTEM", "Subscribe failed: " + err.message)
        }
    })
}

function publish(topic, payload, qos) {
    if (!client) return
    client.publish(topic, payload, { qos: qos })
}

function disconnect() {
    if (client) client.end()
}
