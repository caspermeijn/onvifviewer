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
#include "onvifmediaservice.h"

#include <QDebug>
#include <QUrl>
#include "wsdl_media.h"

using namespace OnvifSoapMedia;

class OnvifMediaService::Private
{
public:
    Private(OnvifDeviceConnection *device) :
        device(device),
        recievedServiceCapabilities(false),
        supportsSnapshotUri(true)
    {;}

    OnvifDeviceConnection * device;
    OnvifSoapMedia::MediaBindingService soapService;
    QList<OnvifMediaProfile> profileList;
    OnvifMediaProfile selectedProfile;
    bool recievedServiceCapabilities;
    bool supportsSnapshotUri;
    QUrl snapshotUri;
    QUrl streamUri;
};

OnvifMediaService::OnvifMediaService(const QString &endpointAddress, OnvifDeviceConnection *parent) :
    QObject(parent),
    d(new Private(parent))
{
    d->soapService.setEndPoint(endpointAddress);

    connect(&d->soapService, &MediaBindingService::getServiceCapabilitiesDone,
            this, &OnvifMediaService::getServiceCapabilitiesDone);
    connect(&d->soapService, &MediaBindingService::getServiceCapabilitiesError,
            this, &OnvifMediaService::getServiceCapabilitiesError);
    connect(&d->soapService, &MediaBindingService::getProfilesDone,
            this, &OnvifMediaService::getProfilesDone);
    connect(&d->soapService, &MediaBindingService::getProfilesError,
            this, &OnvifMediaService::getProfilesError);
    connect(&d->soapService, &MediaBindingService::getSnapshotUriDone,
            this, &OnvifMediaService::getSnapshotUriDone);
    connect(&d->soapService, &MediaBindingService::getSnapshotUriError,
            this, &OnvifMediaService::getSnapshotUriError);
    connect(&d->soapService, &MediaBindingService::getStreamUriDone,
            this, &OnvifMediaService::getStreamUriDone);
    connect(&d->soapService, &MediaBindingService::getStreamUriError,
            this, &OnvifMediaService::getStreamUriError);
}

void OnvifMediaService::connectToService()
{
    if(!d->recievedServiceCapabilities) {
        d->device->updateSoapCredentials(d->soapService.clientInterface());
        d->soapService.asyncGetServiceCapabilities();
    }
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetProfiles();
}

void OnvifMediaService::disconnectFromService()
{
    d->recievedServiceCapabilities = false;
    d->profileList.clear();
    d->selectedProfile = OnvifMediaProfile();
    d->snapshotUri.clear();
    d->streamUri.clear();
}

QList<OnvifMediaProfile> OnvifMediaService::getProfileList() const
{
    return d->profileList;
}

void OnvifMediaService::selectProfile(const OnvifMediaProfile &profile)
{
    d->selectedProfile = profile;

    OnvifSoapMedia::TRT__GetSnapshotUri requestSnapshot;
    requestSnapshot.setProfileToken(d->selectedProfile.token());
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetSnapshotUri(requestSnapshot);

    OnvifSoapMedia::TRT__GetStreamUri requestStream;
    requestStream.setProfileToken(d->selectedProfile.token());
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetStreamUri(requestStream);
}

bool OnvifMediaService::supportsSnapshotUri() const
{
    return d->supportsSnapshotUri;
}

QUrl OnvifMediaService::getSnapshotUri() const
{
    return d->snapshotUri;
}

QUrl OnvifMediaService::getStreamUri() const
{
    return d->streamUri;
}

void OnvifMediaService::setServiceCapabilities(TRT__Capabilities capabilities)
{
    d->recievedServiceCapabilities = true;
    d->supportsSnapshotUri = capabilities.snapshotUri();
    emit supportsSnapshotUriAvailable(d->supportsSnapshotUri);
}

void OnvifMediaService::getServiceCapabilitiesDone(const TRT__GetServiceCapabilitiesResponse &parameters)
{
    setServiceCapabilities(parameters.capabilities());
}

void OnvifMediaService::getServiceCapabilitiesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifMediaService::getProfilesDone(const OnvifSoapMedia::TRT__GetProfilesResponse &parameters)
{
    d->profileList.clear();
    for(auto profile : parameters.profiles()) {
        d->profileList << OnvifMediaProfile(profile);
    }

    emit profileListAvailable(d->profileList);
}

void OnvifMediaService::getProfilesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifMediaService::getSnapshotUriDone(const OnvifSoapMedia::TRT__GetSnapshotUriResponse &parameters)
{
    //TODO: what to do if invalidAfterReboot or invalidAfterConnect?
    //Q_ASSERT(!parameters.mediaUri().invalidAfterConnect());
    //Q_ASSERT(!parameters.mediaUri().invalidAfterReboot());
    //TODO: what to do if timeout?
    //Q_ASSERT(!parameters.mediaUri().timeout());
    d->snapshotUri = QUrl(parameters.mediaUri().uri());
    if(d->snapshotUri.userInfo().isEmpty())
    {
        d->device->updateUrlCredentials(&d->snapshotUri);
    }
    emit snapshotUriAvailable(d->snapshotUri);
}

void OnvifMediaService::getSnapshotUriError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifMediaService::getStreamUriDone(const OnvifSoapMedia::TRT__GetStreamUriResponse &parameters)
{
    //TODO: what to do if invalidAfterReboot or invalidAfterConnect?
    //Q_ASSERT(!parameters.mediaUri().invalidAfterConnect());
    //Q_ASSERT(!parameters.mediaUri().invalidAfterReboot());
    //TODO: what to do if timeout?
    //Q_ASSERT(!parameters.mediaUri().timeout());
    d->streamUri = QUrl(parameters.mediaUri().uri());
    if(d->streamUri.userInfo().isEmpty())
    {
        d->device->updateUrlCredentials(&d->streamUri);
    }
    emit streamUriAvailable(d->streamUri);
}

void OnvifMediaService::getStreamUriError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}
