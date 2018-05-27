import QtQuick 2.1
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.0 as Kirigami
import net.meijn.onvifviewer 1.0

Kirigami.Page {
    property OnvifDevice selectedDevice: deviceManager.at(selectedIndex)

    title: selectedDevice.deviceName
    actions {
        main: Kirigami.Action {
            visible: selectedDevice.isPtzSupported
            iconName: "transform-move"
            onTriggered: {
                print("Action button in buttons page clicked");
                ptzOverlay.sheetOpen = !ptzOverlay.sheetOpen
            }
        }
        contextualActions: [
            Kirigami.Action {
                text:"Device information"
                iconName: "help-about"
                onTriggered: {
                    deviceInformation.sheetOpen = !deviceInformation.sheetOpen
                }
            }
        ]
    }
    Kirigami.OverlaySheet {
        id: ptzOverlay
        Column{
            QQC2.ToolButton {
                icon.name: "go-up"
                icon.source: "3rd-party/breeze-icons/go-up.svg"
                icon.width: Kirigami.Units.iconSizes.medium
                icon.height: Kirigami.Units.iconSizes.medium
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    selectedDevice.ptzUp()
                }
            }
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                QQC2.ToolButton {
                    icon.name: "go-previous"
                    icon.source: "3rd-party/breeze-icons/go-previous.svg"
                    icon.width: Kirigami.Units.iconSizes.medium
                    icon.height: Kirigami.Units.iconSizes.medium
                    onClicked: {
                        selectedDevice.ptzLeft()
                    }
                }
                QQC2.ToolButton {
                    visible: selectedDevice.isPtzHomeSupported
                    icon.name: "go-home"
                    icon.source: "3rd-party/breeze-icons/go-home.svg"
                    icon.width: Kirigami.Units.iconSizes.medium
                    icon.height: Kirigami.Units.iconSizes.medium
                    onClicked: {
                        selectedDevice.ptzHome()
                    }
                    onPressAndHold: {
                        console.log("onPressAndHold")
                        selectedDevice.ptzSaveHomePosition()
                        showPassiveNotification("Saving current position as home")
                    }
                }
                QQC2.ToolButton {
                    icon.name: "go-next"
                    icon.source: "3rd-party/breeze-icons/go-next.svg"
                    icon.width: Kirigami.Units.iconSizes.medium
                    icon.height: Kirigami.Units.iconSizes.medium
                    onClicked: {
                        selectedDevice.ptzRight()
                    }
                }
            }
            QQC2.ToolButton {
                icon.name: "go-down"
                icon.source: "3rd-party/breeze-icons/go-down.svg"
                icon.width: Kirigami.Units.iconSizes.medium
                icon.height: Kirigami.Units.iconSizes.medium
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    selectedDevice.ptzDown()
                }
            }
        }
    }
    Kirigami.OverlaySheet {
        id: deviceInformation
        GridLayout {
            columns: 2
            anchors.margins: Kirigami.Units.gridUnit / 2

            Kirigami.Heading {
                text: "Device information"
                Layout.columnSpan: 2
                level: 2
            }

            QQC2.Label {
                text: "Manufacturer:"
            }
            QQC2.Label {
                text: selectedDevice.deviceInformation.manufacturer
            }

            QQC2.Label {
                text: "Model:"
            }
            QQC2.Label {
                text: selectedDevice.deviceInformation.model
            }

            QQC2.Label {
                text: "Firmware version:"
            }
            QQC2.Label {
                text: selectedDevice.deviceInformation.firmwareVersion
            }

            QQC2.Label {
                text: "Serial number:"
            }
            QQC2.Label {
                text: selectedDevice.deviceInformation.serialNumber
            }

            QQC2.Label {
                text: "Hardware identifier:"
            }
            QQC2.Label {
                text: selectedDevice.deviceInformation.hardwareId
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
            QQC2.ToolButton {
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
