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
    enum JAVA_STATE {
        NONE = 0,
        INVALID = 1,
        WORD_SIZE = 2
    };
    static JAVA_STATE check_java(const QString & java_path);
protected:
    QStringList paths_;
};

#endif // JAVALOCATOR_HPP
