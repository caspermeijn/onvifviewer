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
#include "onvifdevice.h"

#include "onvifdeviceservice.h"
#include "onvifmediaservice.h"
#include "onvifmedia2service.h"
#include "onvifptzservice.h"
#include "onvifsnapshotdownloader.h"
#include <QDebug>
#include <QUrlQuery>

OnvifDevice::OnvifDevice(QObject *parent) :
    QObject(parent),
    m_preferContinuousMove(false),
    m_cachedDeviceInformation(new OnvifDeviceInformation()),
    m_cachedSnapshotDownloader(new OnvifSnapshotDownloader())
{
    connect(&m_connection, &OnvifDeviceConnection::servicesAvailable,
            this, &OnvifDevice::servicesAvailable);
    connect(&m_connection, &OnvifDeviceConnection::errorStringChanged,
            this, &OnvifDevice::errorStringChanged);

    m_ptzStopTimer.setSingleShot(true);
    connect(&m_ptzStopTimer, &QTimer::timeout,
            this, &OnvifDevice::ptzStop);

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

OnvifSnapshotDownloader *OnvifDevice::snapshotDownloader() const
{
    return m_cachedSnapshotDownloader;
}

bool OnvifDevice::supportsSnapshotUri() const
{
    const OnvifMedia2Service* media2Service = m_connection.getMedia2Service();
    if(media2Service)
        return media2Service->supportsSnapshotUri();

    const OnvifMediaService* mediaService = m_connection.getMediaService();
    if(mediaService)
        return mediaService->supportsSnapshotUri();

    return true;
}

QUrl OnvifDevice::snapshotUri() const
{
    const OnvifMedia2Service* media2Service = m_connection.getMedia2Service();
    if(media2Service)
        return media2Service->getSnapshotUri();

    const OnvifMediaService* mediaService = m_connection.getMediaService();
    if(mediaService)
        return mediaService->getSnapshotUri();

    return QUrl();
}

QUrl OnvifDevice::streamUri() const
{
    const OnvifMedia2Service* media2Service = m_connection.getMedia2Service();
    if(media2Service)
        return media2Service->getStreamUri();

    const OnvifMediaService* mediaService = m_connection.getMediaService();
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
    auto * device = qobject_cast<OnvifDeviceConnection *>(sender());
    Q_ASSERT(device);

    OnvifDeviceService *deviceService = device->getDeviceService();
    if(deviceService)
    {
        connect(deviceService, &OnvifDeviceService::deviceInformationAvailable,
                this, &OnvifDevice::deviceInformationAvailable);
    }

    OnvifMediaService *mediaService = device->getMediaService();
    OnvifMedia2Service *media2Service = device->getMedia2Service();
    if(media2Service) {
        media2Service->setPreferredVideoStreamProtocol(preferredVideoStreamProtocol());
        connect(media2Service, &OnvifMedia2Service::profileListAvailable,
                this, &OnvifDevice::profileListAvailable);
        connect(media2Service, &OnvifMedia2Service::streamUriAvailable,
                this, &OnvifDevice::streamUriChanged);
        connect(media2Service, &OnvifMedia2Service::supportsSnapshotUriAvailable,
                this, &OnvifDevice::supportsSnapshotUriChanged);
        connect(media2Service, &OnvifMedia2Service::snapshotUriAvailable,
                this, &OnvifDevice::snapshotUriChanged);
        connect(media2Service, &OnvifMedia2Service::snapshotUriAvailable,
                m_cachedSnapshotDownloader, &OnvifSnapshotDownloader::setSnapshotUri);
    } else if(mediaService) {
        mediaService->setPreferredVideoStreamProtocol(preferredVideoStreamProtocol());
        connect(mediaService, &OnvifMediaService::profileListAvailable,
                this, &OnvifDevice::profileListAvailable);
        connect(mediaService, &OnvifMediaService::streamUriAvailable,
                this, &OnvifDevice::streamUriChanged);
        connect(mediaService, &OnvifMediaService::supportsSnapshotUriAvailable,
                this, &OnvifDevice::supportsSnapshotUriChanged);
        connect(mediaService, &OnvifMediaService::snapshotUriAvailable,
                this, &OnvifDevice::snapshotUriChanged);
        connect(mediaService, &OnvifMediaService::snapshotUriAvailable,
                m_cachedSnapshotDownloader, &OnvifSnapshotDownloader::setSnapshotUri);
    }
}

bool mediaProfileLessThan(const OnvifMediaProfile &p1, const OnvifMediaProfile &p2)
{
    if(p1.videoEncoding() != p2.videoEncoding()) {
        QStringList preferredVideoEncodingList = QStringList()
                << "H265"
                << "H264"
                << "MPV4-ES"
                << "JPEG"
                << "";
        auto index1 = preferredVideoEncodingList.indexOf(p1.videoEncoding());
        auto index2 = preferredVideoEncodingList.indexOf(p2.videoEncoding());
        if(index1 != -1 && index2 != -1) {
            return index1 < index2;
        }
        if(index1 == -1)
            qCritical() << "Unknown video encoding" << p1.videoEncoding();
        if(index2 == -1)
            qCritical() << "Unknown video encoding" << p2.videoEncoding();
        return p1.videoEncoding() < p2.videoEncoding();
    }

    if(p1.resolutionPixels() != p2.resolutionPixels()) {
        return p1.resolutionPixels() > p2.resolutionPixels();
    }

    return p1.token() < p2.token();
}

void OnvifDevice::profileListAvailable(const QList<OnvifMediaProfile> &profileList)
{
    auto * mediaService = qobject_cast<OnvifMediaService *>(sender());
    auto * media2Service = qobject_cast<OnvifMedia2Service *>(sender());
    Q_ASSERT(mediaService || media2Service);

    Q_ASSERT(profileList.size());
    //TODO: Add a proper profile selection
    auto sortedProfileList = profileList;
    qSort(sortedProfileList.begin(), sortedProfileList.end(), mediaProfileLessThan);
    m_selectedMediaProfile = sortedProfileList.first();

    if(media2Service)
        media2Service->selectProfile(m_selectedMediaProfile);
    else if(mediaService)
        mediaService->selectProfile(m_selectedMediaProfile);
}

QString OnvifDevice::preferredVideoStreamProtocol() const
{
    return m_preferredVideoStreamProtocol;
}

void OnvifDevice::setPreferredVideoStreamProtocol(const QString &preferredVideoStreamProtocol)
{
    if(m_preferredVideoStreamProtocol != preferredVideoStreamProtocol)
    {
        m_preferredVideoStreamProtocol = preferredVideoStreamProtocol;
        emit preferredVideoStreamProtocolChanged(m_preferredVideoStreamProtocol);
    }
}

void OnvifDevice::initByUrl(const QUrl &url)
{
    setUserName(url.userName());
    setPassword(url.password());
    QString host = url.host();
    if(url.port() != -1) {
        host += QString(":%1").arg(url.port());
    }
    setHostName(host);
    if(url.hasQuery()) {
        QUrlQuery urlQuery(url);
        if(urlQuery.hasQueryItem("name")) {
            setDeviceName(urlQuery.queryItemValue("name"));
        }
    }
}

bool OnvifDevice::preferContinuousMove() const
{
    return m_preferContinuousMove;
}

void OnvifDevice::setPreferContinuousMove(bool preferContinuousMove)
{
    if(m_preferContinuousMove != preferContinuousMove)
    {
        m_preferContinuousMove = preferContinuousMove;
        emit preferContinuousMoveChanged(m_preferContinuousMove);
    }
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

bool OnvifDevice::isPanTiltSupported() const
{
    if(m_selectedMediaProfile.ptzNodeToken().isEmpty())
        return false;
    const OnvifPtzService * ptzService = m_connection.getPtzService();
    if(!ptzService)
        return false;
    //TODO: Add a workaround for when relative move is not supported...
    //TODO: Add support for a absolute move; getStatus, then move to current pos + relative move
    if(ptzService->isRelativeMoveSupported(m_selectedMediaProfile))
        return true;
    if(ptzService->isContinuousMoveSupported(m_selectedMediaProfile))
        return true;
    return false;
}

bool OnvifDevice::isPtzHomeSupported() const
{
    const OnvifPtzService * ptzService = m_connection.getPtzService();
    if(ptzService) {
        return ptzService->isHomeSupported(m_selectedMediaProfile);
    }
    return false;
}

bool OnvifDevice::isZoomSupported() const
{
    const OnvifPtzService * ptzService = m_connection.getPtzService();
    if(ptzService) {
        return ptzService->isRelativeZoomSupported(m_selectedMediaProfile);
    }
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
    ptzMove(0, 0.1);
}

void OnvifDevice::ptzDown()
{
    ptzMove(0, -0.1);
}

void OnvifDevice::ptzLeft()
{
    ptzMove(-0.1, 0);
}

void OnvifDevice::ptzRight()
{
    ptzMove(0.1, 0);
}

void OnvifDevice::ptzMove(qreal xFraction, qreal yFraction)
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    if(ptzService->isRelativeMoveSupported(m_selectedMediaProfile) && !preferContinuousMove())
        ptzService->relativeMove(m_selectedMediaProfile, xFraction, yFraction);
    else if(ptzService->isContinuousMoveSupported(m_selectedMediaProfile)) {
        ptzService->continuousMove(m_selectedMediaProfile, xFraction, yFraction);
        m_ptzStopTimer.start(500);
    }
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

void OnvifDevice::ptzStop()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    if(ptzService) {
        ptzService->stopMovement(m_selectedMediaProfile);
    }
}

void OnvifDevice::ptzZoomIn()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    ptzService->relativeZoom(m_selectedMediaProfile, 0.1);
}

void OnvifDevice::ptzZoomOut()
{
    OnvifPtzService * ptzService = m_connection.getPtzService();
    Q_ASSERT(ptzService);
    ptzService->relativeZoom(m_selectedMediaProfile, -0.1);
}
