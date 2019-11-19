
#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H


#include <QtCore>
#include <QDateTime>
#include <cstdio>

#include <QLabel>

//#if VieSchedppOnline
#include <QtNetwork>
//#endif // VieSchedppOnline


class DownloadManager: public QObject
{
    Q_OBJECT

//#if VieSchedppOnline
    QNetworkAccessManager manager;
    QVector<QNetworkReply *> currentDownloads;
//#endif // VieSchedppOnline

public:
    DownloadManager();

    void doDownload(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);

//#if VieSchedppOnline
    static bool isHttpRedirect(QNetworkReply *reply);
//#endif // VieSchedppOnline

    bool successful() { return successful_; }

public slots:
    void execute(const QStringList &files, QString outputFolder, QLabel *statusBarText = nullptr);

//#if VieSchedppOnline
    void downloadFinished(QNetworkReply *reply);
//#endif // VieSchedppOnline

    void startNextDownload();

    QString getErrorText(){ return errorText; }

signals:
    void allDownloadsFinished();
    void masterDownloadsFinished();


private:
    QString outputFolder_;
    bool successful_ = true;
    bool master = true;
    QString errorText;

    QStringList files_;
    QLabel *statusBarText_;
};



#endif // DOWNLOADMANAGER_H

