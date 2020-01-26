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
#include "onvifmediaprofile.h"

#include <QDebug>
#include "wsdl_media.h"
#include "wsdl_media2.h"

class OnvifMediaProfilePrivate : public QSharedData
{
public:
    OnvifMediaProfilePrivate() {;}

    OnvifMediaProfilePrivate(const OnvifSoapMedia::TT__Profile& profile) :
        fixed(profile.fixed()),
        name(profile.name()),
        token(profile.token().value()),
        ptzNodeToken(profile.pTZConfiguration().nodeToken()),
        resolutionPixels(profile.videoEncoderConfiguration().resolution().height() * profile.videoEncoderConfiguration().resolution().width())
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
            videoEncoding = "MPV4-ES";
            break;
        }
    }

    OnvifMediaProfilePrivate(const OnvifSoapMedia2::TR2__MediaProfile& profile) :
        fixed(profile.fixed()),
        name(profile.name()),
        token(profile.token().value()),
        ptzNodeToken(profile.configurations().pTZ().nodeToken()),
        videoEncoding(profile.configurations().videoEncoder().encoding()),
        resolutionPixels(profile.configurations().videoEncoder().resolution().height() * profile.configurations().videoEncoder().resolution().width())
    {
        Q_ASSERT(token.size());
    }

    bool fixed = false;
    QString name;
    QString token;
    QString ptzNodeToken;
    QString videoEncoding;
    long resolutionPixels = 0;
};

OnvifMediaProfile::OnvifMediaProfile() :
    d(new OnvifMediaProfilePrivate())
{
}

OnvifMediaProfile::OnvifMediaProfile(const OnvifMediaProfile& other) = default;

OnvifMediaProfile::OnvifMediaProfile(const OnvifSoapMedia::TT__Profile& profile) :
    d(new OnvifMediaProfilePrivate(profile))
{
}

OnvifMediaProfile::OnvifMediaProfile(const OnvifSoapMedia2::TR2__MediaProfile& profile) :
    d(new OnvifMediaProfilePrivate(profile))
{
}

OnvifMediaProfile::~OnvifMediaProfile() = default;

OnvifMediaProfile& OnvifMediaProfile::operator= (const OnvifMediaProfile& other)
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

void OnvifMediaProfile::setName(const QString& name)
{
    d->name = name;
}

QString OnvifMediaProfile::token() const
{
    return d->token;
}

void OnvifMediaProfile::setToken(const QString& token)
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

long OnvifMediaProfile::resolutionPixels() const
{
    return d->resolutionPixels;
}

QDebug operator<< (QDebug debug, const OnvifMediaProfile& p)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "(Fixed: " << p.fixed()
                    << ", Name: " << p.name()
                    << ", Token: " << p.token()
                    << ", VideoEncoding: " << p.videoEncoding()
                    << ", ResolutionPixels: " << p.resolutionPixels() << ')';
    return debug;
}
