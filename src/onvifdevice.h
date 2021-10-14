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
#ifndef ONVIFDEVICE_H
#define ONVIFDEVICE_H

#include "onvifdeviceconnection.h"
#include "onvifmediaprofile.h"
#include <QTimer>
#include <QObject>
#include <QUrl>

class OnvifDeviceInformation;
class OnvifSnapshotDownloader;

class OnvifDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool preferContinuousMove READ preferContinuousMove WRITE setPreferContinuousMove NOTIFY preferContinuousMoveChanged)
    Q_PROPERTY(QString preferredVideoStreamProtocol READ preferredVideoStreamProtocol WRITE setPreferredVideoStreamProtocol NOTIFY preferredVideoStreamProtocolChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(OnvifDeviceInformation* deviceInformation READ deviceInformation NOTIFY deviceInformationChanged)
    Q_PROPERTY(bool supportsSnapshotUri READ supportsSnapshotUri NOTIFY supportsSnapshotUriChanged)
    Q_PROPERTY(QUrl snapshotUri READ snapshotUri NOTIFY snapshotUriChanged)
    Q_PROPERTY(QUrl streamUri READ streamUri NOTIFY streamUriChanged)
    Q_PROPERTY(bool isPanTiltSupported READ isPanTiltSupported)
    Q_PROPERTY(bool isPtzHomeSupported READ isPtzHomeSupported)
    Q_PROPERTY(bool isZoomSupported READ isZoomSupported)
    Q_PROPERTY(OnvifSnapshotDownloader* snapshotDownloader READ snapshotDownloader NOTIFY snapshotDownloaderChanged)
    Q_PROPERTY(QStringList profileNames READ profileNames NOTIFY profileNamesChanged)
    Q_PROPERTY(qreal pan READ pan NOTIFY panChanged)
    Q_PROPERTY(qreal tilt READ tilt NOTIFY tiltChanged)
    Q_PROPERTY(qreal zoom READ zoom NOTIFY zoomChanged)
    Q_PROPERTY(int getPTZStatusInterval READ getPTZStatusInterval WRITE setGetPTZStatusInterval NOTIFY getPTZStatusIntervalChanged)
    Q_PROPERTY(QStringList ptzConfigurationNames READ ptzConfigurationNames NOTIFY ptzConfigurationNamesChanged)
public:
    explicit OnvifDevice(QObject* parent = nullptr);

    Q_INVOKABLE void connectToDevice();
    Q_INVOKABLE void reconnectToDevice();

    OnvifDeviceInformation* deviceInformation() const;
    OnvifSnapshotDownloader* snapshotDownloader() const;
    bool supportsSnapshotUri() const;
    QUrl snapshotUri() const;
    QUrl streamUri() const;
    QString errorString() const;

    QString deviceName() const;
    void setDeviceName(const QString& deviceName);

    QString hostName() const;
    void setHostName(const QString& hostName);

    QString userName() const;
    void setUserName(const QString& userName);

    QString password() const;
    void setPassword(const QString& password);

    bool isPanTiltSupported() const;
    bool isPtzHomeSupported() const;
    bool isZoomSupported() const;

    bool preferContinuousMove() const;
    void setPreferContinuousMove(bool preferContinuousMove);

    QString preferredVideoStreamProtocol() const;
    void setPreferredVideoStreamProtocol(const QString& preferredVideoStreamProtocol);

    void initByUrl(const QUrl& url);
    
    QStringList profileNames() const;

    qreal pan() const;
    qreal tilt() const;
    qreal zoom() const;

    int getPTZStatusInterval() const;

    QStringList ptzConfigurationNames() const;

    Q_INVOKABLE bool isPtzSpaceSupported(int space, const QString& uri) const;

    Q_INVOKABLE qreal panSpaceMax(int space, const QString& uri) const;

    Q_INVOKABLE qreal panSpaceMin(int space, const QString& uri) const;

    Q_INVOKABLE qreal tiltSpaceMax(int space, const QString& uri) const;

    Q_INVOKABLE qreal tiltSpaceMin(int space, const QString& uri) const;

    Q_INVOKABLE qreal zoomSpaceMax(int space, const QString& uri) const;

    Q_INVOKABLE qreal zoomSpaceMin(int space, const QString& uri) const;

signals:
    void deviceNameChanged(const QString& deviceName);
    void hostNameChanged(const QString& hostName);
    void userNameChanged(const QString& userName);
    void passwordChanged(const QString& password);
    void preferContinuousMoveChanged(bool preferContinuousMove);
    void preferredVideoStreamProtocolChanged(const QString& preferredVideoStreamProtocol);
    void errorStringChanged(const QString& errorString);
    void deviceInformationChanged(OnvifDeviceInformation* deviceInformation);
    void supportsSnapshotUriChanged(bool supportsSnapshotUri);
    void snapshotUriChanged(const QUrl& url);
    void streamUriChanged(const QUrl& url);
    void snapshotDownloaderChanged(OnvifSnapshotDownloader* snapshotDownloader);
    void profileNamesChanged(const QStringList& profileNames);
    void panChanged(qreal pan);
    void tiltChanged(qreal tilt);
    void zoomChanged(qreal zoom);
    void getPTZStatusIntervalChanged(int interval);
    void ptzConfigurationNamesChanged(const QStringList& configurationNames);

public slots:
    void ptzUp();
    void ptzDown();
    void ptzLeft();
    void ptzRight();
    void ptzMove(float xFraction, float yFraction);
    void ptzHome();
    void ptzSaveHomePosition();
    void ptzStop();
    void ptzZoomIn();
    void ptzZoomOut();
    void selectMediaProfile(int index);
    void setGetPTZStatusInterval(int interval);
    void selectPTZConfiguration(const QString& configurationName);

private slots:
    void servicesAvailable();
    void deviceInformationAvailable(const OnvifDeviceInformation& deviceInformation);
    void profileListAvailable(const QList<OnvifMediaProfile>& profileList);
    void getPTZStatus();
    void setPan(qreal pan);
    void setTilt(qreal tilt);
    void setZoom(qreal zoom);
    void ptzConfigurationsAvailable(const QStringList& configurationNames);

private:
    OnvifDeviceConnection m_connection;
    QString m_deviceName;
    QString m_hostName;
    QString m_userName;
    QString m_password;
    bool m_preferContinuousMove;
    QString m_preferredVideoStreamProtocol;
    OnvifMediaProfile m_selectedMediaProfile;
    OnvifDeviceInformation* m_cachedDeviceInformation;
    QTimer m_ptzStopTimer;
    OnvifSnapshotDownloader* m_cachedSnapshotDownloader;
    QStringList m_profileNames;
    QList<OnvifMediaProfile> m_sortedProfileList;
    qreal m_pan;
    qreal m_tilt;
    qreal m_zoom;
    int m_getPTZStatusInterval;
    QTimer m_getPTZStatusTimer;
    QStringList m_ptzConfigurationNames;
};

#endif // ONVIFDEVICE_H
