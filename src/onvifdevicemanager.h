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
#ifndef ONVIFDEVICEMANAGER_H
#define ONVIFDEVICEMANAGER_H

#include <QObject>

class OnvifDevice;

class OnvifDeviceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<OnvifDevice*> deviceList READ deviceList NOTIFY deviceListChanged)
    Q_PROPERTY(int size READ size NOTIFY deviceListChanged)
public:
    explicit OnvifDeviceManager(QObject* parent = nullptr);

    QList<OnvifDevice*> deviceList() const;
    Q_INVOKABLE OnvifDevice* at(int i);
    Q_INVOKABLE int appendDevice();
    Q_INVOKABLE void removeDevice(int i);
    Q_INVOKABLE int indexOf(OnvifDevice* device);
    int size();

    OnvifDevice* createNewDevice();

signals:
    void deviceListChanged(const QList<OnvifDevice*>& deviceList);

public slots:
    void loadDevices();
    void saveDevices();

private:
    QList<OnvifDevice*> m_deviceList;
};

#endif // ONVIFDEVICEMANAGER_H
