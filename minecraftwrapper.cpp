#include "minecraftwrapper.hpp"
#include "ui_minecraftwrapper.h"

#include <qevent.h>
#include <QSharedPointer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QMessageBox>
#include <QProcess>

#include "mcconfig.hpp"
#include "modswitch.hpp"
#include "envjavalocator.hpp"
#include "boxshadoweffect.h"

static unsigned long long parseMemory(const QString &memory)
{
    static QRegExp regex("(\\d+)(.\\d+)?\\b*([kKMmGg]?)([Bb]?)");
    double size = 0;
    if (regex.exactMatch(memory.trimmed())) {
        auto groups = regex.capturedTexts();
        size = (groups[1] + groups[2]).toDouble();
        if (groups[3] == "") {
            size /= 1024 * 1024;
        } else if (groups[3] == "k" || groups[3] == "K") {
            size /= 1024;
        } else if (groups[3] == "G" || groups[3] == "g") {
            size *= 1024;
        }
    }
    return static_cast<unsigned long long>(size);
}

MinecraftWrapper::MinecraftWrapper(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MinecraftWrapper),
    journey_map_switch(MCConfig::base_dir.filePath(".minecraft/mods/journeymap-1.7.10-5.1.4p1[旅行者地图].jar")),
    xaeros_map_switch(MCConfig::base_dir.filePath(".minecraft/mods/Xaeros_Minimap_1.12_Forge_1.7.10[地图].jar")),
    settings("Moem", "Minecraft Wrapper")
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    QFile file(":/qss/default.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);

    ui->setupUi(this);

    // ui->usernameEdit->setProperty("hasText", true);

    ui->mainWidget->setGraphicsEffect(new BoxShadowEffect());
    ui->settingWidget->setGraphicsEffect(new BoxShadowEffect());

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
                    auto switcher = QSharedPointer<ModSwitch>(new ModSwitch(MCConfig::base_dir.filePath(modObj["path"].toString())));

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
    connect(ui->gameStartButton, SIGNAL(clicked(bool)), this, SLOT(startGame()));
    //ui->usernameEdit->setProperty("hasText", true);

    for (auto edit : {ui->usernameEdit, ui->javaEdit, ui->memoryEdit}) {
        edit->setStyleSheet(edit->text().length() > 0 ? "color: #666666; border: 2px solid #666666; border-top: none; border-right: none; border-left: none; " : "");
        connect(edit, &QLineEdit::textEdited, [edit](const QString & text) {
            edit->setStyleSheet(text.length() > 0 ? "color: #666666; border: 2px solid #666666; border-top: none; border-right: none; border-left: none; " : "");
        });
    }

    loadSettings();

    connect(&worker, SIGNAL(manifestReady(bool)), this, SLOT(onManifestReady(bool)));
    connect(&worker, SIGNAL(downloadError(QString)), this, SLOT(onDownloadError(QString)));
    connect(&worker, SIGNAL(upgradeReady(float)), this, SLOT(onUpgradeReady(float)));
    ui->gameStartButton->setEnabled(false);

    worker.doWork("https://www.moem.cc/mcUA/manifest.json", "");
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

void MinecraftWrapper::startGame()
{
    // check memory
    unsigned long long memory = parseMemory(ui->memoryEdit->text());
    if (memory < 2048) {
        showErrorMessage("内存不足(至少为2G)");
        return;
    }

    switch (JavaLocator::check_java(ui->javaEdit->text())) {
    case JavaLocator::INVALID:
        showErrorMessage("<a href=\"http://pan.diemoe.net/d/RVCc53\">JAVA无效，建议卸载原JAVA，并点此下载安装64位JAVA安装</a>");
        return;
    case JavaLocator::WORD_SIZE:
        showErrorMessage("<a href=\"http://pan.diemoe.net/d/RVCc53\">请卸载32位的JAVA，并点此下载安装64位JAVA安装</a>");
        return;
    }

    QRegExp regex("[a-zA-Z0-9]+");
    if (!regex.exactMatch(ui->usernameEdit->text())) {
        showErrorMessage("msg"); // 用户名只允许英文和数字
        return;
    }

    QFile commandFile(":/config/command.txt");
    commandFile.open(QFile::ReadOnly);
    QString command = QString::fromUtf8(commandFile.readAll()).replace("\n", "").replace("\r", "");
    QStringList arguments;
    for (auto argument : command.split(" ")) {
        argument.replace("2048m", QString::asprintf("%dm", memory > 2048 ? memory : 2048));
        argument.replace("PATH_PREFIX", MCConfig::base_dir.absolutePath());
        argument.replace("USERNAME", ui->usernameEdit->text());
        arguments << argument;
    }
    QProcess *process = new QProcess();
    process->start(ui->javaEdit->text(), arguments);
    qDebug() << process->waitForStarted();
    process->waitForReadyRead();
    auto output = process->readAllStandardOutput();
    auto error = process->readAllStandardError();
    qDebug() << output;
    qDebug() << error;
    // TODO: display error message
    if (error.size() == 0 && !output.contains("Crash")) {
        saveSettings();
        close();
    } else {
        showErrorMessage(QString::fromLocal8Bit(error));
    }
}

void MinecraftWrapper::onUpgradeReady(float percentage)
{
    ui->progressBar->setValue(percentage * 100);
    if (percentage > 0.99999) {
        ui->progressBar->setVisible(false);
        ui->updatingLabel->setVisible(false);
        ui->gameStartButton->setEnabled(true);
    }
}

void MinecraftWrapper::onManifestReady(bool is_updated)
{
    if (!is_updated) {
        ui->gameStartButton->setEnabled(false);
        ui->progressBar->setValue(0);
        ui->progressBar->show();
    } else {
        ui->gameStartButton->setEnabled(true);
    }
}

void MinecraftWrapper::onDownloadError(QString error)
{
//    qDebug() << error;
}

void MinecraftWrapper::loadSettings()
{
    ui->usernameEdit->setText(settings.value("mc/username", "").toString());
    ui->memoryEdit->setText(settings.value("mc/memory", "2GB").toString());
    auto javaPath = settings.value("mc/javaPath", "").toString();
    if (!(QFile{javaPath}).exists()) {
        // find java
        EnvironmentVariableJavaLocator locator;
        locator.load();
        if (locator.begin() != locator.end()) javaPath = *locator.begin();
        for (auto path : locator) {
            if (path.contains("1.8")) {
                javaPath = path;
                break;
            }
        }
    }
    ui->javaEdit->setText(javaPath);
}

void MinecraftWrapper::saveSettings()
{
    settings.setValue("mc/username", ui->usernameEdit->text());
    settings.setValue("mc/memory", ui->memoryEdit->text());
    settings.setValue("mc/javaPath", ui->javaEdit->text());
    settings.sync();
}

void MinecraftWrapper::showErrorMessage(const QString &text)
{
    QMessageBox::information(NULL, "启动失败", text, QMessageBox::Yes, QMessageBox::Yes);
}

void MinecraftWrapper::closeEvent(QCloseEvent *event)
{
}
