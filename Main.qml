import QtQuick
import QtQuick.Window
import QtMultimedia
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    property string camera_id: ""
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
            id: setPathButton
            height: parent.height
            width: parent.width/6
            anchors {
                left: screenShotButton.right
                top: parent.top
            }
            text: "Set Path"
            onClicked: {
                pathSetPop.open()
            }
        }
    }

    PathPopUp{
        id: pathSetPop
    }

    MediaDevices {
        id: mediaDevices
        onVideoInputsChanged: {
            console.log("Changed device list")
            emergencyDeviceChange()
        }
    }

    CaptureSession {
        id: captureSession
        camera: cameraUser.camera_device
        videoOutput: videoOutput
    }

    VideoOutput {
        id: videoOutput
        anchors {
            top: options.bottom
            left: parent.left
        }
        height: parent.height - options.height
        width: parent.width
    }

    function changeCamera() {
        if (cameraBox.currentIndex >= 0) {
            cameraUser.createCamera(camera_id_list[cameraBox.currentIndex])
        }
    }

    function emergencyDeviceChange() {
        if (mediaDevices.videoInputs.length > 0 && camera_id_list.length > 0) {
            cameraUser.createCamera(camera_id_list[0])
        }
    }

    Connections {
        target: cameraUser
        function onDeviceListChanged(id_list, name_list) {
            camera_id_list = id_list
            camera_name_list = name_list
            if (camera_id_list.length > 0) {
                changeCamera()
            }
        }
        function onFrameCaptured(path) {
            console.log("Frame captured:", path)
        }
        function onSetCaptureImgPathChanged(path){
            console.log("Path changed to:", path)
        }
    }

    Component.onCompleted: {
        if (mediaDevices.videoInputs.length > 0 && camera_id_list.length > 0) {
            cameraUser.createCamera(camera_id_list[0])
        }
    }
}
