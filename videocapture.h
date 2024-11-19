#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <QMediaRecorder>
#include <QMediaCaptureSession>
#include <QAudioInput>
#include <QCamera>
#include <QMediaFormat>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QMediaDevices>
#include <QThread>
#include "utils.h"
#include <QAudioDevice>

class VideoCapture : public QObject
{
    Q_OBJECT

public:
    explicit VideoCapture(QObject *parent = nullptr);
    ~VideoCapture();

    void setActiveSession(QMediaCaptureSession* session);
    void setCamera(QCamera* camera);
    bool startCapturingVideo(QCamera* camera);
    void stopCapturingVideo();
    bool isRecording() const;

signals:
    void recordingError(const QString& error);
    void videoCaptured(const QString& path);

private:
    bool setupCameraForRecording();
    QString generateFileName() const;

    QMediaCaptureSession* activeSession;
    QMediaRecorder* videoCapture;
    QAudioInput* audioInput;
    QCamera* currentCamera;
};
#endif // VIDEOCAPTURE_H
