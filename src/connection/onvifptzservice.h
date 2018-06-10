#ifndef ONVIFPTZSERVICE_H
#define ONVIFPTZSERVICE_H

#include <QObject>

class OnvifDeviceConnection;
class OnvifMediaProfile;
namespace OnvifSoapPtz {
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

class OnvifPtzService : public QObject
{
    Q_OBJECT
public:
    explicit OnvifPtzService(const QString& endpointAddress, OnvifDeviceConnection *parent);

    void connectToService();
    void disconnectFromService();

    void absoluteMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction);

    bool isRelativeMoveSupported(const OnvifMediaProfile &profile);
    void relativeMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction);

    bool isContinuousMoveSupported(const OnvifMediaProfile &profile);
    void continuousMove(const OnvifMediaProfile &profile, qreal xFraction, qreal yFraction);

    bool isHomeSupported(const OnvifMediaProfile &profile);
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

private:
    class Private;
    Private *const d;
};


#endif // ONVIFPTZSERVICE_H
