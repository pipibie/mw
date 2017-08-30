#include "filesyncworker.hpp"

#include <QThread>
#include <QFileInfo>
#include <QDateTime>
#include <QByteArray>
#include <QJsonArray>
#include <QDir>
#include <QDebug>
#include <QStack>
#include <QMap>

#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "mcconfig.hpp"

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
            emit downloadError(reply->errorString());
        } else {
            auto statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            if (statusCode != 304) {
                auto etag = reply->rawHeader(QByteArray{"ETag"});
                QFile manifest(MCConfig::base_dir.absoluteFilePath("manifest.json"));
                manifest.open(QFile::WriteOnly);
                manifest.write(reply->readAll());
                manifest.close();
                emit manifestReady(false);
                checkLocalFiles();
            } else {
                emit manifestReady(true);
                checkLocalFiles();
            }
        }
    } else if (task_id_ < task_list_.size()) {
        auto url_string = url.toString();
        auto task_name = task_list_[task_id_];
        if (url_string.contains(task_name)) {
            if (reply->error()) {
                emit downloadError(reply->errorString());
                goto cleanup;
            }
            QFileInfo finfo(MCConfig::base_dir.absoluteFilePath(task_name));
            QDir dir = finfo.dir();
            if (!dir.exists()) {
                if (!dir.mkdir(dir.absolutePath())) {
                    emit downloadError("Failed to create directory: " + dir.absolutePath());
                    goto cleanup;
                }
            }

            QFile f(finfo.absoluteFilePath());
            if (!f.open(QFile::WriteOnly)) {
                emit downloadError("Failed to write file: " + finfo.absoluteFilePath());
                goto cleanup;
            }
            f.write(reply->readAll());
            f.close();
        }

    cleanup:
        task_id_++;
        upgradeNextFile();
    }
    reply->deleteLater();
}

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

void FileSyncWorker::checkLocalFiles()
{
    task_id_ = 0;
    QFile manifest(MCConfig::base_dir.absoluteFilePath("manifest.json"));
    manifest.open(QFile::ReadOnly);
    auto remote = QJsonDocument::fromJson(manifest.readAll()).array();
    manifest.close();
    auto local = generateManifest(MCConfig::base_dir.absolutePath()).array();

    QMap<QString, QJsonObject> localInfo;
    for (auto obj : local) {
        localInfo[obj.toObject()["name"].toString()] = obj.toObject();
    }

    for (auto remoteFile : remote) {
        if (remoteFile.toObject()["checksum"] != localInfo[remoteFile.toObject()["name"].toString()]["checksum"]) {
            task_list_.append(remoteFile.toObject()["name"].toString());
        }
    }

    upgradeNextFile();
}

void FileSyncWorker::upgradeNextFile()
{
    emit upgradeReady(static_cast<double>(task_id_) / task_list_.size());
    if (task_id_ < task_list_.size()) {
        QNetworkRequest request;
        auto uri = QUrl("https://www.moem.cc/mcUA/" + task_list_[task_id_]);
        request.setUrl(uri);
        manager_.get(request);
    }
}
