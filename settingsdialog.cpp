/* ------------------------------------------------------------------
settingsdialog.cpp is part of picture-show2.
.....................................................................

picture-show is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

picture-show2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with picture-show. If not, see <http://www.gnu.org/licenses/>.

......................................................................

author: Sebastian Schäfer
February 2013

--------------------------------------------------------------------*/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <iostream>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);    
    this->setModal(true);
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    ui->groupBox_news->hide();
    ui->pushButton_load->setFocus();

    this->resizeDialog();

    this->m_helpWindow = new HelpWindow(this);
    this->m_helpWindow->setModal(true);

    this->m_networkManager = new QNetworkAccessManager();
    connect(this->m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkReplyReady(QNetworkReply*)));

    ui->comboBox_directoryPath->setEditable(true);
    if (ui->comboBox_effect->currentIndex() == 0)
        ui->comboBox_fadeTime->setEnabled(false);

    this->languageChangeSignalOff = true;
    this->loadSettings();
    this->languageChangeSignalOff = false;

    if (ui->checkBox_updateNotification->isChecked())
        this->m_networkManager->get(QNetworkRequest(QUrl("http://code.google.com/p/picture-show/")));

    ui->comboBox_directoryPath->setCurrentIndex(0);
}

SettingsDialog::~SettingsDialog()
{
    this->saveSettings();
    delete ui;

    delete this->m_networkManager;
}

QString SettingsDialog::getCurrentDirectory()
{
    return ui->comboBox_directoryPath->lineEdit()->text();
}

void SettingsDialog::setCurrentDirectory(const QString & dir)
{
    ui->comboBox_directoryPath->lineEdit()->setText(dir);
}

double SettingsDialog::getCurrentFadeTime()
{
    if (ui->comboBox_fadeTime->currentIndex() == 0)
        return 0.03; // 33 executions --> 15 ms between shots
    else if (ui->comboBox_fadeTime->currentIndex() == 1)
        return 0.015; // 66 executions --> 15 ms between shots
    else if (ui->comboBox_fadeTime->currentIndex() == 2)
        return 0.0075; // 133 executions --> 15 ms between shots
    else if (ui->comboBox_fadeTime->currentIndex() == 3)
        return 0.005; // 200 executions --> 15 ms between shots
    else
        return 0.01;
}

int SettingsDialog::getCurrentFadeLength()
{
    if (ui->comboBox_fadeTime->currentIndex() == 0)
        return 500;
    else if (ui->comboBox_fadeTime->currentIndex() == 1)
        return 1000;
    else if (ui->comboBox_fadeTime->currentIndex() == 2)
        return 2000;
    else if (ui->comboBox_fadeTime->currentIndex() == 3)
        return 3000;
    else
        return 1000;
}

QVariant SettingsDialog::getCurrentFadeLengthForQml()
{
    return QVariant(this->getCurrentFadeLength());
}

QVariant SettingsDialog::getBlendTypeForQml()
{
    if (ui->comboBox_effect->currentIndex() == 1)
        return QVariant("FADE");
    else if (ui->comboBox_effect->currentIndex() == 2)
        return QVariant("FADE_BLACK");
    else if (ui->comboBox_effect->currentIndex() == 3)
        return QVariant("SLIDE");
    else if (ui->comboBox_effect->currentIndex() == 4)
        return QVariant("TWIST_FADE");
    else if (ui->comboBox_effect->currentIndex() == 5)
        return QVariant("ANDROID_STYLE");
    else
        return QVariant("HARD");
}

QVariant SettingsDialog::getMouseControlForQml()
{
    return QVariant(ui->checkBox_mouseControl->isChecked());
}

QVariant SettingsDialog::getLanguageForQml()
{
    return QVariant(this->getLanguage());
}

void SettingsDialog::resizeDialog()
{
    this->adjustSize();
}

void SettingsDialog::networkReplyReady(QNetworkReply * reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QString dataRetrieved = QString(reply->readAll());

        QRegExp rx("current_version___(.{3,20})___current_version");
        int pos = rx.indexIn(dataRetrieved);

        if (pos != -1)
        {
            QStringList list = rx.capturedTexts();
            if (list.size() > 1)
            {
                QString current_version_text = qApp->applicationVersion();
                QString available_version_text = list.at(1);

                double current_version = 0.0;
                double available_version = 0.0;

                QRegExp versionNr("([0-9]{1,2}.[0-9]{1,3})");

                int posA = versionNr.indexIn(current_version_text);
                QStringList caps = versionNr.capturedTexts();
                if (posA != -1 && caps.size() > 1)
                    current_version = caps.at(1).toDouble();
                else
                    return;

                int posB = versionNr.indexIn(available_version_text);
                QStringList caps2 = versionNr.capturedTexts();
                if (posB != -1 && caps2.size() > 1)
                    available_version = caps2.at(1).toDouble();
                else
                    return;

                if (available_version > current_version)
                {
                    QString updateText = QString("<html><head/><body><p>");
                    updateText += tr("<b>Update verf&uuml;gbar: </b>");
                    updateText += QString("<a href=\"https://code.google.com/p/picture-show/downloads/list\"><span style=\" text-decoration: underline; color:#0000ff;\">");
                    updateText += tr("Version %1").arg(available_version);
                    updateText += QString("</span></a></p></body></html>");

                    ui->label_updateText->setText(updateText);
                    ui->groupBox_news->show();
                }
            }
        }
    }
}

BlendType SettingsDialog::getBlendType()
{
    if (ui->comboBox_effect->currentIndex() == 1)
        return FADE;
    else if (ui->comboBox_effect->currentIndex() == 2)
        return FADE_BLACK;
    else if (ui->comboBox_effect->currentIndex() == 3)
        return SLIDE;
    else if (ui->comboBox_effect->currentIndex() == 4)
        return TWIST_FADE;
    else if (ui->comboBox_effect->currentIndex() == 5)
        return ANDROID_STYLE;
    else
        return HARD;
}

Sorting SettingsDialog::getDirectorySorting()
{
    if (ui->comboBox_sort->currentIndex() == 1)
        return DATE_CREATED;
    else
        return FILENAME;
}

void SettingsDialog::setDirectorySorting(Sorting sort)
{
    if (sort == DATE_CREATED)
        ui->comboBox_sort->setCurrentIndex(1);
    else
        ui->comboBox_sort->setCurrentIndex(0);
}

bool SettingsDialog::getIncludeSubdirs()
{
    return ui->checkBox_subdirs->isChecked();
}

void SettingsDialog::setIncludeSubdirs(bool inc)
{
    if (inc)
        ui->checkBox_subdirs->setCheckState(Qt::Checked);
    else
        ui->checkBox_subdirs->setCheckState(Qt::Unchecked);
}

bool SettingsDialog::getLoopSlideShow()
{
    return ui->checkBox_loop->isChecked();
}

void SettingsDialog::setLoopSlideShow(bool loop)
{
    if (loop)
        ui->checkBox_loop->setCheckState(Qt::Checked);
    else
        ui->checkBox_loop->setCheckState(Qt::Unchecked);
}

ScaleType SettingsDialog::getScaleType()
{
    if (ui->comboBox_scaling->currentIndex() == 0)
        return IMAGE;
    else
        return SCREEN;
}

QString SettingsDialog::getLanguage()
{
    if (ui->comboBox_language->currentIndex() == 0)
        return QString("de");
    else
        return QString("en");
}

bool SettingsDialog::getMouseControl()
{
    return ui->checkBox_mouseControl->isChecked();
}

LoadingType SettingsDialog::getLoadingType()
{
    return LoadingType(ui->comboBox_loadingType->currentIndex());
}

void SettingsDialog::setLoadingType(LoadingType type)
{
    ui->comboBox_loadingType->setCurrentIndex(int(type));
}

void SettingsDialog::setTimerValue(int value)
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    settings.setValue("automaticTimerValue", QVariant(value));
}

int SettingsDialog::getTimerValue()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    int val = settings.value("automaticTimerValue", QVariant(false)).toInt();

    if (val < 4)
        val = 8;
    if (val > 99)
        val = 99;

    return val;
}

void SettingsDialog::setFullScreen(bool fullscreen)
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    settings.setValue("fullscreenState", QVariant(fullscreen));
}

bool SettingsDialog::getFullScreen()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    return settings.value("fullscreenState", QVariant(false)).toBool();
}

void SettingsDialog::setWindowPosition(QPoint pos)
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    settings.setValue("windowPos", QVariant(pos));
}

QPoint SettingsDialog::getWindowPosition()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    return settings.value("windowPos", QVariant(QPoint(50, 50))).toPoint();
}

void SettingsDialog::setFirstStart(bool state)
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    settings.setValue("firstStart", QVariant(state));
}

bool SettingsDialog::getFirstStart()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    return settings.value("firstStart", QVariant(true)).toBool();
}

size_t SettingsDialog::getMaxCacheSize()
{
    return (size_t)ui->spinBox_cacheSize->value() * 1024 * 1024;
}

void SettingsDialog::setWindowSize(QSize size)
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    settings.setValue("windowSize", QVariant(size));
}

QSize SettingsDialog::getWindowSize()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    return settings.value("windowSize", QVariant(QSize(850, 550))).toSize();
}

void SettingsDialog::updateLanguage()
{
    this->languageChangeSignalOff = true;
    this->saveSettings();
    ui->retranslateUi(this);
    int currentIndex = ui->comboBox_directoryPath->currentIndex();
    this->loadSettings();
    ui->comboBox_directoryPath->setCurrentIndex(currentIndex);
    this->languageChangeSignalOff = false;
}

void SettingsDialog::updateHistory()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    QStringList history = settings.value("historyOfDirectories").toStringList();

    ui->comboBox_directoryPath->clear();

    foreach(QString dir, history)
        ui->comboBox_directoryPath->addItem(dir);
}

void SettingsDialog::addDirectoryToHistory(const QString & dir)
{
    if (!ui->checkBox_historySave->isChecked())
        return;

    QString newDir = QString(dir);
    if (dir.endsWith(QDir::separator()))
        newDir = dir.left(dir.length()-1);

    QDir directory(newDir);

    if (!directory.isAbsolute() && !directory.exists())
        return;

    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    QStringList history = settings.value("historyOfDirectories").toStringList();

    history.removeAll(newDir);
    history.prepend(newDir);

    settings.setValue("historyOfDirectories", QVariant(history));

    this->updateHistory();
}

int SettingsDialog::getScaleTypeQml()
{
    if (ui->comboBox_scaling->currentIndex() == 0)
        return 1;
    else
        return 2;
}

void SettingsDialog::on_pushButton_browse_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr(QString::fromLatin1("Öffne Verzeichnis").toStdString().c_str()),
                                                          ui->comboBox_directoryPath->lineEdit()->text(),
                                                          QFileDialog::ShowDirsOnly);


    if (!directory.isEmpty())
        ui->comboBox_directoryPath->lineEdit()->setText(directory);
}

void SettingsDialog::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    ui->comboBox_effect->setCurrentIndex(settings.value("effect", QVariant(1)).toInt());
    ui->comboBox_fadeTime->setCurrentIndex(settings.value("fadeTime", QVariant(2)).toInt());
    ui->comboBox_sort->setCurrentIndex(settings.value("sortOrder", QVariant(0)).toInt());
    ui->comboBox_scaling->setCurrentIndex(settings.value("scaleType", QVariant(0)).toInt());

    int languageID = settings.value("languageID", QVariant(-1)).toInt();
    if (languageID == -1)
    {
        QStringList parts = QLocale::system().name().split("_", QString::SkipEmptyParts);

        qDebug(parts.at(0).toStdString().c_str());
        if (parts.size() > 0 && parts.at(0) == "de")
            languageID = 0;
        else
            languageID = 1;
    }

    QStringList history = settings.value("historyOfDirectories").toStringList();

    ui->comboBox_directoryPath->clear();
    foreach(QString dir, history)
        ui->comboBox_directoryPath->addItem(dir);

    ui->comboBox_language->setCurrentIndex(languageID);
    ui->checkBox_mouseControl->setChecked(settings.value("mouseControl", QVariant(false)).toBool());
    ui->checkBox_historySave->setChecked(settings.value("saveHistory", QVariant(true)).toBool());
    ui->checkBox_subdirs->setChecked(settings.value("includeSubdirs", QVariant(false)).toBool());
    ui->checkBox_loop->setChecked(settings.value("loopSlideShow", QVariant(false)).toBool());
    ui->spinBox_cacheSize->setValue(settings.value("maxCacheSize", QVariant(256)).toInt());
    ui->comboBox_loadingType->setCurrentIndex(settings.value("loadingType", QVariant(0)).toInt());
    ui->checkBox_updateNotification->setChecked(settings.value("checkForUpdates", QVariant(true)).toBool());
}

void SettingsDialog::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    settings.setValue("effect", QVariant(ui->comboBox_effect->currentIndex()));
    settings.setValue("fadeTime", QVariant(ui->comboBox_fadeTime->currentIndex()));
    settings.setValue("sortOrder", QVariant(ui->comboBox_sort->currentIndex()));
    settings.setValue("scaleType", QVariant(ui->comboBox_scaling->currentIndex()));
    settings.setValue("languageID", QVariant(ui->comboBox_language->currentIndex()));
    settings.setValue("mouseControl", QVariant(ui->checkBox_mouseControl->isChecked()));
    settings.setValue("saveHistory", QVariant(ui->checkBox_historySave->isChecked()));
    settings.setValue("includeSubdirs", QVariant(ui->checkBox_subdirs->isChecked()));
    settings.setValue("loopSlideShow", QVariant(ui->checkBox_loop->isChecked()));
    settings.setValue("maxCacheSize", QVariant(ui->spinBox_cacheSize->value()));
    settings.setValue("loadingType", QVariant(ui->comboBox_loadingType->currentIndex()));
    settings.setValue("checkForUpdates", QVariant(ui->checkBox_updateNotification->isChecked()));
}

void SettingsDialog::on_comboBox_language_currentIndexChanged(int index)
{
    if (!this->languageChangeSignalOff)
    {
        QString lang = this->getLanguage();
        emit this->languageChanged(lang);
    }
}

void SettingsDialog::on_pushButton_deleteHistory_clicked()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture-show2");
    QStringList emptyHistory;

    settings.setValue("historyOfDirectories", QVariant(emptyHistory));

    this->updateHistory();
}

void SettingsDialog::on_comboBox_effect_currentIndexChanged(int index)
{
    if (index == 0)
        ui->comboBox_fadeTime->setEnabled(false);
    else
        ui->comboBox_fadeTime->setEnabled(true);
}


void SettingsDialog::on_pushButton_help_clicked()
{
    if (ui->comboBox_language->currentIndex() == 0)
        this->m_helpWindow->setLanguage(GERMAN);
    else
        this->m_helpWindow->setLanguage(ENGLISH);

    this->m_helpWindow->exec();
}
