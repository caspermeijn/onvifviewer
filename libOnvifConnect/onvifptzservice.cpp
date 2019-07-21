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
#include "onvifptzservice.h"

#include "onvifdeviceconnection_p.h"
#include "onvifmediaprofile.h"
#include <QDebug>
#include <QUrl>
#include "wsdl_ptz.h"

using namespace OnvifSoapPtz;

#define Q_FUNC_INFO_AS_STRING (QString(static_cast<const char*>(Q_FUNC_INFO)))

class OnvifPtzServicePrivate
{
    Q_DISABLE_COPY(OnvifPtzServicePrivate)
    Q_DECLARE_PUBLIC(OnvifPtzService)
private:
    OnvifPtzServicePrivate(OnvifPtzService* service, OnvifDeviceConnection* device) :
        q_ptr(service),
        device(device),
        recievedServiceCapabilities(false)
    {;}

    OnvifPtzService* const q_ptr;

    OnvifDeviceConnection* device;
    OnvifSoapPtz::PTZBindingService soapService;
    QList< OnvifSoapPtz::TT__PTZNode > nodeList;
    bool recievedServiceCapabilities;

    void getServiceCapabilitiesDone(const OnvifSoapPtz::TPTZ__GetServiceCapabilitiesResponse& parameters);
    void getServiceCapabilitiesError(const KDSoapMessage& fault);
    void getNodesDone(const OnvifSoapPtz::TPTZ__GetNodesResponse& parameters);
    void getNodesError(const KDSoapMessage& fault);
    void getConfigurationsDone(const OnvifSoapPtz::TPTZ__GetConfigurationsResponse& parameters);
    void getConfigurationsError(const KDSoapMessage& fault);
    void getStatusDone(const OnvifSoapPtz::TPTZ__GetStatusResponse& parameters);
    void getStatusError(const KDSoapMessage& fault);
    void absoluteMoveDone(const OnvifSoapPtz::TPTZ__AbsoluteMoveResponse& parameters);
    void absoluteMoveError(const KDSoapMessage& fault);
    void relativeMoveDone(const OnvifSoapPtz::TPTZ__RelativeMoveResponse& parameters);
    void relativeMoveError(const KDSoapMessage& fault);
    void continuousMoveDone(const OnvifSoapPtz::TPTZ__ContinuousMoveResponse& parameters);
    void continuousMoveError(const KDSoapMessage& fault);
    void gotoHomePositionDone(const OnvifSoapPtz::TPTZ__GotoHomePositionResponse& parameters);
    void gotoHomePositionError(const KDSoapMessage& fault);
    void setHomePositionDone(const OnvifSoapPtz::TPTZ__SetHomePositionResponse& parameters);
    void setHomePositionError(const KDSoapMessage& fault);
    void stopDone(const OnvifSoapPtz::TPTZ__StopResponse& parameters);
    void stopError(const KDSoapMessage& fault);
};

OnvifPtzService::OnvifPtzService(const QString& endpointAddress, OnvifDeviceConnection* parent) :
    QObject(parent),
    d_ptr(new OnvifPtzServicePrivate(this, parent))
{
    Q_D(OnvifPtzService);
    d->soapService.setEndPoint(endpointAddress);

    connect(&d->soapService, &PTZBindingService::getServiceCapabilitiesDone,
    [d](const OnvifSoapPtz::TPTZ__GetServiceCapabilitiesResponse & parameters) {
        d->getServiceCapabilitiesDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::getServiceCapabilitiesError,
    [d](const KDSoapMessage & fault) {
        d->getServiceCapabilitiesError(fault);
    });
    connect(&d->soapService, &PTZBindingService::getNodesDone,
    [d](const OnvifSoapPtz::TPTZ__GetNodesResponse & parameters) {
        d->getNodesDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::getNodesError,
    [d](const KDSoapMessage & fault) {
        d->getNodesError(fault);
    });
    connect(&d->soapService, &PTZBindingService::getConfigurationsDone,
    [d](const OnvifSoapPtz::TPTZ__GetConfigurationsResponse & parameters) {
        d->getConfigurationsDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::getConfigurationsError,
    [d](const KDSoapMessage & fault) {
        d->getConfigurationsError(fault);
    });
    connect(&d->soapService, &PTZBindingService::getStatusDone,
    [d](const OnvifSoapPtz::TPTZ__GetStatusResponse & parameters) {
        d->getStatusDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::getStatusError,
    [d](const KDSoapMessage & fault) {
        d->getStatusError(fault);
    });
    connect(&d->soapService, &PTZBindingService::relativeMoveDone,
    [d](const OnvifSoapPtz::TPTZ__RelativeMoveResponse & parameters) {
        d->relativeMoveDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::relativeMoveError,
    [d](const KDSoapMessage & fault) {
        d->relativeMoveError(fault);
    });
    connect(&d->soapService, &PTZBindingService::absoluteMoveDone,
    [d](const OnvifSoapPtz::TPTZ__AbsoluteMoveResponse & parameters) {
        d->absoluteMoveDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::absoluteMoveError,
    [d](const KDSoapMessage & fault) {
        d->absoluteMoveError(fault);
    });
    connect(&d->soapService, &PTZBindingService::continuousMoveDone,
    [d](const OnvifSoapPtz::TPTZ__ContinuousMoveResponse & parameters) {
        d->continuousMoveDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::continuousMoveError,
    [d](const KDSoapMessage & fault) {
        d->continuousMoveError(fault);
    });
    connect(&d->soapService, &PTZBindingService::gotoHomePositionDone,
    [d](const OnvifSoapPtz::TPTZ__GotoHomePositionResponse & parameters) {
        d->gotoHomePositionDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::gotoHomePositionError,
    [d](const KDSoapMessage & fault) {
        d->gotoHomePositionError(fault);
    });
    connect(&d->soapService, &PTZBindingService::setHomePositionDone,
    [d](const OnvifSoapPtz::TPTZ__SetHomePositionResponse & parameters) {
        d->setHomePositionDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::setHomePositionError,
    [d](const KDSoapMessage & fault) {
        d->setHomePositionError(fault);
    });
    connect(&d->soapService, &PTZBindingService::stopDone,
    [d](const OnvifSoapPtz::TPTZ__StopResponse & parameters) {
        d->stopDone(parameters);
    });
    connect(&d->soapService, &PTZBindingService::stopError,
    [d](const KDSoapMessage & fault) {
        d->stopError(fault);
    });
}

OnvifPtzService::~OnvifPtzService() = default;

void OnvifPtzService::connectToService()
{
    Q_D(OnvifPtzService);
    if (!d->recievedServiceCapabilities) {
        d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
        d->soapService.asyncGetServiceCapabilities();
    }

    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetNodes();

    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetConfigurations();
}

void OnvifPtzService::disconnectFromService()
{
    Q_D(OnvifPtzService);
    d->recievedServiceCapabilities = false;
}

void OnvifPtzService::setServiceCapabilities(const OnvifSoapPtz::TPTZ__Capabilities& /*capabilities*/)
{
    Q_D(OnvifPtzService);
    d->recievedServiceCapabilities = true;
    //TODO: Use capabilities
}

void OnvifPtzService::setServiceCapabilities(const OnvifSoapDevicemgmt::TT__PTZCapabilities& capabilities)
{
    Q_D(OnvifPtzService);
    // No useful capabilities are defines
}

void OnvifPtzService::absoluteMove(const OnvifMediaProfile& profile, qreal xFraction, qreal yFraction)
{
    Q_D(OnvifPtzService);
    Q_ASSERT(-1.0 <= xFraction && xFraction <= 1.0);
    Q_ASSERT(-1.0 <= yFraction && yFraction <= 1.0);

    OnvifSoapPtz::TT__Vector2D vector2D;
    vector2D.setX(xFraction);
    vector2D.setY(yFraction);
    vector2D.setSpace("http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");

    OnvifSoapPtz::TT__PTZVector vector;
    vector.setPanTilt(vector2D);

    OnvifSoapPtz::TPTZ__AbsoluteMove request;
    request.setProfileToken(profile.token());
    request.setPosition(vector);

    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncAbsoluteMove(request);
}

void OnvifPtzService::relativeMove(const OnvifMediaProfile& profile, qreal xFraction, qreal yFraction)
{
    Q_D(OnvifPtzService);
    Q_ASSERT(-1.0 <= xFraction && xFraction <= 1.0);
    Q_ASSERT(-1.0 <= yFraction && yFraction <= 1.0);

    OnvifSoapPtz::TT__Vector2D vector2D;
    vector2D.setX(xFraction);
    vector2D.setY(yFraction);
    vector2D.setSpace("http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace");

    OnvifSoapPtz::TT__PTZVector vector;
    vector.setPanTilt(vector2D);

    OnvifSoapPtz::TPTZ__RelativeMove request;
    request.setProfileToken(profile.token());
    request.setTranslation(vector);

    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncRelativeMove(request);
}

bool OnvifPtzService::isContinuousMoveSupported(const OnvifMediaProfile& profile) const
{
    Q_D(const OnvifPtzService);
    for (auto& node : d->nodeList) {
        if (node.token() == profile.ptzNodeToken()) {
            return !node.supportedPTZSpaces().continuousPanTiltVelocitySpace().empty();
        }
    }
    return false;
}

void OnvifPtzService::continuousMove(const OnvifMediaProfile& profile, qreal xFraction, qreal yFraction)
{
    Q_D(OnvifPtzService);
    Q_ASSERT(-1.0 <= xFraction && xFraction <= 1.0);
    Q_ASSERT(-1.0 <= yFraction && yFraction <= 1.0);

    OnvifSoapPtz::TT__Vector2D vector2D;
    vector2D.setX(xFraction);
    vector2D.setY(yFraction);
    vector2D.setSpace("http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");

    OnvifSoapPtz::TT__PTZSpeed speed;
    speed.setPanTilt(vector2D);

    OnvifSoapPtz::TPTZ__ContinuousMove request;
    request.setProfileToken(profile.token());
    request.setVelocity(speed);

    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncContinuousMove(request);
}

bool OnvifPtzService::isRelativeZoomSupported(const OnvifMediaProfile& profile) const
{
    Q_D(const OnvifPtzService);
    for (auto& node : d->nodeList) {
        if (node.token() == profile.ptzNodeToken()) {
            return !node.supportedPTZSpaces().relativeZoomTranslationSpace().empty();
        }
    }
    return false;
}

void OnvifPtzService::relativeZoom(const OnvifMediaProfile& profile, qreal zoomFraction)
{
    Q_D(OnvifPtzService);
    Q_ASSERT(-1.0 <= zoomFraction && zoomFraction <= 1.0);

    OnvifSoapPtz::TT__Vector1D vector1D;
    vector1D.setX(zoomFraction);
    vector1D.setSpace("http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace");

    OnvifSoapPtz::TT__PTZVector vector;
    vector.setZoom(vector1D);

    OnvifSoapPtz::TPTZ__RelativeMove request;
    request.setProfileToken(profile.token());
    request.setTranslation(vector);

    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncRelativeMove(request);
}

void OnvifPtzService::stopMovement(const OnvifMediaProfile& profile)
{
    Q_D(OnvifPtzService);
    OnvifSoapPtz::TPTZ__Stop request;
    request.setProfileToken(profile.token());
    request.setPanTilt(true);
    request.setZoom(true);

    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncStop(request);
}

bool OnvifPtzService::isHomeSupported(const OnvifMediaProfile& profile) const
{
    Q_D(const OnvifPtzService);
    for (auto& node : d->nodeList) {
        if (node.token() == profile.ptzNodeToken()) {
            return node.homeSupported();
        }
    }
    return false;
}

bool OnvifPtzService::isRelativeMoveSupported(const OnvifMediaProfile& profile) const
{
    Q_D(const OnvifPtzService);
    for (auto& node : d->nodeList) {
        if (node.token() == profile.ptzNodeToken()) {
            return !node.supportedPTZSpaces().relativePanTiltTranslationSpace().empty();
        }
    }
    return false;
}

void OnvifPtzService::goToHome(const OnvifMediaProfile& profile)
{
    Q_D(OnvifPtzService);
    OnvifSoapPtz::TPTZ__GotoHomePosition request;
    request.setProfileToken(profile.token());
    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGotoHomePosition(request);
}

void OnvifPtzService::saveHomePosition(const OnvifMediaProfile& profile)
{
    Q_D(OnvifPtzService);
    OnvifSoapPtz::TPTZ__SetHomePosition request;
    request.setProfileToken(profile.token());
    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncSetHomePosition(request);
}

void OnvifPtzServicePrivate::getServiceCapabilitiesDone(const TPTZ__GetServiceCapabilitiesResponse& parameters)
{
    Q_Q(OnvifPtzService);
    q->setServiceCapabilities(parameters.capabilities());
}

void OnvifPtzServicePrivate::getServiceCapabilitiesError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::getNodesDone(const OnvifSoapPtz::TPTZ__GetNodesResponse& parameters)
{
    nodeList = parameters.pTZNode();
}

void OnvifPtzServicePrivate::getNodesError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::getConfigurationsDone(const TPTZ__GetConfigurationsResponse& /*parameters*/)
{
    // TODO: What can we do with the the PTZ configuration
}

void OnvifPtzServicePrivate::getConfigurationsError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::getStatusDone(const OnvifSoapPtz::TPTZ__GetStatusResponse& /*parameters*/)
{
    //TODO: What can we do with the PTZ status?
}

void OnvifPtzServicePrivate::getStatusError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::absoluteMoveDone(const TPTZ__AbsoluteMoveResponse&)
{
    //NOP
}

void OnvifPtzServicePrivate::absoluteMoveError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::relativeMoveDone(const TPTZ__RelativeMoveResponse&)
{
    //NOP
}

void OnvifPtzServicePrivate::relativeMoveError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::continuousMoveDone(const TPTZ__ContinuousMoveResponse& /*parameters*/)
{
    //NOP
}

void OnvifPtzServicePrivate::continuousMoveError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::gotoHomePositionDone(const OnvifSoapPtz::TPTZ__GotoHomePositionResponse&)
{
    //NOP
}

void OnvifPtzServicePrivate::gotoHomePositionError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::setHomePositionDone(const OnvifSoapPtz::TPTZ__SetHomePositionResponse&)
{
    //NOP
}

void OnvifPtzServicePrivate::setHomePositionError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifPtzServicePrivate::stopDone(const TPTZ__StopResponse& /*parameters*/)
{
}

void OnvifPtzServicePrivate::stopError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}
