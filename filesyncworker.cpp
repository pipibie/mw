#include "filesyncworker.hpp"

#include <QThread>
#include <QFileInfo>
#include <QDateTime>
#include <QByteArray>
#include <QJsonArray>
#include <QDir>
#include <QDebug>
#include <QStack>

#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <future>

FileSyncWorker::FileSyncWorker(QObject *parent)
    : buffer_(new char[BUFFER_SIZE])
{
    connect(&manager_, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
}

void FileSyncWorker::doWork(const QString &uri, const QString &etag)
{
    QNetworkRequest request;
    manifest_uri_ = QUrl(uri);
    request.setUrl(manifest_uri_);
    if (etag.length() > 0) {
        request.setRawHeader("If-None-Match", etag.toStdString().c_str());
    }
    manager_.get(request);
}

void FileSyncWorker::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (url == manifest_uri_) {
        if (reply->error()) {
            fprintf(stderr, "Download of %s failed: %s\n",
                    url.toEncoded().constData(),
                    qPrintable(reply->errorString()));
        } else {
            auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            if (statusCode != 304) {
                auto etag = reply->rawHeader(QByteArray{"ETag"});
                qDebug() << statusCode;
                qDebug() << reply->readAll();
            }
        }
    }

    reply->deleteLater();
}

//void FileSyncWorker::doWork(const QString &parameter)
//{
//QString result;
///* ... here is the expensive or blocking operation ... */
////emit resultReady(result);
//}

QJsonObject FileSyncWorker::generateFileInfo(const QString & filename, const QDir & basedir)
{
    QFile file(filename);
    QJsonObject info;
    QFileInfo finfo(file);
    info["name"] = basedir.relativeFilePath(filename);
    info["last_modified"] = finfo.lastModified().toSecsSinceEpoch();

    unsigned int read = 0;

    if (finfo.isFile()) {
        file.open(QFile::ReadOnly);
        read = file.read(&buffer_[0], BUFFER_SIZE);
        info["checksum"] = qChecksum(&buffer_[0], read);
        file.close();
    }
    return info;
}

static QStringList listFileRecursive(QDir dir, QStringList filters) {
    QStringList results;
    QStack<QDir> dirs;
    dirs.push(dir);
    while (!dirs.empty()) {
        QDir cur_dir = dirs.pop();
        for (auto fname : cur_dir.entryList(filters, QDir::Files)) {
            results.append(cur_dir.absoluteFilePath(fname));
        }

        for (auto dirname : cur_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            dirs.push_back(QDir{cur_dir.absoluteFilePath(dirname)});
        }
    }

    return results;
}

QJsonDocument FileSyncWorker::generateManifest(const QString &basedir)
{
    QDir minecraft(basedir);
    QJsonDocument doc;
    QJsonArray arr;
    arr.append(generateFileInfo(minecraft.absoluteFilePath(".minecraft/options.txt"), minecraft));
    arr.append(generateFileInfo(minecraft.absoluteFilePath(".minecraft/servers.dat"), minecraft));

    QDir configDir(minecraft.absoluteFilePath(".minecraft/config/"));
    for (auto fname : listFileRecursive(configDir, QStringList{"*.cfg", "*.json", "*.txt"})) {
        arr.append(generateFileInfo(fname, minecraft));
    }

    QDir modsDir(minecraft.absoluteFilePath(".minecraft/mods/"));
    for (auto fname : listFileRecursive(modsDir, QStringList{"*.jar", "*.zip"})) {
        arr.append(generateFileInfo(fname, minecraft));
    }
    doc.setArray(arr);
    return doc;
}
