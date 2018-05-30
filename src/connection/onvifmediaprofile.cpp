#include "onvifmediaprofile.h"

#include <QDebug>
#include "wsdl_media.h"
#include "wsdl_media2.h"

class OnvifMediaProfile::Private : public QSharedData
{
public:
    Private() :
        fixed(false)
    {;}

    Private(const OnvifSoapMedia::TT__Profile &profile) :
        fixed(profile.fixed()),
        name(profile.name()),
        token(profile.token().value()),
        ptzNodeToken( profile.pTZConfiguration().nodeToken())
    {
        Q_ASSERT(token.size());
        switch (profile.videoEncoderConfiguration().encoding()) {
        case OnvifSoapMedia::TT__VideoEncoding::JPEG:
            videoEncoding = "JPEG";
            break;
        case OnvifSoapMedia::TT__VideoEncoding::H264:
            videoEncoding = "H264";
            break;
        case OnvifSoapMedia::TT__VideoEncoding::MPEG4:
            videoEncoding = "MPEG4";
            break;
        }
    }

    Private(const OnvifSoapMedia2::TR2__MediaProfile &profile) :
        fixed(profile.fixed()),
        name(profile.name()),
        token(profile.token().value()),
        ptzNodeToken( profile.configurations().pTZ().nodeToken()),
        videoEncoding(profile.configurations().videoEncoder().encoding())
    {
        Q_ASSERT(token.size());
    }

    bool fixed;
    QString name;
    QString token;
    QString ptzNodeToken;
    QString videoEncoding;
};

OnvifMediaProfile::OnvifMediaProfile() :
    d(new Private())
{
}

OnvifMediaProfile::OnvifMediaProfile(const OnvifMediaProfile &other) :
    d(other.d)
{
}

OnvifMediaProfile::OnvifMediaProfile(const OnvifSoapMedia::TT__Profile &profile) :
    d(new Private(profile))
{
}

OnvifMediaProfile::OnvifMediaProfile(const OnvifSoapMedia2::TR2__MediaProfile &profile) :
    d(new Private(profile))
{
}

OnvifMediaProfile::~OnvifMediaProfile()
{
}

OnvifMediaProfile &OnvifMediaProfile::operator=(const OnvifMediaProfile &other)
{
    if (this != &other) {
        OnvifMediaProfile copy(other);
        d.swap(copy.d);
    }
    return *this;
}

QString OnvifMediaProfile::name() const
{
    return d->name;
}

void OnvifMediaProfile::setName(const QString &name)
{
    d->name = name;
}

QString OnvifMediaProfile::token() const
{
    return d->token;
}

void OnvifMediaProfile::setToken(const QString &token)
{
    d->token = token;
}

bool OnvifMediaProfile::fixed() const
{
    return d->fixed;
}

void OnvifMediaProfile::setFixed(bool fixed)
{
    d->fixed = fixed;
}

QString OnvifMediaProfile::ptzNodeToken() const
{
    return d->ptzNodeToken;
}

QString OnvifMediaProfile::videoEncoding() const
{
    return d->videoEncoding;
}

QDebug operator<<(QDebug debug, const OnvifMediaProfile &p)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "(Fixed: " << p.fixed()
                    << ", Name: " << p.name()
                    << ", Token: " << p.token() << ')';
    return debug;
}
