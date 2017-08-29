#include "minecraftwrapper.hpp"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QSettings>
#include <QJsonDocument>

#include "filesyncworker.hpp"

int main(int argc, char *argv[])
{
    FileSyncWorker worker;
    worker.doWork("http://localhost/manifest.json", "\"43e0-557e6c8fae259\"");
    if (argc == 2 && strcmp(argv[1], "--generate-manifest") == 0) {
        FileSyncWorker worker;
        auto doc = worker.generateManifest(".");
        QFile manifest("manifest.json");
        manifest.open(QFile::WriteOnly);
        manifest.write(doc.toJson());
        manifest.close();
        return 0;
    }

    QApplication app(argc, argv);

    // load translation file
    QTranslator mwTranslator;
    mwTranslator.load(":/translations/mw_zh");
    app.installTranslator(&mwTranslator);

    MinecraftWrapper w;
    w.show();

    return app.exec();
}
