#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include <QCamera>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QUrl>
#include "utils.h"

class VideoCapture : public QObject {
    Q_OBJECT
public:
    explicit VideoCapture(QObject *parent = nullptr);
    ~VideoCapture();

    void setCamera(QCamera* camera);

public slots:
    void startCapturingVideo(QCamera* camera);
    void stopCapturingVideo();
    bool isRecording();

private:
    QString generateFileName() const;
    QMediaCaptureSession captureSession;
    QMediaRecorder* videoCapture;
    QString vid_path = Utils::getMediaPath();

signals:
    void videoCaptured(const QString& path);
};

#endif // VIDEOCAPTURE_H
