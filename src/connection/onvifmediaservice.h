#ifndef ONVIFMEDIASERVICE_H
#define ONVIFMEDIASERVICE_H

#include "onvifmediaprofile.h"
#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapMedia {
class TT__Profile;
class TT__PTZConfiguration;
class TRT__GetProfilesResponse;
class TRT__GetServiceCapabilitiesResponse;
class TRT__GetSnapshotUriResponse;
class TRT__GetStreamUriResponse;
}
class KDSoapMessage;

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
