import QtQuick
import QtQuick.Window
import QtMultimedia
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    visible: true
    property string camera_id: ""
    property string border_video_status: "black"
    property bool video_status: false
    property var camera_name_list: []
    property var camera_id_list: []
    property var cameraList: []
    title: "VideoRecorder"

    // Add state properties to track camera status
    property bool isCameraInitialized: false
    property bool isCameraReady: false

    Rectangle {
        id: options
        height: 40
        width: parent.width
        anchors {
            top: parent.top
            left: parent.left
        }
        color: "steelblue"

        ComboBox {
            id: cameraBox
            model: camera_name_list
            height: parent.height
            width: parent.width/6
            anchors {
                left: parent.left
                top: parent.top
            }
            onCurrentIndexChanged: {
                if (currentIndex >= 0 && isCameraInitialized) {
                    changeCamera()
                }
            }
        }

        Button {
            id: screenShotButton
            height: parent.height
            width: parent.width/6
            anchors {
                left: cameraBox.right
                top: parent.top
            }
            text: "Capture frame"
            enabled: isCameraReady
            onClicked: {
                if (cameraBox.currentIndex >= 0) {
                    cameraUser.captureFrame()
                }
            }
        }

        Button {
            id: recordVideoButton
            height: parent.height
            width: parent.width/6
            anchors {
                left: screenShotButton.right
                top: parent.top
            }
            text: video_status ? "Stop Recording" : "Start Recording"
            enabled: isCameraReady
            onClicked: {
                if (cameraBox.currentIndex >= 0) {
                    cameraUser.startStopVideoRecording()
                }
            }
        }

        FolderDialog {
            id: folderDialog
            title: "Select Directory"
            currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
            options: FolderDialog.ShowDirsOnly | FolderDialog.DontResolveSymlinks

            onAccepted: {
                cameraUser.setPath(selectedFolder)
            }

            onRejected: {
                console.log("Folder selection canceled")
            }
        }

        Button {
            id: setPathButton
            height: parent.height
            width: parent.width/6
            anchors {
                left: recordVideoButton.right
                top: parent.top
            }
            text: "Set Path"
            onClicked: {
                folderDialog.open()
            }
        }
    }

    PathPopUp {
        id: pathSetPop
    }

    Camera {
        id: camera
        active: false

        onActiveChanged: {
            console.log("Camera active state changed:", active)
            if (active) {
                // Give some time for the camera to fully initialize
                cameraReadyTimer.start()
            }
        }

        onErrorOccurred: function(error, errorString) {
            console.log("Camera error:", error, errorString)
        }
    }

    // Timer to ensure camera is fully ready
    Timer {
        id: cameraReadyTimer
        interval: 500
        repeat: false
        onTriggered: {
            isCameraReady = true
            console.log("Camera ready timer triggered - camera is now ready")
        }
    }

    // Timer for initial camera setup
    Timer {
        id: initialCameraTimer
        interval: 1000
        repeat: false
        onTriggered: {
            if (camera_id_list.length > 0) {
                console.log("Initial camera timer triggered - setting up first camera")
                cameraBox.currentIndex = 0
                changeCamera()
            }
        }
    }

    CaptureSession {
        id: captureSession
        camera: camera
        videoOutput: videoOutput

        Component.onCompleted: {
            console.log("Setting up capture session")
            cameraUser.setQmlCaptureSession(captureSession)
        }
    }

    Rectangle {
        id: videoOutputBox
        height: parent.height - options.height
        width: parent.width
        border.color: border_video_status
        border.width: 2

        anchors {
            top: options.bottom
            left: parent.left
        }

        VideoOutput {
            id: videoOutput
            anchors.fill: parent
            fillMode: VideoOutput.PreserveAspectFit
        }


    }

    Timer {
            id: cameraChangeTimer
            interval: 100
            repeat: false
            onTriggered: {
                cameraUser.createCamera(camera_id_list[cameraBox.currentIndex])
                camera.active = true
                isCameraInitialized = true
                console.log("Camera change completed, waiting for ready state")
            }
        }

        function changeCamera() {
            if (cameraBox.currentIndex >= 0) {
                console.log("Changing camera to index:", cameraBox.currentIndex)
                console.log("Camera ID:", camera_id_list[cameraBox.currentIndex])

                // Reset states
                isCameraReady = false
                camera.active = false

                // Start the timer for delayed camera creation
                cameraChangeTimer.start()
            }
        }

    Connections {
        target: cameraUser

        function onDeviceListChanged(id_list, name_list) {
            camera_id_list = id_list
            camera_name_list = name_list

            console.log("Updated camera list - IDs:", JSON.stringify(camera_id_list))
            console.log("Names:", JSON.stringify(camera_name_list))

            if (!isCameraInitialized && camera_id_list.length > 0) {
                initialCameraTimer.start()
            }
        }

        function onVideoCaptured(path) {
            console.log("Video saved to:", path)
            video_status = false
            border_video_status = "black"
        }

        function onVideoRecordingStatusChanged(isRecording) {
            console.log("Recording status changed:", isRecording)
            video_status = isRecording
            border_video_status = isRecording ? "red" : "black"
        }

        function onCameraChanged() {
            console.log("Camera changed")
            // Reset camera ready state when camera changes
            isCameraReady = false
            cameraReadyTimer.start()
        }

        function onPathChanged(path) {
            console.log("Save path changed to:", path)
        }
    }

    Component.onCompleted: {
        console.log("Window initialized")
    }
}
