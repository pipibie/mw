#include "minecraftwrapper.hpp"
#include <QApplication>
#include <QDebug>

#include "mcconfig.hpp"
#include "modswitch.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MinecraftWrapper w;
    w.show();

    return a.exec();
}
