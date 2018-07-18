﻿/* Copyright (C) 2018 Casper Meijn <casper@meijn.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import org.kde.kirigami 2.3 as Kirigami
import QtQml.Models 2.1
import QtQuick 2.9
import QtQuick.Controls 2.3 as QQC2
import QtQuick.Layouts 1.3 as QQL

Kirigami.ScrollablePage {
    id: pageOverview
    title: "Overview"
    objectName: "overviewPage"

    actions {
        main: Kirigami.Action {
            iconName: "list-add"
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
            onClicked: {
                selectedIndex = index
                pageStack.pop(pageOverview);
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
                            icon.name: "settings-configure"
                            icon.width: Kirigami.Units.iconSizes.smallMedium
                            icon.height: Kirigami.Units.iconSizes.smallMedium

                            onCanceled: {
                                // For some reason clicks on this button are canceled if the focus is not on this page
                                onClicked()
                            }

                            onClicked: {
                                selectedIndex = index
                                pageStack.pop(pageOverview);
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

                QQC2.Label {
                    id: snapshotUnsupportedText
                    text: "The camera doesn't support the retrieval of snapshots."
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    visible: !model.errorString && !model.supportsSnapshotUri
                    QQL.Layout.fillWidth: true
                    QQL.Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                }

                OnvifCameraViewer {
                    id: viewerItem
                    snapshotUri: model.snapshotUri
                    snapshotInterval: 5000
                    visible: !model.errorString && model.supportsSnapshotUri
                    QQL.Layout.fillWidth: true
                    QQL.Layout.preferredHeight: parent.width / viewerItem.aspectRatio
                }
            }
        }
    }
}
