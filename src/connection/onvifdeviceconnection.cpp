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

using namespace OnvifSoapDevicemgmt;

static const QString c_baseEndpointURI = "http://%1/onvif/device_service";

class OnvifDeviceConnection::Private
{
public:
    Private() :
        deviceService(NULL),
        mediaService(NULL),
        media2Service(NULL),
        ptzService(NULL)
    {;}

    OnvifSoapDevicemgmt::DeviceBindingService soapService;
    OnvifDeviceService * deviceService;
    OnvifMediaService * mediaService;
    OnvifMedia2Service * media2Service;
    OnvifPtzService * ptzService;

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
    d->deviceService = NULL;

    delete d->mediaService;
    d->mediaService = NULL;

    delete d->media2Service;
    d->media2Service = NULL;

    delete d->ptzService;
    d->ptzService = NULL;
}

void OnvifDeviceConnection::getServicesDone(const TDS__GetServicesResponse &parameters)
{
    for(auto service : parameters.service())
    {
        if(service.namespace_() == "http://www.onvif.org/ver10/device/wsdl")
        {
            OnvifSoapDevicemgmt::TDS__DeviceServiceCapabilities capabilities;
            capabilities.deserialize(service.capabilities().any());
            d->isUsernameTokenSupported = capabilities.security().usernameToken();
            d->isHttpDigestSupported = capabilities.security().httpDigest();
            //TODO: pass the recieved capabilities on to the services
            if(!d->deviceService)
            {
                d->deviceService = new OnvifDeviceService(service.xAddr(), this);
            }
        }
        else if(service.namespace_() == "http://www.onvif.org/ver10/media/wsdl")
        {
            if(!d->mediaService)
            {
                d->mediaService = new OnvifMediaService(service.xAddr(), this);
            }
        }
        else if(service.namespace_() == "http://www.onvif.org/ver20/media/wsdl")
        {
            if(!d->media2Service)
            {
                d->media2Service = new OnvifMedia2Service(service.xAddr(), this);
            }
        }
        else if(service.namespace_() == "http://www.onvif.org/ver20/ptz/wsdl")
        {
            if(!d->ptzService)
            {
                d->ptzService = new OnvifPtzService(service.xAddr(), this);
            }
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
            d->deviceService = new OnvifDeviceService(parameters.capabilities().device().xAddr(), this);
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
            d->mediaService = new OnvifMediaService(parameters.capabilities().media().xAddr(), this);
        }
    }
    if(parameters.capabilities().pTZ().xAddr().size())
    {
        if(!d->ptzService)
        {
            d->ptzService = new OnvifPtzService(parameters.capabilities().pTZ().xAddr(), this);
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
