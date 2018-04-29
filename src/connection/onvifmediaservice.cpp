﻿#include "onvifdeviceconnection.h"
#include "onvifmediaservice.h"

#include <QDebug>
#include <QUrl>
#include "wsdl_media.h"

using namespace OnvifSoapMedia;

class OnvifMediaService::Private
{
public:
    Private(OnvifDeviceConnection *device) :
        device(device)
    {;}

    OnvifDeviceConnection * device;
    OnvifSoapMedia::MediaBindingService soapService;
    QList<OnvifMediaProfile> profileList;
    OnvifMediaProfile selectedProfile;
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
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetServiceCapabilities();

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetProfiles();
}

void OnvifMediaService::disconnectFromService()
{
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

QUrl OnvifMediaService::getSnapshotUri() const
{
    return d->snapshotUri;
}

QUrl OnvifMediaService::getStreamUri() const
{
    return d->streamUri;
}

void OnvifMediaService::getServiceCapabilitiesDone(const TRT__GetServiceCapabilitiesResponse &parameters)
{
    //TODO: find out the service capabilities
    //TODO: Check for required options
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
    Q_ASSERT(!parameters.mediaUri().invalidAfterConnect());
//    Q_ASSERT(!parameters.mediaUri().invalidAfterReboot());
    qDebug() << "snapshot timeout:" << parameters.mediaUri().timeout();
    //TODO: what to do if timeout?
//    Q_ASSERT(!parameters.mediaUri().timeout());
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
    Q_ASSERT(!parameters.mediaUri().invalidAfterConnect());
//    Q_ASSERT(!parameters.mediaUri().invalidAfterReboot());
    qDebug() << "snapshot timeout:" << parameters.mediaUri().timeout();
    //TODO: what to do if timeout?
//    Q_ASSERT(!parameters.mediaUri().timeout());
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

OnvifMediaProfile::OnvifMediaProfile() :
    m_fixed(false)
{
}

OnvifMediaProfile::OnvifMediaProfile(const TT__Profile &profile) :
    m_fixed(profile.fixed()),
    m_name(profile.name()),
    m_token(profile.token().value())
{
    Q_ASSERT(m_token.size());
}

QString OnvifMediaProfile::name() const
{
    return m_name;
}

void OnvifMediaProfile::setName(const QString &name)
{
    m_name = name;
}

QString OnvifMediaProfile::token() const
{
    return m_token;
}

void OnvifMediaProfile::setToken(const QString &token)
{
    m_token = token;
}

bool OnvifMediaProfile::fixed() const
{
    return m_fixed;
}

void OnvifMediaProfile::setFixed(bool fixed)
{
    m_fixed = fixed;
}

QDebug operator<<(QDebug debug, const OnvifMediaProfile &p)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "(Fixed: " << p.fixed()
                    << ", Name: " << p.name()
                    << ", Token: " << p.token() << ')';
    return debug;
}