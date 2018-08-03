#!/bin/sh
set -eu
BASEDIR="../src/"	# root of translatable sources
PROJECT="onvifviewer"	# project name
BUGADDR="https://gitlab.com/caspermeijn/onvifviewer/issues"	# MSGID-Bugs
WDIR=`pwd`		# working dir
DESKTOP_FILE="../desktop/net.meijn.onvifviewer.desktop"
APPDATA_FILE="../desktop/net.meijn.onvifviewer.appdata.xml"


echo "Preparing resource files"
cd ${BASEDIR}
# desktop file
intltool-extract --quiet --type=gettext/ini ${DESKTOP_FILE}.template
mv ${DESKTOP_FILE}.template.h ${WDIR}/desktop.cpp
# additional string for KAboutData
echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> ${WDIR}/rc.cpp
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> ${WDIR}/rc.cpp
cd ${WDIR}
echo "Done preparing rc files"


echo "Extracting messages"
cd ${BASEDIR}
find . -name '*.cpp' -o -name '*.h' -o -name '*.qml' | sort > ${WDIR}/infiles.list
cd ${WDIR}
find . -name '*.cpp' | sort >> ${WDIR}/infiles.list
xgettext --from-code=UTF-8 -C -kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
        -kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 -kN_:1 \
        --package-name="${PROJECT}" \
        --msgid-bugs-address="${BUGADDR}" \
        --files-from=infiles.list -D ${BASEDIR} -D ${WDIR} -o messages.pot

# appdata file
itstool -i ../desktop/appstream-metainfo.its -o appdata.pot ${APPDATA_FILE}.template
msgcat --use-first messages.pot appdata.pot > ${PROJECT}.pot

sed -i '/"POT-Creation-Date:/d' ${PROJECT}.pot
echo "Done extracting messages"


echo "Merging translations"
catalogs=`find . -name '*.po'`
for cat in $catalogs; do
  echo $cat
  msgmerge -o $cat.new $cat ${PROJECT}.pot
  mv $cat.new $cat
done
echo "Done merging translations"


echo "Cleaning up"
cd ${WDIR}
rm infiles.list
rm rc.cpp desktop.cpp
rm appdata.pot messages.pot
echo "Done"
