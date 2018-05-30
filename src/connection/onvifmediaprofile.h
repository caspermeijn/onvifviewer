#ifndef ONVIFMEDIAPROFILE_H
#define ONVIFMEDIAPROFILE_H

#include <QSharedDataPointer>

namespace OnvifSoapMedia {
class TT__Profile;
}
namespace OnvifSoapMedia2 {
class TR2__MediaProfile;
}

class OnvifMediaProfile
{
public:
    OnvifMediaProfile();
    OnvifMediaProfile( const OnvifMediaProfile &other );
    OnvifMediaProfile(const OnvifSoapMedia::TT__Profile& profile);
    OnvifMediaProfile(const OnvifSoapMedia2::TR2__MediaProfile& profile);
    ~OnvifMediaProfile();

    OnvifMediaProfile &operator=(const OnvifMediaProfile &other);

    QString name() const;
    void setName(const QString &name);

    QString token() const;
    void setToken(const QString &token);

    bool fixed() const;
    void setFixed(bool fixed);

    QString ptzNodeToken() const;
    QString videoEncoding() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};
QDebug operator<<(QDebug debug, const OnvifMediaProfile &d);

#endif // ONVIFMEDIAPROFILE_H
