
#include "downloadmanager.h"

void DownloadManager::execute(const QStringList &files, QString outputFolder, QLabel *statusBarText)
{
//#if VieSchedppOnline
    statusBarText_ = statusBarText;
    outputFolder_ = outputFolder;
    files_ = files;
    startNextDownload();

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

void DownloadManager::startNextDownload(){
    QString file;
    try {
        if(!files_.isEmpty()){
            file = files_.takeAt(0);
            if(statusBarText_ != nullptr){
                statusBarText_->setText(QString("downloading %1...").arg(file));
            }
            QUrl url = QUrl::fromEncoded(file.toLocal8Bit());
            doDownload(url);
        }
    } catch (...) {
        errorFiles << file;
        errorText.append("Error while downloading file! ");
        startNextDownload();
    }
}

void DownloadManager::checkDownloads()
{
    QStringList ls = QStringList() << "AUTO_DOWNLOAD_CATALOGS" << "AUTO_DOWNLOAD_MASTER";
    errorText = "";
    QDateTime now = QDateTime::currentDateTime();

    {
        int year = now.date().year();
        QStringList files;
        files << QString("master%1.txt").arg(year);
        files << QString("master%1-int.txt").arg(year);

        if (now.date().month() >=11){
            files << QString("master%1.txt").arg(year+1);
            files << QString("master%1-int.txt").arg(year+1);
        }

        QDirIterator it("AUTO_DOWNLOAD_MASTER", files);
        while (it.hasNext()) {
            QFileInfo f(it.next());
            if(f.isDir()){
                continue;
            }
            QDateTime lastMod = f.lastModified();
            int sec = lastMod.secsTo(now);
            if( abs(sec) >3600){
                errorText.append(QString("error downloading %1\n").arg(f.fileName()));
            }
        }
    }

    {
        QDirIterator it("AUTO_DOWNLOAD_CATALOGS");
        while (it.hasNext()) {
            QFileInfo f(it.next());
            if(f.isDir()){
                continue;
            }
            if(f.fileName() == "flux.cat.merged"){
                continue;
            }
            QString name = f.fileName();
            QDateTime lastMod = f.lastModified();
            int sec = lastMod.secsTo(now);
            if( abs(sec) >3600){
                errorText.append(QString("error downloading %1\n").arg(f.fileName()));
            }
        }
    }
}

void DownloadManager::doDownload(const QUrl &url)
{
//#if VieSchedppOnline
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    QString txt(reply->errorString());
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

        errorText.append(QString("error while downloading %1\n").arg(url.toString()));
        errorFiles << url.fileName();
    } else {


        if (isHttpRedirect(reply)) {
            fputs("Request was redirected.\n", stderr);
        } else {

            QString path = url.path();
            QString filename = QFileInfo(path).fileName();
            if (saveToDisk(filename, reply)) {
            }else{
                errorText.append(QString("error while saving %1!").arg(url.toString()));
                errorFiles << url.fileName();
                successful_ = false;
            }
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (files_.isEmpty()) {
        // all downloads finished
        if(master){
            emit DownloadManager::masterDownloadsFinished();
            master = false;
        }else{
            emit DownloadManager::allDownloadsFinished();
        }

    }else {
        startNextDownload();
    }
}
//#endif
