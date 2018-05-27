#ifndef ONVIFDEVICEMANAGER_H
#define ONVIFDEVICEMANAGER_H

#include <QObject>

class OnvifDevice;

class OnvifDeviceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<OnvifDevice*> deviceList READ deviceList NOTIFY deviceListChanged)
public:
    explicit OnvifDeviceManager(QObject *parent = nullptr);

    QList<OnvifDevice*> deviceList() const;
    Q_INVOKABLE OnvifDevice* at(int i);
    Q_INVOKABLE int appendDevice();
    Q_INVOKABLE void removeDevice(int i);

signals:
    void deviceListChanged(const QList<OnvifDevice *> &deviceList);

public slots:
    void loadDevices();
    void saveDevices();

private:
    OnvifDevice * createNewDevice();

private:
    QList<OnvifDevice *> m_deviceList;
};

#endif // ONVIFDEVICEMANAGER_H
