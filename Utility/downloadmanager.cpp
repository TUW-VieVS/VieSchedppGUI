
#include "downloadmanager.h"

void DownloadManager::execute(const QStringList &files, QString outputFolder, QLabel *statusBarText)
{
//#if VieSchedppOnline
    statusBarText_ = statusBarText;
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
    master = true;
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
        successful_ = false;
        QString txt = QString("error while downloading %1!").arg(url.fileName());

        if(statusBarText_ != nullptr){
            statusBarText_->setText(txt);
        }

    } else {
        QString txt = QString("successfully downloaded %1...").arg(url.fileName());


        if (isHttpRedirect(reply)) {
            fputs("Request was redirected.\n", stderr);
        } else {

            QString path = url.path();
            QString filename = QFileInfo(path).fileName();
            if (saveToDisk(filename, reply)) {
                if(statusBarText_ != nullptr){
                    statusBarText_->setText(txt);
                }
            }else{
                QString txt = QString("error while saving %1!").arg(url.fileName());

                if(statusBarText_ != nullptr){
                    statusBarText_->setText(txt);
                }
                successful_ = false;
            }
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty()) {
        // all downloads finished
        if(master){
            emit DownloadManager::masterDownloadsFinished();
            master = false;
        }else{
            emit DownloadManager::allDownloadsFinished();
        }

    }
}
//#endif
