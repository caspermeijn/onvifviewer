#include "onvifmediaprofile.h"

#include <QDebug>
#include "wsdl_media.h"

using namespace OnvifSoapMedia;

class OnvifMediaProfile::Private : public QSharedData
{
public:
    Private() :
        fixed(false)
    {;}

    Private(const TT__Profile &profile) :
        fixed(profile.fixed()),
        name(profile.name()),
        token(profile.token().value())
    {
        Q_ASSERT(token.size());
    }

    bool fixed;
    QString name;
    QString token;
};

OnvifMediaProfile::OnvifMediaProfile() :
    d(new Private())
{
}

OnvifMediaProfile::OnvifMediaProfile(const OnvifMediaProfile &other) :
    d(other.d)
{
}

OnvifMediaProfile::OnvifMediaProfile(const TT__Profile &profile) :
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

QDebug operator<<(QDebug debug, const OnvifMediaProfile &p)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "(Fixed: " << p.fixed()
                    << ", Name: " << p.name()
                    << ", Token: " << p.token() << ')';
    return debug;
}
