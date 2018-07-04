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

#include "onvifdeviceconnection.h"
#include "onvifmediaprofile.h"
#include <QDebug>
#include <QUrl>
#include "wsdl_ptz.h"

using namespace OnvifSoapPtz;

class OnvifPtzService::Private
{
public:
    Private(OnvifDeviceConnection *device) :
        device(device),
        recievedServiceCapabilities(false)
    {;}

    OnvifDeviceConnection * device;
    OnvifSoapPtz::PTZBindingService soapService;
    QList< OnvifSoapPtz::TT__PTZNode > nodeList;
    bool recievedServiceCapabilities;
};

OnvifPtzService::OnvifPtzService(const QString &endpointAddress, OnvifDeviceConnection *parent) :
    QObject(parent),
    d(new Private(parent))
{
    d->soapService.setEndPoint(endpointAddress);

    connect(&d->soapService, &PTZBindingService::getServiceCapabilitiesDone,
            this, &OnvifPtzService::getServiceCapabilitiesDone);
    connect(&d->soapService, &PTZBindingService::getServiceCapabilitiesError,
            this, &OnvifPtzService::getServiceCapabilitiesError);
    connect(&d->soapService, &PTZBindingService::getNodesDone,
            this, &OnvifPtzService::getNodesDone);
    connect(&d->soapService, &PTZBindingService::getNodesError,
            this, &OnvifPtzService::getNodesError);
    connect(&d->soapService, &PTZBindingService::getConfigurationsDone,
            this, &OnvifPtzService::getConfigurationsDone);
    connect(&d->soapService, &PTZBindingService::getConfigurationsError,
            this, &OnvifPtzService::getConfigurationsError);
    connect(&d->soapService, &PTZBindingService::getStatusDone,
            this, &OnvifPtzService::getStatusDone);
    connect(&d->soapService, &PTZBindingService::getStatusError,
            this, &OnvifPtzService::getStatusError);
    connect(&d->soapService, &PTZBindingService::relativeMoveDone,
            this, &OnvifPtzService::relativeMoveDone);
    connect(&d->soapService, &PTZBindingService::relativeMoveError,
            this, &OnvifPtzService::relativeMoveError);
    connect(&d->soapService, &PTZBindingService::absoluteMoveDone,
            this, &OnvifPtzService::absoluteMoveDone);
    connect(&d->soapService, &PTZBindingService::absoluteMoveError,
            this, &OnvifPtzService::absoluteMoveError);
    connect(&d->soapService, &PTZBindingService::continuousMoveDone,
            this, &OnvifPtzService::continuousMoveDone);
    connect(&d->soapService, &PTZBindingService::continuousMoveError,
            this, &OnvifPtzService::continuousMoveError);
    connect(&d->soapService, &PTZBindingService::gotoHomePositionDone,
            this, &OnvifPtzService::gotoHomePositionDone);
    connect(&d->soapService, &PTZBindingService::gotoHomePositionError,
            this, &OnvifPtzService::gotoHomePositionError);
    connect(&d->soapService, &PTZBindingService::setHomePositionDone,
            this, &OnvifPtzService::setHomePositionDone);
    connect(&d->soapService, &PTZBindingService::setHomePositionError,
            this, &OnvifPtzService::setHomePositionError);
    connect(&d->soapService, &PTZBindingService::stopDone,
            this, &OnvifPtzService::stopDone);
    connect(&d->soapService, &PTZBindingService::stopError,
            this, &OnvifPtzService::stopError);
}

void OnvifPtzService::connectToService()
{
    if(!d->recievedServiceCapabilities) {
        d->device->updateSoapCredentials(d->soapService.clientInterface());
        d->soapService.asyncGetServiceCapabilities();
    }

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetNodes();

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetConfigurations();
}

void OnvifPtzService::disconnectFromService()
{
    d->recievedServiceCapabilities = false;
}

void OnvifPtzService::setServiceCapabilities(const OnvifSoapPtz::TPTZ__Capabilities &capabilities)
{
    d->recievedServiceCapabilities = true;
    //TODO: Use capabilities
}

void OnvifPtzService::absoluteMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction)
{
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

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncAbsoluteMove(request);
}

void OnvifPtzService::relativeMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction)
{
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

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncRelativeMove(request);
}

bool OnvifPtzService::isContinuousMoveSupported(const OnvifMediaProfile &profile)
{
    for(auto& node : d->nodeList) {
        if(node.token() == profile.ptzNodeToken()) {
            return node.supportedPTZSpaces().continuousPanTiltVelocitySpace().size();
        }
    }
    return false;
}

void OnvifPtzService::continuousMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction)
{
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

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncContinuousMove(request);
}

bool OnvifPtzService::isRelativeZoomSupported(const OnvifMediaProfile &profile)
{
    for(auto& node : d->nodeList) {
        if(node.token() == profile.ptzNodeToken()) {
            return node.supportedPTZSpaces().relativeZoomTranslationSpace().size();
        }
    }
    return false;
}

void OnvifPtzService::relativeZoom(const OnvifMediaProfile &profile, qreal zoomFraction)
{
    Q_ASSERT(-1.0 <= zoomFraction && zoomFraction <= 1.0);

    OnvifSoapPtz::TT__Vector1D vector1D;
    vector1D.setX(zoomFraction);
    vector1D.setSpace("http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace");

    OnvifSoapPtz::TT__PTZVector vector;
    vector.setZoom(vector1D);

    OnvifSoapPtz::TPTZ__RelativeMove request;
    request.setProfileToken(profile.token());
    request.setTranslation(vector);

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncRelativeMove(request);
}

void OnvifPtzService::stopMovement(const OnvifMediaProfile &profile)
{
    OnvifSoapPtz::TPTZ__Stop request;
    request.setProfileToken(profile.token());
    request.setPanTilt(true);
    request.setZoom(true);

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncStop(request);
}

bool OnvifPtzService::isHomeSupported(const OnvifMediaProfile &profile)
{
    for(auto& node : d->nodeList) {
        if(node.token() == profile.ptzNodeToken()) {
            return node.homeSupported();
        }
    }
    return false;
}

bool OnvifPtzService::isRelativeMoveSupported(const OnvifMediaProfile &profile)
{
    for(auto& node : d->nodeList) {
        if(node.token() == profile.ptzNodeToken()) {
            return node.supportedPTZSpaces().relativePanTiltTranslationSpace().size();
        }
    }
    return false;
}

void OnvifPtzService::goToHome(const OnvifMediaProfile &profile)
{
    OnvifSoapPtz::TPTZ__GotoHomePosition request;
    request.setProfileToken(profile.token());
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGotoHomePosition(request);
}

void OnvifPtzService::saveHomePosition(const OnvifMediaProfile &profile)
{
    OnvifSoapPtz::TPTZ__SetHomePosition request;
    request.setProfileToken(profile.token());
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncSetHomePosition(request);
}

void OnvifPtzService::getServiceCapabilitiesDone(const TPTZ__GetServiceCapabilitiesResponse &parameters)
{
    setServiceCapabilities(parameters.capabilities());
}

void OnvifPtzService::getServiceCapabilitiesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::getNodesDone(const OnvifSoapPtz::TPTZ__GetNodesResponse &parameters)
{
    d->nodeList = parameters.pTZNode();
}

void OnvifPtzService::getNodesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::getConfigurationsDone(const TPTZ__GetConfigurationsResponse &parameters)
{
    // TODO: What can we do with the the PTZ configuration
}

void OnvifPtzService::getConfigurationsError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::getStatusDone(const OnvifSoapPtz::TPTZ__GetStatusResponse &parameters)
{
    //TODO: What can we do with the PTZ status?
}

void OnvifPtzService::getStatusError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::absoluteMoveDone(const TPTZ__AbsoluteMoveResponse &)
{
    //NOP
}

void OnvifPtzService::absoluteMoveError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::relativeMoveDone(const TPTZ__RelativeMoveResponse &)
{
    //NOP
}

void OnvifPtzService::relativeMoveError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::continuousMoveDone(const TPTZ__ContinuousMoveResponse &parameters)
{
    //NOP
}

void OnvifPtzService::continuousMoveError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::gotoHomePositionDone(const OnvifSoapPtz::TPTZ__GotoHomePositionResponse &)
{
    //NOP
}

void OnvifPtzService::gotoHomePositionError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::setHomePositionDone(const OnvifSoapPtz::TPTZ__SetHomePositionResponse &)
{
    //NOP
}

void OnvifPtzService::setHomePositionError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::stopDone(const TPTZ__StopResponse &parameters)
{
    qDebug() << Q_FUNC_INFO;
}

void OnvifPtzService::stopError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}
