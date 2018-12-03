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

#define Q_FUNC_INFO_AS_STRING (QString(static_cast<const char*>(Q_FUNC_INFO)))

class OnvifMedia2ServicePrivate
{
    Q_DISABLE_COPY(OnvifMedia2ServicePrivate)
public:
    OnvifMedia2ServicePrivate(OnvifDeviceConnection *device) :
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
    QString preferredVideoStreamProtocol;
};

OnvifMedia2Service::OnvifMedia2Service(const QString &endpointAddress, const TR2__Capabilities2& capabilities, OnvifDeviceConnection *parent) :
    QObject(parent),
    d_ptr(new OnvifMedia2ServicePrivate(parent))
{
    Q_D(OnvifMedia2Service);
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

OnvifMedia2Service::~OnvifMedia2Service() = default;

void OnvifMedia2Service::connectToService()
{
    Q_D(OnvifMedia2Service);
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    TR2__GetProfiles request;
    request.setType(QStringList() << "All");
    d->soapService.asyncGetProfiles(request);
}

void OnvifMedia2Service::disconnectFromService()
{
    Q_D(OnvifMedia2Service);
    d->profileList.clear();
    d->selectedProfile = OnvifMediaProfile();
    d->snapshotUri.clear();
    d->streamUri.clear();
}

QList<OnvifMediaProfile> OnvifMedia2Service::getProfileList() const
{
    Q_D(const OnvifMedia2Service);
    return d->profileList;
}

void OnvifMedia2Service::selectProfile(const OnvifMediaProfile &profile)
{
    Q_D(OnvifMedia2Service);
    d->selectedProfile = profile;

    if(d->supportsSnapshotUri) {
        TR2__GetSnapshotUri requestSnapshot;
        requestSnapshot.setProfileToken(d->selectedProfile.token());
        d->device->updateSoapCredentials(d->soapService.clientInterface());
        d->soapService.asyncGetSnapshotUri(requestSnapshot);
    }

    TR2__GetStreamUri requestStream;
    requestStream.setProfileToken(d->selectedProfile.token());
    if(!d->preferredVideoStreamProtocol.isEmpty()) {
        requestStream.setProtocol(d->preferredVideoStreamProtocol);
    }
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetStreamUri(requestStream);
}

bool OnvifMedia2Service::supportsSnapshotUri() const
{
    Q_D(const OnvifMedia2Service);
    return d->supportsSnapshotUri;
}

QUrl OnvifMedia2Service::getSnapshotUri() const
{
    Q_D(const OnvifMedia2Service);
    return d->snapshotUri;
}

QUrl OnvifMedia2Service::getStreamUri() const
{
    Q_D(const OnvifMedia2Service);
    return d->streamUri;
}

void OnvifMedia2Service::setPreferredVideoStreamProtocol(const QString &preferredVideoStreamProtocol)
{
    Q_D(OnvifMedia2Service);
    d->preferredVideoStreamProtocol = preferredVideoStreamProtocol;
}

void OnvifMedia2Service::getProfilesDone(const TR2__GetProfilesResponse &parameters)
{
    Q_D(OnvifMedia2Service);
    d->profileList.clear();
    for(const auto& profile : parameters.profiles()) {
        d->profileList << OnvifMediaProfile(profile);
    }
    emit profileListAvailable(d->profileList);
}

void OnvifMedia2Service::getProfilesError(const KDSoapMessage &fault)
{
    Q_D(OnvifMedia2Service);
    d->device->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMedia2Service::getSnapshotUriDone(const TR2__GetSnapshotUriResponse &parameters)
{
    Q_D(OnvifMedia2Service);
    d->snapshotUri = QUrl(parameters.uri());
    d->device->updateUrlHost(&d->snapshotUri);
    if(d->snapshotUri.userInfo().isEmpty())
    {
        d->device->updateUrlCredentials(&d->snapshotUri);
    }
    emit snapshotUriAvailable(d->snapshotUri);
}

void OnvifMedia2Service::getSnapshotUriError(const KDSoapMessage &fault)
{
    Q_D(OnvifMedia2Service);
    d->device->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMedia2Service::getStreamUriDone(const TR2__GetStreamUriResponse &parameters)
{
    Q_D(OnvifMedia2Service);
    d->streamUri = QUrl(parameters.uri());
    d->device->updateUrlHost(&d->streamUri);
    if(d->streamUri.userInfo().isEmpty())
    {
        d->device->updateUrlCredentials(&d->streamUri);
    }
    emit streamUriAvailable(d->streamUri);
}

void OnvifMedia2Service::getStreamUriError(const KDSoapMessage &fault)
{
    Q_D(OnvifMedia2Service);
    d->device->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMedia2Service::setServiceCapabilities(const TR2__Capabilities2 &capabilities)
{
    Q_D(OnvifMedia2Service);
    d->supportsSnapshotUri = capabilities.snapshotUri();
    emit supportsSnapshotUriAvailable(d->supportsSnapshotUri);
}
