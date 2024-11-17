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
                if (state == QMediaRecorder::StoppedState) {
                    emit videoCaptured(videoCapture->outputLocation().toLocalFile());
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

void VideoCapture::setActiveSession(QMediaCaptureSession* session)
{
    activeSession = session;
}

void VideoCapture::setCamera(QCamera* camera)
{
    currentCamera = camera;
}

void VideoCapture::setupCameraForRecording()
{
    if (!currentCamera || !activeSession) return;


    QMediaFormat format;
    format.setFileFormat(QMediaFormat::MPEG4);
    format.setVideoCodec(QMediaFormat::VideoCodec::H264);
    format.setAudioCodec(QMediaFormat::AudioCodec::AAC);



    videoCapture->setMediaFormat(format);
    videoCapture->setQuality(QMediaRecorder::HighQuality);
    videoCapture->setOutputLocation(QUrl::fromLocalFile(generateFileName()));


    activeSession->setRecorder(videoCapture);

    if (!audioInput) {
        auto audioInputs = QMediaDevices::audioInputs();
        if (!audioInputs.isEmpty()) {
            audioInput = new QAudioInput(this);
            audioInput->setDevice(audioInputs.first());
            activeSession->setAudioInput(audioInput);
        }
    }

    // qDebug() << "Camera setup for recording with format:" << format.fileFormat();
}

void VideoCapture::startCapturingVideo(QCamera* camera)
{
    if (!camera || videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        qDebug() << "Cannot start recording: "
                 << (!camera ? "No camera" : "Already recording");
        return;
    }

    currentCamera = camera;

    if (!currentCamera->isActive()) {
        currentCamera->start();
    }

    setupCameraForRecording();

    // Start recording after a short delay
    QTimer::singleShot(100, this, [this]() {
        videoCapture->record();
        qDebug() << "Started recording to:" << videoCapture->outputLocation().toLocalFile();
    });
}

void VideoCapture::stopCapturingVideo()
{
    if (videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        videoCapture->stop();
        qDebug() << "Stopping recording...";

        QTimer::singleShot(100, this, [this]() {
            if (activeSession) {
                activeSession->setRecorder(nullptr);
            }
        });
    }
}

QString VideoCapture::generateFileName() const {
    QString path = Utils::getMediaPath();
    QDir().mkpath(path);
    QString filename = path + "/video_" +
                       QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") ;
    qDebug() << "Generated filename:" << filename;
    return filename;
}

bool VideoCapture::isRecording() const
{
    return videoCapture->recorderState() == QMediaRecorder::RecordingState;
}
