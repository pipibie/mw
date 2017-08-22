#ifndef MODSWITCH_H
#define MODSWITCH_H

#include <QFile>

class ModSwitch {
public:
    ModSwitch(const QString & path);
    ~ModSwitch();
    bool enabled() const;
    bool toggle();
    bool check() const;
protected:
    QString enabledPath_, disabledPath_;
    bool enabled_;
};

#endif // MODSWITCH_H
