#ifndef MINECRAFTWRAPPER_HPP
#define MINECRAFTWRAPPER_HPP

#include <QMainWindow>
#include <QList>
#include <QPair>
#include <QCheckBox>

#include "modswitch.hpp"

namespace Ui {
class MinecraftWrapper;
}

class MinecraftWrapper : public QMainWindow
{
    Q_OBJECT

public:
    explicit MinecraftWrapper(QWidget *parent = 0);
    ~MinecraftWrapper();

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void toggleXaerosMap(bool checked);
    void toggleSettingPanel();
private:
    Ui::MinecraftWrapper *ui;
    QPoint dragPosition;

protected:
    ModSwitch journey_map_switch, xaeros_map_switch;
};

#endif // MINECRAFTWRAPPER_HPP
