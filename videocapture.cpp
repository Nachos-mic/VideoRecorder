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
    if (activeSession != session) {
        activeSession = session;
        if (activeSession && videoCapture) {
            activeSession->setRecorder(nullptr);
            QTimer::singleShot(100, this, [this]() {
                activeSession->setRecorder(videoCapture);
            });
        }
    }
}

void VideoCapture::setCamera(QCamera* camera)
{
    if (currentCamera != camera) {
        currentCamera = camera;
        if (currentCamera && activeSession) {
            const QList<QSize> supportedResolutions = currentCamera->cameraDevice().photoResolutions();
            if (!supportedResolutions.isEmpty()) {
                // QCameraFormat format;
                // format.setResolution(supportedResolutions.first());
                // format.setPixelFormat(QVideoFrameFormat::Format_BGRA8888);
                // format.setMinFrameRate(30.0);
                // currentCamera->setCameraFormat(format);
            }

            activeSession->setCamera(currentCamera);
        }
    }
}

void VideoCapture::setupCameraForRecording()
{
    if (!currentCamera || !activeSession) {
        qDebug() << "Setup failed: Camera or session not available";
        return;
    }

    // Ensure camera is active
    if (!currentCamera->isActive()) {
        currentCamera->start();
    }

    QMediaFormat format;
    format.setFileFormat(QMediaFormat::MPEG4);
    format.setVideoCodec(QMediaFormat::VideoCodec::H264);
    format.setAudioCodec(QMediaFormat::AudioCodec::AAC);

    videoCapture->setMediaFormat(format);
    videoCapture->setQuality(QMediaRecorder::HighQuality);

    QString filePath = generateFileName() + ".mp4";
    videoCapture->setOutputLocation(QUrl::fromLocalFile(filePath));

    if (!audioInput) {
        auto audioInputs = QMediaDevices::audioInputs();
        if (!audioInputs.isEmpty()) {
            audioInput = new QAudioInput(this);
            audioInput->setDevice(audioInputs.first());
            activeSession->setAudioInput(audioInput);
        }
    }

    activeSession->setCamera(currentCamera);
    activeSession->setRecorder(videoCapture);

    qDebug() << "Camera recording setup complete"
             << "\nFormat:" << format.fileFormat()
             << "\nVideo codec:" << format.videoCodec()
             << "\nOutput file:" << filePath;
}

void VideoCapture::startCapturingVideo(QCamera* camera)
{
    if (!camera || videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        qDebug() << "Cannot start recording:"
                 << (!camera ? "No camera" : "Already recording");
        return;
    }

    if (currentCamera != camera) {
        setCamera(camera);
    }

    setupCameraForRecording();

    QTimer::singleShot(500, this, [this]() {
        if (currentCamera && currentCamera->isActive() &&
            activeSession && activeSession->camera() == currentCamera) {
            videoCapture->record();
            qDebug() << "Started recording to:"
                     << videoCapture->outputLocation().toLocalFile();
        } else {
            qDebug() << "Failed to start recording - camera not ready";
        }
    });
}

void VideoCapture::stopCapturingVideo()
{
    if (videoCapture->recorderState() == QMediaRecorder::RecordingState) {
        videoCapture->stop();
        qDebug() << "Stopping recording...";
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
