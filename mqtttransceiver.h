#ifndef MQTTTRANSCEIVER_H
#define MQTTTRANSCEIVER_H

#include <QObject>
#include <QMqttClient>

class MqttTransceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit MqttTransceiver(QObject *parent = nullptr);

    bool isConnected() const;

    Q_INVOKABLE void connectToBroker(const QString &host, int port);
    Q_INVOKABLE void disconnectFromBroker();
    Q_INVOKABLE void subscribe(const QString &topic, int qos = 0);
    Q_INVOKABLE void default_subscribe();
    Q_INVOKABLE void publish(
        const QString &topic, const QString &payload, int qos = 0);
    Q_INVOKABLE void onConnected();

signals:
    void connectedChanged();
    void messageReceived(QString topic, QString payload);

private:
    QMqttClient *m_client = nullptr;
};

#endif // MQTTTRANSCEIVER_H

//     bool isConnected() {
//         return client && client->is_connected();
//     }

//     Q_INVOKABLE bool connectToBroker(
//         const QString& ip, const QString& port)
//     {
//         QString address = "tcp://" + ip + ":" + port;
//         qDebug() << address;

//         try {
//             client = std::make_unique<mqtt::async_client>(
//                 address.toStdString(), "SkyDebuggerClient");
//             setupHandlers();
//             client->connect()->wait();
//             subscribeDefault();
//             emit connectedChanged();
//             return true;
//         } catch (const mqtt::exception& exc) {
//             qWarning() << "Connect failed:" << exc.what();
//             emit messageReceived(
//                 "SYSTEM", QString("Connection failed: %1")
//                     .arg(exc.what()));
//             return false;
//         }
//     }

//     Q_INVOKABLE bool subscribe(const QString& topic, int qos)
//     {
//         if (!client || !client->is_connected()) return false;
//         try {
//             client->subscribe(topic.toStdString(), qos);
//             qDebug() << "Subscribed to:" << topic
//                      << "with QOS:" << qos;
//             emit messageReceived(
//                 "SYSTEM", "Subscribed to: " + topic);
//             return true;
//         } catch (const mqtt::exception& exc) {
//             qWarning() << "Subscribe failed:" << exc.what();
//             return false;
//         }
//     }

//     Q_INVOKABLE bool publish(const QString& topic, const QString& payload, int qos);

// signals:
//     void connectedChanged();
//     // This signal bridges MQTT messages to QML
//     void messageReceived(QString topic, QString payload);

//     void GraphAppend(float x, float y);

// private:
//     std::unique_ptr<mqtt::async_client> client;

//     bool setupHandlers();
//     bool subscribeDefault();
// };
