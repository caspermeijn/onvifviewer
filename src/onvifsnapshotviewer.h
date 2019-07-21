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
#ifndef ONVIFSNAPSHOTVIEWER_H
#define ONVIFSNAPSHOTVIEWER_H

#include "onvifsnapshotdownloader.h"
#include <QQuickPaintedItem>

class QPainter;

class OnvifSnapshotViewer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(OnvifSnapshotDownloader* downloader READ downloader WRITE setDownloader)
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(qreal aspectRatio READ aspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(bool isSnapShotAvailable READ isSnapShotAvailable NOTIFY isSnapShotAvailableChanged)
public:
    OnvifSnapshotViewer(QQuickItem* parent = Q_NULLPTR);
    virtual ~OnvifSnapshotViewer();

    OnvifSnapshotDownloader* downloader() const;
    void setDownloader(OnvifSnapshotDownloader* downloader);

    int interval() const;
    void setInterval(int interval);

    qreal aspectRatio() const;

    bool isSnapShotAvailable() const;

signals:
    void aspectRatioChanged(const qreal& aspectRatio);
    void isSnapShotAvailableChanged();

protected:
    virtual void paint(QPainter* painter) Q_DECL_OVERRIDE;

private slots:
    void snapshotChanged(const QPixmap& snapshot);

private:
    void updateAspectRatio(const QPixmap& pixmap);
    void updateAspectRatio(const qreal& aspectRatio);

private:
    QPointer<OnvifSnapshotDownloader> m_downloader;
    int m_interval = 1000;
    qreal m_aspectRatio = 2;
};

#endif // ONVIFSNAPSHOTVIEWER_H
