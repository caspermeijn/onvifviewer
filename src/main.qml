/* Copyright (C) 2018 Casper Meijn <casper@meijn.net>
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

