#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "videorecorder.h"
#include "imagecapture.h"
#include <QObject>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<QMediaCaptureSession>("CustomComponents", 1, 0, "CaptureSession");
    qmlRegisterUncreatableType<QCamera>("CustomComponents", 1, 0, "Camera", "Camera cannot be created in QML");

    VideoRecorder* camera_app = new VideoRecorder(&engine);
    ImageCapture* imageCaptureManager = new ImageCapture(&engine);
    camera_app->imageCaptureManager = imageCaptureManager;

    QObject::connect(camera_app, &VideoRecorder::frameCapture, imageCaptureManager, &ImageCapture::captureFrame);
    QObject::connect(camera_app, &VideoRecorder::pathChanged, imageCaptureManager, &ImageCapture::setCaptureImgPath);

    engine.rootContext()->setContextProperty("cameraUser", camera_app);

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
