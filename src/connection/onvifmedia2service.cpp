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
#include "onvifdeviceconnection.h"
#include "onvifmedia2service.h"

#include <QDebug>
#include <QUrl>
#include "wsdl_media2.h"

using namespace OnvifSoapMedia2;

class OnvifMedia2Service::Private
{
public:
    Private(OnvifDeviceConnection *device) :
        device(device),
        supportsSnapshotUri(true)
    {;}

    OnvifDeviceConnection * device;
    Media2BindingService soapService;
    QList<OnvifMediaProfile> profileList;
    OnvifMediaProfile selectedProfile;
    bool supportsSnapshotUri;
    QUrl snapshotUri;
    QUrl streamUri;
};

OnvifMedia2Service::OnvifMedia2Service(const QString &endpointAddress, TR2__Capabilities2 capabilities, OnvifDeviceConnection *parent) :
    QObject(parent),
    d(new Private(parent))
{
    d->soapService.setEndPoint(endpointAddress);

    connect(&d->soapService, &Media2BindingService::getProfilesDone,
            this, &OnvifMedia2Service::getProfilesDone);
    connect(&d->soapService, &Media2BindingService::getProfilesError,
            this, &OnvifMedia2Service::getProfilesError);
    connect(&d->soapService, &Media2BindingService::getSnapshotUriDone,
            this, &OnvifMedia2Service::getSnapshotUriDone);
    connect(&d->soapService, &Media2BindingService::getSnapshotUriError,
            this, &OnvifMedia2Service::getSnapshotUriError);
    connect(&d->soapService, &Media2BindingService::getStreamUriDone,
            this, &OnvifMedia2Service::getStreamUriDone);
    connect(&d->soapService, &Media2BindingService::getStreamUriError,
            this, &OnvifMedia2Service::getStreamUriError);

    setServiceCapabilities(capabilities);
}

void OnvifMedia2Service::connectToService()
{
    qDebug() << Q_FUNC_INFO;
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    TR2__GetProfiles request;
    request.setType(QStringList() << "All");
    d->soapService.asyncGetProfiles(request);
}

void OnvifMedia2Service::disconnectFromService()
{
    d->profileList.clear();
    d->selectedProfile = OnvifMediaProfile();
    d->snapshotUri.clear();
    d->streamUri.clear();
}

QList<OnvifMediaProfile> OnvifMedia2Service::getProfileList() const
{
    return d->profileList;
}

void OnvifMedia2Service::selectProfile(const OnvifMediaProfile &profile)
{
    d->selectedProfile = profile;

    if(d->supportsSnapshotUri) {
        TR2__GetSnapshotUri requestSnapshot;
        requestSnapshot.setProfileToken(d->selectedProfile.token());
        d->device->updateSoapCredentials(d->soapService.clientInterface());
        d->soapService.asyncGetSnapshotUri(requestSnapshot);
    }

    TR2__GetStreamUri requestStream;
    requestStream.setProfileToken(d->selectedProfile.token());
    requestStream.setProtocol("RtspUnicast");
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetStreamUri(requestStream);
}

bool OnvifMedia2Service::supportsSnapshotUri() const
{
    return d->supportsSnapshotUri;
}

QUrl OnvifMedia2Service::getSnapshotUri() const
{
    return d->snapshotUri;
}

QUrl OnvifMedia2Service::getStreamUri() const
{
    return d->streamUri;
}

void OnvifMedia2Service::getProfilesDone(const TR2__GetProfilesResponse &parameters)
{
    qDebug() << Q_FUNC_INFO;
    d->profileList.clear();
    for(auto profile : parameters.profiles()) {
        d->profileList << OnvifMediaProfile(profile);
    }
    emit profileListAvailable(d->profileList);
}

void OnvifMedia2Service::getProfilesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifMedia2Service::getSnapshotUriDone(const TR2__GetSnapshotUriResponse &parameters)
{
    d->snapshotUri = QUrl(parameters.uri());
    if(d->snapshotUri.userInfo().isEmpty())
    {
        d->device->updateUrlCredentials(&d->snapshotUri);
    }
    emit snapshotUriAvailable(d->snapshotUri);
}

void OnvifMedia2Service::getSnapshotUriError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifMedia2Service::getStreamUriDone(const TR2__GetStreamUriResponse &parameters)
{
    d->streamUri = QUrl(parameters.uri());
    if(d->streamUri.userInfo().isEmpty())
    {
        d->device->updateUrlCredentials(&d->streamUri);
    }
    emit streamUriAvailable(d->streamUri);
}

void OnvifMedia2Service::getStreamUriError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifMedia2Service::setServiceCapabilities(const TR2__Capabilities2 &capabilities)
{
    d->supportsSnapshotUri = capabilities.snapshotUri();
    emit supportsSnapshotUriAvailable(d->supportsSnapshotUri);
}
