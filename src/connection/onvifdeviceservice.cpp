#include "onvifdeviceconnection.h"
#include "onvifdeviceservice.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QNetworkReply>

#include "wsdl_devicemgmt.h"

using namespace OnvifSoapDevicemgmt;

class OnvifDeviceService::Private
{
public:
    Private(OnvifDeviceConnection *device) :
        device(device)
    {;}

    OnvifDeviceConnection * device;
    DeviceBindingService soapService;
    OnvifDeviceInformation deviceInformation;
};

OnvifDeviceService::OnvifDeviceService(const QString &soapEndpoint, OnvifDeviceConnection *parent) :
    QObject(parent),
    d(new Private(parent))
{
    d->soapService.setEndPoint(soapEndpoint);

    connect(&d->soapService, &DeviceBindingService::getServiceCapabilitiesDone,
            this, &OnvifDeviceService::getServiceCapabilitiesDone);
    connect(&d->soapService, &DeviceBindingService::getServiceCapabilitiesError,
            this, &OnvifDeviceService::getServiceCapabilitiesError);
    connect(&d->soapService, &DeviceBindingService::getDeviceInformationDone,
            this, &OnvifDeviceService::getDeviceInformationDone);
    connect(&d->soapService, &DeviceBindingService::getDeviceInformationError,
            this, &OnvifDeviceService::getDeviceInformationError);
}

void OnvifDeviceService::connectToService()
{
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetServiceCapabilities();

    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetDeviceInformation();
}

void OnvifDeviceService::disconnectFromService()
{
    d->deviceInformation = OnvifDeviceInformation();
}

OnvifDeviceInformation OnvifDeviceService::getDeviceInformation()
{
    return d->deviceInformation;
}

void OnvifDeviceService::getServiceCapabilitiesDone(const TDS__GetServiceCapabilitiesResponse &parameters)
{
    //TODO: check for the required capabilities
}

void OnvifDeviceService::getServiceCapabilitiesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifDeviceService::getDeviceInformationDone(const TDS__GetDeviceInformationResponse &parameters)
{
    d->deviceInformation = OnvifDeviceInformation(parameters);
    emit deviceInformationAvailable(d->deviceInformation);
}

void OnvifDeviceService::getDeviceInformationError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}
