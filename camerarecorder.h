#ifndef CAMERARECORDER_H
#define CAMERARECORDER_H
#include <QCamera>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QImageCapture>
#include <QMediaCaptureSession>

class ShowFeed: public QObject{
    Q_OBJECT

public:
    ShowFeed();
    void useCamera();
    QImageCapture* imageCapture;
    QMediaCaptureSession captureSession;


signals:
    void capturedFrame(const QString& image_path);
};

#endif // CAMERARECORDER_H
