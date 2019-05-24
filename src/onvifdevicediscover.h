/* Copyright (C) 2019 Casper Meijn <casper@meijn.net>
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
#ifndef ONVIFDEVICEDISCOVER_H
#define ONVIFDEVICEDISCOVER_H

#include <QMap>
#include <QObject>
#include <QUrl>
#include <QDateTime>

class WSDiscoveryClient;
class WSDiscoveryProbeJob;
class WSDiscoveryTargetService;

class OnvifDeviceDiscoverMatch : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString hardware READ getHardware CONSTANT)
    Q_PROPERTY(QString endpoint READ getEndpoint CONSTANT)
    Q_PROPERTY(QUrl xAddr READ getXAddr CONSTANT)
    Q_PROPERTY(QString host READ getHost CONSTANT)
public:
    QString getName() const;
    QString getHardware() const;
    QString getEndpoint() const;
    QUrl getXAddr() const;
    QString getHost() const;

protected:
    QString m_name;
    QString m_hardware;
    QString m_endpoint;
    QUrl m_xAddr;
    QDateTime m_lastSeen;

    friend class OnvifDeviceDiscover;
};

class OnvifDeviceDiscover : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAvailable READ isAvailable CONSTANT)
    Q_PROPERTY(QObjectList matchList READ matchList NOTIFY matchListChanged)
public:
    explicit OnvifDeviceDiscover(QObject *parent = nullptr);
    bool isAvailable();
    QObjectList matchList() const;

signals:
    void matchListChanged(QObjectList matchMap);

public slots:
    void start();

private slots:
    void matchReceived(const QSharedPointer<WSDiscoveryTargetService>& matchedService);

private:
    WSDiscoveryClient * m_client;
    WSDiscoveryProbeJob * m_probeJob;
    QMap<QString, OnvifDeviceDiscoverMatch *> m_matchMap;
};

#endif // ONVIFDEVICEDISCOVER_H
