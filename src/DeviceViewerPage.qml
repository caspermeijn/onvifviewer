import QtQuick 2.1
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.0 as Kirigami
import net.meijn.onvifviewer 1.0

Kirigami.Page {
    property OnvifDevice selectedDevice: deviceManager.at(selectedIndex)

    title: selectedDevice.deviceName
    actions {
        main: Kirigami.Action {
            visible: selectedDevice.isPtzSupported
            iconName: "zoom-in"
            onTriggered: {
                print("Action button in buttons page clicked");
                sheet.sheetOpen = !sheet.sheetOpen
            }
        }
    }
    Kirigami.OverlaySheet {
        id: sheet
        Column{
            QQC2.Button {
                icon.name: "go-up"
                icon.source: "3rd-party/breeze-icons/go-up.svg"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    selectedDevice.ptzUp()
                }
            }
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                QQC2.Button {
                    icon.name: "go-previous"
                    icon.source: "3rd-party/breeze-icons/go-previous.svg"
                    onClicked: {
                        selectedDevice.ptzLeft()
                    }
                }
                QQC2.Button {
                    visible: selectedDevice.isPtzHomeSupported
                    icon.name: "go-home"
                    icon.source: "3rd-party/breeze-icons/go-home.svg"
                    onClicked: {
                        selectedDevice.ptzHome()
                    }
                    onPressAndHold: {
                        console.log("onPressAndHold")
                        selectedDevice.ptzSaveHomePosition()
                        showPassiveNotification("Saving current position as home")
                    }
                }
                QQC2.Button {
                    icon.name: "go-next"
                    icon.source: "3rd-party/breeze-icons/go-next.svg"
                    onClicked: {
                        selectedDevice.ptzRight()
                    }
                }
            }
            QQC2.Button {
                icon.name: "go-down"
                icon.source: "3rd-party/breeze-icons/go-down.svg"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    selectedDevice.ptzDown()
                }
            }
        }

    }
    Item {
        anchors.fill: parent
        Column{
            anchors.fill: parent
            Text {
                id: errorText
                text: "An error occured during communication with the camera.\n\nTechnical details:\n" + selectedDevice.errorString
                wrapMode: Text.Wrap
                visible: selectedDevice.errorString
                width: parent.width
            }
            QQC2.Button {
                icon.name: "view-refresh"
                icon.source: "3rd-party/breeze-icons/view-refresh.svg"
                onClicked: {
                    selectedDevice.reconnectToDevice()
                }
                visible: selectedDevice.errorString
            }
        }
        OnvifCameraViewer {
            id: viewerItem
            objectName: "cameraViewer"
            anchors.fill: parent
            snapshotUri: selectedDevice.snapshotUri
            streamUri: selectedDevice.streamUri
            visible: !selectedDevice.errorString
        }
    }
}
