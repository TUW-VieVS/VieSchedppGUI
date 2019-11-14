
#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H


#include <QtCore>

//#if VieSchedppOnline
#include <QtNetwork>
//#endif // VieSchedppOnline

#include <QDateTime>

#include <cstdio>

class DownloadManager: public QObject
{
    Q_OBJECT

//#if VieSchedppOnline
    QNetworkAccessManager manager;
    QVector<QNetworkReply *> currentDownloads;
//#endif // VieSchedppOnline

public:
    DownloadManager();

//#if VieSchedppOnline
    void doDownload(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    static bool isHttpRedirect(QNetworkReply *reply);
//#endif // VieSchedppOnline

public slots:
    void execute(const QStringList &files, QString outputFolder);
//#if VieSchedppOnline
    void downloadFinished(QNetworkReply *reply);
//#endif // VieSchedppOnline

signals:
    void allDownloadsFinished();


private:
    QString outputFolder_;
};



#endif // DOWNLOADMANAGER_H

