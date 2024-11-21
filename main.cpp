#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "videorecorder.h"
#include <QObject>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<VideoRecorder>("VideoRecorder", 1, 0, "VideoRecorder");

    const QUrl url(u"qrc:/VideoRecorder_ALPHA/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
