# ONVIFViewer

**ONVIF camera viewer for Android, Plasma Mobile and Linux desktop**

The goal of this application is to replace the proprietary app that was needed to configure and view my IP camera. The ONVIF protocol can be used to view and configure many types of camera's and is a open standard that can be implemented using standard SOAP libraries. Using Qt5 for the backend and Kirigami UI framework makes this application a cross-platform solution. The primary focus is Plasma mobile and the Linux desktop, but porting to Android and Windows is also possible. 

This project was started as part of the [ONVIF Open Source Spotlight Challange](https://onvif-spotlight.bemyapp.com/#/projects/5ae0bbf7f98fde00047f0605). 
There was no open-source application for viewing ONVIF cameras for Plasma Mobile and Linux desktop. Neither is there a simple to use open-source C++ library to communicate with ONVIF cameras. The communication with the camera is implemented from scratch (using KDSoap) and modular designed, so that it can be separated into a reusable library at a later stage.

## Android
The application is available via the [Google Play Store] (https://play.google.com/store/apps/details?id=net.meijn.onvifviewer).

NOTE: The video codec of Android doesn't support all types of RTSP streams. Therefore currently the video is not a live stream, but a series of snapshots.

[<img src="https://play.google.com/intl/en_us/badges/images/generic/en_badge_web_generic.png"
      alt="Get it on Google Play"
      height="80">](https://play.google.com/store/apps/details?id=net.meijn.onvifviewer)

## Flatpak
On most Linux desktops you can install the application using Flatpak. 

1) First install Flatpak itself using the instructions on their [website](https://www.flatpak.org/setup/).
2) Then you can install the application from the [ONVIFViewer flathub page](https://flathub.org/apps/details/net.meijn.onvifviewer).


## Building
It is also possible to build the application yourself. This requires Qt 5.10 to be installed.

The following instructions should work on [Solus](https://solus-project.com/).

#### Building extra-cmake-modules
    git clone --branch v5.46.0 https://github.com/KDE/extra-cmake-modules.git
    mkdir build-ecm
    cd build-ecm
    cmake -DCMAKE_BUILD_TYPE=Release ../extra-cmake-modules
    make 
    sudo make install
    cd ..

#### Building Kirigami2
    git clone --branch v5.46.0 https://github.com/KDE/kirigami.git
    mkdir build-kirigami
    cd build-kirigami
    cmake -DCMAKE_BUILD_TYPE=Release ../kirigami
    make 
    sudo make install
    cd ..

#### Building KDSoap
    git clone https://github.com/caspermeijn/KDSoap.git
    mkdir build-kdsoap
    cd build-kdsoap
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/ ../KDSoap
    make 
    sudo make install
    cd ..

#### Building onvifviewer
    git clone https://gitlab.com/caspermeijn/onvifviewer.git
    cd onvifviewer/3rdparty
    git clone --branch v5.46.0 https://github.com/KDE/breeze-icons.git
    cd ../..
    mkdir build-onvifviewer
    cd build-onvifviewer
    cmake -DCMAKE_BUILD_TYPE=Release ../onvifviewer
    make 
    sudo make install
    cd ..

## Attribution 
Google Play and the Google Play logo are trademarks of Google LLC.



