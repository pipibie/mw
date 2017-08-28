#include "filesyncworker.hpp"

#include <QThread>
#include <QFileInfo>
#include <QDateTime>
#include <QByteArray>
#include <QDebug>

FileSyncWorker::FileSyncWorker(QObject *parent)
    : buffer_(new std::array<char, BUFFER_SIZE>())
{
}

void FileSyncWorker::doWork(const QString &parameter)
{
QString result;
/* ... here is the expensive or blocking operation ... */
//emit resultReady(result);
}

QJsonObject FileSyncWorker::generateFileInfo(const QString & filename)
{
    QFile file(filename);
    QJsonObject info;
    QFileInfo finfo(file);
    info["name"] = finfo.absoluteFilePath();
    info["last_modified"] = finfo.lastModified().toSecsSinceEpoch();

    unsigned int read = 0;

    if (finfo.isFile()) {
        file.open(QFile::ReadOnly);
        read = file.read(buffer_->begin(), buffer_->size());
        info["checksum"] = qChecksum(buffer_->begin(), read);
        file.close();
    }
    return info;
}

QJsonDocument FileSyncWorker::generateManifest(const QString &basedir)
{
    QDir modDir("../mc/.minecraft/mods/");
    for (auto fname : modDir.entryList(QStringList{"*.jar"}, QDir::Files)) {
        qDebug() << worker.generateFileInfo(modDir.absoluteFilePath(fname));
    }
}
