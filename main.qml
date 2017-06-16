import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 2.1 as Quick
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2


ApplicationWindow {
    id: root
    property int imgSize : 38
    visible: false
    width: 420
    height: 420
    title: qsTr("WinConnect")

    Rectangle {
        id: rectangle
        anchors.fill: parent
        color: "#ffffff"

        Column{
            anchors.centerIn: parent
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                Label {
                    id: label
                    x: 21
                    y: 10
                    width: 154
                    height: 43
                    text: qsTr("<b>Database</b>")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 15
                }

                ToolButton {
                    id: toolButton
                    x: 215
                    y: 10
                    width: root.imgSize
                    height: width
                    text: qsTr("Tool Button")
                    contentItem : Image{
                        source: "qrc:/images/config.png"
                    }
                    onClicked: dbConfig.open()


                }

                Image {
                    id: image
                    width: root.imgSize
                    height: width
                    x: 287
                    y: 10
                    source: Database.connected ? "qrc:/images/check.png" : "qrc:/images/uncheck.png"
                }


            }

            Row{
                Label {
                    id: label1
                    x: 21
                    y: 10
                    width: 154
                    height: 43
                    text: qsTr("<b>WinMedia</b>")
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }

                ToolButton {
                    id: toolButton1
                    x: 215
                    y: 10
                    width: root.imgSize
                    height: width
                    text: qsTr("Tool Button")
                    contentItem : Image{
                        source: "qrc:/images/config.png"
                    }
                    onClicked: winConfig.open()
                }

                Image {
                    id: image1
                    x: 287
                    y: 10
                    width: root.imgSize
                    height: width
                    source: Rami.connected ? "qrc:/images/check.png" : "qrc:/images/uncheck.png"
                }

            }

            Row{
                Label {
                    id: label2
                    x: 21
                    y: 10
                    width: 154
                    height: 43
                    text: qsTr("<b>Cartridge</b>")
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }

                ToolButton {
                    id: toolButton2
                    x: 215
                    y: 10
                    width: root.imgSize
                    height: width
                    text: qsTr("Tool Button")
                    contentItem : Image{
                        source: "qrc:/images/config.png"
                    }
                    onClicked: cartConfig.open()
                }

                Image {
                    id: image2
                    x: 287
                    y: 10
                    width: root.imgSize
                    height: width
                    source: Notifier.connected ? "qrc:/images/check.png" : "qrc:/images/uncheck.png"
                }
            }
        }
    }
    Quick.Dialog{
        id: dbConfig
        title: qsTr("Database Connection")
        x: Math.round((root.width - width) / 2)
        y: Math.round((root.height - height) / 2)
        width: Math.round(Math.min(root.width, root.height) / 3 * 2)
        modal: true
        focus: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            Options.dbIp = dbipinput.text
            Options.dbPort = dbportinput.value
            Options.dbUser = dbuserinput.text
            Options.dbPass = dbpassinput.text
            Options.persistConfig("db")
        }
        contentItem:
        Column{
            Label{
                text: qsTr("ip")
            }
            TextField{
                id: dbipinput
                inputMethodHints: Qt.ImhDigitsOnly
                text: Options.dbIp
                Keys.onReturnPressed: {
                }
            }
            Label{
                text: qsTr("port")
            }
            SpinBox{
                id: dbportinput
                editable: true
                to: 65535
                value: Options.dbPort
            }
            Label{
                text: qsTr("user")
            }
            TextField{
                id: dbuserinput
                inputMethodHints: Qt.ImhDigitsOnly
                text: Options.dbUser
                Keys.onReturnPressed: {
//                    serverportinput.focus = true
                }
            }
            Label{
                text: qsTr("password")
            }
            TextField{
                id: dbpassinput
                inputMethodHints: Qt.ImhDigitsOnly
                text: Options.dbPass
                Keys.onReturnPressed: {
//                    serverportinput.focus = true
                }
            }
        }
    }

    Quick.Dialog{
        id: winConfig
        title: qsTr("WinMedia Connection")
        x: Math.round((root.width - width) / 2)
        y: Math.round((root.height - height) / 2)
        width: Math.round(Math.min(root.width, root.height) / 3 * 2)
        modal: true
        focus: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            Options.winPort = winportinput.value
            Options.persistConfig("win")
        }
        contentItem:
        Column{
            Text{
                text: "The port on which to listen for winMedia to connect"
            }

            Label{
                text: qsTr("port")
            }
            SpinBox{
                id: winportinput
                editable: true
                to: 65535
                value: Options.winPort
            }

        }
    }

    Quick.Dialog{
        id: cartConfig
        title: qsTr("Mobile/Tablet connection")
        x: Math.round((root.width - width) / 2)
        y: Math.round((root.height - height) / 2)
        width: Math.round(Math.min(root.width, root.height) / 3 * 2)
        modal: true
        focus: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            Options.appPort= cartportinput.value
            Options.appIp = cartipinput.text
            Options.persistConfig("app")
        }
        contentItem:
        Column{
            Label{
                text: qsTr("ip")
            }
            TextField{
                id: cartipinput
                inputMethodHints: Qt.ImhDigitsOnly
                text: Options.appIp
                Keys.onReturnPressed: {
                }
            }

            Text{
                text: "The port on which to listen for winMedia to connect"
            }

            Label{
                text: qsTr("port")
            }
            SpinBox{
                id: cartportinput
                editable: true
                to: 65535
                value: Options.appPort
            }

        }
    }
}
