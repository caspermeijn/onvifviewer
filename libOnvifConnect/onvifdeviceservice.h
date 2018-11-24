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
#ifndef ONVIFDEVICESERVICE_H
#define ONVIFDEVICESERVICE_H

#include "onvifconnect_export.h"
#include "onvifdeviceinformation.h"
#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapDevicemgmt {
class TDS__DeviceServiceCapabilities;
class TDS__GetServiceCapabilitiesResponse;
class TDS__GetDeviceInformationResponse;
}
class KDSoapMessage;

class ONVIFCONNECT_EXPORT OnvifDeviceService : public QObject
{
    Q_OBJECT
public:
    explicit OnvifDeviceService(const QString& soapEndpoint, OnvifDeviceConnection *parent);

    void connectToService();
    void disconnectFromService();

    OnvifDeviceInformation getDeviceInformation();

signals:
    void deviceInformationAvailable(const OnvifDeviceInformation& deviceInformation);

private slots:
    void getDeviceInformationDone( const OnvifSoapDevicemgmt::TDS__GetDeviceInformationResponse& parameters );
    void getDeviceInformationError( const KDSoapMessage& fault );

private:
    class Private;
    Private *const d;
};

#endif // ONVIFDEVICESERVICE_H
