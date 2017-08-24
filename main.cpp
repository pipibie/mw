#include "minecraftwrapper.hpp"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // load translation file
    QTranslator mwTranslator;
    mwTranslator.load(":/translations/mw_zh");
    app.installTranslator(&mwTranslator);

    MinecraftWrapper w;
    w.show();

    return app.exec();
}
