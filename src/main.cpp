/* Copyright (C) 2018-2019 Casper Meijn <casper@meijn.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <QCommandLineParser>
#include <QDebug>
#include <QApplication>
#include <QIcon>
#include <QStringList>
#include <QQmlApplicationEngine>
#include <QQmlFileSelector>
#include <QQmlContext>
#include "onvifdevicediscover.h"
#include "onvifdevicemanager.h"
#include "onvifdevicemanagermodel.h"
#include "onvifdevice.h"
#include "onvifdeviceinformation.h"
#include "onvifsnapshotdownloader.h"
#include "onvifsnapshotviewer.h"
#include "version.h"

Q_DECL_EXPORT int main(int argc, char* argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    
#ifdef USE_BREEZE_ICONS
#if (QT_VERSION >= QT_VERSION_CHECK(5,12,0))
    QIcon::setFallbackThemeName("breeze-internal");
#else
    QIcon::setThemeName("breeze-internal");
#endif
#endif

    KLocalizedString::setApplicationDomain("onvifviewer");
    QCoreApplication::setOrganizationName("CasperMeijn");
    QCoreApplication::setOrganizationDomain("meijn.net");
    QCoreApplication::setApplicationName("ONVIFViewer");
    QCoreApplication::setApplicationVersion(onvifviewer_VERSION_STRING);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("net.meijn.onvifviewer")));

    KAboutData about("onvifviewer", i18n("ONVIFViewer"), onvifviewer_VERSION_STRING,
                     i18n("View and control network cameras using the ONVIF protocol"),
                     KAboutLicense::GPL_V3,
                     "Copyright (C) 2018-2019 Casper Meijn <casper@meijn.net>", "",
                     "https://gitlab.com/caspermeijn/onvifviewer",
                     "https://gitlab.com/caspermeijn/onvifviewer/issues");
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                        i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    about.setOrganizationDomain("meijn.net");
    about.setDesktopFileName("net.meijn.onvifviewer");
    //TODO: setProgramLogo is needed, because Kirigami doesn't show the QApplication::windowIcon
    about.setProgramLogo(app.windowIcon());  
    about.addAuthor("Casper Meijn", i18n("Main developer"), QStringLiteral("casper@meijn.net"));
    KAboutData::setApplicationData(about);

    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription(i18n("View and control network cameras using the ONVIF protocol"));
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();
    commandLineParser.addOption({"test", "test description", "test_name"});
    commandLineParser.process(app);

    OnvifDeviceManager deviceManager;
    deviceManager.loadDevices();

    OnvifDeviceManagerModel deviceManagerModel(&deviceManager);

    qmlRegisterType<OnvifDevice> ("net.meijn.onvifviewer", 1, 0, "OnvifDevice");
    qmlRegisterType<OnvifDeviceInformation> ("net.meijn.onvifviewer", 1, 0, "OnvifDeviceInformation");
    qmlRegisterType<OnvifSnapshotDownloader> ("net.meijn.onvifviewer", 1, 0, "OnvifSnapshotDownloader");
    qmlRegisterType<OnvifSnapshotViewer> ("net.meijn.onvifviewer", 1, 0, "OnvifSnapshotViewer");

    OnvifDeviceDiscover deviceDiscover;
    deviceDiscover.start();
    qmlRegisterType<OnvifDeviceDiscover> ("net.meijn.onvifviewer", 1, 0, "OnvifDeviceDiscover");

    QQmlApplicationEngine engine;
    QQmlFileSelector* selector = new QQmlFileSelector(&engine);
#ifdef WITH_QTAV
    selector->setExtraSelectors(QStringList() << "qtav");
#else
    Q_UNUSED(selector);
#endif
    KLocalizedContext localizedContext;
    engine.rootContext()->setContextObject(&localizedContext);
    engine.rootContext()->setContextProperty("deviceManagerModel", &deviceManagerModel);
    engine.rootContext()->setContextProperty("deviceManager", &deviceManager);
    engine.rootContext()->setContextProperty("deviceDiscover", &deviceDiscover);
    engine.rootContext()->setContextProperty(QStringLiteral("onvifviewerAboutData"), QVariant::fromValue(KAboutData::applicationData()));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return 12;
    }

    if (commandLineParser.isSet("test")) {
        QString testOption = commandLineParser.value("test");
        if (testOption == "startup") {
            qDebug() << "Startup test activated, therefore the application will close automatically";
            QTimer::singleShot(0, &app, &QGuiApplication::quit);
        } else {
            qFatal("Invalid test selected");
        }
    }

    if (!commandLineParser.positionalArguments().isEmpty()) {
        auto url = QUrl(commandLineParser.positionalArguments().constFirst());
        if (url.isValid()) {
            OnvifDevice* device = deviceManager.createNewDevice();
            device->initByUrl(url);
            device->connectToDevice();
            QVariant variantDevice = QVariant::fromValue<OnvifDevice*> (device);
            engine.rootObjects().constFirst()->setProperty("previewDevice", variantDevice);
        }
    }

    return QApplication::exec();
}
