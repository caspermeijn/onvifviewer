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
#ifndef ONVIFSNAPSHOTDOWNLOADER_H
#define ONVIFSNAPSHOTDOWNLOADER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QObject>
#include <QPixmap>
#include <QPointer>

class OnvifSnapshotDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPixmap snapshot READ snapshot NOTIFY snapshotChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
public:
    explicit OnvifSnapshotDownloader(QObject* parent = nullptr);

    QPixmap snapshot() const;
    QString error() const;

    void setInterval(QObject* key, int interval);

signals:
    void snapshotChanged(const QPixmap& snapshot);
    void errorChanged(const QString& error);

public slots:
    void setSnapshotUri(const QUrl& snapshotUri);

private slots:
    void startDownload();
    void networkRequestFinished(QNetworkReply* reply);

private:
    void setError(const QString& error);

private:
    QUrl m_snapshotUri;
    QTimer m_downloadTimer;
    QNetworkAccessManager m_networkAccessManager;
    QPointer<QNetworkReply> m_networkReply;
    QPixmap m_snapshot;
    QString m_error;
    QMap<QObject*, int> m_intervalMap;
};

#endif // ONVIFSNAPSHOTDOWNLOADER_H
