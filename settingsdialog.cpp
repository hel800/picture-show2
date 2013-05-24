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
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->groupBox_news->hide();
    ui->pushButton_load->setFocus();
    ui->label_folderImage->setVisible(false);
    ui->line_dropbox->setVisible(false);
    ui->label_collection->setVisible(false);

    ui->label_12->setVisible(false);
    ui->frame_4->setVisible(false);

    this->resizeDialog();
    this->setAcceptDrops(true);

    this->m_dirListReader = new readDirList();
    connect(this->m_dirListReader, SIGNAL(listready()), this, SLOT(readDirListReady()));
    connect(this->m_dirListReader, SIGNAL(finished()), this, SLOT(readDirListCanceled()));
    this->m_dropListChanged = false;

    this->m_helpWindow = new HelpWindow(this);
    this->m_helpWindow->setModal(true);

    this->m_networkManager = new QNetworkAccessManager();
    connect(this->m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkReplyReady(QNetworkReply*)));

    ui->comboBox_directoryPath->setEditable(true);
    if (ui->comboBox_effect->currentIndex() == 0)
        ui->comboBox_fadeTime->setEnabled(false);

    this->m_droppedItemsList = new QSet<QString>();

    this->languageChangeSignalOff = true;
    this->loadSettings();
    this->languageChangeSignalOff = false;

    if (ui->checkBox_updateNotification->isChecked())
        this->m_networkManager->get(QNetworkRequest(QUrl("http://code.google.com/p/picture-show/")));

    ui->comboBox_directoryPath->setCurrentIndex(0);

    // QSettings configuration for OS
#if defined(Q_OS_WIN32)
    m_qSet_format = QSettings::IniFormat;
#else
    m_qSet_format = QSettings::NativeFormat;
#endif

#if defined(Q_OS_WIN32)
    m_qSet_scope = QSettings::SystemScope;
#else
    m_qSet_scope = QSettings::UserScope;
#endif

    m_qSet_organization = "bsSoft";
    m_qSet_application = "picture-show2";
}

SettingsDialog::~SettingsDialog()
{
    this->saveSettings();
    delete ui;

    delete this->m_networkManager;
    delete this->m_droppedItemsList;
}

void SettingsDialog::setOnTopHint(bool state)
{
    if (state)
        this->setWindowFlags( this->windowFlags() | Qt::WindowStaysOnTopHint);
    else
        this->setWindowFlags( this->windowFlags() & ~Qt::WindowStaysOnTopHint);
}

SettingsDialog::OpenMode SettingsDialog::getOpenMode()
{
    if (ui->tabWidget_open->currentIndex() == 1)
        return MODE_DROPLIST;
    else
        return MODE_FOLDER;
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

QVariant SettingsDialog::getBackgroundColorQml()
{
    return QVariant(ui->comboBox_bgColor->currentIndex());
}

void SettingsDialog::showHelpDialog()
{
    this->on_pushButton_help_clicked();
}

void SettingsDialog::resizeDialog()
{
    this->adjustSize();
}

void SettingsDialog::dragEnterEvent( QDragEnterEvent *event )
{
    event->acceptProposedAction();
}

void SettingsDialog::dragMoveEvent( QDragMoveEvent *event )
{
    event->acceptProposedAction();
}

void SettingsDialog::dragLeaveEvent( QDragLeaveEvent *event )
{
    event->accept();
}

void SettingsDialog::dropEvent( QDropEvent * event )
{
    QList<QUrl> url_list = event->mimeData()->urls();
    ui->tabWidget_open->setCurrentIndex(1);

    if (this->m_dirListReader->isRunning())
    {
        this->m_cachedDropList = url_list;
        this->m_dirListReader->cancel();
        return;
    }



    // empty
    if (url_list.size() == 0)
    {
        return;
    }
    else
    {
        QList<QUrl> dupplicates_removed = url_list.toSet().subtract(this->m_current_collection).toList();

        ui->label_collection->setText(tr("In Drop Zone: "));
        ui->label_droppingInstruction->setText(tr("Bitte warten..."));
        ui->pushButton_saveColl->setEnabled(false);
        this->m_dirListReader->setUrlList(dupplicates_removed);
        this->m_dirListReader->start(QThread::NormalPriority);
    }
}

void SettingsDialog::hideEvent(QHideEvent * event)
{
    if (this->m_dirListReader->isRunning())
    {
        this->m_dirListReader->cancel();
        this->m_cachedDropList.clear();
    }

    if (this->getOpenMode() == MODE_FOLDER)
        m_dropListChanged = true;

    event->accept();
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

void SettingsDialog::readDirListReady()
{
    int num_elements_before = this->m_droppedItemsList->size();
    QList<QFileInfo> newItems = this->m_dirListReader->getItemList();

    foreach (QFileInfo info, newItems)
        this->m_droppedItemsList->insert(info.absoluteFilePath());

    int num_elements_after = this->m_droppedItemsList->size();

    int num_elements = num_elements_after;

    if (num_elements > 0)
    {
        this->m_current_collection.unite(this->m_dirListReader->getUrlList().toSet());

        int num_addedElements = num_elements_after - num_elements_before;

        int num_folders = 0;
        foreach (QUrl url, this->m_current_collection)
        {
            QFileInfo folder(url.toLocalFile());
            if (folder.exists() && folder.isDir())
                num_folders++;
        }

        if (num_addedElements == 0)
            ui->label_droppingInstruction->setText(tr("keine neuen Bilder hinzugefügt"));
        else if (num_addedElements == 1)
            ui->label_droppingInstruction->setText(tr("1 Bild hinzugefügt"));
        else
            ui->label_droppingInstruction->setText(tr("%1 Bilder hinzugefügt").arg(num_addedElements));

        ui->label_foldersDropped->setText(tr("%1 Ordner").arg(num_folders));
        ui->label_imagesDropped->setText(tr("%1 Bilder").arg(num_elements));
        ui->label_folderImage->setVisible(true);
        ui->label_collection->setVisible(true);
        ui->line_dropbox->setVisible(true);
        ui->pushButton_clearZone->setEnabled(true);
        ui->pushButton_saveColl->setEnabled(true);

        if (num_addedElements != 0)
            this->m_dropListChanged = true;
    }
    else
    {
        ui->label_droppingInstruction->setText(tr("keine Bilder gefunden"));
    }
}

void SettingsDialog::readDirListCanceled()
{
    if (!this->m_dirListReader->success && this->m_cachedDropList.size() > 0 && !this->m_dirListReader->isRunning())
    {
        ui->label_droppingInstruction->setText(tr("Bitte warten..."));

        QList<QUrl> dupplicates_removed = this->m_cachedDropList.toSet().subtract(this->m_current_collection).toList();

        this->m_dirListReader->setUrlList(dupplicates_removed);
        this->m_dirListReader->start(QThread::NormalPriority);
        this->m_cachedDropList.clear();
    }
    else if (!this->m_dirListReader->success)
    {
        if (this->m_droppedItemsList->size() > 0)
            ui->label_droppingInstruction->setText(tr("Hier weitere Bilder und/oder Ordner ablegen..."));
        else
            ui->label_droppingInstruction->setText(tr("Hier Bilder und/oder Ordner ablegen..."));
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

QSet<QString> * SettingsDialog::getDroppedItems()
{
    this->m_dropListChanged = false;
    return this->m_droppedItemsList;
}

bool SettingsDialog::getDropListChanged()
{
    return this->m_dropListChanged;
}

void SettingsDialog::setTimerValue(int value)
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    settings.setValue("automaticTimerValue", QVariant(value));
}

int SettingsDialog::getTimerValue()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    int val = settings.value("automaticTimerValue", QVariant(false)).toInt();

    if (val < 4)
        val = 8;
    if (val > 99)
        val = 99;

    return val;
}

void SettingsDialog::setFullScreen(bool fullscreen)
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    settings.setValue("fullscreenState", QVariant(fullscreen));
}

bool SettingsDialog::getFullScreen()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    return settings.value("fullscreenState", QVariant(false)).toBool();
}

void SettingsDialog::setWindowPosition(QPoint pos)
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    settings.setValue("windowPos", QVariant(pos));
}

QPoint SettingsDialog::getWindowPosition()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    return settings.value("windowPos", QVariant(QPoint(50, 50))).toPoint();
}

void SettingsDialog::setFirstStart(bool state)
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    settings.setValue("firstStart", QVariant(state));
}

bool SettingsDialog::getFirstStart()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    return settings.value("firstStart", QVariant(true)).toBool();
}

void SettingsDialog::setInfoBarActive(bool state)
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    settings.setValue("infobarActive", QVariant(state));
}

bool SettingsDialog::getInfoBarActive()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    return settings.value("infobarActive", QVariant(false)).toBool();
}

size_t SettingsDialog::getMaxCacheSize()
{
    return (size_t)ui->spinBox_cacheSize->value() * 1024 * 1024;
}

void SettingsDialog::setWindowSize(QSize size)
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    settings.setValue("windowSize", QVariant(size));
}

QSize SettingsDialog::getWindowSize()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    return settings.value("windowSize", QVariant(QSize(850, 550))).toSize();
}

void SettingsDialog::updateLanguage()
{
    this->languageChangeSignalOff = true;
    this->saveSettings();
    ui->retranslateUi(this);
    int currentIndex = ui->comboBox_directoryPath->currentIndex();
    this->loadSettings();
    if (currentIndex != -1)
        ui->comboBox_directoryPath->setCurrentIndex(currentIndex);
    this->languageChangeSignalOff = false;
}

void SettingsDialog::updateHistory()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
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

    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
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
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    ui->comboBox_effect->setCurrentIndex(settings.value("effect", QVariant(1)).toInt());
    ui->comboBox_fadeTime->setCurrentIndex(settings.value("fadeTime", QVariant(1)).toInt());
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
    ui->checkBox_mouseControl->setChecked(settings.value("mouseControl", QVariant(true)).toBool());
    ui->checkBox_historySave->setChecked(settings.value("saveHistory", QVariant(true)).toBool());
    ui->checkBox_subdirs->setChecked(settings.value("includeSubdirs", QVariant(false)).toBool());
    ui->checkBox_loop->setChecked(settings.value("loopSlideShow", QVariant(false)).toBool());
    ui->spinBox_cacheSize->setValue(settings.value("maxCacheSize", QVariant(256)).toInt());
    ui->comboBox_loadingType->setCurrentIndex(settings.value("loadingType", QVariant(0)).toInt());
    ui->checkBox_updateNotification->setChecked(settings.value("checkForUpdates", QVariant(true)).toBool());
    ui->comboBox_bgColor->setCurrentIndex(settings.value("bgColor", QVariant(1)).toInt());
    ui->tabWidget_open->setCurrentIndex(settings.value("openTabIndex", QVariant(0)).toInt());
}

void SettingsDialog::saveSettings()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
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
    settings.setValue("bgColor", QVariant(ui->comboBox_bgColor->currentIndex()));
    settings.setValue("openTabIndex", QVariant(ui->tabWidget_open->currentIndex()));
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
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
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

void SettingsDialog::on_comboBox_scaling_currentIndexChanged(int index)
{
    emit propertiesChanged();
}

void SettingsDialog::on_comboBox_bgColor_currentIndexChanged(int index)
{
    emit propertiesChanged();
}

void SettingsDialog::on_comboBox_directoryPath_currentIndexChanged(int index)
{

}

void SettingsDialog::on_pushButton_load_clicked()
{
    if (this->getOpenMode() == MODE_DROPLIST)
    {
        if (this->m_dirListReader->isRunning())
        {
            QMessageBox::warning(this, tr("Abgelegte Ordner werden durchsucht!"), tr("Die abgelegten Ordner und Bilder werden noch durchsucht. Einen Moment bitte noch."));
            return;
        }

        if (this->m_droppedItemsList->size() == 0)
        {
            QMessageBox::warning(this, tr("Keine Bilder gefunden"), tr("Es wurden keine Bilder/Ordner abgelegt, oder keine unterstützten Bilder in den Ordnern gefunden!"));
            return;
        }
    }

    this->accept();
}

void SettingsDialog::on_pushButton_clearZone_clicked()
{
    if (this->m_dirListReader->isRunning())
        this->m_dirListReader->cancel();

    this->m_droppedItemsList->clear();
    this->m_current_collection.clear();

    ui->label_collection->setText(tr("In Drop Zone: "));
    ui->label_folderImage->setVisible(false);
    ui->line_dropbox->setVisible(false);
    ui->label_collection->setVisible(false);
    ui->label_imagesDropped->setText(tr(""));
    ui->label_foldersDropped->setText(tr(""));
    ui->label_droppingInstruction->setText(tr("Hier Bilder und/oder Ordner ablegen..."));
    ui->pushButton_clearZone->setEnabled(false);
    ui->pushButton_saveColl->setEnabled(false);
}

void SettingsDialog::on_pushButton_saveColl_clicked()
{
    Ui_SaveCollectionDialog scd_gen;
    QDialog saveCollDialog(this);
    saveCollDialog.setWindowFlags( saveCollDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint );
    scd_gen.setupUi(&saveCollDialog);
    if (saveCollDialog.exec() == QDialog::Accepted)
    {
        if (scd_gen.lineEdit_collName->text().isEmpty())
        {
            QMessageBox::warning(this, tr("Sammlung braucht einen Namen!"), tr("Es muss ein eindeutiger Name für die Sammlung festgelegt werden!"));
            this->on_pushButton_saveColl_clicked();
            return;
        }

        QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
        QStringList groups = settings.childGroups();
        foreach (QString gr, groups)
        {
            if (gr.section("_", 1) == scd_gen.lineEdit_collName->text())
            {
                QMessageBox::StandardButton btn = QMessageBox::question(this, tr("Sammlung existiert bereits!"), tr("Eine Sammlung mit dem Namen: \"%1\" existiert bereits! Soll die Sammlung überschrieben werden?").arg(scd_gen.lineEdit_collName->text()));

                if (btn == QMessageBox::Yes)
                {
                    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);

                    settings.beginGroup(gr);
                    settings.remove("");
                    settings.endGroup();
                    break;
                }
                else
                {
                    this->on_pushButton_saveColl_clicked();
                    return;
                }

            }
        }

        QStringList converted_list;
        foreach (QUrl url, this->m_current_collection)
            converted_list.append(url.toString());

        int num_folders = 0;
        int num_images = 0;
        foreach (QUrl url, this->m_current_collection)
        {
            QFileInfo folder(url.toLocalFile());
            if (folder.exists() && folder.isDir())
                num_folders++;
            else if (folder.exists() && folder.isFile())
                num_images++;
        }

        settings.beginGroup("collection_" + scd_gen.lineEdit_collName->text());
        settings.setValue("url_list", QVariant(converted_list));
        settings.setValue("num_folders", QVariant(num_folders));
        settings.setValue("num_images", QVariant(num_images));
        settings.endGroup();

        ui->label_collection->setText(tr("Sammlung: %1").arg(scd_gen.lineEdit_collName->text()));
    }
}

void SettingsDialog::on_pushButton_loadColl_clicked()
{
    if (this->m_dirListReader->isRunning())
        this->m_dirListReader->cancel();

    LoadCollectionDialog loadCollDialog(this);
    loadCollDialog.setSettingsOptions(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    loadCollDialog.createCollection();

    // fill list with collections

    if (loadCollDialog.exec() == QDialog::Accepted)
    {
        QString col_name;
        QList<QUrl> collection_list = loadCollDialog.getSelectedCollection(col_name);

        this->m_droppedItemsList->clear();
        this->m_current_collection.clear();

        if (collection_list.size() > 0)
        {
            ui->label_collection->setText(tr("Sammlung: %1").arg(col_name));
            ui->label_droppingInstruction->setText(tr("Bitte warten..."));
            this->m_dirListReader->setUrlList(collection_list);
            this->m_dirListReader->start(QThread::NormalPriority);
        }
        else
        {
            ui->label_droppingInstruction->setText(tr("Sammlung ungültig!"));
            ui->label_folderImage->setVisible(false);
            ui->line_dropbox->setVisible(false);
            ui->label_collection->setVisible(false);
            ui->label_imagesDropped->setText(tr(""));
            ui->label_foldersDropped->setText(tr(""));
            ui->pushButton_clearZone->setEnabled(false);
            ui->pushButton_saveColl->setEnabled(false);
        }
    }
}
