import QtQuick 2.0
import QtQuick.Controls 2.2 as QQC2
import QtMultimedia 5.9

Item {
    property alias snapshotUri: snapshotViewer.snapshotUri
    property alias snapshotInterval: snapshotViewer.interval
    property alias streamUri: streamViewer.streamUri
    property real aspectRatio: snapshotViewer.visible ? snapshotViewer.aspectRatio : 2

    QQC2.Label {
        id: loadingLabel
        text: qsTr("Loading...")
        visible: !snapshotViewer.visible && !streamViewer.visible
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
        font.pixelSize: 12
    }

    OnvifSnapshotViewer {
        id: snapshotViewer
        anchors.fill: parent
        visible: snapshotViewer.isSnapshotAvailable() && !streamViewer.visible
    }

    OnvifStreamViewer {
        id: streamViewer
        anchors.fill: parent
        visible: streamViewer.isStreamAvailable()
    }
}
