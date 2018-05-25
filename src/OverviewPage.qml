import QtQuick 2.6
import QtQuick.Layouts 1.3 as QQL
import QtQuick.Controls 2.2 as QQC2
import org.kde.kirigami 2.3 as Kirigami
import QtQml.Models 2.1

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

            width: Math.min(parent.width, Kirigami.Units.gridUnit * 20)

            QQL.ColumnLayout {
                id: layout
                spacing: Kirigami.Units.smallSpacing


                QQC2.ToolBar {
                    QQL.Layout.fillWidth: true
                    QQL.RowLayout {
                        anchors.fill: parent
                        QQC2.Label {
                            text: model.deviceName || "Camera " + (model.index + 1)
                            elide: QQC2.Label.ElideRight
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            QQL.Layout.fillWidth: true
                        }
                        QQC2.ToolButton {
                            icon.name: "document-properties"
                            icon.source: "3rd-party/breeze-icons/document-properties.svg"
                            icon.width: Kirigami.Units.iconSizes.smallMedium
                            icon.height: icon.width

                            onClicked: {
                                selectedIndex = index
                                pageStack.push(settingsComponent);
                            }
                        }
                    }
                }

                QQC2.Label {
                    id: errorText
                    text: "An error occured during communication with the camera."
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    visible: model.errorString
                    QQL.Layout.fillWidth: true
                    QQL.Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                }

                OnvifCameraViewer {
                    id: viewerItem
                    snapshotUri: model.snapshotUri
                    snapshotInterval: 5000
                    visible: !model.errorString
                    QQL.Layout.fillWidth: true
                    QQL.Layout.preferredHeight: parent.width / viewerItem.aspectRatio
                }
            }
        }
    }
}
