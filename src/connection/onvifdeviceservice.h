#ifndef ONVIFDEVICESERVICE_H
#define ONVIFDEVICESERVICE_H

#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapDevicemgmt {
class TDS__GetServiceCapabilitiesResponse;
class TDS__GetDeviceInformationResponse;
}
class KDSoapMessage;

class OnvifDeviceInformation
{
public:
    OnvifDeviceInformation();
    OnvifDeviceInformation(const OnvifSoapDevicemgmt::TDS__GetDeviceInformationResponse& deviceInformationResponse);

    QString manufacturer() const;
    QString model() const;
    QString firmwareVersion() const;
    QString serialNumber() const;
    QString hardwareId() const;

private:
    QString m_manufacturer;
    QString m_model;
    QString m_firmwareVersion;
    QString m_serialNumber;
    QString m_hardwareId;
};

QDebug operator<<(QDebug debug, const OnvifDeviceInformation &d);

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
    void getServiceCapabilitiesDone( const OnvifSoapDevicemgmt::TDS__GetServiceCapabilitiesResponse& parameters );
    void getServiceCapabilitiesError( const KDSoapMessage& fault );
    void getDeviceInformationDone( const OnvifSoapDevicemgmt::TDS__GetDeviceInformationResponse& parameters );
    void getDeviceInformationError( const KDSoapMessage& fault );

private:
    class Private;
    Private *const d;
};

#endif // ONVIFDEVICESERVICE_H
