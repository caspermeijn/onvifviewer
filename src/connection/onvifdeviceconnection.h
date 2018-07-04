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
#ifndef ONVIFDEVICECONNECTION_H
#define ONVIFDEVICECONNECTION_H

#include <QObject>

class OnvifDeviceService;
class OnvifMediaService;
class OnvifMedia2Service;
class OnvifPtzService;
class KDSoapClientInterface;
namespace OnvifSoapDevicemgmt {
class TDS__GetServicesResponse;
class TDS__GetCapabilitiesResponse;
}
class KDSoapMessage;

class OnvifDeviceConnection : public QObject
{
    Q_OBJECT
public:
    explicit OnvifDeviceConnection(QObject *parent = nullptr);
    ~OnvifDeviceConnection();

    void setHostname(const QString& hostname);
    void setCredentials(const QString& username, const QString& password);

    QString errorString() const;

    void connectToDevice();
    void disconnectFromDevice();

    OnvifDeviceService *getDeviceService() const;
    OnvifMediaService *getMediaService() const;
    OnvifMedia2Service *getMedia2Service() const;
    OnvifPtzService *getPtzService() const;

    /* internal */
    void updateSoapCredentials(KDSoapClientInterface * clientInterface);
    void updateUrlCredentials(QUrl* url);
    void handleSoapError(const KDSoapMessage& fault, const QString& location);
signals:
    void servicesAvailable();
    void errorStringChanged(const QString& errorString);

private slots:
    void getServicesDone( const OnvifSoapDevicemgmt::TDS__GetServicesResponse& parameters );
    void getServicesError( const KDSoapMessage& fault );
    void getCapabilitiesDone( const OnvifSoapDevicemgmt::TDS__GetCapabilitiesResponse& parameters );
    void getCapabilitiesError( const KDSoapMessage& fault );

private:
    void checkServicesAvailable();
    void updateUsernameToken(KDSoapClientInterface * clientInterface);
    void updateKDSoapAuthentication(KDSoapClientInterface * clientInterface);
    class Private;
    Private *const d;
};

#endif // ONVIFDEVICECONNECTION_H
