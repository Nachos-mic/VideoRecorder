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
#include "utils.h"

class VideoCapture : public QObject {
    Q_OBJECT
public:
    explicit VideoCapture(QObject *parent = nullptr);
    ~VideoCapture();

    void setCamera(QCamera* camera);
    bool isRecording() const;
    void setActiveSession(QMediaCaptureSession* session);

public slots:
    void startCapturingVideo(QCamera* camera);
    void stopCapturingVideo();

private:
    void setupCameraForPreview();
    void setupCameraForRecording();
    QString generateFileName() const;

    QMediaCaptureSession* activeSession;
    QMediaRecorder* videoCapture;
    QAudioInput* audioInput;
    QCamera* currentCamera;
    QString vid_path = Utils::getMediaPath();

signals:
    void videoCaptured(const QString& path);
    void recordingError(const QString& error);
};

#endif // VIDEOCAPTURE_H
