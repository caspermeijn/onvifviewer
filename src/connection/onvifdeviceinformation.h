#ifndef ONVIFDEVICEINFORMATION_H
#define ONVIFDEVICEINFORMATION_H

#include <QSharedDataPointer>
#include <QObject>

namespace OnvifSoapDevicemgmt {
class TDS__GetDeviceInformationResponse;
}

class OnvifDeviceInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString manufacturer READ manufacturer NOTIFY manufacturerChanged)
    Q_PROPERTY(QString model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString firmwareVersion READ firmwareVersion NOTIFY firmwareVersionChanged)
    Q_PROPERTY(QString serialNumber READ serialNumber NOTIFY serialNumberChanged)
    Q_PROPERTY(QString hardwareId READ hardwareId NOTIFY hardwareIdChanged)
public:
    OnvifDeviceInformation(QObject* parent = nullptr);
    OnvifDeviceInformation(const OnvifDeviceInformation &other, QObject* parent = nullptr);
    OnvifDeviceInformation(const OnvifSoapDevicemgmt::TDS__GetDeviceInformationResponse& deviceInformationResponse, QObject* parent = nullptr);
    ~OnvifDeviceInformation();

    OnvifDeviceInformation &operator=(const OnvifDeviceInformation &other);

    QString manufacturer() const;
    QString model() const;
    QString firmwareVersion() const;
    QString serialNumber() const;
    QString hardwareId() const;

signals:
    void manufacturerChanged(const QString& manufacturer);
    void modelChanged(const QString& model);
    void firmwareVersionChanged(const QString& firmwareVersion);
    void serialNumberChanged(const QString& serialNumber);
    void hardwareIdChanged(const QString& hardwareId);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

QDebug operator<<(QDebug debug, const OnvifDeviceInformation &d);

#endif // ONVIFDEVICEINFORMATION_H
