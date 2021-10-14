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
#ifndef ONVIFPTZSERVICE_H
#define ONVIFPTZSERVICE_H

#include "onvifconnect_export.h"
#include <QObject>

class OnvifDeviceConnection;
class OnvifMediaProfile;
namespace OnvifSoapDevicemgmt
{
class TT__PTZCapabilities;
}
namespace OnvifSoapPtz
{
class TPTZ__Capabilities;
}

class OnvifPtzServicePrivate;
class ONVIFCONNECT_EXPORT OnvifPtzService : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnvifPtzService)
    QScopedPointer<OnvifPtzServicePrivate> const d_ptr;
public:
    explicit OnvifPtzService(const QString& endpointAddress, OnvifDeviceConnection* parent);
    ~OnvifPtzService();

    void connectToService();
    void disconnectFromService();

    //TODO: Move setServiceCapabilities to private class
    void setServiceCapabilities(const OnvifSoapPtz::TPTZ__Capabilities& capabilities);
    void setServiceCapabilities(const OnvifSoapDevicemgmt::TT__PTZCapabilities& capabilities);

    void absoluteMove(const OnvifMediaProfile& profile, float xFraction, float yFraction);

    bool isRelativeMoveSupported(const OnvifMediaProfile& profile) const;
    void relativeMove(const OnvifMediaProfile& profile, float xFraction, float yFraction);

    bool isContinuousMoveSupported(const OnvifMediaProfile& profile) const;
    void continuousMove(const OnvifMediaProfile& profile, float xFraction, float yFraction);

    bool isRelativeZoomSupported(const OnvifMediaProfile& profile) const;
    void relativeZoom(const OnvifMediaProfile& profile, float zoomFraction);

    bool isHomeSupported(const OnvifMediaProfile& profile) const;
    void goToHome(const OnvifMediaProfile& profile);
    void saveHomePosition(const OnvifMediaProfile& profile);

    void stopMovement(const OnvifMediaProfile& profile);

    void getStatus(const OnvifMediaProfile& profile);

    void setConfiguration(const QString& configuration);

    enum PTZSpaces {
        AbsolutePanTiltPositionSpace,
        AbsoluteZoomPositionSpace,
        RelativePanTiltTranslationSpace,
        RelativeZoomTranslationSpace,
        ContinuousPanTiltVelocitySpace,
        ContinuousZoomVelocitySpace,
        PanTiltSpeedSpace,
        ZoomSpeedSpace
    };
    Q_ENUM(PTZSpaces)

    bool isSpaceSupported(const OnvifMediaProfile& profile, PTZSpaces space, const QString& uri) const;

    qreal panSpaceMax(const OnvifMediaProfile& profile, PTZSpaces space, const QString& uri) const;

    qreal panSpaceMin(const OnvifMediaProfile& profile, PTZSpaces space, const QString& uri) const;

    qreal tiltSpaceMax(const OnvifMediaProfile& profile, PTZSpaces space, const QString& uri) const;

    qreal tiltSpaceMin(const OnvifMediaProfile& profile, PTZSpaces space, const QString& uri) const;

    qreal zoomSpaceMax(const OnvifMediaProfile& profile, PTZSpaces space, const QString& uri) const;

    qreal zoomSpaceMin(const OnvifMediaProfile& profile, PTZSpaces space, const QString& uri) const;

signals:
    void configurationsChanged(const QStringList&);
    void panChanged(qreal);
    void tiltChanged(qreal);
    void zoomChanged(qreal);
};

#endif // ONVIFPTZSERVICE_H
