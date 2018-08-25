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

static const QString c_baseEndpointURI = "http://%1/onvif/device_service";

class OnvifDeviceConnection::Private
{
public:
    Private() :
        deviceService(nullptr),
        mediaService(nullptr),
        media2Service(nullptr),
        ptzService(nullptr)
    {;}

    OnvifSoapDevicemgmt::DeviceBindingService soapService;
    OnvifDeviceService * deviceService;
    OnvifMediaService * mediaService;
    OnvifMedia2Service * media2Service;
    OnvifPtzService * ptzService;

    QString hostname;
    QString username;
    QString password;

    QString errorString;

    bool isUsernameTokenSupported = false;
    bool isHttpDigestSupported = false;

    bool getServicesFinished = false;
    bool getCapabilitiesFinished = false;
};

OnvifDeviceConnection::OnvifDeviceConnection(QObject *parent) :
    QObject(parent),
    d(new OnvifDeviceConnection::Private)
{
    connect(&d->soapService, &DeviceBindingService::getServicesDone,
            this, &OnvifDeviceConnection::getServicesDone);
    connect(&d->soapService, &DeviceBindingService::getServicesError,
            this, &OnvifDeviceConnection::getServicesError);
    connect(&d->soapService, &DeviceBindingService::getCapabilitiesDone,
            this, &OnvifDeviceConnection::getCapabilitiesDone);
    connect(&d->soapService, &DeviceBindingService::getCapabilitiesError,
            this, &OnvifDeviceConnection::getCapabilitiesError);
}

OnvifDeviceConnection::~OnvifDeviceConnection()
{
    delete d;
}

void OnvifDeviceConnection::setHostname(const QString& hostname)
{
    d->hostname = hostname;
    d->soapService.setEndPoint(c_baseEndpointURI.arg(hostname));
}

void OnvifDeviceConnection::setCredentials(const QString &username, const QString &password)
{
    d->username = username;
    d->password = password;
}

QString OnvifDeviceConnection::errorString() const
{
    return d->errorString;
}

void OnvifDeviceConnection::connectToDevice()
{
    d->isUsernameTokenSupported = false;
    d->isHttpDigestSupported = false;

    d->getCapabilitiesFinished = false;
    d->getServicesFinished = false;

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
    d->getCapabilitiesFinished = false;
    d->getServicesFinished = false;

    delete d->deviceService;
    d->deviceService = nullptr;

    delete d->mediaService;
    d->mediaService = nullptr;

    delete d->media2Service;
    d->media2Service = nullptr;

    delete d->ptzService;
    d->ptzService = nullptr;
}

void OnvifDeviceConnection::getServicesDone(const TDS__GetServicesResponse &parameters)
{
    for(auto service : parameters.service())
    {
        QUrl xAddrUrl(service.xAddr());
        this->updateUrlHost(&xAddrUrl);
        if(service.namespace_() == "http://www.onvif.org/ver10/device/wsdl")
        {
            if(QUrl(service.xAddr()) != QUrl(c_baseEndpointURI.arg(d->hostname))) {
                qWarning() << "Warning: The recieved address of the device service doesn't match the address of the initial connection.";
                qWarning() << "Recieved address:" << QUrl(service.xAddr()).toString();
                qWarning() << "Initial connection:" << QUrl(c_baseEndpointURI.arg(d->hostname)).toString();
            }
            OnvifSoapDevicemgmt::TDS__DeviceServiceCapabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            d->isUsernameTokenSupported = capabilities.security().usernameToken();
            d->isHttpDigestSupported = capabilities.security().httpDigest();
            if(!d->deviceService)
            {
                d->deviceService = new OnvifDeviceService(xAddrUrl.toString(), this);
            }
        }
        else if(service.namespace_() == "http://www.onvif.org/ver10/media/wsdl")
        {
            if(!d->mediaService)
            {
                d->mediaService = new OnvifMediaService(xAddrUrl.toString(), this);
            }
            OnvifSoapMedia::TRT__Capabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            d->mediaService->setServiceCapabilities(capabilities);
        }
        else if(service.namespace_() == "http://www.onvif.org/ver20/media/wsdl")
        {
            if(!d->media2Service)
            {
                OnvifSoapMedia2::TR2__Capabilities2 capabilities;
                capabilities.deserialize(service.capabilities().any());
                d->media2Service = new OnvifMedia2Service(xAddrUrl.toString(), capabilities, this);
            }
        }
        else if(service.namespace_() == "http://www.onvif.org/ver20/ptz/wsdl")
        {
            if(!d->ptzService)
            {
                d->ptzService = new OnvifPtzService(xAddrUrl.toString(), this);
            }
            OnvifSoapPtz::TPTZ__Capabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            d->ptzService->setServiceCapabilities(capabilities);
        }
    }

    d->getServicesFinished = true;
    checkServicesAvailable();
}

void OnvifDeviceConnection::getServicesError(const KDSoapMessage &fault)
{
    handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifDeviceConnection::getCapabilitiesDone(const TDS__GetCapabilitiesResponse &parameters)
{
    if(parameters.capabilities().analytics().xAddr().size())
    {
        // Not yet supported
    }
    if(parameters.capabilities().device().xAddr().size())
    {
        if(!d->deviceService)
        {
            QUrl xAddrUrl(parameters.capabilities().device().xAddr());
            this->updateUrlHost(&xAddrUrl);
            d->deviceService = new OnvifDeviceService(xAddrUrl.toString(), this);
        }
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
        if(!d->mediaService)
        {
            QUrl xAddrUrl(parameters.capabilities().media().xAddr());
            this->updateUrlHost(&xAddrUrl);
            d->mediaService = new OnvifMediaService(xAddrUrl.toString(), this);
        }
    }
    if(parameters.capabilities().pTZ().xAddr().size())
    {
        if(!d->ptzService)
        {
            QUrl xAddrUrl(parameters.capabilities().pTZ().xAddr());
            this->updateUrlHost(&xAddrUrl);
            d->ptzService = new OnvifPtzService(xAddrUrl.toString(), this);
        }
    }

    d->getCapabilitiesFinished = true;
    checkServicesAvailable();
}

void OnvifDeviceConnection::getCapabilitiesError(const KDSoapMessage &fault)
{
    handleSoapError(fault, Q_FUNC_INFO);
}

void OnvifDeviceConnection::checkServicesAvailable()
{
    if(d->getServicesFinished && d->getCapabilitiesFinished)
    {
        if (d->deviceService)
            d->deviceService->connectToService();
        if (d->mediaService)
            d->mediaService->connectToService();
        if (d->media2Service)
            d->media2Service->connectToService();
        if (d->ptzService)
            d->ptzService->connectToService();
        emit servicesAvailable();
    }
}

OnvifDeviceService *OnvifDeviceConnection::getDeviceService() const
{
    return d->deviceService;
}

OnvifMediaService *OnvifDeviceConnection::getMediaService() const
{
    return d->mediaService;
}

OnvifMedia2Service *OnvifDeviceConnection::getMedia2Service() const
{
    return d->media2Service;
}

OnvifPtzService *OnvifDeviceConnection::getPtzService() const
{
    return d->ptzService;
}

void OnvifDeviceConnection::updateUrlHost(QUrl *url)
{
    QUrl origUrl(c_baseEndpointURI.arg(d->hostname));
    if(url->host() != origUrl.host()) {
        url->setHost(origUrl.host());
        url->setPort(origUrl.port());
    }
}

void OnvifDeviceConnection::updateSoapCredentials(KDSoapClientInterface *clientInterface)
{
    if(d->isHttpDigestSupported)
        updateKDSoapAuthentication(clientInterface);
    else if(d->isUsernameTokenSupported)
        updateUsernameToken(clientInterface);
    // Some camera's don't require authentication and therefore don't ask for any
}

void OnvifDeviceConnection::updateUsernameToken(KDSoapClientInterface *clientInterface)
{
    QByteArray nonce = "abc" + QByteArray::number(qrand());
    KDSoapValue nonceValue("Nonce", nonce);
    nonceValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");
    nonceValue.childValues().attributes().append(KDSoapValue("EncodingType", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary"));

    QString timestamp = QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ssZ");
    KDSoapValue createdValue("Created", timestamp);
    createdValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd");

    QByteArray passwordConcat = nonce + timestamp.toUtf8() + d->password.toUtf8();
    QByteArray passwordHash = QCryptographicHash::hash(passwordConcat, QCryptographicHash::Sha1);
    KDSoapValue passwordValue ("Password", passwordHash);
    passwordValue.setNamespaceUri("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");
    passwordValue.childValues().attributes().append(KDSoapValue("Type", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest"));

    KDSoapValue usernameValue("Username", d->username);
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

void OnvifDeviceConnection::updateKDSoapAuthentication(KDSoapClientInterface *clientInterface)
{
    KDSoapAuthentication auth;
    auth.setUser(d->username);
    auth.setPassword(d->password);
    clientInterface->setAuthentication(auth);
}

void OnvifDeviceConnection::updateUrlCredentials(QUrl *url)
{
    Q_ASSERT(url);
    url->setUserName(d->username);
    url->setPassword(d->password);
}

void OnvifDeviceConnection::handleSoapError(const KDSoapMessage &fault, const QString &location)
{
    d->errorString = "";
    if(fault.childValues().child(QLatin1String("faultcode")).value().toInt() == QNetworkReply::OperationCanceledError) {
        d->errorString = "A possible authentication error. Please install a more recent version of KDSoap for more detailed error message.";
    }
    else if(fault.childValues().child(QLatin1String("faultcode")).value().toInt() == QNetworkReply::AuthenticationRequiredError) {
        d->errorString = "Authentication error occured. Credentials are probably incorrect.";
        if(!d->isHttpDigestSupported && !d->isUsernameTokenSupported)
        {
            d->errorString = "None of the authentication methods are available";
        }
    }
    else {
        d->errorString = location + ": " + fault.faultAsString();
    }
    qCritical() << d->errorString;
    disconnectFromDevice();
    emit errorStringChanged(d->errorString);
}
