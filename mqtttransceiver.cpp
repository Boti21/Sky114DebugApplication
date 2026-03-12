#ifdef USE_PAHO

#include "mqtttransceiver.h"

MqttTransceiver::MqttTransceiver(QObject *parent)
    : QObject(parent)
{}

bool MqttTransceiver::subscribeDefault()
{
    subscribe("foo", 0);
    subscribe("robobot/cmd/ti", 0);
    subscribe("robobot/cmd/T0", 0);
    subscribe("robobot/graph", 0);
    return true;
}

bool MqttTransceiver::setupHandlers()
{
    client->set_connected_handler(
        [this](const std::string& cause)
        {
            qDebug() << "MQTT Connection Established!";
            QMetaObject::invokeMethod(this, [this]() {
                emit messageReceived("SYSTEM", "Connected!");
                emit connectedChanged();
            }, Qt::QueuedConnection);
        });

    client->set_connection_lost_handler(
        [this](const std::string& cause)
        {
            qWarning() << "Connection LOST:"
                       << QString::fromStdString(cause);
            QMetaObject::invokeMethod(this, [this, cause]() {
                emit messageReceived(
                    "SYSTEM",
                    "Connection lost: "
                        + QString::fromStdString(cause));
                emit connectedChanged();
            }, Qt::QueuedConnection);
        });

    client->set_message_callback(
        [this](mqtt::const_message_ptr msg)
        {
            QString topic =
                QString::fromStdString(msg->get_topic());
            QString payload =
                QString::fromStdString(msg->get_payload_str());

            // Bridge to UI thread
            QMetaObject::invokeMethod(
                this, [this, topic, payload]()
                {
                    emit messageReceived(topic, payload);

                }, Qt::QueuedConnection);
        });

    return true;
}

Q_INVOKABLE bool MqttTransceiver::publish(
        const QString& topic, const QString& payload, int qos)
    {
        if (!client || !client->is_connected()) return false;
        try {
            client->publish(
                topic.toStdString(), payload.toStdString(),
                qos, false);
            return true;
        } catch (const mqtt::exception& exc) {
            qWarning() << "Publish failed:" << exc.what();
            return false;
        }
    }

#endif

#ifndef USE_PAHO
#include "mqtttransceiver.h"
#include <QDebug>

    MqttTransceiver::MqttTransceiver(QObject *parent)
        : QObject(parent), m_client(new QMqttClient(this))
    {
        // Assign a unique client ID
        m_client->setClientId("SkyDebuggerClient");

        // Connect Qt's signals to our slots
        connect(m_client, &QMqttClient::connected,
                this, &MqttTransceiver::onConnected);
        connect(m_client, &QMqttClient::disconnected,
                this, &MqttTransceiver::onDisconnected);
    }

    MqttTransceiver::~MqttTransceiver() {}

    bool MqttTransceiver::connectToBroker(
        const QString& ip, const QString& port)
    {
        // Qt MQTT uses a hostname + port pattern
        m_client->setHostname(ip);
        m_client->setPort(port.toUShort());

        qDebug() << "Connecting to" << ip << ":" << port;
        m_client->connectToHost();

        return true; // Actual connection is async
    }

    bool MqttTransceiver::subscribe(
        const QString& topic, quint8 qos)
    {
        if (!isConnected()) return false;

        QMqttSubscription *subscription =
            m_client->subscribe(topic, qos);

        if (!subscription) {
            qWarning() << "Subscribe failed:" << topic;
            return false;
        }

        // Connect the subscription's message signal
        connect(subscription, &QMqttSubscription::messageReceived,
                this, [this, topic](const QMqttMessage& msg) {
                    QString payload = QString::fromUtf8(
                        msg.payload());
                    emit messageReceived(topic, payload);
                });

        qDebug() << "Subscribed to:" << topic << "with QOS:"
                 << qos;
        emit messageReceived("SYSTEM",
                             "Subscribed to: " + topic);

        return true;
    }

    bool MqttTransceiver::publish(
        const QString& topic, const QString& payload, quint8 qos)
    {
        if (!isConnected()) return false;

        bool success = m_client->publish(
            QMqttTopicName(topic),
            payload.toUtf8(),
            qos);

        if (!success) {
            qWarning() << "Publish failed:" << topic;
        }

        return success;
    }

    void MqttTransceiver::onConnected()
    {
        qDebug() << "MQTT Connection Established!";
        emit messageReceived("SYSTEM", "Connected!");
        emit connectedChanged();
        subscribeDefault();
    }

    void MqttTransceiver::onDisconnected()
    {
        qWarning() << "MQTT Connection lost";
        emit messageReceived("SYSTEM", "Connection lost");
        emit connectedChanged();
    }

    void MqttTransceiver::subscribeDefault()
    {
        subscribe("foo", 0);
        subscribe("robobot/cmd/ti", 0);
        subscribe("robobot/cmd/T0", 0);
        subscribe("robobot/graph", 0);
    }

    void MqttTransceiver::setupHandlers()
    {
        // No longer needed! Qt's signals handle everything.
    }

#endif
