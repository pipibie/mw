#include "modswitch.hpp"

//#include <stdexcept>
//#include <experimental/filesystem>
#include <QException>
#include <QDebug>


//namespace fs = std::experimental::filesystem;

ModSwitch::ModSwitch(const QString & path)
    : enabledPath_(path), disabledPath_(path + ".disabled")
{
}

ModSwitch::~ModSwitch()
{
}

bool ModSwitch::enabled() const
{
    QFile f(enabledPath_);
    return f.exists();
}

bool ModSwitch::toggle()
{
    qDebug() << (QFile {enabledPath_}).fileName();
    if (enabled()) {
        return (QFile {enabledPath_}).rename(disabledPath_);
    } else {
        return (QFile {disabledPath_}).rename(enabledPath_);
    }
}

bool ModSwitch::check() const {
    return (QFile {enabledPath_}).exists() ^ (QFile {disabledPath_}).exists();
}
