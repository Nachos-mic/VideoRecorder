#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtMultimedia>
#include <QCamera>
#include <QCameraDevice>
#include <iostream>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/VideoRecorder_ALPHA/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : cameras) {
        if (cameraDevice.description() == "mycamera")
            qDebug() << cameraDevice.description();
    }

    return app.exec();
}
