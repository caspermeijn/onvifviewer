/* Copyright (C) 2018 Casper Meijn <casper@meijn.net>
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
#include <KLocalizedContext>
#include <KLocalizedString>
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "onvifdevicemanager.h"
#include "onvifdevicemanagermodel.h"
#include "onvifdevice.h"
#include "onvifdeviceinformation.h"
#include "version.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("onvifviewer");

    QCoreApplication::setOrganizationName("CasperMeijn");
    QCoreApplication::setOrganizationDomain("meijn.net");
    QCoreApplication::setApplicationName("ONVIFViewer");
    QCoreApplication::setApplicationVersion(onvifviewer_VERSION_STRING);

    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription("View and control network cameras using the ONVIF protocol");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();
    commandLineParser.addOption({"test", "test description", "test_name"});
    commandLineParser.process(app);

    OnvifDeviceManager deviceManager;
    deviceManager.loadDevices();

    OnvifDeviceManagerModel deviceManagerModel(&deviceManager);

    qmlRegisterType<OnvifDevice>("net.meijn.onvifviewer", 1, 0, "OnvifDevice");
    qmlRegisterType<OnvifDeviceInformation>("net.meijn.onvifviewer", 1, 0, "OnvifDeviceInformation");

#ifdef WITH_BREEZE_ICONS
    if(QIcon::themeName().isEmpty())
        QIcon::setThemeName("breeze");
#endif

    QQmlApplicationEngine engine;
    KLocalizedContext localizedContext;
    engine.rootContext()->setContextObject(&localizedContext);
    engine.rootContext()->setContextProperty("deviceManagerModel", &deviceManagerModel);
    engine.rootContext()->setContextProperty("deviceManager", &deviceManager);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return 12;
    }

    if(commandLineParser.isSet("test")) {
        QString testOption = commandLineParser.value("test");
        if(testOption == "startup") {
            qDebug() << "Startup test activated, therefore the application will close automatically";
            QTimer::singleShot(0, &app, &QGuiApplication::quit);
        } else {
            qFatal("Invalid test selected");
        }
    }

    return QGuiApplication::exec();
}
