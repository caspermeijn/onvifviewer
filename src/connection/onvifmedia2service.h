#ifndef ONVIFMEDIA2SERVICE_H
#define ONVIFMEDIA2SERVICE_H

#include "onvifmediaprofile.h"
#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapMedia2 {
class TT__Profile;
class TT__PTZConfiguration;
class TR2__GetProfilesResponse;
class TR2__GetServiceCapabilitiesResponse;
class TR2__GetSnapshotUriResponse;
class TR2__GetStreamUriResponse;
}
class KDSoapMessage;

class OnvifMedia2Service : public QObject
{
    Q_OBJECT
public:
    explicit OnvifMedia2Service(const QString& endpointAddress, OnvifDeviceConnection *parent);

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
    void getServiceCapabilitiesDone( const OnvifSoapMedia2::TR2__GetServiceCapabilitiesResponse& parameters );
    void getServiceCapabilitiesError( const KDSoapMessage& fault );
    void getProfilesDone( const OnvifSoapMedia2::TR2__GetProfilesResponse& parameters );
    void getProfilesError( const KDSoapMessage& fault );
    void getSnapshotUriDone( const OnvifSoapMedia2::TR2__GetSnapshotUriResponse& parameters );
    void getSnapshotUriError( const KDSoapMessage& fault );
    void getStreamUriDone( const OnvifSoapMedia2::TR2__GetStreamUriResponse& parameters );
    void getStreamUriError( const KDSoapMessage& fault );

private:
    class Private;
    Private *const d;
};

#endif // ONVIFMEDIA2SERVICE_H
