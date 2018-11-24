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
#ifndef ONVIFMEDIAPROFILE_H
#define ONVIFMEDIAPROFILE_H

#include "onvifconnect_export.h"
#include <QSharedDataPointer>

namespace OnvifSoapMedia {
class TT__Profile;
}
namespace OnvifSoapMedia2 {
class TR2__MediaProfile;
}

class ONVIFCONNECT_EXPORT OnvifMediaProfile
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
    long resolutionPixels() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};
QDebug operator<<(QDebug debug, const OnvifMediaProfile &d);

#endif // ONVIFMEDIAPROFILE_H
