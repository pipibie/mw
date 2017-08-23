#include "minecraftwrapper.hpp"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

#include "mcconfig.hpp"
#include "modswitch.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator mwTranslator;
    mwTranslator.load("mw_" + QLocale::system().name());
    app.installTranslator(&mwTranslator);


    MinecraftWrapper w;
    w.show();

    return app.exec();
}
