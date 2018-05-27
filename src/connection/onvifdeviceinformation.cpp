#include "onvifdeviceinformation.h"

#include <QDebug>
#include "wsdl_devicemgmt.h"

using namespace OnvifSoapDevicemgmt;

class OnvifDeviceInformation::Private : public QSharedData
{
public:
    Private()
    {;}

    Private(const TDS__GetDeviceInformationResponse &deviceInformationResponse) :
        manufacturer(deviceInformationResponse.manufacturer()),
        model(deviceInformationResponse.model()),
        firmwareVersion(deviceInformationResponse.firmwareVersion()),
        serialNumber(deviceInformationResponse.serialNumber()),
        hardwareId(deviceInformationResponse.hardwareId())
    {
    }

    QString manufacturer;
    QString model;
    QString firmwareVersion;
    QString serialNumber;
    QString hardwareId;
};

OnvifDeviceInformation::OnvifDeviceInformation(QObject *parent) :
    QObject(parent),
    d(new Private())
{
}

OnvifDeviceInformation::OnvifDeviceInformation(const OnvifDeviceInformation &other, QObject *parent) :
    QObject(parent),
    d(other.d)
{
}

OnvifDeviceInformation::OnvifDeviceInformation(const TDS__GetDeviceInformationResponse &profile, QObject *parent) :
    QObject(parent),
    d(new Private(profile))
{
}

OnvifDeviceInformation::~OnvifDeviceInformation()
{
}

OnvifDeviceInformation &OnvifDeviceInformation::operator=(const OnvifDeviceInformation &other)
{
    if (this != &other) {
        OnvifDeviceInformation copy(other);
        d.swap(copy.d);
    }
    return *this;
}

QString OnvifDeviceInformation::manufacturer() const
{
    return d->manufacturer;
}

QString OnvifDeviceInformation::model() const
{
    return d->model;
}

QString OnvifDeviceInformation::firmwareVersion() const
{
    return d->firmwareVersion;
}

QString OnvifDeviceInformation::serialNumber() const
{
    return d->serialNumber;
}

QString OnvifDeviceInformation::hardwareId() const
{
    return d->hardwareId;
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
