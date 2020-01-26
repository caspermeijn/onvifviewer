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
#ifndef ONVIFDEVICECONNECTION_H
#define ONVIFDEVICECONNECTION_H

#include "onvifconnect_export.h"
#include <QObject>

class OnvifDeviceService;
class OnvifMediaService;
class OnvifMedia2Service;
class OnvifPtzService;

class OnvifDeviceConnectionPrivate;
class ONVIFCONNECT_EXPORT OnvifDeviceConnection : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnvifDeviceConnection)
    friend class OnvifDeviceService;
    friend class OnvifDeviceServicePrivate;
    friend class OnvifMediaService;
    friend class OnvifMediaServicePrivate;
    friend class OnvifMedia2Service;
    friend class OnvifMedia2ServicePrivate;
    friend class OnvifPtzService;
    friend class OnvifPtzServicePrivate;
    QScopedPointer<OnvifDeviceConnectionPrivate> const d_ptr;
public:
    explicit OnvifDeviceConnection(QObject* parent = nullptr);
    ~OnvifDeviceConnection();

    void setHostname(const QString& hostname);
    void setCredentials(const QString& username, const QString& password);

    QString errorString() const;

    void connectToDevice();
    void disconnectFromDevice();

    OnvifDeviceService* getDeviceService() const;
    OnvifMediaService* getMediaService() const;
    OnvifMedia2Service* getMedia2Service() const;
    OnvifPtzService* getPtzService() const;

signals:
    void servicesAvailable();
    void errorStringChanged(const QString& errorString);
};

#endif // ONVIFDEVICECONNECTION_H
