
#include "downloadmanager.h"

//DownloadManager::DownloadManager()
//{
//    connect(&manager, SIGNAL(finished(QNetworkReply*)),
//            SLOT(downloadFinished(QNetworkReply*)));
//}

void DownloadManager::execute(const QStringList &files, QString outputFolder)
{
//#if VieSchedppOnline
    outputFolder_ = outputFolder;
    for (const QString &arg : files) {
        QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());
        doDownload(url);
    }
//#endif
}

DownloadManager::DownloadManager()
{
//#if VieSchedppOnline
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
//#endif
}

void DownloadManager::doDownload(const QUrl &url)
{
//#if VieSchedppOnline
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    currentDownloads.append(reply);
//#endif
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    QString filePath = QDir(outputFolder_).filePath(filename);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
//        fprintf(stderr, "Could not open %s for writing: %s\n",
//                qPrintable(filename),
//                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

//#if VieSchedppOnline
bool DownloadManager::isHttpRedirect(QNetworkReply *reply)
{
    int statusCode = 0;
    statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}
//#endif


//#if VieSchedppOnline
void DownloadManager::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
//        fprintf(stderr, "Download of %s failed: %s\n",
//                url.toEncoded().constData(),
//                qPrintable(reply->errorString()));
        successful_ = false;
    } else {
        if (isHttpRedirect(reply)) {
            fputs("Request was redirected.\n", stderr);
        } else {

            QString path = url.path();
            QString filename = QFileInfo(path).fileName();
            if (saveToDisk(filename, reply)) {
//                printf("Download of %s succeeded (saved to %s)\n",
//                       url.toEncoded().constData(), qPrintable(filename));
            }else{
                successful_ = false;
            }
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty()) {
        // all downloads finished
        emit DownloadManager::allDownloadsFinished();
    }
}
//#endif
