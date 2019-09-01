# Copyright (C) 2018-2019 Casper Meijn <casper@meijn.net>
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

set(${PROJECT_NAME}_SRCS
    "main.cpp"
    "onvifdevice.cpp"
    "onvifdevicediscover.cpp"
    "onvifdevicemanager.cpp"
    "onvifdevicemanagermodel.cpp"
    "onvifsnapshotdownloader.cpp"
    "onvifsnapshotviewer.cpp"
    "qml.qrc"
)

if(WITH_BREEZE_ICONS)
    set(${PROJECT_NAME}_BREEZE_ICONS "breeze-icons.qrc")
endif()

ecm_setup_version(PROJECT VERSION_HEADER version.h)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_WSDL} ${${PROJECT_NAME}_BREEZE_ICONS})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Svg Qt5::Quick KF5::CoreAddons KF5::Kirigami2 KF5::I18n KDSoap::kdsoap libOnvifConnect)
if(KF5XmlGui_FOUND)
    target_link_libraries(${PROJECT_NAME} KF5::XmlGui)
endif()

if(KDSoapWSDiscoveryClient_FOUND)
    target_link_libraries(${PROJECT_NAME} KDSoap::WSDiscoveryClient)
    add_definitions(-DWITH_KDSOAP_WSDISCOVERY_CLIENT)
endif()
if(QTAV_FOUND)
    target_link_libraries(${PROJECT_NAME} ${QTAV_LIBRARIES})
    add_definitions(-DWITH_QTAV)
endif()

install(TARGETS ${PROJECT_NAME} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
