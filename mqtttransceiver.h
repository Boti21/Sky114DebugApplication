#ifndef MQTTTRANSCEIVER_H
#define MQTTTRANSCEIVER_H

#define USE_PAHO

#ifdef USE_PAHO

#include <QObject>
#include <mqtt/async_client.h>
#include <QDebug>
#include <memory>
#include <string>

class MqttTransceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit MqttTransceiver(QObject *parent = nullptr);

    bool isConnected() {
        return client && client->is_connected();
    }

    Q_INVOKABLE bool connectToBroker(
        const QString& ip, const QString& port)
    {
        QString address = "tcp://" + ip + ":" + port;
        qDebug() << address;

        try {
            client = std::make_unique<mqtt::async_client>(
                address.toStdString(), "SkyDebuggerClient");
            setupHandlers();
            client->connect()->wait();
            subscribeDefault();
            emit connectedChanged();
            return true;
        } catch (const mqtt::exception& exc) {
            qWarning() << "Connect failed:" << exc.what();
            emit messageReceived(
                "SYSTEM", QString("Connection failed: %1")
                    .arg(exc.what()));
            return false;
        }
    }

    Q_INVOKABLE bool subscribe(const QString& topic, int qos)
    {
        if (!client || !client->is_connected()) return false;
        try {
            client->subscribe(topic.toStdString(), qos);
            qDebug() << "Subscribed to:" << topic
                     << "with QOS:" << qos;
            emit messageReceived(
                "SYSTEM", "Subscribed to: " + topic);
            return true;
        } catch (const mqtt::exception& exc) {
            qWarning() << "Subscribe failed:" << exc.what();
            return false;
        }
    }

    Q_INVOKABLE bool publish(const QString& topic, const QString& payload, int qos);

signals:
    void connectedChanged();
    // This signal bridges MQTT messages to QML
    void messageReceived(QString topic, QString payload);

    void GraphAppend(float x, float y);

private:
    std::unique_ptr<mqtt::async_client> client;

    bool setupHandlers();
    bool subscribeDefault();
};
#endif

#ifndef USE_PAHO

#include <QObject>
#include <QMqttClient>
#include <QMqttSubscription>
#include <memory>

class MqttTransceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY
                   connectedChanged)

public:
    explicit MqttTransceiver(QObject *parent = nullptr);
    ~MqttTransceiver();

    bool isConnected() const {
        return m_client && m_client->state() ==
                               QMqttClient::Connected;
    }

    Q_INVOKABLE bool connectToBroker(
        const QString& ip, const QString& port);
    Q_INVOKABLE bool subscribe(
        const QString& topic, quint8 qos);
    Q_INVOKABLE bool publish(
        const QString& topic, const QString& payload,
        quint8 qos);

signals:
    void connectedChanged();
    void messageReceived(QString topic, QString payload);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QByteArray& message,
                           const QMqttTopicName& topic);

private:
    QMqttClient *m_client;

    void setupHandlers();
    void subscribeDefault();
};

#endif
#endif // MQTTTRANSCEIVER_H
