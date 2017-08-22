#include "minecraftwrapper.hpp"
#include "ui_minecraftwrapper.h"

#include <qevent.h>
#include <QSharedPointer>
#include <QDebug>

#include <functional>

#include "mcconfig.hpp"
#include "modswitch.hpp"

MinecraftWrapper::MinecraftWrapper(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MinecraftWrapper),
    journey_map_switch(MCConfig::base_dir.filePath(".minecraft/mods/journeymap-1.7.10-5.1.4p1[旅行者地图].jar")),
    xaeros_map_switch(MCConfig::base_dir.filePath(".minecraft/mods/Xaeros_Minimap_1.12_Forge_1.7.10[地图].jar"))
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    QFile file(":/qss/default.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);

    ui->setupUi(this);

    ui->settingWidget->hide();

    if (xaeros_map_switch.enabled() == journey_map_switch.enabled()) {
        xaeros_map_switch.toggle();
    }

    if (journey_map_switch.enabled()) {
        ui->useXaerosRadioButton->setChecked(false);
        ui->useJourneyRadioButton->setChecked(true);
    } else {
        ui->useXaerosRadioButton->setChecked(true);
        ui->useJourneyRadioButton->setChecked(false);
    }

    QList< QPair<QString, QString> > mods = {
        { "自定义史蒂夫", MCConfig::base_dir.filePath(".minecraft/mods/[自定义史蒂夫]CustomSteve1710-Beta.jar") },
        { "光影", MCConfig::base_dir.filePath(".minecraft/mods/[光影]GLSL-Shaders-Mod-1.7.10.jar") },
        { "内存清理", MCConfig::base_dir.filePath(".minecraft/mods/[内存清理]Memory Cleaner Mod 1.7.10.jar") },
        { "FPS加速", MCConfig::base_dir.filePath(".minecraft/mods/[加速]BetterFps-1.3.2.jar") },
        { "加速", MCConfig::base_dir.filePath(".minecraft/mods/fastcraft-1.25.jar") } // 不太对
    };

    for (auto pair : mods) {
        QCheckBox *checkbox = new QCheckBox(pair.first, this);
        ui->toggleContainer->addWidget(checkbox);
        auto switcher = QSharedPointer<ModSwitch>(new ModSwitch(pair.second));

        checkbox->setChecked(switcher->enabled());

        connect(checkbox, &QCheckBox::clicked, [switcher](bool value) {
            if (switcher->enabled() != value) {
                switcher->toggle();
            }
        });
    }

    connect(ui->settingButton, SIGNAL(clicked(bool)), this, SLOT(toggleSettingPanel()));
    connect(ui->useXaerosRadioButton, SIGNAL(toggled(bool)), this, SLOT(toggleXaerosMap(bool)));
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

void MinecraftWrapper::toggleXaerosMap(bool checked)
{
    if (xaeros_map_switch.enabled() != checked) {
        journey_map_switch.toggle();
        xaeros_map_switch.toggle();
    }
}

void MinecraftWrapper::toggleSettingPanel()
{
    if (ui->settingWidget->isHidden()) {
        ui->settingWidget->show();
    } else {
        ui->settingWidget->hide();
    }
}
