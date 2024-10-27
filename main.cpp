#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "devicecheck.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    DeviceCheck check;

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/VideoRecorder_ALPHA/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
