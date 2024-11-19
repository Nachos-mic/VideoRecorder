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

    connect(videoCapture, &QMediaRecorder::durationChanged,
            this, [](qint64 duration) {
                qDebug() << "Recording duration:" << duration;
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
    if (session != activeSession) {
        activeSession = session;
        qDebug() << "Active session set:" << (session != nullptr);
    }
}

void VideoCapture::setCamera(QCamera* camera)
{
    currentCamera = camera;
}

bool VideoCapture::setupCameraForRecording()
{
    if (!currentCamera || !activeSession) {
        qDebug() << "Setup failed: Camera or session not available";
        return false;
    }

    QMediaFormat format;
    format.setFileFormat(QMediaFormat::MPEG4);
    format.setVideoCodec(QMediaFormat::VideoCodec::H264);
    format.setAudioCodec(QMediaFormat::AudioCodec::AAC);

    videoCapture->setMediaFormat(format);
    videoCapture->setQuality(QMediaRecorder::HighQuality);

    QString filePath = generateFileName() + ".mp4";
    videoCapture->setOutputLocation(QUrl::fromLocalFile(filePath));

    // Verify the camera is active
    if (!currentCamera->isActive()) {
        currentCamera->start();
        QThread::msleep(100);
    }

    // Setup audio
    if (!audioInput) {
        audioInput = activeSession->audioInput();
        if (!audioInput) {
            auto audioInputs = QMediaDevices::audioInputs();
            if (!audioInputs.isEmpty()) {
                audioInput = new QAudioInput(this);
                audioInput->setDevice(audioInputs.first());
                activeSession->setAudioInput(audioInput);
            }
        }
    }

    // Use existing session
    if (!activeSession->camera()) {
        activeSession->setCamera(currentCamera);
    }
    activeSession->setRecorder(videoCapture);

    qDebug() << "Camera recording setup complete"
             << "\nCamera active:" << currentCamera->isActive()
             << "\nFormat:" << format.fileFormat()
             << "\nVideo codec:" << format.videoCodec()
             << "\nOutput file:" << filePath
             << "\nSession has camera:" << (activeSession->camera() != nullptr)
             << "\nSession has recorder:" << (activeSession->recorder() != nullptr);

    return true;
}

bool VideoCapture::startCapturingVideo(QCamera* camera)
{
    if (!camera || videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        qDebug() << "Cannot start recording:"
                 << (!camera ? "No camera" : "Already recording");
        return false;
    }

    if (!activeSession) {
        qDebug() << "No active session available";
        return false;
    }

    currentCamera = camera;

    if (!setupCameraForRecording()) {
        qDebug() << "Failed to setup camera for recording";
        return false;
    }

    videoCapture->record();
    qDebug() << "Started recording to:" << videoCapture->outputLocation().toLocalFile();
    return true;
}

void VideoCapture::stopCapturingVideo()
{
    if (videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        videoCapture->stop();
        qDebug() << "Stopping recording...";
        QTimer::singleShot(500, this, [this]() {
            if (activeSession) {
                activeSession->setRecorder(nullptr);
            }
        });
    }
}

QString VideoCapture::generateFileName() const
{
    QString path = Utils::getMediaPath();
    QDir().mkpath(path);
    QString filename = path + "/video_" +
                       QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    qDebug() << "Generated filename:" << filename;
    return filename;
}

bool VideoCapture::isRecording() const
{
    return videoCapture->recorderState() == QMediaRecorder::RecordingState;
}
