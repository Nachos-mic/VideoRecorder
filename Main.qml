import QtQuick
import QtQuick.Window
import QtMultimedia
import QtQuick.Controls

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
            id: recordVideoButton
            height: parent.height
            width: parent.width/6
            anchors {
                left: screenShotButton.right
                top: parent.top
            }
            text: "Capture a Video"
            onClicked: {
                if (cameraBox.currentIndex >= 0 && video_status === false) {
                    //cameraUser.captureVideo()
                    border_video_status = "red"
                    video_status = true
                }
                else if(video_status === true){
                    //camera.User.saveVideo()
                    border_video_status = "black"
                    video_status = false
                }
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
            changeCamera()
        }
    }

    CaptureSession {
        id: captureSession
        camera: cameraUser.camera_device
        videoOutput: videoOutput
    }

    Rectangle{

        id:videoOutputBox
        height: parent.height - options.height
        width: parent.width
        border.color: video_status

        anchors {
            top: options.bottom
            left: parent.left
        }

        VideoOutput {
            id: videoOutput
            anchors {
                centerIn: parent.center
            }
            height: parent.height
            width: parent.width
        }
    }

    function changeCamera() {
        if (cameraBox.currentIndex >= 0) {
            cameraUser.createCamera(camera_id_list[cameraBox.currentIndex])
            console.log(camera_id_list[0])
            console.log("changeCamera|||" + camera_id_list[cameraBox.currentIndex])
        }
    }

    // function emergencyDeviceChange() {
    //     if (mediaDevices.videoInputs.length > 0 && camera_id_list.length > 0) {
    //         cameraUser.createCamera(camera_id_list[0])
    //         console.log("emergencyDeviceChange|||")
    //     }
    // }

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
        function onFrameCapture(path) {
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
