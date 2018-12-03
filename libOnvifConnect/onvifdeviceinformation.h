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
#ifndef ONVIFDEVICEINFORMATION_H
#define ONVIFDEVICEINFORMATION_H

#include "onvifconnect_export.h"
#include <QSharedDataPointer>
#include <QObject>

namespace OnvifSoapDevicemgmt {
class TDS__GetDeviceInformationResponse;
}

class OnvifDeviceInformationPrivate;
class ONVIFCONNECT_EXPORT OnvifDeviceInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString manufacturer READ manufacturer NOTIFY manufacturerChanged)
    Q_PROPERTY(QString model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString firmwareVersion READ firmwareVersion NOTIFY firmwareVersionChanged)
    Q_PROPERTY(QString serialNumber READ serialNumber NOTIFY serialNumberChanged)
    Q_PROPERTY(QString hardwareId READ hardwareId NOTIFY hardwareIdChanged)
public:
    OnvifDeviceInformation(QObject* parent = nullptr);
    OnvifDeviceInformation(const OnvifDeviceInformation &other, QObject* parent = nullptr);
    OnvifDeviceInformation(const OnvifSoapDevicemgmt::TDS__GetDeviceInformationResponse& deviceInformationResponse, QObject* parent = nullptr);
    ~OnvifDeviceInformation();

    OnvifDeviceInformation &operator=(const OnvifDeviceInformation &other);

    QString manufacturer() const;
    QString model() const;
    QString firmwareVersion() const;
    QString serialNumber() const;
    QString hardwareId() const;

signals:
    void manufacturerChanged(const QString& manufacturer);
    void modelChanged(const QString& model);
    void firmwareVersionChanged(const QString& firmwareVersion);
    void serialNumberChanged(const QString& serialNumber);
    void hardwareIdChanged(const QString& hardwareId);

private:
    QSharedDataPointer<OnvifDeviceInformationPrivate> d;
};

QDebug operator<<(QDebug debug, const OnvifDeviceInformation &d);

#endif // ONVIFDEVICEINFORMATION_H
