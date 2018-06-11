#ifndef ONVIFDEVICESERVICE_H
#define ONVIFDEVICESERVICE_H

#include "onvifdeviceinformation.h"
#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapDevicemgmt {
class TDS__DeviceServiceCapabilities;
class TDS__GetServiceCapabilitiesResponse;
class TDS__GetDeviceInformationResponse;
}
class KDSoapMessage;

class OnvifDeviceService : public QObject
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
