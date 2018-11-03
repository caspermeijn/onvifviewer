# ONVIFViewer

**ONVIF camera viewer for Android, Plasma Mobile and Linux desktop**

The goal of this project is to replace the proprietary app that was needed to configure and view my IP camera. The ONVIF protocol can be used to view and configure many types of camera's and is a open standard that can be implemented using standard SOAP libraries. Using Qt5 for the back-end and Kirigami UI framework makes this application a cross-platform solution. The primary focus is Plasma mobile and the Linux desktop, but porting to Android and Windows is also possible. 

This project was started as part of the [ONVIF Open Source Spotlight Challange](https://onvif-spotlight.bemyapp.com/#/projects/5ae0bbf7f98fde00047f0605) and the application finished in [fourth place](https://www.onvif.org/blog/2018/07/onvif-challenge-announces-top-10/) (out of 37 submissions). 
Before this project started, there was no open-source application for viewing ONVIF cameras for Plasma Mobile and Linux desktop. Neither is there a simple to use open-source C++ library to communicate with ONVIF cameras. The communication with the camera is implemented from scratch (using KDSoap) and modular designed, so that it can be separated into a reusable library at a later stage.


## Flatpak
On most Linux desktops you can install the application using Flatpak. 

1) First install Flatpak itself using the instructions on their [website](https://www.flatpak.org/setup/).
2) Then you can install the application from the [ONVIFViewer flathub page](https://flathub.org/apps/details/net.meijn.onvifviewer).

[<img width='240' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-en.png'/>](https://flathub.org/apps/details/net.meijn.onvifviewer)

## Android
The application is available via the [Google Play Store] (https://play.google.com/store/apps/details?id=net.meijn.onvifviewer).

NOTE: The video codec of Android doesn't support all types of RTSP streams. Therefore currently the video is not a live stream, but a series of snapshots.

[<img src="https://play.google.com/intl/en_us/badges/images/generic/en_badge_web_generic.png"
      alt="Get it on Google Play"
      height="80">](https://play.google.com/store/apps/details?id=net.meijn.onvifviewer)
      
## Translations
You can help translating this application using [Weblate](https://hosted.weblate.org/engage/onvifviewer/). You can login on the website and translate the texts to your language. The translations will be included in the next release.

[<img src="https://hosted.weblate.org/widgets/onvifviewer/-/287x66-grey.png" alt="Vertalingsstatus" />](https://hosted.weblate.org/engage/onvifviewer/?utm_source=widget)

## Donations
You can donate via Bitcoin at [15PerwiiGxPf27AxVTYq7hGYJ52WfM9EWo](bitcoin:15PerwiiGxPf27AxVTYq7hGYJ52WfM9EWo).

You can donate via PayPal via: [<img src="https://www.paypalobjects.com/nl_NL/NL/i/btn/btn_donateCC_LG.gif" alt="Donate" />](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=RNGGP3C6J84QU)

## Building from source
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
    git clone --branch v5.48.0 https://github.com/KDE/kirigami.git
    mkdir build-kirigami
    cd build-kirigami
    cmake -DCMAKE_BUILD_TYPE=Release ../kirigami
    make 
    sudo make install
    cd ..

#### Building KDSoap
    git clone https://github.com/kdsoap/KDSoap.git
    mkdir build-kdsoap
    cd build-kdsoap
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/ ../KDSoap
    make 
    sudo make install
    cd ..

#### Building onvifviewer
    git clone https://gitlab.com/caspermeijn/onvifviewer.git
    mkdir build-onvifviewer
    cd build-onvifviewer
    cmake -DCMAKE_BUILD_TYPE=Release ../onvifviewer
    make 
    sudo make install
    cd ..

## Attribution 
Google Play and the Google Play logo are trademarks of Google LLC.



