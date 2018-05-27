#include "onvifdevice.h"
#include "onvifdevicemanager.h"
#include "onvifdeviceservice.h"
#include "onvifmediaservice.h"

#include <QSettings>
#include <QQmlContext>

OnvifDeviceManager::OnvifDeviceManager(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<QList<OnvifDevice*>>("QList<OnvifDevice*>");
}

void OnvifDeviceManager::loadDevices()
{
    Q_ASSERT(m_deviceList.isEmpty());

    QSettings settings;
    int size = settings.beginReadArray("devices");
    for(int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        OnvifDevice * device = createNewDevice();
        device->setDeviceName(settings.value("deviceName").toString());
        device->setHostName(settings.value("hostName").toString());
        device->setUserName(settings.value("userName").toString());
        device->setPassword(settings.value("password").toString());

        device->connectToDevice();
    }
    settings.endArray();

    if(m_deviceList.isEmpty()) {
        // These are example devices from the Onvif spotlight challange
        OnvifDevice * device1 = createNewDevice();
        device1->setDeviceName("Bosch (Dinion7000HD)");
        device1->setHostName("193.159.244.134");
        device1->setUserName("service");
        device1->setPassword("Xbks8tr8vT");
        device1->connectToDevice();

        OnvifDevice * device2 = createNewDevice();
        device2->setDeviceName("Bosch (Autodom7000)");
        device2->setHostName("193.159.244.132");
        device2->setUserName("service");
        device2->setPassword("Xbks8tr8vT");
        device2->connectToDevice();

        OnvifDevice * device3 = createNewDevice();
        device3->setDeviceName("Axis");
        device3->setHostName("195.60.68.239");
        device3->setUserName("operator");
        device3->setPassword("Onv!f2018");
        device3->connectToDevice();
    }
}

void OnvifDeviceManager::saveDevices()
{
    QSettings settings;
    settings.beginWriteArray("devices");
    for(int i = 0; i < m_deviceList.count(); i++) {
        auto device = m_deviceList.at(i);
        settings.setArrayIndex(i);
        settings.setValue("deviceName", device->deviceName());
        settings.setValue("hostName", device->hostName());
        settings.setValue("userName", device->userName());
        settings.setValue("password", device->password());
    }
    settings.endArray();
}

OnvifDevice *OnvifDeviceManager::createNewDevice()
{
    OnvifDevice * device = new OnvifDevice(this);

    m_deviceList.append(device);
    emit deviceListChanged(m_deviceList);
    return device;
}

QList<OnvifDevice *> OnvifDeviceManager::deviceList() const
{
    return m_deviceList;
}

OnvifDevice *OnvifDeviceManager::at(int i)
{
    return m_deviceList.at(i);
}

int OnvifDeviceManager::appendDevice()
{
    createNewDevice();
    return m_deviceList.count() - 1;
}

void OnvifDeviceManager::removeDevice(int i)
{
    OnvifDevice * device = m_deviceList.takeAt(i);
    emit deviceListChanged(m_deviceList);
    device->deleteLater();
}
