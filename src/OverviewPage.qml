import QtQuick 2.6
import QtQuick.Layouts 1.3 as QQL
import QtQuick.Controls 2.2 as QQC2
import org.kde.kirigami 2.3 as Kirigami
import QtQml.Models 2.1
import org.kde.kirigami 2.2

Kirigami.ScrollablePage {
    title: "Overview"
    objectName: "overviewPage"

    actions {
        main: Kirigami.Action {
            iconName: "document-new"
            onTriggered: {
                selectedIndex = deviceManager.appendDevice()
                pageStack.push(settingsComponent);
            }
        }
    }

    ListView {
        id: view
        model: deviceManagerModel
        anchors.fill: parent

        delegate: Kirigami.AbstractListItem {
            onPressed: {
                selectedIndex = index
                pageStack.push(deviceViewerComponent);
            }

            QQL.ColumnLayout {
                id: containerLayout

                QQL.ColumnLayout {
                    id: layout
                    spacing: Units.smallSpacing*2
                    QQL.Layout.maximumWidth: 300

                    QQL.RowLayout {
                        QQC2.Label {
                            id: labelItem
                            text: model.deviceName
                            QQL.Layout.fillWidth: true

                            elide: Text.ElideRight
                            horizontalAlignment: Qt.AlignHCenter
                        }

                        QQC2.ToolButton {
                            icon.name: "document-properties"

                            onClicked: {
                                selectedIndex = index
                                pageStack.push(settingsComponent);
                            }
                        }
                    }
                    Item {
                        QQL.Layout.fillWidth: true
                        QQL.Layout.maximumWidth: 300
                        QQL.Layout.preferredHeight: width / viewerItem.aspectRatio
                        Text {
                            id: errorText
                            text: "An error occured during communication with the camera."
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            anchors.fill: parent
                            anchors.margins: 25
                            visible: model.errorString

                        }
                        OnvifCameraViewer {
                            id: viewerItem
                            snapshotUri: model.snapshotUri
                            snapshotInterval: 5000
                            anchors.fill: parent
                            visible: !model.errorString
                        }
                    }
                }
            }
        }
    }
}
