#include "javalocator.hpp"

#include <QProcess>
#include <QDebug>

JavaLocator::JavaLocator()
{
}

JavaLocator::~JavaLocator()
{
}

QStringList::const_iterator JavaLocator::begin()
{
    return paths_.cbegin();
}

QStringList::const_iterator JavaLocator::end()
{
    return paths_.cend();
}

JavaLocator::JAVA_STATE JavaLocator::check_java(const QString &java_path)
{
    QProcess *process = new QProcess();
    process->start(java_path, QStringList{"-version"});
    if (!process->waitForFinished(1000)) return INVALID;
    auto output = process->readAllStandardError();
    auto ret = output.contains("64-Bit");
    delete process;
    return ret ? NONE : WORD_SIZE;
}
