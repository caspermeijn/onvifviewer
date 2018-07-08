# Copyright (C) 2018 Casper Meijn <casper@meijn.net>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.7)

install(FILES net.meijn.onvifviewer.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES net.meijn.onvifviewer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES ApplicationIcon.svg DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps RENAME net.meijn.onvifviewer.svg)
foreach(_iconSize 16 22 24 32 48 64 128 256)
  install(FILES ApplicationIcon-${_iconSize}.png DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/${_iconSize}x${_iconSize}/apps RENAME net.meijn.onvifviewer.png)
endforeach()
