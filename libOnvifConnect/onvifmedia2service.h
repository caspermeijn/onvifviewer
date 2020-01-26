/* Copyright (C) 2018 Casper Meijn <casper@meijn.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
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

#include "onvifconnect_export.h"
#include "onvifmediaprofile.h"
#include <QObject>

class OnvifDeviceConnection;
namespace OnvifSoapMedia2
{
class TR2__Capabilities2;
}

class OnvifMedia2ServicePrivate;
class ONVIFCONNECT_EXPORT OnvifMedia2Service : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnvifMedia2Service)
    QScopedPointer<OnvifMedia2ServicePrivate> const d_ptr;
public:
    //TODO: Move capabilities to setServiceCapabilities in private class
    explicit OnvifMedia2Service(const QString& endpointAddress, const OnvifSoapMedia2::TR2__Capabilities2& capabilities, OnvifDeviceConnection* parent);
    ~OnvifMedia2Service();

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
};

#endif // ONVIFMEDIA2SERVICE_H
