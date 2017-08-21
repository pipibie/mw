#include "minecraftwrapper.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MinecraftWrapper w;
    w.show();

    return a.exec();
}
