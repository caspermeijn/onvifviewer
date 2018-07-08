#!/bin/sh
set -eu

generate_application_icon() {
  SIZE=$1
  inkscape ApplicationIcon.svg --export-png "ApplicationIcon-$SIZE.png" --export-width "$SIZE"
}

generate_application_icon 16
generate_application_icon 22
generate_application_icon 24
generate_application_icon 32
generate_application_icon 48
generate_application_icon 64
generate_application_icon 128
generate_application_icon 256

generate_android_icon() {
  SIZE=$1
  DENSITY=$2
  inkscape ApplicationIcon.svg --export-png "../android/res/drawable-$DENSITY/icon.png" --export-width "$SIZE"
}

generate_android_icon 48 mdpi
generate_android_icon 72 hdpi
generate_android_icon 96 xhdpi
generate_android_icon 144 xxhdpi
generate_android_icon 192 xxxhdpi

generate_android_splash() {
  SIZE=$1
  DENSITY=$2
  inkscape ApplicationIcon.svg --export-png "../android/res/drawable-$DENSITY/onvifviewer_splash.png" --export-width "$SIZE" --export-background "#2C7C90"
}

generate_android_splash 200 mdpi
generate_android_splash 300 hdpi
generate_android_splash 400 xhdpi
generate_android_splash 600 xxhdpi
generate_android_splash 800 xxxhdpi
