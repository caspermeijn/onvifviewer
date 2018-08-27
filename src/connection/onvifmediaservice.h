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
#ifndef ONVIFMEDIASERVICE_H
#define ONVIFMEDIASERVICE_H

#include "onvifmediaprofile.h"
#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapMedia {
class TT__Profile;
class TT__PTZConfiguration;
class TRT__Capabilities;
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

    bool supportsSnapshotUri() const;
    QUrl getSnapshotUri() const;
    QUrl getStreamUri() const;

    void setServiceCapabilities(const OnvifSoapMedia::TRT__Capabilities& capabilities);

    void setPreferredVideoStreamProtocol(const QString& preferredVideoStreamProtocol);

signals:
    void profileListAvailable(const QList<OnvifMediaProfile>& profileList);
    void supportsSnapshotUriAvailable(bool supportsSnapshotUri);
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
