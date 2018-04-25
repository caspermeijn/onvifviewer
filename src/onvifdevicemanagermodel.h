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
