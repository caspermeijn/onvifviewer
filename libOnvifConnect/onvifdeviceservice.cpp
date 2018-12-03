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
#include "onvifdeviceconnection.h"
#include "onvifdeviceservice.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QNetworkReply>

#include "wsdl_devicemgmt.h"

using namespace OnvifSoapDevicemgmt;

#define Q_FUNC_INFO_AS_STRING (QString(static_cast<const char*>(Q_FUNC_INFO)))

class OnvifDeviceServicePrivate
{
    Q_DISABLE_COPY(OnvifDeviceServicePrivate)
public:
    OnvifDeviceServicePrivate(OnvifDeviceConnection *device) :
        device(device)
    {;}

    OnvifDeviceConnection * device;
    DeviceBindingService soapService;
    OnvifDeviceInformation deviceInformation;
};

OnvifDeviceService::OnvifDeviceService(const QString &soapEndpoint, OnvifDeviceConnection *parent) :
    QObject(parent),
    d_ptr(new OnvifDeviceServicePrivate(parent))
{
    Q_D(OnvifDeviceService);
    d->soapService.setEndPoint(soapEndpoint);

    connect(&d->soapService, &DeviceBindingService::getDeviceInformationDone,
            this, &OnvifDeviceService::getDeviceInformationDone);
    connect(&d->soapService, &DeviceBindingService::getDeviceInformationError,
            this, &OnvifDeviceService::getDeviceInformationError);
}

OnvifDeviceService::~OnvifDeviceService() = default;

void OnvifDeviceService::connectToService()
{
    Q_D(OnvifDeviceService);
    d->device->updateSoapCredentials(d->soapService.clientInterface());
    d->soapService.asyncGetDeviceInformation();
}

void OnvifDeviceService::disconnectFromService()
{
    Q_D(OnvifDeviceService);
    d->deviceInformation = OnvifDeviceInformation();
}

OnvifDeviceInformation OnvifDeviceService::getDeviceInformation()
{
    Q_D(OnvifDeviceService);
    return d->deviceInformation;
}

void OnvifDeviceService::getDeviceInformationDone(const TDS__GetDeviceInformationResponse &parameters)
{
    Q_D(OnvifDeviceService);
    d->deviceInformation = OnvifDeviceInformation(parameters);
    emit deviceInformationAvailable(d->deviceInformation);
}

void OnvifDeviceService::getDeviceInformationError(const KDSoapMessage &fault)
{
    Q_D(OnvifDeviceService);
    d->device->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}
