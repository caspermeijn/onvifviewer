#ifndef ONVIFDEVICECONNECTION_H
#define ONVIFDEVICECONNECTION_H

#include <QObject>

class OnvifDeviceService;
class OnvifMediaService;
class OnvifMedia2Service;
class OnvifPtzService;
class KDSoapClientInterface;
namespace OnvifSoapDevicemgmt {
class TDS__GetServicesResponse;
class TDS__GetCapabilitiesResponse;
}
class KDSoapMessage;

class OnvifDeviceConnection : public QObject
{
    Q_OBJECT
public:
    explicit OnvifDeviceConnection(QObject *parent = nullptr);
    ~OnvifDeviceConnection();

    void setHostname(const QString& hostname);
    void setCredentials(const QString& username, const QString& password);

    QString errorString() const;

    void connectToDevice();
    void disconnectFromDevice();

    OnvifDeviceService *getDeviceService() const;
    OnvifMediaService *getMediaService() const;
    OnvifMedia2Service *getMedia2Service() const;
    OnvifPtzService *getPtzService() const;

    /* internal */
    void updateSoapCredentials(KDSoapClientInterface * clientInterface);
    void updateUrlCredentials(QUrl* url);
    void handleSoapError(const KDSoapMessage& fault, const QString& location);
signals:
    void servicesAvailable();
    void errorStringChanged(const QString& errorString);

private slots:
    void getServicesDone( const OnvifSoapDevicemgmt::TDS__GetServicesResponse& parameters );
    void getServicesError( const KDSoapMessage& fault );
    void getCapabilitiesDone( const OnvifSoapDevicemgmt::TDS__GetCapabilitiesResponse& parameters );
    void getCapabilitiesError( const KDSoapMessage& fault );

private:
    void checkServicesAvailable();
    void updateUsernameToken(KDSoapClientInterface * clientInterface);
    void updateKDSoapAuthentication(KDSoapClientInterface * clientInterface);
    class Private;
    Private *const d;
};

#endif // ONVIFDEVICECONNECTION_H
