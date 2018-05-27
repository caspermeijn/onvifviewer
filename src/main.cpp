#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include "onvifdevicemanager.h"
#include "onvifdevicemanagermodel.h"
#include "onvifdevice.h"
#include "onvifdeviceinformation.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Casper Meijn");
    QCoreApplication::setOrganizationDomain("meijn.net");
    QCoreApplication::setApplicationName("OnvifViewer");

    OnvifDeviceManager deviceManager;
    deviceManager.loadDevices();

    OnvifDeviceManagerModel deviceManagerModel(&deviceManager);

    qmlRegisterType<OnvifDevice>("net.meijn.onvifviewer", 1, 0, "OnvifDevice");
    qmlRegisterType<OnvifDeviceInformation>("net.meijn.onvifviewer", 1, 0, "OnvifDeviceInformation");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("deviceManagerModel", &deviceManagerModel);
    engine.rootContext()->setContextProperty("deviceManager", &deviceManager);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return 12;

    return app.exec();
}
