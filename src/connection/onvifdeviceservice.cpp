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
        qDebug() << "Device service capabilities:";
        qDebug() << "\tUsernameToken:" << parameters.capabilities().security().usernameToken();
        qDebug() << "\tHttpDigest:" << parameters.capabilities().security().httpDigest();
        qDebug() << "\tAuxiliaryCommands:" << parameters.capabilities().misc().auxiliaryCommands().entries();
}

void OnvifDeviceService::getServiceCapabilitiesError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifDeviceService::getDeviceInformationDone(const TDS__GetDeviceInformationResponse &parameters)
{
    d->deviceInformation = OnvifDeviceInformation(parameters);
    qDebug() << "DeviceInformation" << d->deviceInformation;
    emit deviceInformationAvailable(d->deviceInformation);
}

void OnvifDeviceService::getDeviceInformationError(const KDSoapMessage &fault)
{
    d->device->handleSoapError(fault, Q_FUNC_INFO);
}

OnvifDeviceInformation::OnvifDeviceInformation()
{
}

OnvifDeviceInformation::OnvifDeviceInformation(const TDS__GetDeviceInformationResponse &deviceInformationResponse) :
    m_manufacturer(deviceInformationResponse.manufacturer()),
    m_model(deviceInformationResponse.model()),
    m_firmwareVersion(deviceInformationResponse.firmwareVersion()),
    m_serialNumber(deviceInformationResponse.serialNumber()),
    m_hardwareId(deviceInformationResponse.hardwareId())
{
}

QString OnvifDeviceInformation::manufacturer() const
{
    return m_manufacturer;
}

QString OnvifDeviceInformation::model() const
{
    return m_model;
}

QString OnvifDeviceInformation::firmwareVersion() const
{
    return m_firmwareVersion;
}

QString OnvifDeviceInformation::serialNumber() const
{
    return m_serialNumber;
}

QString OnvifDeviceInformation::hardwareId() const
{
    return m_hardwareId;
}

QDebug operator<<(QDebug debug, const OnvifDeviceInformation &d)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "(Manufacturer: " << d.manufacturer()
                    << ", Model: " << d.model()
                    << ", FirmwareVersion: " << d.firmwareVersion()
                    << ", SerialNumber: " << d.serialNumber()
                    << ", HardwareId: " << d.hardwareId() << ')';
    return debug;
}
