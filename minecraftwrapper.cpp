#include "minecraftwrapper.hpp"
#include "ui_minecraftwrapper.h"

#include <qevent.h>
#include <QSharedPointer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

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

    // ui->usernameEdit->setProperty("hasText", true);

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

    QJsonParseError error;
    QFile modFile(":/config/mods.json");
    modFile.open(QFile::ReadOnly);
    QJsonDocument document = QJsonDocument::fromJson(modFile.readAll(), &error);
    if (!document.isNull() && document.isArray()) {
        for (auto mod : document.array()) {
            if (mod.isObject()) {
                auto modObj = mod.toObject();
                if (modObj.contains("name") && modObj.contains("path")) {
                    QCheckBox *checkbox = new QCheckBox(modObj["name"].toString(), this);
                    ui->toggleContainer->addWidget(checkbox);
                    auto switcher = QSharedPointer<ModSwitch>(new ModSwitch(modObj["path"].toString()));

                    checkbox->setChecked(switcher->enabled());

                    connect(checkbox, &QCheckBox::clicked, [switcher](bool value) {
                        if (switcher->enabled() != value) {
                            switcher->toggle();
                        }
                    });
                }
            }
        }
    }

    connect(ui->settingButton, SIGNAL(clicked(bool)), this, SLOT(toggleSettingPanel()));
    connect(ui->useXaerosRadioButton, SIGNAL(toggled(bool)), this, SLOT(toggleXaerosMap(bool)));
    //ui->usernameEdit->setProperty("hasText", true);

    for (auto edit : {ui->usernameEdit, ui->javaEdit, ui->memoryEdit}) {
        edit->setStyleSheet(edit->text().length() > 0 ? "color: #666666; border: 2px solid #666666; border-top: none; border-right: none; border-left: none; " : "");
        connect(edit, &QLineEdit::textEdited, [edit](const QString & text) {
            edit->setStyleSheet(text.length() > 0 ? "color: #666666; border: 2px solid #666666; border-top: none; border-right: none; border-left: none; " : "");
        });
    }


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
    ui->retranslateUi(this);
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
