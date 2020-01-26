/* Copyright (C) 2018 Casper Meijn <casper@meijn.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
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
#include "onvifmediaservice.h"

#include <QDebug>
#include <QUrl>
#include "wsdl_media.h"

using namespace OnvifSoapMedia;

#define Q_FUNC_INFO_AS_STRING (QString(static_cast<const char*>(Q_FUNC_INFO)))

class OnvifMediaServicePrivate
{
    Q_DISABLE_COPY(OnvifMediaServicePrivate)
    Q_DECLARE_PUBLIC(OnvifMediaService)
private:
    OnvifMediaServicePrivate(OnvifMediaService* service, OnvifDeviceConnection* device) :
        q_ptr(service),
        device(device),
        recievedServiceCapabilities(false),
        supportsSnapshotUri(true)
    {;}

    OnvifMediaService* const q_ptr;

    OnvifDeviceConnection* device;
    OnvifSoapMedia::MediaBindingService soapService;
    QList<OnvifMediaProfile> profileList;
    OnvifMediaProfile selectedProfile;
    bool recievedServiceCapabilities;
    bool supportsSnapshotUri;
    QUrl snapshotUri;
    QUrl streamUri;
    QString preferredVideoStreamProtocol;

    void getServiceCapabilitiesDone(const OnvifSoapMedia::TRT__GetServiceCapabilitiesResponse& parameters);
    void getServiceCapabilitiesError(const KDSoapMessage& fault);
    void getProfilesDone(const OnvifSoapMedia::TRT__GetProfilesResponse& parameters);
    void getProfilesError(const KDSoapMessage& fault);
    void getSnapshotUriDone(const OnvifSoapMedia::TRT__GetSnapshotUriResponse& parameters);
    void getSnapshotUriError(const KDSoapMessage& fault);
    void getStreamUriDone(const OnvifSoapMedia::TRT__GetStreamUriResponse& parameters);
    void getStreamUriError(const KDSoapMessage& fault);
};

OnvifMediaService::OnvifMediaService(const QString& endpointAddress, OnvifDeviceConnection* parent) :
    QObject(parent),
    d_ptr(new OnvifMediaServicePrivate(this, parent))
{
    Q_D(OnvifMediaService);
    d->soapService.setEndPoint(endpointAddress);

    connect(&d->soapService, &MediaBindingService::getServiceCapabilitiesDone,
    [d](const OnvifSoapMedia::TRT__GetServiceCapabilitiesResponse & parameters) {
        d->getServiceCapabilitiesDone(parameters);
    });
    connect(&d->soapService, &MediaBindingService::getServiceCapabilitiesError,
    [d](const KDSoapMessage & fault) {
        d->getServiceCapabilitiesError(fault);
    });
    connect(&d->soapService, &MediaBindingService::getProfilesDone,
    [d](const OnvifSoapMedia::TRT__GetProfilesResponse & parameters) {
        d->getProfilesDone(parameters);
    });
    connect(&d->soapService, &MediaBindingService::getProfilesError,
    [d](const KDSoapMessage & fault) {
        d->getProfilesError(fault);
    });
    connect(&d->soapService, &MediaBindingService::getSnapshotUriDone,
    [d](const OnvifSoapMedia::TRT__GetSnapshotUriResponse & parameters) {
        d->getSnapshotUriDone(parameters);
    });
    connect(&d->soapService, &MediaBindingService::getSnapshotUriError,
    [d](const KDSoapMessage & fault) {
        d->getSnapshotUriError(fault);
    });
    connect(&d->soapService, &MediaBindingService::getStreamUriDone,
    [d](const OnvifSoapMedia::TRT__GetStreamUriResponse & parameters) {
        d->getStreamUriDone(parameters);
    });
    connect(&d->soapService, &MediaBindingService::getStreamUriError,
    [d](const KDSoapMessage & fault) {
        d->getStreamUriError(fault);
    });
}

OnvifMediaService::~OnvifMediaService() = default;

void OnvifMediaService::connectToService()
{
    Q_D(OnvifMediaService);
    if (!d->recievedServiceCapabilities) {
        d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
        d->soapService.asyncGetServiceCapabilities();
    }
    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
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

void OnvifMediaService::selectProfile(const OnvifMediaProfile& profile)
{
    Q_D(OnvifMediaService);
    d->selectedProfile = profile;

    OnvifSoapMedia::TRT__GetSnapshotUri requestSnapshot;
    requestSnapshot.setProfileToken(d->selectedProfile.token());
    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetSnapshotUri(requestSnapshot);

    OnvifSoapMedia::TRT__GetStreamUri requestStream;
    requestStream.setProfileToken(d->selectedProfile.token());
    if (d->preferredVideoStreamProtocol == "RtspOverHttp") {
        OnvifSoapMedia::TT__StreamSetup streamSetup;
        streamSetup.setStream(OnvifSoapMedia::TT__StreamType::RTP_Unicast);
        OnvifSoapMedia::TT__Transport transport;
        transport.setProtocol(OnvifSoapMedia::TT__TransportProtocol::HTTP);
        streamSetup.setTransport(transport);
        requestStream.setStreamSetup(streamSetup);
    } else if (d->preferredVideoStreamProtocol == "RtspUnicast") {
        OnvifSoapMedia::TT__StreamSetup streamSetup;
        streamSetup.setStream(OnvifSoapMedia::TT__StreamType::RTP_Unicast);
        OnvifSoapMedia::TT__Transport transport;
        transport.setProtocol(OnvifSoapMedia::TT__TransportProtocol::UDP);
        streamSetup.setTransport(transport);
        requestStream.setStreamSetup(streamSetup);
    } else if (d->preferredVideoStreamProtocol == "RTSP") {
        OnvifSoapMedia::TT__StreamSetup streamSetup;
        streamSetup.setStream(OnvifSoapMedia::TT__StreamType::RTP_Unicast);
        OnvifSoapMedia::TT__Transport transport;
        transport.setProtocol(OnvifSoapMedia::TT__TransportProtocol::RTSP);
        streamSetup.setTransport(transport);
        requestStream.setStreamSetup(streamSetup);
    } else if (!d->preferredVideoStreamProtocol.isEmpty()) {
        qWarning() << "Warning: unknown preferredVideoStreamProtocol" << d->preferredVideoStreamProtocol;
    }
    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
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

void OnvifMediaService::setServiceCapabilities(const OnvifSoapDevicemgmt::TT__MediaCapabilities& capabilities)
{
    Q_D(OnvifMediaService);
    Q_UNUSED(capabilities);
    // Lets assume that snapshots are supported and hope the GetServiceCapabilities call provide the exact answer
    d->supportsSnapshotUri = true;
    emit supportsSnapshotUriAvailable(d->supportsSnapshotUri);
}

void OnvifMediaService::setPreferredVideoStreamProtocol(const QString& preferredVideoStreamProtocol)
{
    Q_D(OnvifMediaService);
    d->preferredVideoStreamProtocol = preferredVideoStreamProtocol;
}

void OnvifMediaServicePrivate::getServiceCapabilitiesDone(const TRT__GetServiceCapabilitiesResponse& parameters)
{
    Q_Q(OnvifMediaService);
    q->setServiceCapabilities(parameters.capabilities());
}

void OnvifMediaServicePrivate::getServiceCapabilitiesError(const KDSoapMessage& fault)
{
    // Some older devices don't support the GetServiceCapabilities call
    // Therefore we mark the service finished and ignore any error
    recievedServiceCapabilities = true;
    qDebug() << "The Media::GetServiceCapabilities call failed; this is expected for older ONVIF devices:" << fault.faultAsString();

    // Lets assume that snapshots are supported
    supportsSnapshotUri = true;
}

void OnvifMediaServicePrivate::getProfilesDone(const OnvifSoapMedia::TRT__GetProfilesResponse& parameters)
{
    Q_Q(OnvifMediaService);
    profileList.clear();
    const auto& responseProfileList = parameters.profiles();
    for (auto& profile : responseProfileList) {
        profileList << OnvifMediaProfile(profile);
    }

    emit q->profileListAvailable(profileList);
}

void OnvifMediaServicePrivate::getProfilesError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMediaServicePrivate::getSnapshotUriDone(const OnvifSoapMedia::TRT__GetSnapshotUriResponse& parameters)
{
    Q_Q(OnvifMediaService);
    //TODO: what to do if invalidAfterReboot or invalidAfterConnect?
    //Q_ASSERT(!parameters.mediaUri().invalidAfterConnect());
    //Q_ASSERT(!parameters.mediaUri().invalidAfterReboot());
    //TODO: what to do if timeout?
    //Q_ASSERT(!parameters.mediaUri().timeout());
    snapshotUri = QUrl(parameters.mediaUri().uri());
    device->d_ptr->updateUrlHost(&snapshotUri);
    if (snapshotUri.userInfo().isEmpty()) {
        device->d_ptr->updateUrlCredentials(&snapshotUri);
    }
    emit q->snapshotUriAvailable(snapshotUri);
}

void OnvifMediaServicePrivate::getSnapshotUriError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifMediaServicePrivate::getStreamUriDone(const OnvifSoapMedia::TRT__GetStreamUriResponse& parameters)
{
    Q_Q(OnvifMediaService);
    //TODO: what to do if invalidAfterReboot or invalidAfterConnect?
    //Q_ASSERT(!parameters.mediaUri().invalidAfterConnect());
    //Q_ASSERT(!parameters.mediaUri().invalidAfterReboot());
    //TODO: what to do if timeout?
    //Q_ASSERT(!parameters.mediaUri().timeout());
    streamUri = QUrl(parameters.mediaUri().uri());
    device->d_ptr->updateUrlHost(&streamUri);
    if (streamUri.userInfo().isEmpty()) {
        device->d_ptr->updateUrlCredentials(&streamUri);
    }
    emit q->streamUriAvailable(streamUri);
}

void OnvifMediaServicePrivate::getStreamUriError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}
