#ifndef LINUXJAVALOCATOR_HPP
#define LINUXJAVALOCATOR_HPP

#include "javalocator.hpp"

class EnvironmentVariableJavaLocator : public JavaLocator
{
public:
    EnvironmentVariableJavaLocator();
    void load();
};

#endif // LINUXJAVALOCATOR_HPP
