import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import VideoRecorder 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: "VideoRecorder"

    VideoRecorder {
        id: videoRecorder
    }

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
            model: videoRecorder.cameraList
            height: parent.height
            width: parent.width/6
            anchors {
                left: parent.left
                top: parent.top
            }
            onActivated: videoRecorder.setCamera(currentIndex)
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
            onClicked: videoRecorder.captureFrame()
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
            onClicked: folderDialog.open()
        }
    }

    Rectangle {
        id: videoOutputBox
        height: parent.height - options.height
        width: parent.width
        border.color: "black"
        anchors {
            top: options.bottom
            left: parent.left
        }

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: videoRecorder.frame
            cache: false
        }
    }

    FolderDialog {
        id: folderDialog
        currentFolder: videoRecorder.currentPath
        onAccepted: videoRecorder.setCurrentPath(selectedFolder)
    }
}
