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

OnvifDeviceConnectionPrivate::OnvifDeviceConnectionPrivate(OnvifDeviceConnection *connection) :
    q_ptr(connection)
{;}

OnvifDeviceConnection::OnvifDeviceConnection(QObject *parent) :
    QObject(parent),
    d_ptr(new OnvifDeviceConnectionPrivate(this))
{
    Q_D(OnvifDeviceConnection);
    connect(&d->soapService, &DeviceBindingService::getServicesDone,
            [d](const OnvifSoapDevicemgmt::TDS__GetServicesResponse& parameters){d->getServicesDone(parameters);});
    connect(&d->soapService, &DeviceBindingService::getServicesError,
            [d](const KDSoapMessage& fault){d->getServicesError(fault);});
    connect(&d->soapService, &DeviceBindingService::getCapabilitiesDone,
            [d](const OnvifSoapDevicemgmt::TDS__GetCapabilitiesResponse& parameters){d->getCapabilitiesDone(parameters);});
    connect(&d->soapService, &DeviceBindingService::getCapabilitiesError,
            [d](const KDSoapMessage& fault){d->getCapabilitiesError(fault);});
}

OnvifDeviceConnection::~OnvifDeviceConnection() = default;

void OnvifDeviceConnection::setHostname(const QString& hostname)
{
    Q_D(OnvifDeviceConnection);
    d->hostname = hostname;
    d->soapService.setEndPoint(OnvifDeviceConnectionPrivate::c_baseEndpointURI.arg(hostname));
}

void OnvifDeviceConnection::setCredentials(const QString &username, const QString &password)
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

    TDS__GetServices request;
    request.setIncludeCapability(true);
    // Access level pre-auth => no credentials needes
    d->soapService.asyncGetServices(request);
    // Access level pre-auth => no credentials needes
    d->soapService.asyncGetCapabilities(TDS__GetCapabilities());

    d->errorString.clear();
    emit errorStringChanged(d->errorString);
}

void OnvifDeviceConnection::disconnectFromDevice()
{
    Q_D(OnvifDeviceConnection);
    d->isGetCapabilitiesFinished = false;
    d->isGetServicesFinished = false;

    if(d->deviceService)
        d->deviceService->deleteLater();
    d->deviceService = nullptr;

    if(d->mediaService)
        d->mediaService->deleteLater();
    d->mediaService = nullptr;

    if(d->media2Service)
        d->media2Service->deleteLater();
    d->media2Service = nullptr;

    if(d->ptzService)
        d->ptzService->deleteLater();
    d->ptzService = nullptr;
}

void OnvifDeviceConnectionPrivate::getServicesDone(const TDS__GetServicesResponse &parameters)
{
    Q_Q(OnvifDeviceConnection);
    const auto& serviceList = parameters.service();
    for(auto& service : serviceList)
    {
        QUrl xAddrUrl(service.xAddr());
        updateUrlHost(&xAddrUrl);
        if(service.namespace_() == "http://www.onvif.org/ver10/device/wsdl")
        {
            if(QUrl(service.xAddr()) != QUrl(OnvifDeviceConnectionPrivate::c_baseEndpointURI.arg(hostname))) {
                qWarning() << "Warning: The recieved address of the device service doesn't match the address of the initial connection.";
                qWarning() << "Recieved address:" << QUrl(service.xAddr()).toString();
                qWarning() << "Initial connection:" << QUrl(OnvifDeviceConnectionPrivate::c_baseEndpointURI.arg(hostname)).toString();
            }
            OnvifSoapDevicemgmt::TDS__DeviceServiceCapabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            isUsernameTokenSupported = capabilities.security().usernameToken();
            isHttpDigestSupported = capabilities.security().httpDigest();
            if(!deviceService)
            {
                deviceService = new OnvifDeviceService(xAddrUrl.toString(), q);
            }
        }
        else if(service.namespace_() == "http://www.onvif.org/ver10/media/wsdl")
        {
            if(!mediaService)
            {
                mediaService = new OnvifMediaService(xAddrUrl.toString(), q);
            }
            OnvifSoapMedia::TRT__Capabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            mediaService->setServiceCapabilities(capabilities);
        }
        else if(service.namespace_() == "http://www.onvif.org/ver20/media/wsdl")
        {
            if(!media2Service)
            {
                OnvifSoapMedia2::TR2__Capabilities2 capabilities;
                capabilities.deserialize(service.capabilities().any());
                media2Service = new OnvifMedia2Service(xAddrUrl.toString(), capabilities, q);
            }
        }
        else if(service.namespace_() == "http://www.onvif.org/ver20/ptz/wsdl")
        {
            if(!ptzService)
            {
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

void OnvifDeviceConnectionPrivate::getServicesError(const KDSoapMessage &fault)
{
    // Some older devices don't support the GetServices call, only the GetCapabilities call
    // Therefore we mark the service finished and ignore any error
    isGetServicesFinished = true;
    qDebug() << "The GetServices call failed; this is expected for older ONVIF devices:" << fault.faultAsString();
    checkServicesAvailable();
}

void OnvifDeviceConnectionPrivate::getCapabilitiesDone(const TDS__GetCapabilitiesResponse &parameters)
{
    Q_Q(OnvifDeviceConnection);
    if(parameters.capabilities().analytics().xAddr().size())
    {
        // Not yet supported
    }
    if(parameters.capabilities().device().xAddr().size())
    {
        if(!deviceService)
        {
            QUrl xAddrUrl(parameters.capabilities().device().xAddr());
            updateUrlHost(&xAddrUrl);
            deviceService = new OnvifDeviceService(xAddrUrl.toString(), q);
        }
        // isUsernameTokenSupported and isHttpDigestSupported are not reported by the GetCapabilities call
    }
    if(parameters.capabilities().events().xAddr().size())
    {
        // Not yet supported
    }
    if(parameters.capabilities().imaging().xAddr().size())
    {
        // Not yet supported
    }
    if(parameters.capabilities().media().xAddr().size())
    {
        if(!mediaService)
        {
            QUrl xAddrUrl(parameters.capabilities().media().xAddr());
            updateUrlHost(&xAddrUrl);
            mediaService = new OnvifMediaService(xAddrUrl.toString(), q);
        }
        mediaService->setServiceCapabilities(parameters.capabilities().media());
    }
    if(parameters.capabilities().pTZ().xAddr().size())
    {
        if(!ptzService)
        {
            QUrl xAddrUrl(parameters.capabilities().pTZ().xAddr());
            updateUrlHost(&xAddrUrl);
            ptzService = new OnvifPtzService(xAddrUrl.toString(), q);
        }
        ptzService->setServiceCapabilities(parameters.capabilities().pTZ());
    }

    isGetCapabilitiesFinished = true;
    checkServicesAvailable();
}

void OnvifDeviceConnectionPrivate::getCapabilitiesError(const KDSoapMessage &fault)
{
    handleSoapError(fault, Q_FUNC_INFO_AS_STRING);
}

void OnvifDeviceConnectionPrivate::checkServicesAvailable()
{
    Q_Q(OnvifDeviceConnection);
    if(isGetServicesFinished && isGetCapabilitiesFinished)
    {
        if (deviceService)
            deviceService->connectToService();
        if (mediaService)
            mediaService->connectToService();
        if (media2Service)
            media2Service->connectToService();
        if (ptzService)
            ptzService->connectToService();
        emit q->servicesAvailable();
    }
}

OnvifDeviceService *OnvifDeviceConnection::getDeviceService() const
{
    Q_D(const OnvifDeviceConnection);
    return d->deviceService;
}

OnvifMediaService *OnvifDeviceConnection::getMediaService() const
{
    Q_D(const OnvifDeviceConnection);
    return d->mediaService;
}

OnvifMedia2Service *OnvifDeviceConnection::getMedia2Service() const
{
    Q_D(const OnvifDeviceConnection);
    return d->media2Service;
}

OnvifPtzService *OnvifDeviceConnection::getPtzService() const
{
    Q_D(const OnvifDeviceConnection);
    return d->ptzService;
}

void OnvifDeviceConnectionPrivate::updateUrlHost(QUrl *url)
{
    if(url->scheme() == "http") {
        QUrl origUrl(OnvifDeviceConnectionPrivate::c_baseEndpointURI.arg(hostname));
        if(url->host() != origUrl.host()) {
            url->setHost(origUrl.host());
            url->setPort(origUrl.port());
        }
    }
}

void OnvifDeviceConnectionPrivate::updateSoapCredentials(KDSoapClientInterface *clientInterface)
{
    if(isHttpDigestSupported)
        updateKDSoapAuthentication(clientInterface);
    else if(isUsernameTokenSupported)
        updateUsernameToken(clientInterface);
    // Some camera's don't require authentication and therefore don't ask for any
}

void OnvifDeviceConnectionPrivate::updateUsernameToken(KDSoapClientInterface *clientInterface)
{
    QByteArray nonce = "abc" + QByteArray::number(qrand());
    KDSoapValue nonceValue("Nonce", nonce);
    nonceValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");
    nonceValue.childValues().attributes().append(KDSoapValue("EncodingType", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary"));

    QString timestamp = QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ssZ");
    KDSoapValue createdValue("Created", timestamp);
    createdValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd");

    QByteArray passwordConcat = nonce + timestamp.toUtf8() + password.toUtf8();
    QByteArray passwordHash = QCryptographicHash::hash(passwordConcat, QCryptographicHash::Sha1);
    KDSoapValue passwordValue ("Password", passwordHash);
    passwordValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");
    passwordValue.childValues().attributes().append(KDSoapValue("Type", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest"));

    KDSoapValue usernameValue("Username", username);
    usernameValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");

    KDSoapValue usernameTokenValue("UsernameToken", QVariant());
    usernameTokenValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");
    usernameTokenValue.childValues().append(usernameValue);
    usernameTokenValue.childValues().append(passwordValue);
    usernameTokenValue.childValues().append(nonceValue);
    usernameTokenValue.childValues().append(createdValue);

    KDSoapValue securityValue("Security", QVariant());
    securityValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");
    securityValue.childValues().append(usernameTokenValue);

    KDSoapMessage wsseHeader;
    wsseHeader.setUse(KDSoapMessage::LiteralUse);
    wsseHeader.childValues().append(securityValue);

    clientInterface->setHeader("wsse", wsseHeader);
}

void OnvifDeviceConnectionPrivate::updateKDSoapAuthentication(KDSoapClientInterface *clientInterface)
{
    KDSoapAuthentication auth;
    auth.setUser(username);
    auth.setPassword(password);
    clientInterface->setAuthentication(auth);
}

void OnvifDeviceConnectionPrivate::updateUrlCredentials(QUrl *url)
{
    Q_ASSERT(url);
    url->setUserName(username);
    url->setPassword(password);
}

void OnvifDeviceConnectionPrivate::handleSoapError(const KDSoapMessage &fault, const QString &location)
{
    Q_Q(OnvifDeviceConnection);
    errorString = "";
    if(fault.childValues().child(QLatin1String("faultcode")).value().toInt() == QNetworkReply::OperationCanceledError) {
        errorString = "A possible authentication error. Please install a more recent version of KDSoap for more detailed error message.";
    }
    else if(fault.childValues().child(QLatin1String("faultcode")).value().toInt() == QNetworkReply::AuthenticationRequiredError) {
        errorString = "Authentication error occured. Credentials are probably incorrect.";
        if(!isHttpDigestSupported && !isUsernameTokenSupported)
        {
            errorString = "None of the authentication methods are available";
        }
    }
    else {
        errorString = location + ": " + fault.faultAsString();
    }
    qCritical() << errorString;
    q->disconnectFromDevice();
    emit q->errorStringChanged(errorString);
}
