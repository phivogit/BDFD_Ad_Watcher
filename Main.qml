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
    title: "BDFD Ad Watcher v.4.0"
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
        id: stackLayout
        currentIndex: bar.currentIndex
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bar.top
        anchors.leftMargin: parent.width*0.01
        anchors.rightMargin: parent.width*0.01
        Rectangle {
            id: tab1
            width: parent.width
            height: parent.height
            Text {
                id: title
                text: "BDFD Ad watcher 4.0"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Math.min(Math.max(12, parent.width * 0.1), 40)
            }
            Text {
                id: title2
                text: "Bluestacks Portrait layout, 900x1600."
                anchors.left: parent.left
                anchors.top: title.bottom
                leftPadding: parent.width*0.01
            }
            Rectangle {
                id: outerBorder
                border.width: 2
                anchors.top: title2.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                Rectangle {
                    id: mainBorder
                    anchors.top: parent.top
                    anchors.left: parent.left
                    width: Math.min(Math.max(19, parentWindow.width * 0.25), 1000)
                    anchors.bottom: parent.bottom
                    color: "transparent"
                    border.color: "black"
                    ColumnLayout {
                        id: maincol
                        anchors.fill: parent
                        anchors.margins: 10
                        Button {
                            id: connectButton
                            contentItem: Text {
                                text: "Connect / Disconnect to bluestacks"
                                wrapMode: Text.WordWrap
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            onClicked: {
                                backend.connectBlueStacks()
                            }
                        }
                        Button {
                            id: startButton
                            text: "Start"
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            onClicked: {
                                backend.start()
                            }
                        }
                        Button {
                            id: stopButton
                            text: "Stop"
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            onClicked: {
                                backend.stop()
                            }
                        }
                    }
                }
                Rectangle {
                    id: outputFrame
                    anchors.left: mainBorder.right
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    color: "transparent"
                    border.color: "black"
                    clip: true
                    Text {
                        id: outputTitle
                        text: "Output"
                        anchors.top: parent.top
                        anchors.horizontalCenter: outputFrame.horizontalCenter
                        font.bold: true
                        height: outputFrame.height * 0.1
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
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
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
            }
        }
        Rectangle {
            id: configurationsTab
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            anchors.topMargin: parent.height*0.05
            anchors.leftMargin: parentWindow.width*0.01
            anchors.rightMargin: parentWindow.width*0.01
            Text {
                id: configNote
                text: "Connect to bluestacks before setting anything."
                anchors.top: parent.top
                anchors.left: parent.left
                height: parent.height*0.015
            }

            Text {
                id: adbTitle
                anchors.top: configNote.bottom
                anchors.topMargin: 5
                anchors.left: parent.left
                text: "Bluestacks Android Debug Bridge Port (example: 127.0.0.1:5555): "
            }
            TextField {
                id: adbPortInput
                anchors.left: adbTitle.right
                anchors.verticalCenter: adbTitle.verticalCenter
                text: "127.0.0.1:5555"
                onTextEdited: backend.updatePort(adbPortInput.text)
            }
            Rectangle {
                id: config1
                border.color: "black"
                border.width: 5
                height: parent.height*0.15
                anchors.top: adbTitle.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: parentWindow.height*0.01
                anchors.leftMargin: parentWindow.width*0.01
                anchors.rightMargin: parentWindow.width*0.01
                RowLayout {
                    width: parent.width
                    height: parent.height
                    anchors.margins: 10
                    anchors.fill: parent
                    spacing: 10
                    Button {
                        id: btnGetPos
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Get Position + Color"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 0
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture Screen"
                        onClicked: backend.captureScreen()
                    }
                }
            }
            Rectangle {
                id: config2
                border.color: "black"
                border.width: 5
                height: parent.height*0.15
                anchors.top: config1.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: parentWindow.height*0.01
                anchors.leftMargin: parentWindow.width*0.01
                anchors.rightMargin: parentWindow.width*0.01
                RowLayout {
                    width: parent.width
                    height: parent.height
                    anchors.margins: 10
                    anchors.fill: parent
                    spacing: 10
                    Button {
                        id: textInputAdButtonLocation
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Set Ad Button Location"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 3
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id: getLocXButton1
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Set Main X button location"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 1
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id:getLocXButton2
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Set X button 2 location"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 2
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id:getAdContinueButton
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Set Ad Continue Button location"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 7
                                       getPosScreen.visible = true
                                   }
                    }
                }
            }
            Rectangle {
                id: config3
                border.color: "black"
                border.width: 5
                height: parent.height*0.15
                anchors.top: config2.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: parentWindow.height*0.01
                anchors.leftMargin: parentWindow.width*0.01
                anchors.rightMargin: parentWindow.width*0.01
                RowLayout {
                    width: parent.width
                    height: parent.height
                    anchors.margins: 10
                    anchors.fill: parent
                    spacing: 10
                    Button {
                        id: getLocAdWatchButton
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Set Ad Watch Button location"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 3
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id:getLocCaptchaTextBox
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Set Captcha Textbox location"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 4
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id:getLocCaptchaConfirm
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Set Captcha Confirm Button location"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 5
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id:getLocXButton1_left
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Set left X button location"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 5
                                       getPosScreen.visible = true
                                   }
                    }
                }
            }
            Rectangle {
                id: configSeparator
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: config3.bottom
                height: parent.height*0.05
                anchors.topMargin: parentWindow.height*0.01
                border.width: 2
                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    text: "Capture Templates"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            Rectangle {
                id: config4
                border.color: "black"
                border.width: 1
                height: parent.height*0.1
                anchors.top: configSeparator.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: parentWindow.height*0.01
                anchors.leftMargin: parentWindow.width*0.01
                anchors.rightMargin: parentWindow.width*0.01
                RowLayout {
                    width: parent.width
                    height: parent.height
                    anchors.margins: 3
                    anchors.fill: parent
                    spacing: 10
                    Button {
                        id: captureAdWatchButton
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture Ad Watch Button"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "adWatchButton"
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id: captureXButton1
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture white-themed X button"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "XButton1"
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id: captureXButton1_black
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture dark-themed X button"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "XButton1_black"
                                       getPosScreen.visible = true
                                   }
                    }
                }
            }
            Rectangle {
                id: config5
                border.color: "black"
                border.width: 1
                height: parent.height*0.1
                anchors.top: config4.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: parentWindow.width*0.01
                anchors.rightMargin: parentWindow.width*0.01
                RowLayout {
                    width: parent.width
                    height: parent.height
                    anchors.margins: 3
                    anchors.fill: parent
                    spacing: 10
                    Button {
                        id: captureXButton1_skip
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture skip ad button"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "XButton1_skip"
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id: captureXButton2
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture popup X button"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "XButton2"
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id: captureCaptchaTextBox
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture captcha textbox"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "captchaTextbox"
                                       getPosScreen.visible = true
                                   }
                    }
                }
            }
            Rectangle {
                id: config6
                border.color: "black"
                border.width: 1
                height: parent.height*0.1
                anchors.top: config5.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: parentWindow.width*0.01
                anchors.rightMargin: parentWindow.width*0.01
                RowLayout {
                    width: parent.width
                    height: parent.height
                    anchors.margins: 3
                    anchors.fill: parent
                    spacing: 10
                    Button {
                        id: captureSuccessScreen
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture success message"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "successScreen"
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id: captureAdContinueScreen
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture ad continue screen"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "adContinueScreen"
                                       getPosScreen.visible = true
                                   }
                    }
                    Button {
                        id: captureAdWaitingScreen
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: "Capture ad waiting screen"
                        onClicked: () => {
                                       backend.captureScreen()
                                       getPosScreen.mode = 6
                                       getPosScreen.widgetName = "adWaitingScreen"
                                       getPosScreen.visible = true
                                   }
                    }
                }
            }
            Rectangle {
                id: getPosScreen
                anchors.fill: parent
                visible: false
                property string widgetName: "placeholder"
                property int clickCount: 1
                property int mode: 0
                Image {
                    id: getPosImg
                    source: backend.imgPath
                    anchors.fill: parent
                    MouseArea {
                        anchors.fill: parent
                        onClicked: (mouse) => {
                                        if (getPosScreen.mode == 0) {
                                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 0)
                                            getPosScreen.visible = false
                                        } else if (getPosScreen.mode == 1) {
                                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 1)
                                            getPosScreen.visible = false
                                        } else if (getPosScreen.mode == 2) {
                                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 2)
                                            getPosScreen.visible = false
                                        } else if (getPosScreen.mode == 3) {
                                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 3)
                                            getPosScreen.visible = false
                                        } else if (getPosScreen.mode == 4) {
                                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 4)
                                            getPosScreen.visible = false
                                        } else if (getPosScreen.mode == 5) {
                                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 5)
                                            getPosScreen.visible = false
                                        } else if (getPosScreen.mode == 6) {
                                            if (getPosScreen.clickCount <2) {
                                               backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 6, getPosScreen.clickCount)
                                               getPosScreen.clickCount+=1
                                           } else {
                                               backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 6, getPosScreen.clickCount, getPosScreen.widgetName)
                                               getPosScreen.clickCount = 1
                                               getPosScreen.widgetName = "placeholder"
                                               getPosScreen.visible = false
                                           }
                                        } else if (getPosScreen.mode == 7) {
                                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 7)
                                            getPosScreen.visible = false
                                        } else if (getPosScreen.mode == 8) {
                                            backend.getPos(mouse.x, mouse.y, parent.width, parent.height, 8)
                                            getPosScreen.visible = false
                                        }
                                   }
                    }
                }
                Button {
                    id: getPosCancel
                    text: "Cancel"
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    height: parent.height * 0.05
                    width: parent.width * 0.1
                    onClicked: () => {
                        getPosScreen.visible = false
                        getPosScreen.clickCount = 1
                        getPosScreen.widgetName = "placeholder"
                    }
                }
            }
        }
    }
}


