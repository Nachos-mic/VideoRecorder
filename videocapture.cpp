#include "videocapture.h"

VideoCapture::VideoCapture(QObject *parent)
    : QObject(parent)
    , activeSession(nullptr)
    , videoCapture(new QMediaRecorder(this))
    , audioInput(nullptr)
    , currentCamera(nullptr)
{
    connect(videoCapture, &QMediaRecorder::errorOccurred,
            this, [this](QMediaRecorder::Error error, const QString &errorString) {
                qDebug() << "Recording error:" << error << errorString;
                emit recordingError(errorString);
            });

    connect(videoCapture, &QMediaRecorder::recorderStateChanged,
            this, [this](QMediaRecorder::RecorderState state) {
                qDebug() << "Recorder state changed to:" << state;
                if (state == QMediaRecorder::StoppedState && currentCamera) {
                    // Restore camera state after recording
                    setupCameraForPreview();
                }
            });
}

VideoCapture::~VideoCapture()
{
    if (videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        videoCapture->stop();
    }
    delete audioInput;
}

void VideoCapture::setupCameraForPreview()
{
    if (!currentCamera || !activeSession) return;

    currentCamera->stop();
    activeSession->setCamera(currentCamera);
    activeSession->setRecorder(nullptr);
    currentCamera->start();
    qDebug() << "Camera setup for preview";
}

void VideoCapture::setupCameraForRecording()
{
    if (!currentCamera || !activeSession) return;

    currentCamera->stop();
    activeSession->setRecorder(videoCapture);

    if (!audioInput) {
        auto audioInputs = QMediaDevices::audioInputs();
        if (!audioInputs.isEmpty()) {
            audioInput = new QAudioInput(this);
            audioInput->setDevice(audioInputs.first());
            activeSession->setAudioInput(audioInput);
        }
    }

    currentCamera->start();
    qDebug() << "Camera setup for recording";
}

void VideoCapture::setActiveSession(QMediaCaptureSession* session)
{
    activeSession = session;
}

void VideoCapture::setCamera(QCamera* camera)
{
    currentCamera = camera;
    if (camera && activeSession) {
        setupCameraForPreview();
    }
}

void VideoCapture::startCapturingVideo(QCamera* camera)
{
    if (!camera || videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        qDebug() << "Cannot start recording: "
                 << (!camera ? "No camera" : "Already recording");
        return;
    }

    currentCamera = camera;
    setupCameraForRecording();

    QMediaFormat format;
    format.setVideoCodec(QMediaFormat::VideoCodec::H264);
    format.setAudioCodec(QMediaFormat::AudioCodec::AAC);



    videoCapture->setQuality(QMediaRecorder::Quality::NormalQuality);

    QString outputFile = generateFileName();
    qDebug() << "Setting output location to:" << outputFile;

    videoCapture->record();
    qDebug() << "Started recording to:" << videoCapture->outputLocation().toLocalFile();
}

void VideoCapture::stopCapturingVideo()
{
    if (videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        videoCapture->stop();
        QString recordedFile = videoCapture->outputLocation().toLocalFile();
        qDebug() << "Stopped recording. File saved as:" << recordedFile;
        setupCameraForPreview();
        emit videoCaptured(recordedFile);
    }
}

bool VideoCapture::isRecording() const
{
    return videoCapture->recorderState() == QMediaRecorder::RecordingState;
}



QString VideoCapture::generateFileName() const {
    QString path = Utils::getMediaPath();
    QDir().mkpath(path);
    return path + "/video_" +
           QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +
           ".mp4";
}




