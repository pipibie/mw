#ifndef MINECRAFTWRAPPER_HPP
#define MINECRAFTWRAPPER_HPP

#include <QMainWindow>

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

private:
    Ui::MinecraftWrapper *ui;
    QPoint dragPosition;
};

#endif // MINECRAFTWRAPPER_HPP
