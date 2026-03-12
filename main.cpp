#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/QQmlEngine>

#include "mqtttransceiver.h"

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");

    QGuiApplication app(argc, argv);


    QQmlApplicationEngine engine;

    qmlRegisterType<MqttTransceiver>("com.mqtt.transceiver", 1, 0, "MqttTransceiver");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Sky114DebugApplication", "Main");

    return app.exec();
}
