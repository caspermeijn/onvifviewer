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

get_filename_component(KDWSDL_IMPORT_PATH ../3rdparty/wsdl/ ABSOLUTE)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapDevicemgmt -import-path ${KDWSDL_IMPORT_PATH} -use-local-files-only)
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver10/device/wsdl/devicemgmt.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapMedia -import-path ${KDWSDL_IMPORT_PATH} -use-local-files-only)
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver10/media/wsdl/media.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapMedia2 -import-path ${KDWSDL_IMPORT_PATH} -use-local-files-only)
#TODO: Fix the media2.wsdl file, as this is not the original file path
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver20/media/wsdl/media2.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapPtz -import-path ${KDWSDL_IMPORT_PATH} -use-local-files-only)
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver20/ptz/wsdl/ptz.wsdl)

set(${PROJECT_NAME}_connection_SRCS
    "connection/onvifdeviceconnection.cpp"
    "connection/onvifdeviceinformation.cpp"
    "connection/onvifdeviceservice.cpp"
    "connection/onvifmediaservice.cpp"
    "connection/onvifmedia2service.cpp"
    "connection/onvifmediaprofile.cpp"
    "connection/onvifptzservice.cpp"
)
include_directories("connection")

set(${PROJECT_NAME}_SRCS
    "main.cpp"
    "onvifdevice.cpp"
    "onvifdevicemanager.cpp"
    "onvifdevicemanagermodel.cpp"
    "qml.qrc"
)

if(WITH_BREEZE_ICONS)
    set(${PROJECT_NAME}_BREEZE_ICONS "breeze-icons.qrc")
endif()

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_connection_SRCS} ${${PROJECT_NAME}_WSDL} ${${PROJECT_NAME}_BREEZE_ICONS})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Svg Qt5::Quick KF5::Kirigami2 KDSoap::kdsoap)

install(TARGETS ${PROJECT_NAME} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

kirigami_package_breeze_icons(ICONS go-up go-previous go-home go-next go-down list-add settings-configure transform-move view-refresh)
