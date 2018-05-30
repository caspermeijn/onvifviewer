cmake_minimum_required(VERSION 3.7)

project(onvifviewer VERSION "0.1")
SET(CMAKE_CXX_STANDARD 11)
set(QT_MIN_VERSION "5.9.0")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Svg Qml Quick QuickControls2 Xml)
find_package(KF5Kirigami2 REQUIRED)
find_package(Qt5Widgets)
find_package(KDSoap REQUIRED)

find_package(ECM 5.38.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
set(ENABLE_CLAZY TRUE)
include(ECMQMLModules)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

#TODO: figure out whye these qml module checks fail for gitlab-ci
#ecm_find_qmlmodule(QtQuick 2.9 REQUIRED)
#ecm_find_qmlmodule(QtQuick.Controls 1.4 REQUIRED)
##TODO: figure out why QtQuick.Controls 2.0 is not foun
##ecm_find_qmlmodule(QtQuick.Controls 2.0 REQUIRED)
#ecm_find_qmlmodule(QtQuick.Layouts 1.3 REQUIRED)
#ecm_find_qmlmodule(QtQml.Models 2.1 REQUIRED)
#ecm_find_qmlmodule(org.kde.kirigami 2.0 REQUIRED)

include(FeatureSummary)

set(KSWSDL2CPP_OPTION -namespace OnvifSoapDevicemgmt)
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver10/device/wsdl/devicemgmt.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapMedia)
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver10/media/wsdl/media.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapMedia2)
#TODO: Fix the media2.wsdl file, as this is not the original file path
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver20/media/wsdl/media2.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapPtz)
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

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_connection_SRCS} ${${PROJECT_NAME}_WSDL})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Svg Qt5::Quick KF5::Kirigami2 KDSoap::kdsoap)

install(TARGETS ${PROJECT_NAME} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

kirigami_package_breeze_icons(ICONS go-up go-previous go-home go-next go-down list-add settings-configure transform-move view-refresh)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
