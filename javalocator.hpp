#ifndef JAVALOCATOR_HPP
#define JAVALOCATOR_HPP

#include <QStringList>

class JavaLocator
{
public:
    JavaLocator();
    virtual ~JavaLocator();
    QStringList::const_iterator begin();
    QStringList::const_iterator end();
    virtual void load() = 0;
protected:
    QStringList paths_;
};

#endif // JAVALOCATOR_HPP
