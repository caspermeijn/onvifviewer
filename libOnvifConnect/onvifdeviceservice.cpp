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
#include "onvifdeviceconnection_p.h"
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
    Q_DECLARE_PUBLIC(OnvifDeviceService)
public:
    OnvifDeviceServicePrivate(OnvifDeviceService* service, OnvifDeviceConnection* device) :
        q_ptr(service),
        device(device)
    {;}

    OnvifDeviceService* const q_ptr;
    OnvifDeviceConnection* device;
    DeviceBindingService soapService;
    OnvifDeviceInformation deviceInformation;

    void getDeviceInformationDone(const OnvifSoapDevicemgmt::TDS__GetDeviceInformationResponse& parameters);
    void getDeviceInformationError(const KDSoapMessage& fault);
};

OnvifDeviceService::OnvifDeviceService(const QString& soapEndpoint, OnvifDeviceConnection* parent) :
    QObject(parent),
    d_ptr(new OnvifDeviceServicePrivate(this, parent))
{
    Q_D(OnvifDeviceService);
    d->soapService.setEndPoint(soapEndpoint);

    connect(&d->soapService, &DeviceBindingService::getDeviceInformationDone,
    [d](const OnvifSoapDevicemgmt::TDS__GetDeviceInformationResponse & parameters) {
        d->getDeviceInformationDone(parameters);
    });
    connect(&d->soapService, &DeviceBindingService::getDeviceInformationError,
    [d](const KDSoapMessage & fault) {
        d->getDeviceInformationError(fault);
    });
}

OnvifDeviceService::~OnvifDeviceService() = default;

void OnvifDeviceService::connectToService()
{
    Q_D(OnvifDeviceService);
    d->device->d_ptr->updateSoapCredentials(d->soapService.clientInterface());
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

void OnvifDeviceServicePrivate::getDeviceInformationDone(const TDS__GetDeviceInformationResponse& parameters)
{
    Q_Q(OnvifDeviceService);
    deviceInformation = OnvifDeviceInformation(parameters);
    emit q->deviceInformationAvailable(deviceInformation);
}

void OnvifDeviceServicePrivate::getDeviceInformationError(const KDSoapMessage& fault)
{
    device->d_ptr->handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}
