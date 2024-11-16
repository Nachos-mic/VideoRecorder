#include "videocapture.h"

VideoCapture::VideoCapture(QObject *parent)
    : QObject(parent)
    , videoCapture(new QMediaRecorder(this))
{

}

VideoCapture::~VideoCapture() {
}




void VideoCapture::setCamera(QCamera* camera) {
    if (camera) {
        captureSession.setCamera(camera);
        captureSession.setRecorder(videoCapture);
        qDebug() << "CaptureSession active";
    }
}

QString VideoCapture::generateFileName() const {
    QString path = Utils::getMediaPath();
    QDir().mkpath(path);
    return path + "/video_" +
           QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +
           ".mp4";
}

void VideoCapture::startCapturingVideo(QCamera* camera) {
    if (!camera || !videoCapture) {
        qDebug() << "Camera or VideoCapture not available";
        return;
    }

    captureSession.setCamera(camera);
    captureSession.setRecorder(videoCapture);


    videoCapture->setQuality(QMediaRecorder::HighQuality);
    videoCapture->setOutputLocation(QUrl::fromLocalFile(generateFileName()));

    videoCapture->record();
    qDebug() << "Recording video to:" << videoCapture->outputLocation().toLocalFile();

}

void VideoCapture::stopCapturingVideo()
{
    if (videoCapture && videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        videoCapture->stop();
        qDebug() << "Stopped recording. File saved to:" << vid_path;
    }
}

bool VideoCapture::isRecording() const
{
    return videoCapture && videoCapture->recorderState() == QMediaRecorder::RecordingState;
}
Impr

