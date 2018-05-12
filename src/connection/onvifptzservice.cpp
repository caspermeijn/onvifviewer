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
        device(device)
    {;}

    OnvifDeviceConnection * device;
    OnvifSoapPtz::PTZBindingService soapService;
    QList< OnvifSoapPtz::TT__PTZNode > nodeList;
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
    connect(&d->soapService, &PTZBindingService::gotoHomePositionDone,
            this, &OnvifPtzService::gotoHomePositionDone);
    connect(&d->soapService, &PTZBindingService::gotoHomePositionError,
            this, &OnvifPtzService::gotoHomePositionError);
}

void OnvifPtzService::connectToService()
{
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetServiceCapabilities();

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetNodes();

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetConfigurations();
}

void OnvifPtzService::disconnectFromService()
{

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

void OnvifPtzService::getServiceCapabilitiesDone(const TPTZ__GetServiceCapabilitiesResponse &parameters)
{
    qDebug() << "PTZ capabilities"
             << "eFlip:" << parameters.capabilities().eFlip()
             << "reverse:" << parameters.capabilities().reverse()
             << "moveStatus:" << parameters.capabilities().moveStatus()
             << "statusPosition:" << parameters.capabilities().statusPosition();
}

void OnvifPtzService::getServiceCapabilitiesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::getNodesDone(const OnvifSoapPtz::TPTZ__GetNodesResponse &parameters)
{
    d->nodeList = parameters.pTZNode();
    for(auto node : parameters.pTZNode())
    {
        qDebug() << "PTZ node"
                 << "name:" << node.name()
                 << "token:" << node.token()
                 << "homeSupported" << node.homeSupported()
                 << "fixedHomePosition" << node.fixedHomePosition()
                 << "geoMove" << node.geoMove()
                 << "maximumNumberOfPresets" << node.maximumNumberOfPresets()
                 << "auxiliaryCommands" << node.auxiliaryCommands() ;
    }
}

void OnvifPtzService::getNodesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::getConfigurationsDone(const TPTZ__GetConfigurationsResponse &parameters)
{
    for(auto config : parameters.pTZConfiguration())
    {
        qDebug() << "PTZ configuration"
                 << "name:" << config.name()
                 << "token:" << config.token()
                 << "defaultRelativePanTiltTranslationSpace" << config.defaultRelativePanTiltTranslationSpace()
                 << "defaultPTZSpeed" << config.defaultPTZSpeed().panTilt().x() << config.defaultPTZSpeed().panTilt().y() << config.defaultPTZSpeed().panTilt().space()
                 << "defaultPTZTimeout" << config.defaultPTZTimeout()
                 << "moveRamp" << config.moveRamp()
                 << "panTiltLimits" << config.panTiltLimits().range().xRange().min() << config.panTiltLimits().range().xRange().max() << config.panTiltLimits().range().yRange().min()  << config.panTiltLimits().range().yRange().max() << config.panTiltLimits().range().uRI() ;
    }
}

void OnvifPtzService::getConfigurationsError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::getStatusDone(const OnvifSoapPtz::TPTZ__GetStatusResponse &parameters)
{
    qDebug() << "PTZ status"
             << "position pantilt:" << parameters.pTZStatus().position().panTilt().x() << parameters.pTZStatus().position().panTilt().y() << parameters.pTZStatus().position().panTilt().space()
             << "moveStatus pantilt:" << parameters.pTZStatus().moveStatus().panTilt().type()
             << "error:" << parameters.pTZStatus().error()
             << "time:" << parameters.pTZStatus().utcTime();
}

void OnvifPtzService::getStatusError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::absoluteMoveDone(const TPTZ__AbsoluteMoveResponse &parameters)
{
    qDebug() << Q_FUNC_INFO;
}

void OnvifPtzService::absoluteMoveError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::relativeMoveDone(const TPTZ__RelativeMoveResponse &parameters)
{
    qDebug() << Q_FUNC_INFO;
}

void OnvifPtzService::relativeMoveError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifPtzService::gotoHomePositionDone(const OnvifSoapPtz::TPTZ__GotoHomePositionResponse &parameters)
{
    qDebug() << Q_FUNC_INFO;
}

void OnvifPtzService::gotoHomePositionError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}
