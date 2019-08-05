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
#include "onvifdeviceinformation.h"

#include <QDebug>
#include "wsdl_devicemgmt.h"

using namespace OnvifSoapDevicemgmt;

class OnvifDeviceInformationPrivate : public QSharedData
{
public:
    OnvifDeviceInformationPrivate()
    {;}

    OnvifDeviceInformationPrivate(const TDS__GetDeviceInformationResponse& deviceInformationResponse) :
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

OnvifDeviceInformation::OnvifDeviceInformation(QObject* parent) :
    QObject(parent),
    d(new OnvifDeviceInformationPrivate())
{
}

OnvifDeviceInformation::OnvifDeviceInformation(const OnvifDeviceInformation& other, QObject* parent) :
    QObject(parent),
    d(other.d)
{
}

OnvifDeviceInformation::OnvifDeviceInformation(const TDS__GetDeviceInformationResponse& profile, QObject* parent) :
    QObject(parent),
    d(new OnvifDeviceInformationPrivate(profile))
{
}

OnvifDeviceInformation::~OnvifDeviceInformation() = default;

OnvifDeviceInformation& OnvifDeviceInformation::operator= (const OnvifDeviceInformation& other)
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

QDebug operator<< (QDebug debug, const OnvifDeviceInformation& d)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "(Manufacturer: " << d.manufacturer()
                    << ", Model: " << d.model()
                    << ", FirmwareVersion: " << d.firmwareVersion()
                    << ", SerialNumber: " << d.serialNumber()
                    << ", HardwareId: " << d.hardwareId() << ')';
    return debug;
}
