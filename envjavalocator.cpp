#include "envjavalocator.hpp"

//#if (defined (LINUX) || defined (__linux__))
#include <unistd.h>
//#endif

#include <QFile>

#ifdef _WIN32
    const char WHICH_DELIMITER = ';';
    const char * JAVA_EXECUTABLE = "javaw.exe";
#else
    const char WHICH_DELIMITER = ':';
    const char * JAVA_EXECUTABLE = "java";
#endif

EnvironmentVariableJavaLocator::EnvironmentVariableJavaLocator()
{
}

void EnvironmentVariableJavaLocator::load()
{
    paths_.clear();
    QString pathEnv(getenv("PATH"));
    for (auto path : pathEnv.split(WHICH_DELIMITER)) {
        auto javaPath = path.append("/").append(JAVA_EXECUTABLE);
        if (QFile(javaPath).exists()) paths_.append(javaPath);
    }
}
