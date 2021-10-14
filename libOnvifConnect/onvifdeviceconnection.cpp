/* Copyright (C) 2018 Casper Meijn <casper@meijn.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
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

#include <KDSoapClient/KDSoapAuthentication.h>
#include "onvifdeviceservice.h"
#include "onvifmediaservice.h"
#include "onvifmedia2service.h"
#include "onvifptzservice.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QNetworkReply>
#include <QUrl>
#include "wsdl_devicemgmt.h"
#include "wsdl_media.h"
#include "wsdl_media2.h"
#include "wsdl_ptz.h"

using namespace OnvifSoapDevicemgmt;

#define Q_FUNC_INFO_AS_STRING (QString(static_cast<const char*>(Q_FUNC_INFO)))

const QString OnvifDeviceConnectionPrivate::c_baseEndpointURI = QLatin1String("http://%1/onvif/device_service");

OnvifDeviceConnectionPrivate::OnvifDeviceConnectionPrivate(OnvifDeviceConnection* connection) :
    q_ptr(connection)
{;}

OnvifDeviceConnection::OnvifDeviceConnection(QObject* parent) :
    QObject(parent),
    d_ptr(new OnvifDeviceConnectionPrivate(this))
{
    Q_D(OnvifDeviceConnection);
    connect(&d->soapService, &DeviceBindingService::getSystemDateAndTimeDone,
    [d](const OnvifSoapDevicemgmt::TDS__GetSystemDateAndTimeResponse & parameters) {
        d->getSystemDateAndTimeDone(parameters);
    });
    connect(&d->soapService, &DeviceBindingService::getSystemDateAndTimeError,
    [d](const KDSoapMessage & fault) {
        d->getSystemDateAndTimeError(fault);
    });
    connect(&d->soapService, &DeviceBindingService::getServicesDone,
    [d](const OnvifSoapDevicemgmt::TDS__GetServicesResponse & parameters) {
        d->getServicesDone(parameters);
    });
    connect(&d->soapService, &DeviceBindingService::getServicesError,
    [d](const KDSoapMessage & fault) {
        d->getServicesError(fault);
    });
    connect(&d->soapService, &DeviceBindingService::getCapabilitiesDone,
    [d](const OnvifSoapDevicemgmt::TDS__GetCapabilitiesResponse & parameters) {
        d->getCapabilitiesDone(parameters);
    });
    connect(&d->soapService, &DeviceBindingService::getCapabilitiesError,
    [d](const KDSoapMessage & fault) {
        d->getCapabilitiesError(fault);
    });
}

OnvifDeviceConnection::~OnvifDeviceConnection() = default;

void OnvifDeviceConnection::setHostname(const QString& hostname)
{
    Q_D(OnvifDeviceConnection);
    d->hostname = hostname;
    d->soapService.setEndPoint(OnvifDeviceConnectionPrivate::c_baseEndpointURI.arg(hostname));
}

void OnvifDeviceConnection::setCredentials(const QString& username, const QString& password)
{
    Q_D(OnvifDeviceConnection);
    d->username = username;
    d->password = password;
}

QString OnvifDeviceConnection::errorString() const
{
    Q_D(const OnvifDeviceConnection);
    return d->errorString;
}

void OnvifDeviceConnection::connectToDevice()
{
    Q_D(OnvifDeviceConnection);
    // ONVIF Profile S mandates WS-UsernameToken authentication, therfore we can
    //    assume this is supported unless the GetServiceCapabilities tells otherwise
    d->isUsernameTokenSupported = true;
    d->isHttpDigestSupported = false;

    d->isGetCapabilitiesFinished = false;
    d->isGetServicesFinished = false;

    d->getSystemDateAndTime(std::bind(&OnvifDeviceConnectionPrivate::getServicesAndCapabilities, d));

    d->errorString.clear();
    emit errorStringChanged(d->errorString);
}

void OnvifDeviceConnection::disconnectFromDevice()
{
    Q_D(OnvifDeviceConnection);
    d->isGetCapabilitiesFinished = false;
    d->isGetServicesFinished = false;
    d->deviceDateAndTime = QDateTime();
    d->elapsedTime.invalidate();

    if (d->deviceService) {
        d->deviceService->deleteLater();
    }
    d->deviceService = nullptr;

    if (d->mediaService) {
        d->mediaService->deleteLater();
    }
    d->mediaService = nullptr;

    if (d->media2Service) {
        d->media2Service->deleteLater();
    }
    d->media2Service = nullptr;

    if (d->ptzService) {
        d->ptzService->deleteLater();
    }
    d->ptzService = nullptr;
}

void OnvifDeviceConnectionPrivate::getServicesAndCapabilities()
{
    updateSoapCredentials(soapService.clientInterface());
    TDS__GetServices request;
    request.setIncludeCapability(true);
    // Access level pre-auth => no credentials needed
    soapService.asyncGetServices(request);
    // Access level pre-auth => no credentials needed
    soapService.asyncGetCapabilities(TDS__GetCapabilities());
}

void OnvifDeviceConnectionPrivate::getSystemDateAndTimeDone(const OnvifSoapDevicemgmt::TDS__GetSystemDateAndTimeResponse& parameters)
{
    if (parameters.systemDateAndTime().hasValueForUTCDateTime()) {
        const QString dateTime = QString("%1-%2-%3 %4:%5:%6").arg(parameters.systemDateAndTime().uTCDateTime().date().year())
                .arg(parameters.systemDateAndTime().uTCDateTime().date().month(), 2, 10, QLatin1Char('0'))
                .arg(parameters.systemDateAndTime().uTCDateTime().date().day(), 2, 10, QLatin1Char('0'))
                .arg(parameters.systemDateAndTime().uTCDateTime().time().hour(), 2, 10, QLatin1Char('0'))
                .arg(parameters.systemDateAndTime().uTCDateTime().time().minute(), 2, 10, QLatin1Char('0'))
                .arg(parameters.systemDateAndTime().uTCDateTime().time().second(), 2, 10, QLatin1Char('0'));
        deviceDateAndTime = QDateTime::fromString(dateTime, QString("yyyy-MM-dd HH:mm:ss"));
    }
    if (systemDateAndTimeResumeFunction) {
        systemDateAndTimeResumeFunction();
    }
}

void OnvifDeviceConnectionPrivate::getSystemDateAndTimeError(const KDSoapMessage& fault)
{
    qDebug() << "The GetSystemDateAndTime call failed; using client date and time:" << fault.faultAsString();
    if (systemDateAndTimeResumeFunction) {
        systemDateAndTimeResumeFunction();
    }
}

void OnvifDeviceConnectionPrivate::getServicesDone(const TDS__GetServicesResponse& parameters)
{
    Q_Q(OnvifDeviceConnection);
    const auto& serviceList = parameters.service();
    for (auto& service : serviceList) {
        QUrl xAddrUrl(service.xAddr());
        updateUrlHost(&xAddrUrl);
        if (service.namespace_() == "http://www.onvif.org/ver10/device/wsdl") {
            if (QUrl(service.xAddr()) != QUrl(OnvifDeviceConnectionPrivate::c_baseEndpointURI.arg(hostname))) {
                qWarning() << "Warning: The recieved address of the device service doesn't match the address of the initial connection.";
                qWarning() << "Recieved address:" << QUrl(service.xAddr()).toString();
                qWarning() << "Initial connection:" << QUrl(OnvifDeviceConnectionPrivate::c_baseEndpointURI.arg(hostname)).toString();
            }
            OnvifSoapDevicemgmt::TDS__DeviceServiceCapabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            isUsernameTokenSupported = capabilities.security().usernameToken();
            isHttpDigestSupported = capabilities.security().httpDigest();
            if (!deviceService) {
                deviceService = new OnvifDeviceService(xAddrUrl.toString(), q);
            }
        } else if (service.namespace_() == "http://www.onvif.org/ver10/media/wsdl") {
            if (!mediaService) {
                mediaService = new OnvifMediaService(xAddrUrl.toString(), q);
            }
            OnvifSoapMedia::TRT__Capabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            mediaService->setServiceCapabilities(capabilities);
        } else if (service.namespace_() == "http://www.onvif.org/ver20/media/wsdl") {
            if (!media2Service) {
                OnvifSoapMedia2::TR2__Capabilities2 capabilities;
                capabilities.deserialize(service.capabilities().any());
                media2Service = new OnvifMedia2Service(xAddrUrl.toString(), capabilities, q);
            }
        } else if (service.namespace_() == "http://www.onvif.org/ver20/ptz/wsdl") {
            if (!ptzService) {
                ptzService = new OnvifPtzService(xAddrUrl.toString(), q);
            }
            OnvifSoapPtz::TPTZ__Capabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            ptzService->setServiceCapabilities(capabilities);
        }
    }

    isGetServicesFinished = true;
    checkServicesAvailable();
}

void OnvifDeviceConnectionPrivate::getServicesError(const KDSoapMessage& fault)
{
    // Some older devices don't support the GetServices call, only the GetCapabilities call
    // Therefore we mark the service finished and ignore any error
    isGetServicesFinished = true;
    qDebug() << "The GetServices call failed; this is expected for older ONVIF devices:" << fault.faultAsString();
    checkServicesAvailable();
}

void OnvifDeviceConnectionPrivate::getCapabilitiesDone(const TDS__GetCapabilitiesResponse& parameters)
{
    Q_Q(OnvifDeviceConnection);
    if (parameters.capabilities().analytics().xAddr().size()) {
        // Not yet supported
    }
    if (parameters.capabilities().device().xAddr().size()) {
        if (!deviceService) {
            QUrl xAddrUrl(parameters.capabilities().device().xAddr());
            updateUrlHost(&xAddrUrl);
            deviceService = new OnvifDeviceService(xAddrUrl.toString(), q);
        }
        // isUsernameTokenSupported and isHttpDigestSupported are not reported by the GetCapabilities call
    }
    if (parameters.capabilities().events().xAddr().size()) {
        // Not yet supported
    }
    if (parameters.capabilities().imaging().xAddr().size()) {
        // Not yet supported
    }
    if (parameters.capabilities().media().xAddr().size()) {
        if (!mediaService) {
            QUrl xAddrUrl(parameters.capabilities().media().xAddr());
            updateUrlHost(&xAddrUrl);
            mediaService = new OnvifMediaService(xAddrUrl.toString(), q);
        }
        mediaService->setServiceCapabilities(parameters.capabilities().media());
    }
    if (parameters.capabilities().pTZ().xAddr().size()) {
        if (!ptzService) {
            QUrl xAddrUrl(parameters.capabilities().pTZ().xAddr());
            updateUrlHost(&xAddrUrl);
            ptzService = new OnvifPtzService(xAddrUrl.toString(), q);
        }
        ptzService->setServiceCapabilities(parameters.capabilities().pTZ());
    }

    isGetCapabilitiesFinished = true;
    checkServicesAvailable();
}

void OnvifDeviceConnectionPrivate::getCapabilitiesError(const KDSoapMessage& fault)
{
    handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifDeviceConnectionPrivate::checkServicesAvailable()
{
    Q_Q(OnvifDeviceConnection);
    if (isGetServicesFinished && isGetCapabilitiesFinished) {
        if (deviceService) {
            deviceService->connectToService();
        }
        if (mediaService) {
            mediaService->connectToService();
        }
        if (media2Service) {
            media2Service->connectToService();
        }
        if (ptzService) {
            ptzService->connectToService();
        }
        emit q->servicesAvailable();
    }
}

OnvifDeviceService* OnvifDeviceConnection::getDeviceService() const
{
    Q_D(const OnvifDeviceConnection);
    return d->deviceService;
}

OnvifMediaService* OnvifDeviceConnection::getMediaService() const
{
    Q_D(const OnvifDeviceConnection);
    return d->mediaService;
}

OnvifMedia2Service* OnvifDeviceConnection::getMedia2Service() const
{
    Q_D(const OnvifDeviceConnection);
    return d->media2Service;
}

OnvifPtzService* OnvifDeviceConnection::getPtzService() const
{
    Q_D(const OnvifDeviceConnection);
    return d->ptzService;
}

void OnvifDeviceConnectionPrivate::updateUrlHost(QUrl* url)
{
    if (url->scheme() == "http") {
        QUrl origUrl(OnvifDeviceConnectionPrivate::c_baseEndpointURI.arg(hostname));
        if (url->host() != origUrl.host()) {
            url->setHost(origUrl.host());
            url->setPort(origUrl.port());
        }
    }
}

void OnvifDeviceConnectionPrivate::updateSoapCredentials(KDSoapClientInterface* clientInterface)
{
    if (isHttpDigestSupported || isUsernameTokenSupported) {
        KDSoapAuthentication auth;
        auth.setUser(username);
        auth.setPassword(password);
        auth.setUseWSUsernameToken(isUsernameTokenSupported);
        if (deviceDateAndTime.isValid()) {
            if(elapsedTime.isValid()){
                deviceDateAndTime = deviceDateAndTime.addMSecs(elapsedTime.elapsed());
            } else {
                elapsedTime.start();
            }
            auth.setOverrideWSUsernameCreatedTime(deviceDateAndTime);
        }
        clientInterface->setAuthentication(auth);
    }
    // Some camera's don't require authentication and therefore don't ask for any
}

void OnvifDeviceConnectionPrivate::clearSoapCredentials()
{
    deviceDateAndTime = QDateTime();
    elapsedTime.invalidate();
    soapService.clientInterface()->setAuthentication(KDSoapAuthentication());
}

void OnvifDeviceConnectionPrivate::getSystemDateAndTime(std::function<void()> resumeFunction)
{
    systemDateAndTimeResumeFunction = resumeFunction;
    clearSoapCredentials();
    soapService.asyncGetSystemDateAndTime(TDS__GetSystemDateAndTime());
}

void OnvifDeviceConnectionPrivate::updateUrlCredentials(QUrl* url)
{
    Q_ASSERT(url);
    url->setUserName(username);
    url->setPassword(password);
}

void OnvifDeviceConnectionPrivate::handleSoapError(const KDSoapMessage& fault, const QString& location)
{
    Q_Q(OnvifDeviceConnection);
    errorString = "";
    if (fault.childValues().child(QLatin1String("faultcode")).value().toInt() == QNetworkReply::OperationCanceledError) {
        errorString = "A possible authentication error. Please install a more recent version of KDSoap for more detailed error message.";
    } else if (fault.childValues().child(QLatin1String("faultcode")).value().toInt() == QNetworkReply::AuthenticationRequiredError) {
        errorString = "Authentication error occured. Credentials are probably incorrect.";
        if (!isHttpDigestSupported && !isUsernameTokenSupported) {
            errorString = "None of the authentication methods are available";
        }
    } else if (location.contains("OnvifPtzServicePrivate::getServiceCapabilitiesError") &&
               (fault.faultAsString().contains("not implemented") || fault.faultAsString().contains("Action failed"))) {
        // Some devices report having PTZ service but fail with "not implemented" or "Action failed" when getServiceCapabilities is called
        // Therefore we disable the service and ignore any error
        qDebug() << "The PTZ GetServicesCapabilities call failed; this is expected for some ONVIF devices:" << fault.faultAsString();
        if (ptzService) {
            ptzService->disconnectFromService();
            ptzService->deleteLater();
        }
        ptzService = nullptr;
        return;
    } else {
        errorString = location + ": " + fault.faultAsString();
    }
    qCritical() << errorString;
    q->disconnectFromDevice();
    emit q->errorStringChanged(errorString);
}
