#ifndef ONVIFMEDIASERVICE_H
#define ONVIFMEDIASERVICE_H

#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapMedia {
class TT__Profile;
class TRT__GetProfilesResponse;
class TRT__GetServiceCapabilitiesResponse;
class TRT__GetSnapshotUriResponse;
class TRT__GetStreamUriResponse;
}
class KDSoapMessage;

class OnvifMediaProfile
{
public:
    OnvifMediaProfile();
    OnvifMediaProfile(const OnvifSoapMedia::TT__Profile& profile);

    QString name() const;
    void setName(const QString &name);

    QString token() const;
    void setToken(const QString &token);

    bool fixed() const;
    void setFixed(bool fixed);

private:
    bool m_fixed;
    QString m_name;
    QString m_token;
};
QDebug operator<<(QDebug debug, const OnvifMediaProfile &d);

class OnvifMediaService : public QObject
{
    Q_OBJECT
public:
    explicit OnvifMediaService(const QString& endpointAddress, OnvifDeviceConnection *parent);

    void connectToService();
    void disconnectFromService();

    QList<OnvifMediaProfile> getProfileList() const;

    void selectProfile(const OnvifMediaProfile& profile);

    QUrl getSnapshotUri() const;
    QUrl getStreamUri() const;

signals:
    void profileListAvailable(const QList<OnvifMediaProfile>& profileList);
    void snapshotUriAvailable(const QUrl& snapshotUri);
    void streamUriAvailable(const QUrl& streamUri);

private slots:
    void getServiceCapabilitiesDone( const OnvifSoapMedia::TRT__GetServiceCapabilitiesResponse& parameters );
    void getServiceCapabilitiesError( const KDSoapMessage& fault );
    void getProfilesDone( const OnvifSoapMedia::TRT__GetProfilesResponse& parameters );
    void getProfilesError( const KDSoapMessage& fault );
    void getSnapshotUriDone( const OnvifSoapMedia::TRT__GetSnapshotUriResponse& parameters );
    void getSnapshotUriError( const KDSoapMessage& fault );
    void getStreamUriDone( const OnvifSoapMedia::TRT__GetStreamUriResponse& parameters );
    void getStreamUriError( const KDSoapMessage& fault );

private:
    class Private;
    Private *const d;
};

#endif // ONVIFMEDIASERVICE_H
