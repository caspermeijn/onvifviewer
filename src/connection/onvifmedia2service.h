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
#ifndef ONVIFMEDIA2SERVICE_H
#define ONVIFMEDIA2SERVICE_H

#include "onvifmediaprofile.h"
#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapMedia2 {
class TT__Profile;
class TT__PTZConfiguration;
class TR2__Capabilities2;
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
    explicit OnvifMedia2Service(const QString& endpointAddress, const OnvifSoapMedia2::TR2__Capabilities2& capabilities, OnvifDeviceConnection *parent);

    void connectToService();
    void disconnectFromService();

    QList<OnvifMediaProfile> getProfileList() const;

    void selectProfile(const OnvifMediaProfile& profile);

    bool supportsSnapshotUri() const;
    QUrl getSnapshotUri() const;
    QUrl getStreamUri() const;

    void setPreferredVideoStreamProtocol(const QString& preferredVideoStreamProtocol);

signals:
    void profileListAvailable(const QList<OnvifMediaProfile>& profileList);
    void supportsSnapshotUriAvailable(bool supportsSnapshotUri);
    void snapshotUriAvailable(const QUrl& snapshotUri);
    void streamUriAvailable(const QUrl& streamUri);

private slots:
    void getProfilesDone( const OnvifSoapMedia2::TR2__GetProfilesResponse& parameters );
    void getProfilesError( const KDSoapMessage& fault );
    void getSnapshotUriDone( const OnvifSoapMedia2::TR2__GetSnapshotUriResponse& parameters );
    void getSnapshotUriError( const KDSoapMessage& fault );
    void getStreamUriDone( const OnvifSoapMedia2::TR2__GetStreamUriResponse& parameters );
    void getStreamUriError( const KDSoapMessage& fault );

private:
    void setServiceCapabilities(const OnvifSoapMedia2::TR2__Capabilities2& capabilities);

private:
    class Private;
    Private *const d;
};

#endif // ONVIFMEDIA2SERVICE_H
