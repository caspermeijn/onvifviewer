#include "onvifdevice.h"

#include "onvifdeviceservice.h"
#include "onvifmediaservice.h"
#include "onvifmedia2service.h"
#include "onvifptzservice.h"
#include <QDebug>

OnvifDevice::OnvifDevice(QObject *parent) :
    QObject(parent),
    m_cachedDeviceInformation(new OnvifDeviceInformation())
{
    connect(&m_connection, &OnvifDeviceConnection::servicesAvailable,
            this, &OnvifDevice::servicesAvailable);
    connect(&m_connection, &OnvifDeviceConnection::errorStringChanged,
            this, &OnvifDevice::errorStringChanged);

    // TODO: Figure out why qRegisterMetaType is needed, when we already called Q_DECLARE_METATYPE
    qRegisterMetaType<OnvifDeviceInformation>("OnvifDeviceInformation");
}

void OnvifDevice::connectToDevice()
{
    m_connection.connectToDevice();
}

void OnvifDevice::reconnectToDevice()
{
    m_connection.disconnectFromDevice();
    m_connection.connectToDevice();

    emit snapshotUriChanged(snapshotUri());
    emit streamUriChanged(streamUri());
}

OnvifDeviceInformation *OnvifDevice::deviceInformation() const
{
    return m_cachedDeviceInformation;
}

QUrl OnvifDevice::snapshotUri() const
{
    OnvifMedia2Service* media2Service = m_connection.getMedia2Service();
    if(media2Service)
        return media2Service->getSnapshotUri();

    OnvifMediaService* mediaService = m_connection.getMediaService();
    if(mediaService)
        return mediaService->getSnapshotUri();

    return QUrl();
}

QUrl OnvifDevice::streamUri() const
{
    OnvifMedia2Service* media2Service = m_connection.getMedia2Service();
    if(media2Service)
        return media2Service->getStreamUri();

    OnvifMediaService* mediaService = m_connection.getMediaService();
    if(mediaService)
        return mediaService->getStreamUri();

    return QUrl();
}

QString OnvifDevice::errorString() const
{
    return m_connection.errorString();
}

void OnvifDevice::servicesAvailable()
{
    OnvifDeviceConnection * device = qobject_cast<OnvifDeviceConnection *>(sender());
    Q_ASSERT(device);

    OnvifDeviceService *deviceService = device->getDeviceService();
    if(deviceService)
    {
        connect(deviceService, &OnvifDeviceService::deviceInformationAvailable,
                this, &OnvifDevice::deviceInformationAvailable);
    }

    OnvifMediaService *mediaService = device->getMediaService();
    OnvifMedia2Service *media2Service = device->getMedia2Service();
    if(media2Service)
    {
        connect(media2Service, &OnvifMedia2Service::profileListAvailable,
                this, &OnvifDevice::profileListAvailable);
        connect(media2Service, &OnvifMedia2Service::streamUriAvailable,
                this, &OnvifDevice::streamUriChanged);
        connect(media2Service, &OnvifMedia2Service::snapshotUriAvailable,
                this, &OnvifDevice::snapshotUriChanged);
    }
    else if(mediaService)
    {
        connect(mediaService, &OnvifMediaService::profileListAvailable,
                this, &OnvifDevice::profileListAvailable);
        connect(mediaService, &OnvifMediaService::streamUriAvailable,
                this, &OnvifDevice::streamUriChanged);
        connect(mediaService, &OnvifMediaService::snapshotUriAvailable,
                this, &OnvifDevice::snapshotUriChanged);
    }
}
}

void OnvifDevice::profileListAvailable(const QList<OnvifMediaProfile> &profileList)
{
    OnvifMediaService * mediaService = qobject_cast<OnvifMediaService *>(sender());
    OnvifMedia2Service * media2Service = qobject_cast<OnvifMedia2Service *>(sender());
    Q_ASSERT(mediaService || media2Service);

    Q_ASSERT(profileList.size());
    //TODO: Add a proper profile selection
    m_selectedMediaProfile = profileList.first();

    if(mediaService)
        mediaService->selectProfile(m_selectedMediaProfile);
    if(media2Service)
        media2Service->selectProfile(m_selectedMediaProfile);
}

void OnvifDevice::deviceInformationAvailable(const OnvifDeviceInformation &deviceInformation)
{
    *m_cachedDeviceInformation = deviceInformation;
    emit deviceInformationChanged(m_cachedDeviceInformation);
}

QString OnvifDevice::deviceName() const
{
    return m_deviceName;
}

void OnvifDevice::setDeviceName(const QString &deviceName)
{
    if(m_deviceName != deviceName)
    {
        m_deviceName = deviceName;
        emit deviceNameChanged(m_deviceName);
    }
}

QString OnvifDevice::password() const
{
    return m_password;
}

void OnvifDevice::setPassword(const QString &password)
{
    if(m_password != password)
    {
        m_password = password;
        m_connection.setCredentials(m_userName, m_password);
        emit passwordChanged(m_password);
    }
}

bool OnvifDevice::isPtzSupported() const
{
    if(m_selectedMediaProfile.ptzNodeToken().isEmpty())
        return false;
    OnvifPtzService * ptzService = m_connection.getPtzService();
    if(!ptzService)
        return false;
    //TODO: Add a workaround for when relative move is not supported...
    //TODO: Add support for a absolute move; getStatus, then move to current pos + relative move
    //TODO: Add support for a continuous move; start movement, wait a second, stop movement
    if(!ptzService->isRelativeMoveSupported(m_selectedMediaProfile))
        return false;
    return true;
}

bool OnvifDevice::isPtzHomeSupported() const
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    if(ptzService)
        return ptzService->isHomeSupported(m_selectedMediaProfile);
    else
        return false;
}

QString OnvifDevice::userName() const
{
    return m_userName;
}

void OnvifDevice::setUserName(const QString &userName)
{
    if(m_userName != userName)
    {
        m_userName = userName;
        m_connection.setCredentials(m_userName, m_password);
        emit userNameChanged(m_userName);
    }
}

QString OnvifDevice::hostName() const
{
    return m_hostName;
}

void OnvifDevice::setHostName(const QString &hostName)
{
    if(m_hostName != hostName)
    {
        m_hostName = hostName;
        m_connection.setHostname(m_hostName);
        emit hostNameChanged(m_hostName);
    }
}


void OnvifDevice::ptzUp()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    ptzService->relativeMove(m_selectedMediaProfile, 0, 0.1);
}

void OnvifDevice::ptzDown()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    ptzService->relativeMove(m_selectedMediaProfile, 0, -0.1);

}

void OnvifDevice::ptzLeft()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    ptzService->relativeMove(m_selectedMediaProfile, -0.1, 0);
}

void OnvifDevice::ptzRight()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    ptzService->relativeMove(m_selectedMediaProfile, 0.1, 0);
}

void OnvifDevice::ptzHome()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    ptzService->goToHome(m_selectedMediaProfile);
}

void OnvifDevice::ptzSaveHomePosition()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    ptzService->saveHomePosition(m_selectedMediaProfile);
}
