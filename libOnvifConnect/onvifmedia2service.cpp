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
#include "onvifdeviceconnection_p.h"
#include "onvifmedia2service.h"

#include <QDebug>
#include <QUrl>
#include "wsdl_media2.h"

using namespace OnvifSoapMedia2;

#define Q_FUNC_INFO_AS_STRING (QString(static_cast<const char*>(Q_FUNC_INFO)))

class OnvifMedia2ServicePrivate
{
    Q_DISABLE_COPY(OnvifMedia2ServicePrivate)
    Q_DECLARE_PUBLIC(OnvifMedia2Service)
private:
    OnvifMedia2ServicePrivate(OnvifMedia2Service * service, OnvifDeviceConnection *device) :
        q_ptr(service),
        device(device),
        supportsSnapshotUri(true)
    {;}

    OnvifMedia2Service * const q_ptr;

    OnvifDeviceConnection * device;
    Media2BindingService soapService;
    QList<OnvifMediaProfile> profileList;
    OnvifMediaProfile selectedProfile;
    bool supportsSnapshotUri;
    QUrl snapshotUri;
    QUrl streamUri;
    QString preferredVideoStreamProtocol;

    void getProfilesDone( const OnvifSoapMedia2::TR2__GetProfilesResponse& parameters );
    void getProfilesError( const KDSoapMessage& fault );
    void getSnapshotUriDone( const OnvifSoapMedia2::TR2__GetSnapshotUriResponse& parameters );
    void getSnapshotUriError( const KDSoapMessage& fault );
    void getStreamUriDone( const OnvifSoapMedia2::TR2__GetStreamUriResponse& parameters );
    void getStreamUriError( const KDSoapMessage& fault );

    void setServiceCapabilities(const OnvifSoapMedia2::TR2__Capabilities2& capabilities);
};

OnvifMedia2Service::OnvifMedia2Service(const QString &endpointAddress, const TR2__Capabilities2& capabilities, OnvifDeviceConnection *parent) :
    QObject(parent),
    d_ptr(new OnvifMedia2ServicePrivate(this, parent))
{
    Q_D(OnvifMedia2Service);
    d->soapService.setEndPoint(endpointAddress);

    connect(&d->soapService, &Media2BindingService::getProfilesDone,
            [d](const OnvifSoapMedia2::TR2__GetProfilesResponse& parameters){d->getProfilesDone(parameters);});
    connect(&d->soapService, &Media2BindingService::getProfilesError,
            [d](const KDSoapMessage& fault){d->getProfilesError(fault);});
    connect(&d->soapService, &Media2BindingService::getSnapshotUriDone,
            [d](const OnvifSoapMedia2::TR2__GetSnapshotUriResponse& parameters){d->getSnapshotUriDone(parameters);});
    connect(&d->soapService, &Media2BindingService::getSnapshotUriError,
            [d](const KDSoapMessage& fault){d->getSnapshotUriError(fault);});
    connect(&d->soapService, &Media2BindingService::getStreamUriDone,
            [d](const OnvifSoapMedia2::TR2__GetStreamUriResponse& parameters){d->getStreamUriDone(parameters);});
    connect(&d->soapService, &Media2BindingService::getStreamUriError,
            [d](const KDSoapMessage& fault){d->getStreamUriError(fault);});

    d->setServiceCapabilities(capabilities);
}

OnvifMedia2Service::~OnvifMedia2Service() = default;

void OnvifMedia2Service::connectToService()
{
    Q_D(OnvifMedia2Service);
    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
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
        d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
        d->soapService.asyncGetSnapshotUri(requestSnapshot);
    }

    TR2__GetStreamUri requestStream;
    requestStream.setProfileToken(d->selectedProfile.token());
    if(!d->preferredVideoStreamProtocol.isEmpty()) {
        requestStream.setProtocol(d->preferredVideoStreamProtocol);
    }
    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
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

void OnvifMedia2ServicePrivate::getProfilesDone(const TR2__GetProfilesResponse &parameters)
{
    Q_Q(OnvifMedia2Service);
    profileList.clear();
    const auto& responseProfileList = parameters.profiles();
    for(auto& profile : responseProfileList) {
        profileList << OnvifMediaProfile(profile);
    }
    emit q->profileListAvailable(profileList);
}

void OnvifMedia2ServicePrivate::getProfilesError(const KDSoapMessage &fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMedia2ServicePrivate::getSnapshotUriDone(const TR2__GetSnapshotUriResponse &parameters)
{
    Q_Q(OnvifMedia2Service);
    snapshotUri = QUrl(parameters.uri());
    device->d_ptr->updateUrlHost(&snapshotUri);
    if(snapshotUri.userInfo().isEmpty())
    {
        device->d_ptr->updateUrlCredentials(&snapshotUri);
    }
    emit q->snapshotUriAvailable(snapshotUri);
}

void OnvifMedia2ServicePrivate::getSnapshotUriError(const KDSoapMessage &fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMedia2ServicePrivate::getStreamUriDone(const TR2__GetStreamUriResponse &parameters)
{
    Q_Q(OnvifMedia2Service);
    streamUri = QUrl(parameters.uri());
    device->d_ptr->updateUrlHost(&streamUri);
    if(streamUri.userInfo().isEmpty())
    {
        device->d_ptr->updateUrlCredentials(&streamUri);
    }
    emit q->streamUriAvailable(streamUri);
}

void OnvifMedia2ServicePrivate::getStreamUriError(const KDSoapMessage &fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMedia2ServicePrivate::setServiceCapabilities(const TR2__Capabilities2 &capabilities)
{
    Q_Q(OnvifMedia2Service);
    supportsSnapshotUri = capabilities.snapshotUri();
    emit q->supportsSnapshotUriAvailable(supportsSnapshotUri);
}
