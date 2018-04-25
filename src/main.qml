import QtQuick 2.9
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Controls 1.4 as QQC
import org.kde.kirigami 2.0 as Kirigami

Kirigami.ApplicationWindow {
    id: root

    property int selectedIndex: 0

    header: Kirigami.ApplicationHeader {}
    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }
    pageStack.initialPage: overviewComponent
    Component {
        id: deviceViewerComponent
        DeviceViewerPage{}
    }
    Component {
        id: settingsComponent
        DeviceSettingsPage{}
    }
    Component {
        id: overviewComponent
        OverviewPage{}
    }
}

