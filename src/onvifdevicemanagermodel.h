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
#ifndef ONVIFDEVICEMANAGERMODEL_H
#define ONVIFDEVICEMANAGERMODEL_H

#include <QAbstractListModel>

class OnvifDevice;
class OnvifDeviceManager;

class OnvifDeviceManagerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit OnvifDeviceManagerModel(const OnvifDeviceManager* deviceManager, QObject *parent = nullptr);

public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QHash<int, QByteArray> roleNames() const;

private slots:
    void deviceListChanged();
    void deviceChanged();

private:
    const OnvifDeviceManager* m_deviceManager;
};

#endif // ONVIFDEVICEMANAGERMODEL_H
