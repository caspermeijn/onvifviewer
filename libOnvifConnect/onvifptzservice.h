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
#ifndef ONVIFPTZSERVICE_H
#define ONVIFPTZSERVICE_H

#include "onvifconnect_export.h"
#include <QObject>

class OnvifDeviceConnection;
class OnvifMediaProfile;
namespace OnvifSoapPtz {
class TPTZ__Capabilities;
class TPTZ__RelativeMoveResponse;
class TPTZ__GetServiceCapabilitiesResponse;
class TPTZ__GetNodesResponse;
class TPTZ__GotoHomePositionResponse;
class TPTZ__AbsoluteMoveResponse;
class TPTZ__ContinuousMoveResponse;
class TPTZ__GetConfigurationsResponse;
class TPTZ__GetStatusResponse;
class TPTZ__SetHomePositionResponse;
class TPTZ__StopResponse;
}
class KDSoapMessage;

class OnvifPtzServicePrivate;
class ONVIFCONNECT_EXPORT OnvifPtzService : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnvifPtzService)
    QScopedPointer<OnvifPtzServicePrivate> const d_ptr;
public:
    explicit OnvifPtzService(const QString& endpointAddress, OnvifDeviceConnection *parent);
    ~OnvifPtzService();

    void connectToService();
    void disconnectFromService();

    void setServiceCapabilities(const OnvifSoapPtz::TPTZ__Capabilities& capabilities);

    void absoluteMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction);

    bool isRelativeMoveSupported(const OnvifMediaProfile &profile) const;
    void relativeMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction);

    bool isContinuousMoveSupported(const OnvifMediaProfile &profile) const;
    void continuousMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction);

    bool isRelativeZoomSupported(const OnvifMediaProfile &profile) const;
    void relativeZoom(const OnvifMediaProfile &profile, qreal zoomFraction);

    bool isHomeSupported(const OnvifMediaProfile &profile) const;
    void goToHome(const OnvifMediaProfile &profile);
    void saveHomePosition(const OnvifMediaProfile &profile);

    void stopMovement(const OnvifMediaProfile &profile);

private slots:
    void getServiceCapabilitiesDone( const OnvifSoapPtz::TPTZ__GetServiceCapabilitiesResponse& parameters );
    void getServiceCapabilitiesError( const KDSoapMessage& fault );
    void getNodesDone( const OnvifSoapPtz::TPTZ__GetNodesResponse& parameters );
    void getNodesError( const KDSoapMessage& fault );
    void getConfigurationsDone( const OnvifSoapPtz::TPTZ__GetConfigurationsResponse& parameters );
    void getConfigurationsError( const KDSoapMessage& fault );
    void getStatusDone( const OnvifSoapPtz::TPTZ__GetStatusResponse& parameters );
    void getStatusError( const KDSoapMessage& fault );
    void absoluteMoveDone( const OnvifSoapPtz::TPTZ__AbsoluteMoveResponse& parameters );
    void absoluteMoveError( const KDSoapMessage& fault );
    void relativeMoveDone( const OnvifSoapPtz::TPTZ__RelativeMoveResponse& parameters );
    void relativeMoveError( const KDSoapMessage& fault );
    void continuousMoveDone( const OnvifSoapPtz::TPTZ__ContinuousMoveResponse& parameters );
    void continuousMoveError( const KDSoapMessage& fault );
    void gotoHomePositionDone( const OnvifSoapPtz::TPTZ__GotoHomePositionResponse& parameters );
    void gotoHomePositionError( const KDSoapMessage& fault );
    void setHomePositionDone( const OnvifSoapPtz::TPTZ__SetHomePositionResponse& parameters );
    void setHomePositionError( const KDSoapMessage& fault );
    void stopDone( const OnvifSoapPtz::TPTZ__StopResponse& parameters );
    void stopError( const KDSoapMessage& fault );
};


#endif // ONVIFPTZSERVICE_H
