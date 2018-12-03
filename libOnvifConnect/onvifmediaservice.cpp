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

#define Q_FUNC_INFO_AS_STRING (QString(static_cast<const char*>(Q_FUNC_INFO)))

class OnvifMediaServicePrivate
{
    Q_DISABLE_COPY(OnvifMediaServicePrivate)
public:
    OnvifMediaServicePrivate(OnvifDeviceConnection *device) :
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
    QString preferredVideoStreamProtocol;
};

OnvifMediaService::OnvifMediaService(const QString &endpointAddress, OnvifDeviceConnection *parent) :
    QObject(parent),
    d_ptr(new OnvifMediaServicePrivate(parent))
{
    Q_D(OnvifMediaService);
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

OnvifMediaService::~OnvifMediaService() = default;

void OnvifMediaService::connectToService()
{
    Q_D(OnvifMediaService);
    if(!d->recievedServiceCapabilities) {
        d->device->updateSoapCredentials(d->soapService.clientInterface());
        d->soapService.asyncGetServiceCapabilities();
    }
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetProfiles();
}

void OnvifMediaService::disconnectFromService()
{
    Q_D(OnvifMediaService);
    d->recievedServiceCapabilities = false;
    d->profileList.clear();
    d->selectedProfile = OnvifMediaProfile();
    d->snapshotUri.clear();
    d->streamUri.clear();
}

QList<OnvifMediaProfile> OnvifMediaService::getProfileList() const
{
    Q_D(const OnvifMediaService);
    return d->profileList;
}

void OnvifMediaService::selectProfile(const OnvifMediaProfile &profile)
{
    Q_D(OnvifMediaService);
    d->selectedProfile = profile;

    OnvifSoapMedia::TRT__GetSnapshotUri requestSnapshot;
    requestSnapshot.setProfileToken(d->selectedProfile.token());
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetSnapshotUri(requestSnapshot);

    OnvifSoapMedia::TRT__GetStreamUri requestStream;
    requestStream.setProfileToken(d->selectedProfile.token());
    if(d->preferredVideoStreamProtocol == "RtspOverHttp") {
        OnvifSoapMedia::TT__StreamSetup streamSetup;
        streamSetup.setStream(OnvifSoapMedia::TT__StreamType::RTP_Unicast);
        OnvifSoapMedia::TT__Transport transport;
        transport.setProtocol(OnvifSoapMedia::TT__TransportProtocol::HTTP);
        streamSetup.setTransport(transport);
        requestStream.setStreamSetup(streamSetup);
    } else if(d->preferredVideoStreamProtocol == "RtspUnicast") {
        OnvifSoapMedia::TT__StreamSetup streamSetup;
        streamSetup.setStream(OnvifSoapMedia::TT__StreamType::RTP_Unicast);
        OnvifSoapMedia::TT__Transport transport;
        transport.setProtocol(OnvifSoapMedia::TT__TransportProtocol::UDP);
        streamSetup.setTransport(transport);
        requestStream.setStreamSetup(streamSetup);
    } else if(d->preferredVideoStreamProtocol == "RTSP") {
        OnvifSoapMedia::TT__StreamSetup streamSetup;
        streamSetup.setStream(OnvifSoapMedia::TT__StreamType::RTP_Unicast);
        OnvifSoapMedia::TT__Transport transport;
        transport.setProtocol(OnvifSoapMedia::TT__TransportProtocol::RTSP);
        streamSetup.setTransport(transport);
        requestStream.setStreamSetup(streamSetup);
    } else if(!d->preferredVideoStreamProtocol.isEmpty()){
        qWarning() << "Warning: unknown preferredVideoStreamProtocol" << d->preferredVideoStreamProtocol;
    }
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetStreamUri(requestStream);
}

bool OnvifMediaService::supportsSnapshotUri() const
{
    Q_D(const OnvifMediaService);
    return d->supportsSnapshotUri;
}

QUrl OnvifMediaService::getSnapshotUri() const
{
    Q_D(const OnvifMediaService);
    return d->snapshotUri;
}

QUrl OnvifMediaService::getStreamUri() const
{
    Q_D(const OnvifMediaService);
    return d->streamUri;
}

void OnvifMediaService::setServiceCapabilities(const TRT__Capabilities& capabilities)
{
    Q_D(OnvifMediaService);
    d->recievedServiceCapabilities = true;
    d->supportsSnapshotUri = capabilities.snapshotUri();
    emit supportsSnapshotUriAvailable(d->supportsSnapshotUri);
}

void OnvifMediaService::setPreferredVideoStreamProtocol(const QString &preferredVideoStreamProtocol)
{
    Q_D(OnvifMediaService);
    d->preferredVideoStreamProtocol = preferredVideoStreamProtocol;
}

void OnvifMediaService::getServiceCapabilitiesDone(const TRT__GetServiceCapabilitiesResponse &parameters)
{
    setServiceCapabilities(parameters.capabilities());
}

void OnvifMediaService::getServiceCapabilitiesError(const KDSoapMessage &fault)
{
    Q_D(OnvifMediaService);
    d->device->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMediaService::getProfilesDone(const OnvifSoapMedia::TRT__GetProfilesResponse &parameters)
{
    Q_D(OnvifMediaService);
    d->profileList.clear();
    for(const auto& profile : parameters.profiles()) {
        d->profileList << OnvifMediaProfile(profile);
    }

    emit profileListAvailable(d->profileList);
}

void OnvifMediaService::getProfilesError(const KDSoapMessage &fault)
{
    Q_D(OnvifMediaService);
    d->device->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMediaService::getSnapshotUriDone(const OnvifSoapMedia::TRT__GetSnapshotUriResponse &parameters)
{
    Q_D(OnvifMediaService);
    //TODO: what to do if invalidAfterReboot or invalidAfterConnect?
    //Q_ASSERT(!parameters.mediaUri().invalidAfterConnect());
    //Q_ASSERT(!parameters.mediaUri().invalidAfterReboot());
    //TODO: what to do if timeout?
    //Q_ASSERT(!parameters.mediaUri().timeout());
    d->snapshotUri = QUrl(parameters.mediaUri().uri());
    d->device->updateUrlHost(&d->snapshotUri);
    if(d->snapshotUri.userInfo().isEmpty())
    {
        d->device->updateUrlCredentials(&d->snapshotUri);
    }
    emit snapshotUriAvailable(d->snapshotUri);
}

void OnvifMediaService::getSnapshotUriError(const KDSoapMessage &fault)
{
    Q_D(OnvifMediaService);
    d->device->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMediaService::getStreamUriDone(const OnvifSoapMedia::TRT__GetStreamUriResponse &parameters)
{
    Q_D(OnvifMediaService);
    //TODO: what to do if invalidAfterReboot or invalidAfterConnect?
    //Q_ASSERT(!parameters.mediaUri().invalidAfterConnect());
    //Q_ASSERT(!parameters.mediaUri().invalidAfterReboot());
    //TODO: what to do if timeout?
    //Q_ASSERT(!parameters.mediaUri().timeout());
    d->streamUri = QUrl(parameters.mediaUri().uri());
    d->device->updateUrlHost(&d->streamUri);
    if(d->streamUri.userInfo().isEmpty())
    {
        d->device->updateUrlCredentials(&d->streamUri);
    }
    emit streamUriAvailable(d->streamUri);
}

void OnvifMediaService::getStreamUriError(const KDSoapMessage &fault)
{
    Q_D(OnvifMediaService);
    d->device->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}
