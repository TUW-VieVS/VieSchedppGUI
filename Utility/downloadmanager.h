#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#if VieSchedppOnline

#include <QtCore>
#include <QtNetwork>
#include <QDateTime>

#include <cstdio>

class DownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;
    QVector<QNetworkReply *> currentDownloads;

public:
    DownloadManager();
    void doDownload(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    static bool isHttpRedirect(QNetworkReply *reply);

public slots:
    void execute(const QStringList &files);
    void downloadFinished(QNetworkReply *reply);
};


#endif // VieSchedppOnline

#endif // DOWNLOADMANAGER_H
