#ifndef ONVIFMEDIAPROFILE_H
#define ONVIFMEDIAPROFILE_H

#include <QSharedDataPointer>

namespace OnvifSoapMedia {
class TT__Profile;
}

class OnvifMediaProfile
{
public:
    OnvifMediaProfile();
    OnvifMediaProfile( const OnvifMediaProfile &other );
    OnvifMediaProfile(const OnvifSoapMedia::TT__Profile& profile);
    ~OnvifMediaProfile();

    OnvifMediaProfile &operator=(const OnvifMediaProfile &other);

    QString name() const;
    void setName(const QString &name);

    QString token() const;
    void setToken(const QString &token);

    bool fixed() const;
    void setFixed(bool fixed);

private:
    class Private;
    QSharedDataPointer<Private> d;
};
QDebug operator<<(QDebug debug, const OnvifMediaProfile &d);

#endif // ONVIFMEDIAPROFILE_H
