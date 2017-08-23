#ifndef MINECRAFTWRAPPER_HPP
#define MINECRAFTWRAPPER_HPP

#include <QMainWindow>
#include <QList>
#include <QPair>
#include <QCheckBox>
#include <QSettings>

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
    void startGame();
private:
    void loadSettings();
    void saveSettings();
private:
    Ui::MinecraftWrapper *ui;
    QPoint dragPosition;
    QSettings settings;
protected:
    ModSwitch journey_map_switch;
    ModSwitch xaeros_map_switch;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MINECRAFTWRAPPER_HPP
