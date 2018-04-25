OnvifViewer

Onvif camera viewer for Plasma Mobile and Linux desktop

## Issue ## 
Currently there are no open-source application for viewing Onvif cameras for Plasma Mobile and Linux desktop. Neither is there a simple to use open-source C++ library to communicate with Onvif cameras.

## Solution ##
Using Qt5 for the backend and Kirigami UI framework provides a cross-platform solution. The primary focus is Plasma mobile and the Linux desktop, but porting to Android and Windows is also possible. 

The communication with the camera is implemented from scratch (using KDSoap) and modular designed, so that it can be separated into a reusable library at a later stage.
