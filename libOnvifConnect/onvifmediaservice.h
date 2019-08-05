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

#include "onvifconnect_export.h"
#include "onvifmediaprofile.h"
#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapDevicemgmt
{
class TT__MediaCapabilities;
}
namespace OnvifSoapMedia
{
class TRT__Capabilities;
}

class OnvifMediaServicePrivate;
class ONVIFCONNECT_EXPORT OnvifMediaService : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnvifMediaService)
    QScopedPointer<OnvifMediaServicePrivate> const d_ptr;
public:
    explicit OnvifMediaService(const QString& endpointAddress, OnvifDeviceConnection* parent);
    ~OnvifMediaService();

    void connectToService();
    void disconnectFromService();

    QList<OnvifMediaProfile> getProfileList() const;

    void selectProfile(const OnvifMediaProfile& profile);

    bool supportsSnapshotUri() const;
    QUrl getSnapshotUri() const;
    QUrl getStreamUri() const;

    //TODO: Move setServiceCapabilities to private class
    void setServiceCapabilities(const OnvifSoapMedia::TRT__Capabilities& capabilities);
    void setServiceCapabilities(const OnvifSoapDevicemgmt::TT__MediaCapabilities& capabilities);

    void setPreferredVideoStreamProtocol(const QString& preferredVideoStreamProtocol);

signals:
    void profileListAvailable(const QList<OnvifMediaProfile>& profileList);
    void supportsSnapshotUriAvailable(bool supportsSnapshotUri);
    void snapshotUriAvailable(const QUrl& snapshotUri);
    void streamUriAvailable(const QUrl& streamUri);
};

#endif // ONVIFMEDIASERVICE_H
