#include "javalocator.hpp"

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
