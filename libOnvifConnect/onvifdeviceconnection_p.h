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

#ifndef ONVIFDEVICECONNECTION_P_H
#define ONVIFDEVICECONNECTION_P_H

#include "onvifdeviceconnection.h"
#include "wsdl_devicemgmt.h"

class OnvifDeviceConnectionPrivate
{
    Q_DISABLE_COPY(OnvifDeviceConnectionPrivate)
    Q_DECLARE_PUBLIC(OnvifDeviceConnection)
private:
    OnvifDeviceConnectionPrivate(OnvifDeviceConnection* connection);

    OnvifDeviceConnection* const q_ptr;

    OnvifSoapDevicemgmt::DeviceBindingService soapService;
    OnvifDeviceService* deviceService = nullptr;
    OnvifMediaService* mediaService = nullptr;
    OnvifMedia2Service* media2Service = nullptr;
    OnvifPtzService* ptzService = nullptr;

    QString hostname;
    QString username;
    QString password;

    QString errorString;

    bool isUsernameTokenSupported = false;
    bool isHttpDigestSupported = false;

    bool isGetServicesFinished = false;
    bool isGetCapabilitiesFinished = false;

    static const QString c_baseEndpointURI;

    void getServicesDone(const OnvifSoapDevicemgmt::TDS__GetServicesResponse& parameters);
    void getServicesError(const KDSoapMessage& fault);
    void getCapabilitiesDone(const OnvifSoapDevicemgmt::TDS__GetCapabilitiesResponse& parameters);
    void getCapabilitiesError(const KDSoapMessage& fault);

    void checkServicesAvailable();

public:
    void updateUrlHost(QUrl* url);
    void updateSoapCredentials(KDSoapClientInterface* clientInterface);
    void updateUrlCredentials(QUrl* url);
    void handleSoapError(const KDSoapMessage& fault, const QString& location);
};

#endif // ONVIFDEVICECONNECTION_P_H
