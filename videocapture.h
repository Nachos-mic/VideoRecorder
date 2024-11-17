#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include <QCamera>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QUrl>
#include <QMediaFormat>
#include <QAudioInput>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QTimer>
#include "utils.h"

class VideoCapture : public QObject {
    Q_OBJECT

public:
    explicit VideoCapture(QObject *parent = nullptr);
    ~VideoCapture();

    void setActiveSession(QMediaCaptureSession* session);
    void setCamera(QCamera* camera);
    bool isRecording() const;

public slots:
    void startCapturingVideo(QCamera* camera);
    void stopCapturingVideo();

signals:
    void videoCaptured(const QString& path);
    void recordingError(const QString& error);

private:
    void setupCameraForRecording();
    QString generateFileName() const;

    QMediaCaptureSession* activeSession;
    QMediaRecorder* videoCapture;
    QAudioInput* audioInput;
    QCamera* currentCamera;
    QString vid_path;
};

#endif // VIDEOCAPTURE_H
