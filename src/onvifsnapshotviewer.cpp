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
#include "onvifsnapshotviewer.h"

#include <QPainter>
#include <QPixmap>

OnvifSnapshotViewer::OnvifSnapshotViewer(QQuickItem* parent) :
    QQuickPaintedItem(parent)
{
}

OnvifSnapshotViewer::~OnvifSnapshotViewer()
{
    if (m_downloader) {
        m_downloader->setInterval(this, -1);
    }
}

void OnvifSnapshotViewer::paint(QPainter* painter)
{
    if (m_downloader) {
        const QPixmap& snapshot = m_downloader->snapshot();
        int heightPixmap = (int)(width() / aspectRatio());
        int y = ((int)height() - heightPixmap) / 2;
        painter->drawPixmap(0, y, (int)width(), heightPixmap, snapshot);
    }
}

void OnvifSnapshotViewer::snapshotChanged(const QPixmap& snapshot)
{
    update();
    updateAspectRatio(snapshot);
    emit isSnapShotAvailableChanged();
}

qreal OnvifSnapshotViewer::aspectRatio() const
{
    return m_aspectRatio;
}

void OnvifSnapshotViewer::updateAspectRatio(const QPixmap& pixmap)
{
    qreal height = pixmap.height();
    qreal width = pixmap.width();
    updateAspectRatio(width / height);
}

void OnvifSnapshotViewer::updateAspectRatio(const qreal& aspectRatio)
{
    if (m_aspectRatio != aspectRatio) {
        m_aspectRatio = aspectRatio;
        emit aspectRatioChanged(m_aspectRatio);
    }
}

bool OnvifSnapshotViewer::isSnapShotAvailable() const
{
    return !m_downloader->snapshot().isNull();
}

int OnvifSnapshotViewer::interval() const
{
    return m_interval;
}

void OnvifSnapshotViewer::setInterval(int interval)
{
    m_interval = interval;
    if (m_downloader) {
        m_downloader->setInterval(this, m_interval);
    }
}

OnvifSnapshotDownloader* OnvifSnapshotViewer::downloader() const
{
    return m_downloader;
}

void OnvifSnapshotViewer::setDownloader(OnvifSnapshotDownloader* downloader)
{
    if (m_downloader) {
        m_downloader->setInterval(this, -1);
        disconnect(m_downloader, nullptr, this, nullptr);
    }
    m_downloader = downloader;
    if (m_downloader) {
        if (!m_downloader->snapshot().isNull()) {
            snapshotChanged(m_downloader->snapshot());
        }
        connect(m_downloader, &OnvifSnapshotDownloader::snapshotChanged, this, &OnvifSnapshotViewer::snapshotChanged);
        m_downloader->setInterval(this, m_interval);
    }
}
