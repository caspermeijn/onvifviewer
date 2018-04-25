import QtQuick 2.0
import QtMultimedia 5.9

Item {
    id: viewer

    property alias streamUri: video.source

    function isStreamAvailable() {
        return video.playbackState === MediaPlayer.PlayingState && video.hasVideo && video.source
    }

    onVisibleChanged: {
        if(viewer.visible)
            video.play()
        else
            video.stop()
    }

    Video {
        id: video
        muted: true
        anchors.fill: parent

        autoLoad: true
        autoPlay: true
    }
}
