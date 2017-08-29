#ifndef FILESYNCWORKER_HPP
#define FILESYNCWORKER_HPP

#include <array>

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QScopedPointer>
#include <QDir>

class FileSyncWorker : public QObject
{
    Q_OBJECT
public:
    explicit FileSyncWorker(QObject *parent = 0);
signals:
    void resultReady(float percentage);

public slots:
    void doWork(const QString &parameter);
public:
    QJsonObject generateFileInfo(const QString & filename, const QDir & basedir);
    QJsonDocument generateManifest(const QString & basedir);
private:
    static const int BUFFER_SIZE = 1024 * 1024 * 10;
    QScopedArrayPointer<char> buffer_;
};

#endif // FILESYNCWORKER_HPP
