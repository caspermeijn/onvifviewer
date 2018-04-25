#ifndef ONVIFDEVICE_H
#define ONVIFDEVICE_H

#include "onvifdeviceconnection.h"
#include <QObject>
#include <QUrl>

class OnvifDeviceInformation;
class OnvifMediaProfile;

class OnvifDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(QUrl snapshotUri READ snapshotUri NOTIFY snapshotUriChanged)
    Q_PROPERTY(QUrl streamUri READ streamUri NOTIFY streamUriChanged)
public:
    explicit OnvifDevice(QObject *parent = nullptr);

    void connectToDevice();
    Q_INVOKABLE void reconnectToDevice();

    QUrl snapshotUri() const;
    QUrl streamUri() const;
    QString errorString() const;

    QString deviceName() const;
    void setDeviceName(const QString &deviceName);

    QString hostName() const;
    void setHostName(const QString &hostName);

    QString userName() const;
    void setUserName(const QString &userName);

    QString password() const;
    void setPassword(const QString &password);

signals:
    void deviceNameChanged(const QString& deviceName);
    void hostNameChanged(const QString& hostName);
    void userNameChanged(const QString& userName);
    void passwordChanged(const QString& password);
    void errorStringChanged(const QString& errorString);
    void snapshotUriChanged(const QUrl& url);
    void streamUriChanged(const QUrl& url);

public slots:
    void ptzUp();
    void ptzDown();
    void ptzLeft();
    void ptzRight();
    void ptzHome();

private slots:
    void servicesAvailable();
    void deviceInformationAvailable(const OnvifDeviceInformation& deviceInformation);
    void profileListAvailable(const QList<OnvifMediaProfile>& profileList);
    void snapshotUriAvailable(const QUrl& snapshotUri);
    void streamUriAvailable(const QUrl& streamUri);

private:
    OnvifDeviceConnection m_connection;
    QString m_deviceName;
    QString m_hostName;
    QString m_userName;
    QString m_password;
};


#endif // ONVIFDEVICE_H
