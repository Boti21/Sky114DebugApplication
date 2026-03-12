import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.mqtt.transceiver 1.0
import QtGraphs

Window {
    width: 1000
    height: 800
    visible: true
    title: "MQTT Debugger"


    MqttTransceiver {
        id: txrx

        onMessageReceived: (topic, payload) => {
            messageModel.insert(0, {
                "topic": topic,
                "payload": payload,
                "timestamp": new Date().toLocaleTimeString()
            })

            if(topic == "robobot/graph")
            {
                const tokens = payload.split(" ")
                x = parseFloat(tokens[0])
                y = parseFloat(tokens[1])
                graph.addPoint(x, y)
                //graph.addPoint(y)
            }

            // if (messageModel.count > 200) {
            //     messageModel.remove(messageModel.count - 1)
            // }
        }

        // onGraphAppend: (x, y) => {
        //                    if(graphDataLen >= 199)
        //                    {
        //                        graph.remove(0)
        //                        graph.append(x, y)
        //                        graphDataLen += 1
        //                    }
        //                }
    }

    ListModel {
        id: messageModel
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                text: "MQTT Debug"
            }
            TabButton {
                text: "Robot Control"
            }
            TabButton {
                text: "Graphs"
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // ═══════════════════════════════════
            // TAB 0: MQTT Debug
            // ═══════════════════════════════════
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // ── Connection Section ──
                    GroupBox {
                        title: "Connection"
                        Layout.fillWidth: true

                        RowLayout {
                            anchors.fill: parent
                            spacing: 10

                            TextField {
                                id: ipInput
                                placeholderText: "127.0.0.1"
                                text: "127.0.0.1"
                                Layout.fillWidth: true
                            }

                            TextField {
                                id: portInput
                                placeholderText: "1885"
                                text: "1885"
                                Layout.preferredWidth: 80
                                validator: IntValidator {
                                    bottom: 1; top: 65535
                                }
                            }

                            Button {
                                text: txrx.connected
                                    ? "Connected" : "Connect"
                                enabled: !txrx.connected
                                onClicked: {
                                    txrx.connectToBroker(
                                        ipInput.text,
                                        portInput.text)
                                }
                            }
                        }
                    }

                    // ── Subscribe Section ──
                    GroupBox {
                        title: "Subscribe"
                        Layout.fillWidth: true

                        RowLayout {
                            anchors.fill: parent
                            spacing: 10

                            TextField {
                                id: subTopicInput
                                placeholderText:
                                    "topic/to/subscribe"
                                Layout.fillWidth: true
                            }

                            SpinBox {
                                id: subQosInput
                                from: 0; to: 2; value: 0
                                Layout.preferredWidth: 100
                            }

                            Button {
                                text: "Subscribe"
                                enabled: txrx.connected
                                onClicked: {
                                    if (subTopicInput.text
                                        !== "") {
                                        txrx.subscribe(
                                            subTopicInput.text,
                                            subQosInput.value)
                                    }
                                }
                            }
                        }
                    }

                    // ── Publish Section ──
                    GroupBox {
                        title: "Publish"
                        Layout.fillWidth: true

                        RowLayout {
                            anchors.fill: parent
                            spacing: 10

                            TextField {
                                id: pubTopicInput
                                placeholderText:
                                    "topic/to/publish"
                                Layout.fillWidth: true
                            }

                            TextField {
                                id: pubPayloadInput
                                placeholderText:
                                    "message payload"
                                Layout.fillWidth: true

                                onAccepted: {
                                    if (pubTopicInput.text
                                            !== ""
                                        && text !== "") {
                                        txrx.publish(
                                            pubTopicInput.text,
                                            text,
                                            pubQosInput.value)
                                        text = ""
                                    }
                                }
                            }

                            SpinBox {
                                id: pubQosInput
                                from: 0; to: 2; value: 0
                                Layout.preferredWidth: 100
                            }

                            Button {
                                text: "Send"
                                enabled: txrx.connected
                                onClicked: {
                                    if (pubTopicInput.text
                                            !== ""
                                        && pubPayloadInput.text
                                            !== "") {
                                        txrx.publish(
                                            pubTopicInput.text,
                                            pubPayloadInput
                                                .text,
                                            pubQosInput.value)
                                        pubPayloadInput.text
                                            = ""
                                    }
                                }
                            }
                        }
                    }

                    // ── Message Log ──
                    GroupBox {
                        title: "Messages ("
                            + messageModel.count + ")"
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 5

                            Button {
                                text: "Clear"
                                Layout.alignment:
                                    Qt.AlignRight
                                onClicked:
                                    messageModel.clear()
                            }

                            ListView {
                                id: messageList
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                model: messageModel
                                clip: true
                                spacing: 2

                                delegate: Rectangle {
                                    width: messageList.width
                                    height:
                                        msgCol.implicitHeight
                                        + 10
                                    color: index % 2 === 0
                                        ? "#f0f0f0"
                                        : "#ffffff"
                                    border.color:
                                        model.topic
                                            === "SYSTEM"
                                        ? "#ff9800"
                                        : "#cccccc"
                                    border.width: 1
                                    radius: 3

                                    ColumnLayout {
                                        id: msgCol
                                        anchors.fill: parent
                                        anchors.margins: 5
                                        spacing: 2

                                        RowLayout {
                                            spacing: 10

                                            Text {
                                                text: model
                                                    .timestamp
                                                font.pixelSize:
                                                    11
                                                color: "#888"
                                            }

                                            Text {
                                                text: model
                                                    .topic
                                                font.bold:
                                                    true
                                                font.pixelSize:
                                                    12
                                                color: model
                                                    .topic
                                                    === "SYSTEM"
                                                    ? "#ff9800"
                                                    : "#2196F3"
                                            }
                                        }

                                        Text {
                                            text: model
                                                .payload
                                            font.pixelSize: 13
                                            wrapMode:
                                                Text.Wrap
                                            Layout.fillWidth:
                                                true
                                        }
                                    }
                                }

                                ScrollBar.vertical:
                                    ScrollBar {}
                            }
                        }
                    }
                }
            }

            // ═══════════════════════════════════
            // TAB 1: Robot Control
            // ═══════════════════════════════════
            Item {
                id: robotTab

                // Grab focus when this tab becomes visible
                onVisibleChanged: {
                    if (visible) controlArea.forceActiveFocus()
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // ── Topic Config ──
                    GroupBox {
                        title: "Control Topic"
                        Layout.fillWidth: true

                        RowLayout {
                            anchors.fill: parent
                            spacing: 10

                            Text {
                                text: "Publish to:"
                            }

                            TextField {
                                id: controlTopicInput
                                text: "robobot/controller"
                                Layout.fillWidth: true

                                // After editing, return
                                // focus to the control area
                                onAccepted: {
                                    controlArea
                                        .forceActiveFocus()
                                }
                            }
                        }
                    }

                    // ── Mapping Config ──
                    GroupBox
                    {
                        title: "Mapping topic"
                        Layout.fillWidth: true

                        RowLayout
                        {
                            anchors.fill: parent
                            spacing: 10

                            Text
                            {
                                text: "Publish to:"
                            }


                            TextField
                            {
                                id: mappingTopicInput
                                text: "robobot/mapping"
                                Layout.fillWidth: true

                                // After editing, return
                                // focus to the control area
                                onAccepted:
                                {
                                    controlArea
                                        .forceActiveFocus()
                                }
                            }

                            TextField
                            {
                                id: mappingCommand
                                placeholderText: "Enter mapping commands"
                                text: "Junction1"
                                Layout.fillWidth: true

                                onAccepted:
                                {
                                    controlArea.forceActiveFocus()
                                }
                            }
                            Button
                            {
                                id: mappingCommandSend
                                text: "Send"
                                Layout.preferredWidth: 60
                                onClicked:
                                {
                                    txrx.publish(mappingTopicInput.text, mappingCommand.text, 0)
                                    console.log("mappingCommandSend pressed")
                                }
                            }
                        }
                    }

                    // ── Visual Key Display ──
                    GroupBox {
                        title: "Keyboard Control"
                            + (controlArea.activeFocus
                                ? " (Active)"
                                : " (Click here to focus)")
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        // This is the focusable area
                        // that captures keys
                        Item {
                            id: controlArea
                            anchors.fill: parent
                            focus: true

                            Keys.onPressed: (event) => {
                                let cmd = ""

                                switch (event.key) {
                                case Qt.Key_W:
                                case Qt.Key_Up:
                                    cmd = "forward"
                                    upIndicator.highlight()
                                    break
                                case Qt.Key_S:
                                case Qt.Key_Down:
                                    cmd = "backward"
                                    downIndicator.highlight()
                                    break
                                case Qt.Key_A:
                                case Qt.Key_Left:
                                    cmd = "left"
                                    leftIndicator.highlight()
                                    break
                                case Qt.Key_D:
                                case Qt.Key_Right:
                                    cmd = "right"
                                    rightIndicator
                                        .highlight()
                                    break
                                case Qt.Key_Space:
                                    cmd = "stop"
                                    stopIndicator.highlight()
                                    break
                                }

                                if (cmd !== ""
                                    && txrx.connected)
                                {
                                    txrx.publish(
                                        controlTopicInput
                                            .text,
                                        cmd, 0)
                                    lastCmdText.text =
                                        "Last: " + cmd

                                    console.log(controlTopicInput.text, cmd);
                                }

                                event.accepted = true
                            }

                            // Click to regain focus
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    controlArea
                                        .forceActiveFocus()
                                }
                            }

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 10

                                Text {
                                    text: "Use W/A/S/D or"
                                        + " Arrow Keys"
                                        + " | Space = Stop"
                                    font.pixelSize: 14
                                    color: "#666"
                                    Layout.alignment:
                                        Qt.AlignHCenter
                                }

                                // Arrow key visual
                                GridLayout {
                                    columns: 3
                                    rowSpacing: 5
                                    columnSpacing: 5
                                    Layout.alignment:
                                        Qt.AlignHCenter

                                    // Row 1: Up
                                    Item {
                                        Layout.preferredWidth:
                                            70
                                        Layout.preferredHeight:
                                            70
                                    }
                                    KeyIndicator {
                                        id: upIndicator
                                        label: "W\n▲"
                                    }
                                    Item {
                                        Layout.preferredWidth:
                                            70
                                        Layout.preferredHeight:
                                            70
                                    }

                                    // Row 2: Left, Stop,
                                    // Right
                                    KeyIndicator {
                                        id: leftIndicator
                                        label: "◄\nA"
                                    }
                                    KeyIndicator {
                                        id: stopIndicator
                                        label: "■\nSPC"
                                        baseColor: "#ffcdd2"
                                        activeColor: "#f44336"
                                    }
                                    KeyIndicator {
                                        id: rightIndicator
                                        label: "►\nD"
                                    }

                                    // Row 3: Down
                                    Item {
                                        Layout.preferredWidth:
                                            70
                                        Layout.preferredHeight:
                                            70
                                    }
                                    KeyIndicator {
                                        id: downIndicator
                                        label: "▼\nS"
                                    }
                                    Item {
                                        Layout.preferredWidth:
                                            70
                                        Layout.preferredHeight:
                                            70
                                    }
                                }

                                Text {
                                    id: lastCmdText
                                    text: "Last: none"
                                    font.pixelSize: 16
                                    font.bold: true
                                    Layout.alignment:
                                        Qt.AlignHCenter
                                }

                                Text {
                                    text: txrx.connected
                                        ? "● Connected"
                                        : "● Disconnected"
                                    color: txrx.connected
                                        ? "green" : "red"
                                    font.pixelSize: 14
                                    Layout.alignment:
                                        Qt.AlignHCenter
                                }
                            }
                        }
                    }
                }
            }

            Item
            {
                GraphsView
                {
                    id: graphView
                    anchors.fill: parent

                    axisX: ValueAxis
                    {
                        max: 5
                    }
                    axisY: ValueAxis
                    {
                        max: 5
                        //min: 5
                    }

                    LineSeries
                    {
                        id: graph
                        color: "#00ff00"
                        axisX: axisX
                        axisY: axisY
                        // XYPoint { x: 0.5; y: 0.5 }
                        // XYPoint { x: 1; y: 1 }
                        // XYPoint { x: 2; y: 2 }
                        // XYPoint { x: 2.5; y: 1.5 }

                        function addPoint(x, y)
                        {
                            if(axisX.max < (x + 5))
                            {
                                this.axisX.max += 20;
                            }

                            if(this.axisY.max < (y + 5))
                            {
                                graphView.axisY.max += 20;
                            }
                            if(axisY.min > (y - 5))
                            {
                                graphView.axisY.min += 20;
                            }


                            graph.append(x, y)
                        }
                    }
                }

            }
        }
    }

    // ═══════════════════════════════════
    // Reusable key indicator component
    // ═══════════════════════════════════
    component KeyIndicator: Rectangle {
        id: keyRect

        property string label: ""
        property color baseColor: "#e0e0e0"
        property color activeColor: "#4CAF50"

        Layout.preferredWidth: 70
        Layout.preferredHeight: 70
        radius: 8
        color: baseColor
        border.color: "#999"
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: keyRect.label
            font.pixelSize: 14
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

        function highlight() {
            keyRect.color = activeColor
            flashTimer.restart()
        }

        Timer {
            id: flashTimer
            interval: 150
            onTriggered: keyRect.color = baseColor
        }
    }
}
