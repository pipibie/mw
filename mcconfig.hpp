#ifndef MCCONFIG_HPP
#define MCCONFIG_HPP

#include <QString>
#include <QDir>


namespace MCConfig {
#ifdef QT_NO_DEBUG
    const QDir base_dir("./");
#else
    const QDir base_dir("../mc/");
#endif
}


#endif // MCCONFIG_HPP
