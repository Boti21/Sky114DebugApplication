#include "mqtttransceiver.h"
#include <QMqttTopicFilter>
#include <QDebug>

MqttTransceiver::MqttTransceiver(QObject *parent)
    : QObject(parent)
{
    m_client = new QMqttClient(this);
    m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);

    connect(m_client, &QMqttClient::connected, this, &MqttTransceiver::onConnected);

    connect(m_client, &QMqttClient::stateChanged,
            this, [this](QMqttClient::ClientState state) {
                qDebug() << "MQTT State:" << state
                         << "host:" << m_client->hostname()
                         << "port:" << m_client->port();
                emit connectedChanged();
                if (state == QMqttClient::Connected) {
                    emit messageReceived("SYSTEM", "Connected!");
                } else if (state == QMqttClient::Disconnected) {
                    emit messageReceived("SYSTEM", "Disconnected");
                }
            });

    connect(m_client, &QMqttClient::errorChanged,
            this, [this](QMqttClient::ClientError error) {
                qDebug() << "MQTT Error:" << error;
                if (error != QMqttClient::NoError) {
                    emit messageReceived(
                        "SYSTEM", "Error: " + QString::number(error));
                }
            });

    connect(m_client, &QMqttClient::messageReceived,
            this, [this](const QByteArray &message,
                   const QMqttTopicName &topic) {
                emit messageReceived(topic.name(),
                                     QString::fromUtf8(message));
            });;
}

bool MqttTransceiver::isConnected() const
{
    return m_client->state() == QMqttClient::Connected;
}

void MqttTransceiver::connectToBroker(const QString &host, int port)
{
    if (m_client->state() != QMqttClient::Disconnected)
        m_client->disconnectFromHost();

    m_client->setHostname(host);
    m_client->setPort(port);
    m_client->setClientId(
        "SkyDebuggerClient_");
        //+ QString::number(qrand(), 16));
    m_client->connectToHostWebSocket();

    default_subscribe();
}

void MqttTransceiver::disconnectFromBroker()
{
    m_client->disconnectFromHost();
}

void MqttTransceiver::subscribe(const QString &topic, int qos)
{
    if (!isConnected()) return;

    auto subscription = m_client->subscribe(
        QMqttTopicFilter(topic), static_cast<quint8>(qos));

    if (!subscription) {
        emit messageReceived("SYSTEM",
                             "Subscribe failed: " + topic);
        return;
    }
    emit messageReceived("SYSTEM", "Subscribed to: " + topic);
}

void MqttTransceiver::default_subscribe()
{
    subscribe("robobot/map", 0);
    subscribe("robobot/iwo/ang", 0);
    subscribe("robobot/iwo/pos", 0);
}

void MqttTransceiver::publish(
    const QString &topic, const QString &payload, int qos)
{
    if (!isConnected()) return;
    m_client->publish(QMqttTopicName(topic),
                      payload.toUtf8(),
                      static_cast<quint8>(qos));
}
void MqttTransceiver::onConnected() {
    qDebug() << "Connected! Registering subscriptions...";

    default_subscribe();
}
