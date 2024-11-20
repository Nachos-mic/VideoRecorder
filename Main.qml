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
            onCurrentIndexChanged: changeCamera()
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
            onClicked: {
                if (cameraBox.currentIndex >= 0) {
                    cameraUser.captureFrame()
                }
            }
        }

       Button {
            id: startButton
            height: parent.height
            width: parent.width/6
            text: "Record"
            visible: recorder.recorderState !== MediaRecorder.RecordingState
            onClicked: {
                videoOutputBox.border.color = "red"
                recorder.record()
            }
            anchors {
                left: screenShotButton.right
                top: parent.top
            }
        }

        Button {
            id: stopButton
            height: parent.height
            width: parent.width/6
            text: "Stop/Save"
            visible: recorder.recorderState === MediaRecorder.RecordingState
            onClicked:{
                videoOutputBox.border.color = "black"
                recorder.stop()
            }
            anchors {
                left: screenShotButton.right
                top: parent.top
            }
        }

        Button {
            id: setPathButton
            height: parent.height
            width: parent.width/6
            anchors {
                left: stopButton.right
                top: parent.top
            }
            text: "Set Path"
            onClicked: {
                folderDialog.open()
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

    MediaDevices {
        id: mediaDevices
        onVideoInputsChanged: {
            console.log("Changed device list")
            changeCamera()
        }
    }

    CaptureSession {
        id: captureSession
        camera: video_status ? cameraUser.previewCamera : cameraUser.camera_device
        videoOutput: videoOutput
        audioInput: AudioInput {}
            recorder: MediaRecorder {
                id: recorder
                outputLocation: cameraUser.getPath() + "/" + "vid_" +
                                new Date().toISOString().replace(/[:.]/g, "-") + ".mp4";
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


    function changeCamera() {
        if (cameraBox.currentIndex >= 0) {
            cameraUser.createCamera(camera_id_list[cameraBox.currentIndex])
            console.log("changeCamera|||" + camera_id_list[cameraBox.currentIndex])
        }
    }

    Connections {
        target: cameraUser
        function onDeviceListChanged(id_list, name_list) {
            camera_id_list = id_list.toString().split(',')
            camera_name_list = name_list.toString().split(',')

            console.log("Converted camera_id_list:", camera_id_list)
            console.log("First camera ID:", camera_id_list[0])

            if (camera_id_list.length > 0) {
                changeCamera()
            }
        }
        function onFrameCaptured(path) {
            console.log("Frame captured:", path)
        }
        function onPathChanged(path){
            console.log("Path changed to:", path)
        }

    }

    Component.onCompleted: {
        if (mediaDevices.videoInputs.length > 0 && camera_id_list.length > 0) {
            cameraUser.createCamera(camera_id_list[0])
        }
    }
}
