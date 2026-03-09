import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: parentWindow
    width: 640
    height: 480
    visible: true
    minimumWidth: 400
    minimumHeight: 100
    title: "BDFD Ad Watcher Ver 3.0"
    Text {
        id: title
        text: "BDFD Ad watcher"
        anchors.horizontalCenter: parent.horizontalCenter
        topPadding: 20
        font.pixelSize: Math.min(Math.max(12, parent.width * 0.1), 40)
    }
    TabBar {
        id: bar
        width: parent.width
        anchors.bottom: parent.bottom
        TabButton {
            text: qsTr("Main Tab")
        }
        TabButton {
            text: qsTr("Configurations")
        }
    }
    StackLayout {
        width: parent.width
        currentIndex: bar.currentIndex
        Rectangle {
            id: tab1

            Rectangle {
                id: mainBorder
                height: Math.min(Math.max(20, parentWindow.height * 0.7), 1030)
                width: Math.min(Math.max(19, parentWindow.width * 0.25), 1000)
                x: parentWindow.width * 0.05
                y: parentWindow.height * 0.2

                color: "transparent"
                border.color: "black"
                border.width: 2
                ColumnLayout {
                    id: maincol
                    anchors.fill: parent
                    anchors.margins: 20
                    Button {
                        id: connectButton
                        text: "Connect / Disconnect to bluestacks"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        onClicked: {
                            backend.connectBlueStacks()
                        }
                    }
                    Button {
                        id: startButton
                        text: "Start"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        onClicked: {
                            backend.start()
                        }
                    }
                    Button {
                        id: stopButton
                        text: "Stop"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        onClicked: {
                            backend.stop()
                        }
                    }
                }
            }

            Rectangle {
                id: debugBorder
                height: Math.min(Math.max(20, parentWindow.height * 0.7), 1030)
                width: Math.min(Math.max(5, (parentWindow.width - mainBorder.width - outputFrame.width - parentWindow.width * 0.05) * 0.9), 1000)
                anchors.left: outputFrame.right
                y: parentWindow.height * 0.2

                color: "transparent"
                border.color: "black"
                border.width: 2
                ColumnLayout {
                    id: debugcol
                    anchors.fill: parent
                    anchors.margins: 20
                    Button {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture Screen"
                        onClicked: backend.captureScreen()
                    }
                    Button {
                        id: getPosButton
                        text: "Get Position + Color"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id: getData
                        text: "Get training data"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        onClicked: backend.startTraining()
                    }
                }
            }
            Rectangle {
                id: outputFrame
                height: Math.min(Math.max(20, parentWindow.height * 0.4), 1030)
                anchors.left: mainBorder.right
                width: Math.min(Math.max(5, parentWindow.width * 0.4), 1000)
                anchors.bottom: mainBorder.bottom

                color: "transparent"
                border.color: "black"
                border.width: 2
                clip: true
                Text {
                    id: outputTitle
                    text: "Output"
                    anchors.top: parent.top
                    anchors.horizontalCenter: outputFrame.horizontalCenter
                    font.bold: true
                    height: outputFrame.height * 0.2
                    width: outputFrame.width * 0.1
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 5
                    font.pixelSize: 75
                }
                ScrollView {
                    id: outputScroll
                    anchors.top: outputTitle.bottom
                    anchors.bottom: outputFrame.bottom // Fills the remaining space!
                    anchors.left: outputFrame.left
                    anchors.right: outputFrame.right
                    anchors.margins: 10
                    clip: true // Ensures text doesn't spill out

                    TextArea {
                        id: outputText
                        readOnly: true
                        wrapMode: Text.Wrap
                        text: ">>"

                        Connections {
                            target: backend
                            function onLogUpdated(msg) {
                                outputText.append(">> " + msg)
                                outputText.cursorPosition = outputText.length - 1
                            }
                        }
                    }
                }

            }
            Rectangle {
                id: getPosScreen
                anchors.fill: parent
                visible: false
                Image {
                    id: getPosImg
                    source: backend.imgPath
                    anchors.fill: parent
                    MouseArea {
                        anchors.fill: parent
                        onClicked: (mouse) => {
                                        backend.getPos(mouse.x, mouse.y, parent.width, parent.height)
                                       getPosScreen.visible = false
                                   }
                    }
                }
                Button {
                    id: getPosCancel
                    text: "Cancel"
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    height: parent.height * 0.1
                    width: parent.width * 0.2
                    onClicked: () => {
                        getPosScreen.visible = false
                    }
                }
            }
        }
        Rectangle {
            id: configurationsTab
            Text {
                id: adbTitle
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: parentWindow.height*0.2
                anchors.leftMargin: parentWindow.width*0.01
                text: "Bluestacks Android Debug Bridge Port (example: 127.0.0.1:5555): "
            }
            TextField {
                id: adbPortInput
                anchors.left: adbTitle.right
                anchors.verticalCenter: adbTitle.verticalCenter
                text: "127.0.0.1:5555"
                onTextEdited: backend.updatePort(adbPortInput.text)
            }
        }
    }
}


