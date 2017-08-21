#include "minecraftwrapper.hpp"
#include "ui_minecraftwrapper.h"

#include <qevent.h>

MinecraftWrapper::MinecraftWrapper(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MinecraftWrapper)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    QFile file(":/qss/default.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);

    ui->setupUi(this);
    ui->usernameEdit->setAlignment(Qt::AlignCenter);
    //ui->mainWidget->setStyleSheet("background-color:white; border-radius: 5px;");
    ui->retranslateUi(this);
}

MinecraftWrapper::~MinecraftWrapper()
{
    delete ui;
}

void MinecraftWrapper::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MinecraftWrapper::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}
