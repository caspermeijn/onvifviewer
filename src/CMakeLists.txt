cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Qml Quick QuickControls2)
find_package(KF5Kirigami2 REQUIRED)
find_package(Qt5Widgets)
find_package(KDSoap REQUIRED)

include(FeatureSummary)
include(GNUInstallDirs)

set(KSWSDL2CPP_OPTION -namespace OnvifSoapDevicemgmt)
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver10/device/wsdl/devicemgmt.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapMedia)
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver10/media/wsdl/media.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapPtz)
KDSOAP_GENERATE_WSDL(${PROJECT_NAME}_WSDL ../3rdparty/wsdl/www.onvif.org/ver20/ptz/wsdl/ptz.wsdl)

set(${PROJECT_NAME}_connection_SRCS
    "connection/onvifdeviceconnection.cpp"
    "connection/onvifdeviceservice.cpp"
    "connection/onvifmediaservice.cpp"
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

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick KF5::Kirigami2 KDSoap::kdsoap)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
