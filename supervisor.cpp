/* ------------------------------------------------------------------
supervisor.cpp is part of picture-show2.
.....................................................................

picture-show2 is free software: you can redistribute it and/or modify
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

#include "supervisor.h"

Supervisor::Supervisor(QObject *parent) :
    QObject(parent)
{
    m_quickView = new QtQuick2ApplicationViewer();
    m_setDialog = new SettingsDialog();

    if (m_setDialog->getLanguage() != "de")
    {
        if (m_translator.load(QCoreApplication::applicationDirPath() + "/picture-show2_" + m_setDialog->getLanguage(), "."))
            qApp->installTranslator(&m_translator);
    }

    m_setDialog->updateLanguage();

    m_quickView->setMinimumSize(QSize(640, 480));
    QQmlContext * context = m_quickView->engine()->rootContext();
    context->setContextProperty("_settings_dialog", m_setDialog);
    context->setContextProperty("_supervisor", this);

    qmlRegisterType<QTimer>("my.library", 1, 0, "QTimer");

//    m_quickView->engine()->setImportPathList(QStringList());
    m_quickView->engine()->addImportPath("./qml");

    m_quickView->setMainQmlFile(QString("qrc:///qml/main.qml"));
//    m_quickView->setMainQmlFile(QString("./qml/picture-show2/main.qml"));

    // Display error, if QML is not ready
    if (m_quickView->status() == QQuickView::Error || m_quickView->status() == QQuickView::Null)
    {
        QString msg = "";
        foreach (QQmlError err, m_quickView->errors())
        {
            msg += err.toString() + "\n";
        }

        QMessageBox warning(QMessageBox::Critical,
                            tr("Error"),
                            tr("QML Error: %1").arg(msg),
                            QMessageBox::Ok);
        warning.exec();

        m_qml_ready = false;
        return;
    }

    m_qml_ready = true;

    m_quickView->setWindowProps(this->checkValidPosition(m_setDialog->getWindowPosition(), m_setDialog->getWindowSize()), m_setDialog->getWindowSize());
    m_quickView->setTitle("picture show 2");
    m_quickView->showExpanded(m_setDialog->getFullScreen());

//    if (this->isFullscreen())
//        m_setDialog->setOnTopHint(true);

    this->moveOpenDialogToMiddle();

    ///------   ------///
    m_dirLoader = new loadDirectory();
    m_dirLoader->setDirectoryList(&m_current_directory_list);
    m_currentIndex = -1;
    m_imgProvider = new ImageProvider(&m_current_directory_list, &m_currentIndex);
    m_imgProvider->setLoadingPointers(&m_loadingStateCurrent, &m_loadingStateNext, &m_loadingStatePrev, &m_loadingStateJumpto);
    m_imgProvider->setLoadingType(m_setDialog->getLoadingType());
    m_imgProvider->setVirtualScreenSize(m_quickView->screen()->virtualGeometry().size());
    m_quickView->engine()->addImageProvider(QLatin1String("pictures"), m_imgProvider);
    m_blendingsActive = 0;
    m_overlayTransitions = 0;
    m_waitingActive = false;

    m_timerValue = m_setDialog->getTimerValue();
    m_timerInputValue = QString("8");

    m_automaticForward = new QTimer(this);
    m_automaticForwardActive = false;
    connect(m_automaticForward, SIGNAL(timeout()), this, SLOT(nextImagePressed()));

    m_messageTimeout = new QTimer(this);
    m_messageTimeout->setSingleShot(true);
    connect(m_messageTimeout, SIGNAL(timeout()), this, SLOT(hideMessage()));

    m_inputMessageTimeout = new QTimer(this);
    m_inputMessageTimeout->setSingleShot(true);
    connect(m_inputMessageTimeout, SIGNAL(timeout()), this, SLOT(inputModeTimeout()));

    m_inputTimeout = new QTimer(this);
    m_inputTimeout->setSingleShot(true);
    connect(m_inputTimeout, SIGNAL(timeout()), this, SLOT(timerInputValueTimeout()));

    m_mousePressTimer = new QTimer(this);
    m_mousePressTimer->setSingleShot(true);
    connect(m_mousePressTimer, SIGNAL(timeout()), this, SLOT(leftMousePress()));

    m_jumptoPreview = new QTimer(this);
    m_jumptoPreview->setSingleShot(true);
    m_jumptoPreview->setInterval(400);
    connect(m_jumptoPreview, SIGNAL(timeout()), this, SLOT(showJumpToPreview()));

    m_showLoaded = false;
    m_currentlyLoading = false;

    m_cur = 1;
    m_prev = 2;
    m_next = 3;

    m_loadingStateNext = IMAGE_READY;
    m_loadingStateCurrent = IMAGE_READY;
    m_loadingStatePrev = IMAGE_READY;

    m_helpActive = false;
    m_messageActive = false;
    m_infoActive = m_setDialog->getInfoBarActive();

    m_currentlyWaiting = false;
    m_activeInputMode = NO_MODE;
    m_questionMode = QUESTION_NONE;
    m_jumptoPreviewVisible = false;

    m_timerFirstStart = true;

    this->bindings();

    if (!m_setDialog->getFirstStart())
        QTimer::singleShot(900, m_setDialog, SLOT(show()));
    else
        QTimer::singleShot(900, this, SLOT(showHelpOverlay()));

    emit refresh();
}

Supervisor::~Supervisor()
{
    if (!m_qml_ready)
    {
        delete m_setDialog;
        return;
    }

    if (m_quickView->isExpanded())
    {
        m_setDialog->setFullScreen(true);
        m_setDialog->setWindowPosition(m_quickView->getLastWindowPosBeforeFullscreen());
        m_setDialog->setWindowSize(m_quickView->getLastWindowSizeBeforeFullscreen());
    }
    else
    {
        m_setDialog->setFullScreen(false);
        m_setDialog->setWindowPosition(m_quickView->position());
        m_setDialog->setWindowSize(m_quickView->size());
    }

    m_setDialog->setTimerValue(m_timerValue);
    m_setDialog->setInfoBarActive(m_infoActive);

    m_quickView->rootObject()->disconnect();
    this->disconnect();

    delete m_inputMessageTimeout;
    delete m_imgProvider;
    delete m_dirLoader;
    delete m_setDialog;
}

void Supervisor::setView(QtQuick2ApplicationViewer *view)
{
    m_quickView = view;
}

void Supervisor::setDialog(SettingsDialog * dialog)
{
    m_setDialog = dialog;
}

void Supervisor::applyNewOptions()
{
    emit refresh();
    m_imgProvider->setCacheSize(m_setDialog->getMaxCacheSize());
    m_imgProvider->setLoadingType(m_setDialog->getLoadingType());

    if ( (!m_showLoaded) ||
         (m_setDialog->getOpenMode() == SettingsDialog::MODE_FOLDER && m_dirLoader->getDirectory() != m_setDialog->getCurrentDirectory()) ||
         (m_setDialog->getOpenMode() == SettingsDialog::MODE_DROPLIST && m_setDialog->getDropListChanged()) )
    {
        m_wTask.clear();
        this->hideOverlays();

        if (!this->anyBlendingsActive())
            this->startDirectoryLoading();
        else
            this->queueTask(DIR_LOAD);

        return;
    }

    if ( (m_dirLoader->getSorting() != m_setDialog->getDirectorySorting()) ||
         (m_dirLoader->getIncludeSubdirs() != m_setDialog->getIncludeSubdirs()) )
    {
        // MESSAGE!!!
        QMessageBox msgBox(m_setDialog);
        msgBox.setWindowTitle(tr("Frage"));
        msgBox.setIcon(QMessageBox::Question);

        if (m_dirLoader->getIncludeSubdirs() != m_setDialog->getIncludeSubdirs())
        {
            // MESSAGE
            msgBox.setText(tr("Das Bilderverzeichnis wurde geändert. Damit die Bilder aus dem neuen Verzeichnis angezeigt werden, muss eine neue Show geladen werden."));
            msgBox.setInformativeText(tr("\"Ja\" klicken, um neus Verzeichnis zu laden, \"Nein\" um altes Verzeichnis weiter anzuzeigen."));
        }
        else if (m_dirLoader->getSorting() != m_setDialog->getDirectorySorting())
        {
            // MESSAGE
            msgBox.setText(tr("Der Sortierungsmodus wurde geändert. Damit die neue Sortierung angewendet wird, muss die Show neu geladen werden. Sie startet neu von Beginn."));
            msgBox.setInformativeText(tr("\"Ja\" klicken, zum neustarten, \"Nein\" um alte Sortierung weiter zu verwenden."));
        }
        else
        {
            // MESSAGE
            msgBox.setText(tr("Einstellungen wurden geändert. Damit sie wirksam werden, muss die Show neu geladen werden. Sie startet neu von Beginn."));
            msgBox.setInformativeText(tr("\"Ja\" klicken, zum neustarten, \"Nein\" um die alte Einstellung weiter zu verwenden."));
        }

        QPushButton *yesButton = msgBox.addButton(tr("Ja"), QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton(tr("Nein"), QMessageBox::NoRole);
        msgBox.setDefaultButton(noButton);

        msgBox.exec();

        if (msgBox.clickedButton() == dynamic_cast<QAbstractButton*>(yesButton))
        {
            m_wTask.clear();
            this->hideOverlays();

            if (!this->anyBlendingsActive())
                this->startDirectoryLoading();
            else
                this->queueTask(DIR_LOAD);

            return;
        }
        else
        {
            m_setDialog->setDirectorySorting(m_dirLoader->getSorting());
            m_setDialog->setCurrentDirectory(m_dirLoader->getDirectory());
            m_setDialog->setIncludeSubdirs(m_dirLoader->getIncludeSubdirs());
        }
    }
}

void Supervisor::startDirectoryLoading(bool forceLargeData)
{
    if (m_currentlyLoading)
        return;

    m_currentlyLoading = true;

    if (m_setDialog->getOpenMode() == SettingsDialog::MODE_FOLDER)
    {
        m_dirLoader->setDirectory(m_setDialog->getCurrentDirectory());
        m_dirLoader->setOpenModeToFolder();
    }
    else
    {
        m_dirLoader->setDirectory("");
        m_dirLoader->setDropList(m_setDialog->getDroppedItems());
        m_dirLoader->setOpenModeToDropList();
    }

    m_dirLoader->setSorting(m_setDialog->getDirectorySorting());
    m_dirLoader->setIncludeSubdirs(m_setDialog->getIncludeSubdirs());
    if (forceLargeData)
        m_dirLoader->setForceLargeData(true);
    else
        m_dirLoader->setForceLargeData(false);
    m_dirLoader->start(QThread::NormalPriority);

    m_imgProvider->deleteCache();
    m_wTask.clear();

    if (m_showLoaded && !this->anyBlendingsActive())
    {
        m_blendingsActive++;
        emit blendOutShow(m_cur);
        m_showLoaded = false;

        if (m_infoActive)
        {
            m_overlayTransitions++;
            emit infoBox();
            m_infoActive = false;
        }
    }

    m_waitingActive = true;
    emit waiting(QVariant(true));

    this->hideOverlays();
}

void Supervisor::directoryLoadingFinished(bool success)
{
    m_currentlyLoading = false;

    if (success && m_current_directory_list.size() > 0)
    {
        if (m_showLoaded && !this->anyBlendingsActive())
        {
            m_blendingsActive++;
            emit blendOutShow(m_cur);
            m_showLoaded = false;

            if (m_infoActive)
            {
                m_overlayTransitions++;
                emit infoBox();
                m_infoActive = false;
            }
        }

        if (m_setDialog->getOpenMode() == SettingsDialog::MODE_FOLDER)
            m_setDialog->addDirectoryToHistory(m_dirLoader->getDirectory());

        m_currentIndex = 0;
        int tmp_cur = m_cur;
        m_cur = this->giveFreeSlot(m_cur, m_prev);
        m_next = this->giveFreeSlot(m_cur, tmp_cur);
        m_loadingStateCurrent = IMAGE_LOADING;
        emit loadImage(QVariant("current"), QVariant(m_cur));

        m_loadingStateNext = IMAGE_LOADING;
        emit loadImage(QVariant("next"), QVariant(m_next));

        this->queueTask(START_TASK);
    }
    else
    {
        emit waiting(QVariant(false));

        if (m_dirLoader->getErrorMsg() == "LARGE_DATA")
        {
            m_questionMode = QUESTION_LARGE_DATA;
            this->showCustomQuestion(QString("qrc:///img/message_question.png"),
                                     tr("Viele Daten"),
                                     tr("Das Laden kann einige Zeit in Anspruch nehmen. Soll fortgesetzt werden?"),
                                     tr("Ja"), tr("Nein"));
        }
        else if (m_dirLoader->getErrorMsg() == "LARGE_DATA2")
        {
            m_questionMode = QUESTION_LARGE_DATA;
            this->showCustomQuestion(QString("qrc:///img/message_question.png"),
                                     tr("Viele Bilder"),
                                     tr("Das Laden von vielen Bildern mit Datumssortierung kann Zeit in Anspruch nehmen. Soll fortgesetzt werden?"),
                                     tr("Ja"), tr("Nein"));
        }
        else if (m_dirLoader->getErrorMsg() == "LARGE_DATA3")
        {
            m_questionMode = QUESTION_LARGE_DATA;
            this->showCustomQuestion(QString("qrc:///img/message_question.png"),
                                     tr("Viele Unterordner"),
                                     tr("Es wurden viele Unterordner gefunden. Das Laden kann einige Zeit in Anspruch nehmen. Soll fortgesetzt werden?"),
                                     tr("Ja"), tr("Nein"));
        }
        else
        {
            this->showCustomMessage(QString("qrc:///img/message_error.png"),
                                tr("Fehler"),
                                tr("%1: %2").arg(m_dirLoader->getErrorMsg()).arg(m_dirLoader->getDirectory()));
            if (m_wTask.isEmpty())
                this->queueTask(OPEN_DIALOG);
        }
    }
}

void Supervisor::nextImagePressed()
{
    m_automaticForward->stop();
    if (!m_showLoaded || ( !m_setDialog->getLoopSlideShow() && this->isLastImage() && !this->anyBlendingsActive()))
    {
        // show info message
        if (!this->anyBlendingsActive() && m_automaticForwardActive && this->isLastImage())
        {
            this->showCustomMessage(QString("qrc:///img/message_stop.png"),
                                    tr("Timer beendet"),
                                    tr("Das letzte Bild der Show wurde erreicht"),
                                    3000);
        }

        m_automaticForwardActive = false;
        return;
    }

    if (this->anyBlendingsActive() || m_loadingStateNext == IMAGE_LOADING)
    {
        this->queueTask(NEXT_TASK);
        return;
    }

    if (this->overlayActive())
    {
        this->hideOverlays();
        this->queueTask(NEXT_TASK);
        return;
    }

    // increase index or start from new if looping is active
    if (m_setDialog->getLoopSlideShow() && m_current_directory_list.size() <= m_currentIndex + 1)
        m_currentIndex = 0;
    else
        m_currentIndex++;

    m_blendingsActive++;
    emit blendImage(QVariant(m_cur), QVariant(m_next), QVariant(true));
    int tmp_next = this->giveFreeSlot(m_cur, m_next);
    m_cur = m_next;

    if (m_loadingStateNext == IMAGE_ERROR)
    {
        this->showCustomMessage(QString("qrc:///img/message_error.png"),
                                tr("Fehler"),
                                tr("Es gab ein Problem beim Laden des Bildes: \"%1\".").arg(m_current_directory_list.at(m_currentIndex).fileName()));
//        return;
    }

    m_next = tmp_next;
    m_loadingStateNext = IMAGE_LOADING;
    emit loadImage(QVariant("next"), QVariant(m_next));
}

void Supervisor::prevImagePressed()
{
    if (!m_showLoaded || (!m_setDialog->getLoopSlideShow() && this->isFirstImage()))
    {
        return;
    }

    m_automaticForward->stop();

    if (this->anyBlendingsActive() || m_loadingStatePrev == IMAGE_LOADING)
    {
        this->queueTask(PREV_TASK);
        return;
    }

    if (this->overlayActive())
    {
        this->hideOverlays();
        this->queueTask(PREV_TASK);
        return;
    }

    // decrease index or goto last one if looping is active
    if (m_setDialog->getLoopSlideShow() && this->isFirstImage())
        m_currentIndex = m_current_directory_list.size() - 1;
    else
        m_currentIndex--;

    m_blendingsActive++;
    emit blendImage(QVariant(m_cur), QVariant(m_prev), QVariant(false));
    int tmp_next = this->giveFreeSlot(m_cur, m_prev);
    m_cur = m_prev;

    if (m_loadingStatePrev == IMAGE_ERROR)
    {
        this->showCustomMessage(QString("qrc:///img/message_error.png"),
                                tr("Fehler"),
                                tr("Es gab ein Problem beim Laden des Bildes: \"%1\".").arg(m_current_directory_list.at(m_currentIndex+1).fileName()));
    }

    m_next = tmp_next;
    m_loadingStateNext = IMAGE_LOADING;
    emit loadImage(QVariant("next"), QVariant(m_next));
}

void Supervisor::jumpToImage(bool fromPreview)
{
    // set current Index to jump image
    QString new_index = QString(m_jumpToImageValue).replace("_", "").trimmed();
    if (new_index.isEmpty())
        return;

    int index = new_index.toInt() - 1;
    if (index < 0 || index >= m_current_directory_list.size() || index == m_currentIndex)
    {
        if (fromPreview)
        {
            m_activeInputMode = NO_MODE;
            this->hideMessage();
        }
        return;
    }

    // set new index
    m_currentIndex = index;

    // find appropriate slots
    int tmp_cur = m_cur;
    m_cur = this->giveFreeSlot(m_cur, m_prev);
    m_next = this->giveFreeSlot(m_cur, tmp_cur);

    // load new current image
    m_loadingStateCurrent = IMAGE_LOADING;
    emit loadImage(QVariant("current"), QVariant(m_cur));

    // load next image
    m_loadingStateNext = IMAGE_LOADING;
    emit loadImage(QVariant("next"), QVariant(m_next));

    // clear queued tasks
    m_wTask.clear();

    if (!fromPreview)
    {
        this->queueTask(JUMP_FADE_OUT);
        this->queueTask(START_TASK);
    }
    else
    {
        m_blendingsActive++;
        emit fadeToJumpToPreview(m_cur);
        this->hideMessage();
//        m_jumptoPreviewVisible = false;
//        m_messageTimeout->stop();
//        m_jumptoPreview->stop();
//        m_activeInputMode = NO_MODE;
//        m_messageActive = false;
    }
}

void Supervisor::blendingFinished()
{
    if (m_showLoaded && (m_wTask.isEmpty() || m_wTask.head() != JUMP_FADE_OUT))
    {
        m_prev = this->giveFreeSlot(m_cur, m_next);
        m_loadingStatePrev = IMAGE_LOADING;
        emit loadImage(QVariant("prev"), QVariant(m_prev));
    }

    m_blendingsActive--;
    if (m_blendingsActive < 0)
    {
        qDebug("WHATT!!???!: Blendings active war kleiner 0!!!");
        m_blendingsActive = 0;
    }

    if (m_automaticForwardActive)
        m_automaticForward->start(m_timerValue * 1000);

    this->processWaitingQueue();
}

int Supervisor::blendingCount()
{
    return m_blendingsActive;
}

void Supervisor::overlayTransitionFinished()
{
    m_overlayTransitions--;
    if (m_overlayTransitions < 0)
    {
        qDebug("WHATT!!???!: overlay TRANSITIONS war kleiner 0!!!");
        m_overlayTransitions = 0;
    }

    this->processWaitingQueue();
}

int Supervisor::overlayTransitionCount()
{
    return m_overlayTransitions;
}

bool Supervisor::showLoaded()
{
    return m_showLoaded;
}

void Supervisor::waitingFinished()
{
    m_waitingActive = false;
    this->processWaitingQueue();
}

void Supervisor::imageLoadingFinished(QVariant slot)
{
//    std::cout << slot.toString().toStdString().c_str() << std::endl;

    QString slot_string = slot.toString();
    if ( (slot_string.section("/", -1).section("_", 0, 0) == "jumpto") &&
         (m_activeInputMode == MODE_JUMPTO) &&
         (m_jumpToImageValue.trimmed() == slot_string.section("/", -1).section("_", 1).trimmed()) )
    {
        QString jumpto_number = slot_string.section("/", -1).section("_", 1);
        bool ok = false;
        jumpto_number.toInt(&ok);

        if (ok && m_loadingStateJumpto == IMAGE_READY)
        {
            // show jumpto image...
            this->blendJumpToPreview();
        }
        else if (ok && m_loadingStateJumpto == IMAGE_ERROR)
        {
            this->blendJumpToPreview(false);
        }
    }

    this->processWaitingQueue();
}

void Supervisor::startShowFinished()
{
    m_showLoaded = true;

    if (m_infoActive)
    {
        m_overlayTransitions++;
        emit infoBox();
    }
    else if (m_setDialog->getFirstStart())
    {
        m_infoActive = true;
        m_overlayTransitions++;
        emit infoBox();
        QTimer::singleShot(1000, this, SLOT(showBubbleMessage_info()));
    }

    m_setDialog->setFirstStart(false);

    this->processWaitingQueue();
}

void Supervisor::jumtoPreviewReady()
{
    m_jumptoPreviewVisible = true;
}

bool Supervisor::isFullscreen()
{
    return m_quickView->isExpanded();
}

bool Supervisor::isInfoActive()
{
    return m_infoActive;
}

void Supervisor::answerOfQuestion(QVariant answer)
{
    AnswerButton answer_button = AnswerButton(answer.toInt());

    switch (m_questionMode) {
    case QUESTION_EXIT:
        if (answer_button == ANSWER_BUTTON_1)
            emit quit();
        break;
    case QUESTION_LARGE_DATA:
        if (answer_button == ANSWER_BUTTON_1)
        {
            this->startDirectoryLoading(true);
        }
        else
        {
            this->moveOpenDialogToMiddle();
            QTimer::singleShot(500, m_setDialog, SLOT(show()));
        }
        break;
    default:
        break;
    }

    m_questionMode = QUESTION_NONE;
}

QVariant Supervisor::getImageNumSlashTotalNumber()
{
    return QVariant(tr("%1/%2").arg(m_currentIndex+1).arg(m_current_directory_list.size()));
}

QVariant Supervisor::getExifTagOfCurrent(QVariant tagname)
{
    if (tagname.toString().isEmpty())
        return QVariant("");

    QString current_file_name = m_current_directory_list.at(m_currentIndex).absoluteFilePath();
    EXIFInfo &exif = m_imgProvider->getExifForFile(current_file_name);

    QString tag_string = tagname.toString();

    if (tag_string == "cameraMake")
        return QVariant(QString::fromStdString(exif.Make));
    else if (tag_string == "fileName")
        return QVariant(m_current_directory_list.at(m_currentIndex).fileName());
    else if (tag_string == "cameraModel")
        return QVariant(QString::fromStdString(exif.Model));
    else if (tag_string == "dateTimeModified")
        return QVariant(QString::fromStdString(exif.DateTime));
    else if (tag_string == "dateTimeOriginal")
    {
        QString dateTime = QString::fromStdString(exif.DateTimeOriginal).trimmed();
        QRegExp exp("^[0-9]{4}:[0-9]{2}:[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$");
        if (exp.exactMatch(dateTime))
        {
            QDateTime dt = QDateTime::fromString(dateTime, "yyyy:MM:dd hh:mm:ss");
            if (dt.isValid())
                dateTime = dt.toString("dd.MM.yyyy hh:mm:ss");
            else
                dateTime = "";
        }
        return QVariant(dateTime);
    }
    else if (tag_string == "imgDescription")
    {
        return QVariant(QString::fromStdString(exif.ImageDescription).trimmed());
    }
    else if (tag_string == "exposureTime")
        return QVariant(exif.ExposureTime);
    else if (tag_string == "resolutionAndSize")
    {
        QFileInfo info(current_file_name);
        QString res = QString::fromStdString(exif.Resolution);
        return QVariant(tr("%1  (%2 kB)").arg(res).arg(info.size() / 1024));
    }
    else if (tag_string == "cameraModelMake")
    {
        QString cam = "";
        QString model = QString::fromStdString(exif.Model);
        QString make = QString::fromStdString(exif.Make);

        if (!model.isEmpty())
            cam += model;
        else if (!make.isEmpty())
            cam += make;

        if (!model.isEmpty() && !make.isEmpty())
            cam += " (" + make + ")";

        return QVariant(cam);
    }
    else if (tag_string == "acquisitionParameters")
    {
        QString params = "";
        bool sep_needed = false;

        if (exif.ExposureTime > 0.5 && exif.ExposureTime != 0.0)
        {
            params += tr("%1 Sek  ").arg(QString::number(exif.ExposureTime, 'f', 1));
            sep_needed = true;
        }
        else if (exif.ExposureTime <= 0.5 && exif.ExposureTime != 0.0)
        {
            double den = 1.0 / exif.ExposureTime;
            params += tr("1/%1  Sek").arg(QString::number(int(den + .5)));
            sep_needed = true;
        }

        if (exif.FNumber != 0.0)
        {
            if (sep_needed) params += "   |   ";
            params += tr("f/%1").arg(QString::number(exif.FNumber, 'f', 1));
            sep_needed = true;
        }

        if (exif.FocalLength >= 1.0)
        {
            if (sep_needed) params += "   |   ";
            params += tr("%1  mm").arg(QString::number(exif.FocalLength, 'f', 0));
        }

        return QVariant(params);
    }
    else
        return QVariant("");
}

void Supervisor::panoramaError(QVariant title, QVariant text)
{
    m_panoramaModeActive = false;
    m_overlayTransitions--;

    if (!this->anyBlendingsActive())
    {
        this->showCustomMessage(QString("qrc:///img/message_info.png"), title.toString(), text.toString(), 2500);
    }
}

QVariant Supervisor::getAppVersion()
{
    return QVariant(qApp->applicationVersion());
}

void Supervisor::keyPressEvent( QKeyEvent * event )
{
    emit bubbleBox(QVariant(""), QVariant(8000));

    if (m_messageActive && m_questionMode != QUESTION_NONE)
    {
        QObject * object = m_quickView->rootObject();
        if (object)
        {
            QQuickItem * msgBox = object->findChild<QQuickItem *>("messageBox");
            if (msgBox)
                m_quickView->sendEvent(msgBox, event);
        }

        if (event->key() != Qt::Key_Return
                && event->key() != Qt::Key_Enter
                && event->key() != Qt::Key_Escape
                && event->key() != Qt::Key_F
                && event->key() != Qt::Key_F11)
            return;
    }

    // leave Panoramamode if active
    bool panoramaModeWasActive = false;
    if (m_panoramaModeActive &&
        event->key() != Qt::Key_Escape &&
        event->key() != Qt::Key_I)
    {
        m_panoramaModeActive = false;
        emit stopPanorama();
        panoramaModeWasActive = true;
    }

    switch (event->key()) {

    case Qt::Key_O:
        {
            if (m_currentlyWaiting || m_waitingActive)
                return;

            m_wTask.clear();

            if (/*m_blendingsActive == 0 && */m_automaticForwardActive)
                this->startInputMode(MODE_TIMER_OFF, 2000);
//            else if (m_automaticForwardActive)
//                this->queueTask(STOP_TIMER);

            this->moveOpenDialogToMiddle();
            m_quickView->unsetCursor();
            m_setDialog->show();
        }
        break;
    case Qt::Key_Escape:
        {
            if (m_messageActive)
            {
                this->hideMessage();
            }
            else if (m_helpActive)
            {
                this->hideHelpOverlay();
            }
            else if (m_panoramaModeActive)
            {
                m_panoramaModeActive = false;
                emit stopPanorama();
            }
            else
            {
                m_wTask.clear();

                m_questionMode = QUESTION_EXIT;
                QString exit_message;
                bool forward_was_active = false;
//                QString exit_message = tr("ESC zum Beenden drücken, ENTER zum Abbruch. Das ist eine sehr lange Nachricht, die nicht zu Enden schein, mal sehen, was er daraus mach.......Toll, imer noch nicht z7u ende");
                if (m_automaticForwardActive)
                {
                    m_automaticForwardActive = false;
                    m_automaticForward->stop();
                    exit_message.append(tr("Timer beendet!"));
                    forward_was_active = true;
                }
                this->showCustomQuestion(QString("qrc:///img/message_exit.png"), tr("Beenden?"), exit_message, tr("Ja"), tr("Nein"), forward_was_active);
            }
        }
        break;
    case Qt::Key_M:
        this->applyNewOptions();
        break;
    case Qt::Key_PageDown:
        {
            this->nextImagePressed();
        }
        break;
    case Qt::Key_Right:    
        {
            if (m_activeInputMode == MODE_JUMPTO)
                this->upOrDownPressed(true);
            else
                this->nextImagePressed();
        }
        break;
    case Qt::Key_PageUp:
        {
            this->prevImagePressed();
        }
        break;
    case Qt::Key_Left:
        {
        if (m_activeInputMode == MODE_JUMPTO)
            this->upOrDownPressed(false);
        else
            this->prevImagePressed();
        }
        break;
    case Qt::Key_Up:
        {
            this->upOrDownPressed(true);
        }
        break;
    case Qt::Key_Down:
        {
            this->upOrDownPressed(false);
        }
        break;
    case Qt::Key_F:
    case Qt::Key_F11:
        {
        if (m_quickView->isExpanded())
            {
                m_quickView->showExpanded(false);
//                m_setDialog->setOnTopHint(false);
            }
            else
            {
                m_quickView->showExpanded(true);
//                m_setDialog->setOnTopHint(true);
            }
            emit refresh();
        }
        break;
    case Qt::Key_H:
    case Qt::Key_F1:
        {
            if (!this->anyBlendingsActive() && !m_messageActive)
            {
                m_overlayTransitions++;
                emit showHelp();
                m_helpActive = !m_helpActive;

                if (m_automaticForwardActive)
                {
                    m_automaticForwardActive = false;
                    m_automaticForward->stop();
                    this->startInputMode(MODE_TIMER_OFF, 1500);
                }
            }
        }
        break;
    case Qt::Key_F2:
        {
            if (m_helpActive)
            {
                this->moveOpenDialogToMiddle();
                m_setDialog->showHelpDialog();
            }
        }
        break;
    case Qt::Key_J:
        {
            if (m_blendingsActive == 0 && m_showLoaded && ((m_overlayTransitions == 0) || (m_overlayTransitions == 1 && panoramaModeWasActive)) && !m_helpActive)
            {
                if (m_activeInputMode == MODE_JUMPTO)
                {
                    this->hideMessage();
                }
                else
                {
                    m_jumpToImageValue = QString::number(m_currentIndex + 1);
                    this->startInputMode(MODE_JUMPTO, -1);
                }
            }
            else if (m_blendingsActive > 0)
            {
                m_wTask.clear();
                this->queueTask(JUMP_TO_DIALOG);
            }
        }
        break;
    case Qt::Key_P:
        {
            if (!this->overlayActive() && m_showLoaded)
            {
                if (m_panoramaModeActive)
                {
                    m_panoramaModeActive = false;
                    emit stopPanorama();
                }
                else if (!this->anyBlendingsActive())
                {
                    m_overlayTransitions++;
                    m_panoramaModeActive = true;
                    emit startPanorama();

                    if (m_automaticForwardActive)
                    {
                        m_automaticForwardActive = false;
                        m_automaticForward->stop();
                        this->startInputMode(MODE_TIMER_OFF, 1500);
                    }
                }
            }
        }
        break;
    case Qt::Key_I:
        {
            if (!this->overlayActive() && m_showLoaded)
            {
                m_overlayTransitions++;
                m_infoActive = !m_infoActive;
                emit infoBox();
            }
        }
        break;
    case Qt::Key_Space:
        {
            if (m_showLoaded && /*m_blendingsActive == 0 && */ ((m_overlayTransitions == 0) || (m_overlayTransitions == 1 && panoramaModeWasActive)))
            {
                m_wTask.clear();

                if (m_automaticForwardActive || m_activeInputMode == MODE_TIMER_ON)
                {
                    this->startInputMode(MODE_TIMER_OFF, 2000);
                }
                else
                {
                    m_timerInputValue = QString::number(m_timerValue);
                    if (m_timerFirstStart)
                        this->startInputMode(MODE_TIMER_ON);
                    else
                        this->startInputMode(MODE_TIMER_ON, 2000);
                }
            }
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        {
            if (m_activeInputMode != NO_MODE)
                this->inputModeTimeout();
            else if (m_messageActive)
                this->hideMessage();
        }
        break;
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        {
            this->numberPressed(event->text());
        }
        break;
    default:
        break;
    }
}

//void Supervisor::keyReleaseEvent ( QKeyEvent * event )
//{

//}

//void Supervisor::mouseMoveEvent ( QMouseEvent * event )
//{

//}

void Supervisor::mouseDoubleClickEvent ( QMouseEvent * event )
{
    if (event->button() != Qt::LeftButton)
        return;

    m_mousePressTimer->stop();

    if (m_quickView->isExpanded())
        m_quickView->showExpanded(false);
    else
        m_quickView->showExpanded(true);

    emit refresh();
}

void Supervisor::mousePressEvent ( QMouseEvent * event )
{
    if (m_setDialog->getMouseControl())
    {
        switch (event->button()) {
        case Qt::LeftButton:
        {
            if (this->showLoaded() && m_activeInputMode == MODE_JUMPTO && m_overlayTransitions == 0)
            {
                this->inputModeTimeout();
                return;
            }

            int dc_speed = qApp->styleHints()->mouseDoubleClickInterval();
            m_mousePressTimer->start(dc_speed + 1);
            break;
        }
        case Qt::MidButton:
        {
            if (m_currentlyWaiting)
                return;

            if (m_blendingsActive == 0 && m_automaticForwardActive)
                this->startInputMode(MODE_TIMER_OFF, 2000);
            if (m_automaticForwardActive)
                this->queueTask(STOP_TIMER);

            this->moveOpenDialogToMiddle();
            m_quickView->unsetCursor();
            m_setDialog->show();
        }
            break;
        case Qt::RightButton:
            if (this->showLoaded() && m_activeInputMode == MODE_JUMPTO && m_overlayTransitions == 0)
            {
                this->hideMessage();
                return;
            }
            this->nextImagePressed();
            break;
        default:
            break;
        }
    }
}

void Supervisor::mouseMoveEvent ( QMouseEvent * event )
{
    if (event->buttons() != Qt::LeftButton)
        return;

    m_mousePressTimer->stop();
}

void Supervisor::mouseWheelEvent ( QWheelEvent * event )
{
    if (m_showLoaded && m_setDialog->getMouseControl() && !this->anyBlendingsActive() && m_activeInputMode == NO_MODE && !m_helpActive)
    {
        m_jumpToImageValue = QString::number(m_currentIndex + 1);
        this->startInputMode(MODE_JUMPTO, -1);
        return;
    }

    if (event->angleDelta().y() > 0)
    {
        this->upOrDownPressed(true);
    }
    else if (event->angleDelta().y() < 0)
    {
        this->upOrDownPressed(false);
    }
}

void Supervisor::leftMousePress()
{
    if (m_setDialog->getMouseControl())
        this->prevImagePressed();
}

void Supervisor::resizeEvent( QResizeEvent * event )
{
    if (m_panoramaModeActive)
    {
        m_panoramaModeActive = false;
        emit stopPanorama();
    }

    emit refresh();
}

void Supervisor::processWaitingQueue()
{
    if (m_wTask.isEmpty())
        return;

    if (m_wTask.head() == START_TASK)
    {
        if (m_loadingStateCurrent != IMAGE_LOADING)
        {
            emit waiting(QVariant(false));
        }

        if (m_loadingStateCurrent == IMAGE_ERROR && m_current_directory_list.size() == 1)
        {
            showCustomMessage(QString("qrc:///img/message_error.png"),
                              tr("Fehler"),
                              tr("Die Show beinhaltet nur ein Bild, welches nicht geladen werden konnte: \"%1\"!").arg(m_current_directory_list.at(m_currentIndex).fileName()));
            m_wTask.clear();
            this->queueTask(OPEN_DIALOG);
            return;
        }

        if (m_loadingStateCurrent != IMAGE_LOADING && !this->anyBlendingsActive() && !m_waitingActive && !m_showLoaded)
        {
            m_wTask.dequeue();
            m_blendingsActive++;
            m_currentlyLoading = false;
            emit startTheShow(m_cur);

            // ERROR on Loading
            if (m_loadingStateCurrent == IMAGE_ERROR)
            {
                showCustomMessage(QString("qrc:///img/message_error.png"),
                                  tr("Fehler"),
                                  tr("Es gab ein Problem beim Laden des Bildes: \"%1\"! Es befinden sich weitere Bilder in ther Show!").arg(m_current_directory_list.at(m_currentIndex).fileName()));
            }
        }
    }
    else if (m_wTask.head() == NEXT_TASK)
    {
        if (!this->anyBlendingsActive() && m_loadingStateNext != IMAGE_LOADING)
        {
            m_wTask.dequeue();
            this->nextImagePressed();
        }
    }
    else if (m_wTask.head() == PREV_TASK)
    {
        if (!this->anyBlendingsActive() && m_loadingStatePrev != IMAGE_LOADING)
        {
            m_wTask.dequeue();
            this->prevImagePressed();
        }
    }
    else if (m_wTask.head() == HIDE_HELP)
    {
        if (m_helpActive && !this->anyBlendingsActive())
        {
            m_wTask.dequeue();
            this->hideHelpOverlay();
        }
    }
    else if (m_wTask.head() == HIDE_MESSAGE)
    {
        if (m_messageActive && !this->anyBlendingsActive())
        {
            m_wTask.dequeue();
            this->hideMessage();
        }
    }
    else if (m_wTask.head() == STOP_TIMER)
    {
        if (!this->anyBlendingsActive() && m_automaticForwardActive)
        {
            m_wTask.dequeue();
            this->startInputMode(MODE_TIMER_OFF, 2000);
        }
    }
    else if (m_wTask.head() == JUMP_FADE_OUT)
    {
        if (m_showLoaded && !this->anyBlendingsActive() && !this->overlayActive())
        {
            m_wTask.dequeue();
            m_blendingsActive++;
            emit blendOutShow(m_prev);
            m_showLoaded = false;

            if (m_infoActive)
            {
                m_overlayTransitions++;
                emit infoBox();
            }

            m_waitingActive = true;
            emit waiting(QVariant(true));
        }
    }
    else if (m_wTask.head() == DIR_LOAD)
    {
        if (!this->anyBlendingsActive())
        {
            m_wTask.dequeue();
            this->startDirectoryLoading();
        }
    }
    else if (m_wTask.head() == OPEN_DIALOG)
    {
        if (!this->anyBlendingsActive() && !m_messageActive)
        {
            m_wTask.dequeue();
            this->moveOpenDialogToMiddle();
            m_quickView->unsetCursor();
            m_setDialog->show();
        }
    }
    else if (m_wTask.head() == JUMP_TO_DIALOG)
    {
        if (!this->anyBlendingsActive() && !m_helpActive)
        {
            m_wTask.dequeue();
            if (m_activeInputMode == MODE_JUMPTO)
            {
                this->hideMessage();
            }
            else
            {
                m_jumpToImageValue = QString::number(m_currentIndex + 1);
                this->startInputMode(MODE_JUMPTO, -1);
            }
        }
    }
}

void Supervisor::changeLanguage(QVariant language)
{
    if (language.toString().isEmpty())
        return;

    qApp->removeTranslator(&m_translator);

    if (language.toString() != "de")
        if (m_translator.load(QCoreApplication::applicationDirPath() + "/picture-show2_" + language.toString()))
            qApp->installTranslator(&m_translator);

    m_setDialog->updateLanguage();
    emit languageChanged(language);
}

void Supervisor::numberPressed(const QString & number)
{
    if (m_activeInputMode == MODE_TIMER_ON)
    {
        m_inputTimeout->stop();

        if (m_timerInputValue.count("_") == 0 && number != "0")
        {
            m_timerInputValue = number + QString("_");
            m_inputTimeout->start(1500);
            this->startInputMode(MODE_TIMER_ON);
        }
        else if (m_timerInputValue.count("_") == 1)
        {
            m_timerInputValue.replace("_", number);
            this->startInputMode(MODE_TIMER_ON, 2000);
        }        
    }
    else if (m_activeInputMode == MODE_JUMPTO)
    {
        m_jumptoPreview->stop();

        if (number == "0")
        {
            QString temp_jump = "";
            if (m_jumpToImageValue.contains("_"))
                temp_jump = QString(m_jumpToImageValue).replace("_", "").trimmed() + number;
            else
                temp_jump = number;

            if (temp_jump.startsWith("0"))
                return;

//            if (temp_jump.toInt() > m_current_directory_list.size())
//                temp_jump.toInt
        }

        m_inputTimeout->stop();

        if (m_jumpToImageValue.contains("_"))
        {
            m_jumpToImageValue = m_jumpToImageValue.replace("_", "").trimmed() + number;
            if (m_jumpToImageValue.toInt() > m_current_directory_list.size())
                m_jumpToImageValue = QString::number(m_current_directory_list.size());
        }
        else
        {
            m_jumpToImageValue = number;
            if (m_jumpToImageValue.toInt() > m_current_directory_list.size())
                m_jumpToImageValue = QString::number(m_current_directory_list.size());
        }

        int remain = QString::number(m_current_directory_list.size()).size() - m_jumpToImageValue.size();
        for (int i = 0; i < remain; i++)
            m_jumpToImageValue.prepend("_ ");

        if (m_jumpToImageValue.contains("_"))
            m_inputTimeout->start(1500);
        else
            m_jumptoPreview->start();

        this->startInputMode(MODE_JUMPTO, -1);
    }
}

void Supervisor::upOrDownPressed(bool up)
{
    if (m_overlayTransitions > 0)
        return;

    if (m_activeInputMode == MODE_TIMER_ON)
    {
        m_inputTimeout->stop();

        m_timerInputValue.replace("_", "");
        int cur_val = m_timerInputValue.toInt();

        if (up && cur_val != 0 && cur_val + 1 < 100)
            m_timerInputValue = QString::number(cur_val + 1);
        else if (!up && cur_val != 0 && cur_val - 1 > 0)
            m_timerInputValue = QString::number(cur_val - 1);

        if (m_timerFirstStart)
            this->startInputMode(MODE_TIMER_ON);
        else
            this->startInputMode(MODE_TIMER_ON, 2000);
    }
    else if (m_activeInputMode == MODE_JUMPTO)
    {
        m_inputTimeout->stop();

        m_jumpToImageValue.replace("_", "");
        int cur_val = m_jumpToImageValue.toInt();

        if (up && cur_val != 0 && cur_val + 1 <= m_current_directory_list.size())
        {
            m_jumpToImageValue = QString::number(cur_val + 1);
            m_jumptoPreview->start();
            this->startInputMode(MODE_JUMPTO, -1);
        }
        else if (!up && cur_val != 0 && cur_val - 1 > 0)
        {
            m_jumpToImageValue = QString::number(cur_val - 1);
            m_jumptoPreview->start();
            this->startInputMode(MODE_JUMPTO, -1);
        }
    }
}

void Supervisor::showJumpToPreview()
{
    if (m_activeInputMode == MODE_JUMPTO)
    {
        QString jumptoString = "jumpto_" + m_jumpToImageValue;
        emit loadImage(QVariant(jumptoString), 4);
    }
}

void Supervisor::bindings()
{
    if (m_quickView == NULL && m_setDialog == NULL)
        return;

    // SettingsDialog Bindings
    connect(m_setDialog, SIGNAL(accepted()), this, SLOT(applyNewOptions()));
    connect(m_setDialog, SIGNAL(accepted()), m_quickView, SLOT(hideCursorOnFullscreen()));
    connect(m_setDialog, SIGNAL(rejected()), m_quickView, SLOT(hideCursorOnFullscreen()));
    connect(m_setDialog, SIGNAL(languageChanged(QVariant)), this, SLOT(changeLanguage(QVariant)));
    connect(m_setDialog, SIGNAL(propertiesChanged()), this, SIGNAL(refresh()));

    // QQuickWindow Bindings
    connect(m_quickView, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
//    connect(m_quickView, SIGNAL(keyReleased(QKeyEvent*)), this, SLOT(keyReleaseEvent(QKeyEvent*)));
    connect(m_quickView, SIGNAL(mouseMoved(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));
    connect(m_quickView, SIGNAL(mouseDoubleClicked(QMouseEvent*)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*)));
    connect(m_quickView, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
    connect(m_quickView, SIGNAL(mouseWheelTurned(QWheelEvent*)), this, SLOT(mouseWheelEvent(QWheelEvent*)));
    connect(m_quickView, SIGNAL(windowResized(QResizeEvent*)), this, SLOT(resizeEvent(QResizeEvent*)));
    connect(m_quickView->engine(), SIGNAL(warnings(QList<QQmlError>)), this, SLOT(errorOccurred(QList<QQmlError>)));

    connect(m_dirLoader, SIGNAL(loadDirectoryFinished(bool)), this, SLOT(directoryLoadingFinished(bool)));

    // QML Bindings
    QObject * rootObject = (QObject*)m_quickView->rootObject();
    connect(this, SIGNAL(blendImage(QVariant,QVariant,QVariant)), rootObject, SLOT(image_transition(QVariant,QVariant,QVariant)));
    connect(this, SIGNAL(blendOutShow(QVariant)), rootObject, SLOT(show_to_waiting(QVariant)));
    connect(this, SIGNAL(quit()), rootObject, SLOT(quit()));
    connect(this, SIGNAL(waiting(QVariant)), rootObject, SLOT(animate_wait(QVariant)));
    connect(this, SIGNAL(refresh()), rootObject, SLOT(refresh()));
    connect(this, SIGNAL(showHelp()), rootObject, SLOT(show_help()));
//    connect(this, SIGNAL(showMessage(QVariant,QVariant,QVariant,QVariant)), rootObject, SLOT(show_message(QVariant,QVariant,QVariant,QVariant)));
    connect(this, SIGNAL(startTheShow(QVariant)), rootObject, SLOT(waiting_to_one(QVariant)));
    connect(this, SIGNAL(loadImage(QVariant,QVariant)), rootObject, SLOT(load_new_image(QVariant,QVariant)));
    connect(this, SIGNAL(languageChanged(QVariant)), rootObject, SLOT(refresh()));
    connect(this, SIGNAL(startPanorama()), rootObject, SLOT(start_panorama()));
    connect(this, SIGNAL(stopPanorama()), rootObject, SLOT(stop_panorama()));
    connect(this, SIGNAL(infoBox()), rootObject, SLOT(show_hide_info()));
    connect(this, SIGNAL(bubbleBox(QVariant,QVariant)), rootObject, SLOT(show_hide_bubble(QVariant,QVariant)));
    connect(this, SIGNAL(fadeToJumpToPreview(QVariant)), rootObject, SLOT(fade_preview_to_screen(QVariant)));
}

bool Supervisor::isQmlReady()
{
    return m_qml_ready;
}

bool Supervisor::overlayActive()
{
    if (m_helpActive || m_messageActive)
        return true;

    return false;
}

void Supervisor::hideOverlays()
{
    if (m_messageActive)
        this->hideMessage();

    if (m_helpActive)
        this->hideHelpOverlay();
}

void Supervisor::showHelpOverlay()
{
    if (!m_helpActive)
    {
        m_overlayTransitions++;
        emit showHelp();
        emit bubbleBox(QVariant(tr("Willkommen in picture-show 2!\n\"O\" drücken, um Bilder zu öffnen!")), QVariant(60000));
        m_helpActive = true;
    }
}

void Supervisor::hideHelpOverlay()
{
    if (m_helpActive)
    {
//        if (m_blendingActive)
//        {
//            queueTask(HIDE_HELP);
//            return;
//        }

        m_overlayTransitions++;
        emit showHelp();
        m_helpActive = false;
    }
}

void Supervisor::showCustomMessage(QString imageUrl, QString title, QString text, int timeout, bool info)
{
    QObject * object = m_quickView->rootObject();
    if (object)
    {
        QObject * msgBox = object->findChild<QObject *>("messageBox");
        if (!msgBox)
            return;

        m_messageTimeout->stop();
        m_overlayTransitions++;
        m_messageActive = true;
        m_jumptoPreviewVisible = false;

        QMetaObject::invokeMethod(msgBox, "show_message",
                                  Q_ARG(QVariant, QVariant(imageUrl)),
                                  Q_ARG(QVariant, QVariant(title)),
                                  Q_ARG(QVariant, QVariant(text)),
                                  Q_ARG(QVariant, QVariant(info)),
                                  Q_ARG(QVariant, QVariant(0)),
                                  Q_ARG(QVariant, QVariant("")),
                                  Q_ARG(QVariant, QVariant("")));

        if (timeout != 0)
            m_messageTimeout->start(timeout);
    }
}

void Supervisor::showCustomQuestion(QString imageUrl, QString title, QString text, QString b1_text, QString b2_text, bool info)
{
    QObject * object = m_quickView->rootObject();
    if (object)
    {
        QObject * msgBox = object->findChild<QObject *>("messageBox");
        if (!msgBox)
            return;

        m_messageTimeout->stop();
        m_overlayTransitions++;
        m_messageActive = true;
        m_jumptoPreviewVisible = false;

        QMetaObject::invokeMethod(msgBox, "show_message",
                                  Q_ARG(QVariant, QVariant(imageUrl)),
                                  Q_ARG(QVariant, QVariant(title)),
                                  Q_ARG(QVariant, QVariant(text)),
                                  Q_ARG(QVariant, QVariant(info)),
                                  Q_ARG(QVariant, QVariant(2)),
                                  Q_ARG(QVariant, QVariant(b1_text)),
                                  Q_ARG(QVariant, QVariant(b2_text)));

    }
}

void Supervisor::showBubbleMessage_info()
{
    emit bubbleBox(QVariant(tr("Informationsleiste mit \"I\" ein- und wieder ausblenden!")), QVariant(8000));
}

void Supervisor::hideMessage()
{
    if (m_messageActive)
    {
        QObject *object = m_quickView->rootObject();
        if (object)
        {
            QObject * msgBox = object->findChild<QObject *>("messageBox");
            if (!msgBox)
                return;

            m_questionMode = QUESTION_NONE;
            m_jumptoPreviewVisible = false;

            m_messageTimeout->stop();
            m_jumptoPreview->stop();
            m_overlayTransitions++;

            QMetaObject::invokeMethod(msgBox, "hide_message");

            m_activeInputMode = NO_MODE;
            m_messageActive = false;
        }
    }
}

void Supervisor::blendJumpToPreview(bool state)
{
    QObject *object = m_quickView->rootObject();
    if (object)
    {
        QObject * msgBox = object->findChild<QObject *>("messageBox");
        if (!msgBox)
            return;

        if (state)
            QMetaObject::invokeMethod(msgBox, "blend_jumpto");
        else
            QMetaObject::invokeMethod(msgBox, "blend_jumpto_preview_error");
    }
}

void Supervisor::queueTask(WaitingTask task)
{
    // queue task (if no more than 6 are queued) to be processed
    // when circumstances have changed --> see processWaitingQueue();
    if (m_wTask.size() < 6)
    {
        if (task == NEXT_TASK || task == PREV_TASK)
        {
            if (m_wTask.contains(task))
                return;
        }
        m_wTask.enqueue(task);
    }
}

void Supervisor::startInputMode(InputMode mode, int timeout)
{
    // stop input Message Timeout (in case another input mode is started during another active input mode)
    m_inputMessageTimeout->stop();

    // if a question has been asked, set back to no question
    m_questionMode = QUESTION_NONE;

    // start specific input mode and set a timeout
    switch (mode) {

    case MODE_TIMER_ON:
    {
        this->showCustomMessage(QString("qrc:///img/message_play.png"), tr("Starte Timer:  %1 Sekunden").arg(m_timerInputValue), QString(""));
    }
        break;

    case MODE_TIMER_OFF:
    {
        m_automaticForwardActive = false;
        m_automaticForward->stop();
        this->showCustomMessage(QString("qrc:///img/message_stop.png"), tr("Stoppe Timer"), QString(""), 2000);
    }
        break;

    case MODE_JUMPTO:
    {
        QString announce = tr("Bildnummer eingeben und ENTER drücken");
        if (m_automaticForwardActive)
            announce.prepend(tr("Timer gestoppt. "));

        m_automaticForwardActive = false;
        m_automaticForward->stop();

        this->showCustomMessage(QString("qrc:///img/message_jumpto.png"),
                               tr("Springe zu: %1 / %2").arg(m_jumpToImageValue).arg(m_current_directory_list.size()),
                               announce,
                               0,
                               true);

    }
        break;
    default:
        break;
    }

    m_activeInputMode = mode;
    if (timeout > 0)
        m_inputMessageTimeout->start(timeout);
}

void Supervisor::inputModeTimeout()
{
    m_inputMessageTimeout->stop();

    // react to message timeout depending on curren input mode
    switch (m_activeInputMode) {

    case MODE_TIMER_ON:
    {
        m_timerInputValue.replace("_", QString(""));
        bool ok = false;
        m_timerValue = m_timerInputValue.toInt(&ok);
        if (ok)
        {
            if (m_timerValue < 1)
                m_timerValue = 1;

            m_automaticForwardActive = true;
            m_automaticForward->start(m_timerValue * 1000);
            m_timerFirstStart = false;
        }

        m_activeInputMode = NO_MODE;
        this->hideMessage();
    }
        break;

    case MODE_JUMPTO:
    {
        this->jumpToImage(m_jumptoPreviewVisible);

        if (!m_jumptoPreviewVisible)
        {
            m_activeInputMode = NO_MODE;
            this->hideMessage();
        }
    }
        break;

    default:
        break;
    }
}

void Supervisor::timerInputValueTimeout()
{
    if (m_activeInputMode == MODE_TIMER_ON)
    {
        m_timerInputValue.replace("_", QString(""));
        if (m_timerInputValue.toInt() < 1)
            m_timerInputValue = QString("1");

        if (m_timerFirstStart)
            this->startInputMode(MODE_TIMER_ON);
        else
            this->startInputMode(MODE_TIMER_ON, 1000);
    }
    else if (m_activeInputMode == MODE_JUMPTO)
    {
        m_jumpToImageValue.replace("_", QString("")).trimmed();
        this->startInputMode(MODE_JUMPTO, -1);
        this->showJumpToPreview();
    }
}

void Supervisor::errorOccurred(const QList<QQmlError> & warnings)
{
    foreach(QQmlError err, warnings)
    {
        std::cout << err.toString().toStdString().c_str() << std::endl;
    }
}

int Supervisor::giveFreeSlot(int occupied1, int occupied2)
{
    if ((occupied1 == 1 && occupied2 == 2) || (occupied2 == 1 && occupied1 == 2))
        return 3;
    else if ((occupied1 == 1 && occupied2 == 3) || (occupied2 == 1 && occupied1 == 3))
        return 2;
    else
        return 1;
}

QPoint Supervisor::checkValidPosition(QPoint suggestion, QSize sizeAsInfo)
{
    QScreen * activeScreen = m_quickView->screen();
    int abs_w = activeScreen->virtualGeometry().width();
    int abs_h = activeScreen->virtualGeometry().height();
    QPoint valid = suggestion;
    int numberOfScreens = QGuiApplication::screens().size();

    if (suggestion.x() + 10 > abs_w)
    {
        int new_x = abs_w - sizeAsInfo.width();
        if (new_x < 0)
            valid.setX(0);
        else
            valid.setX(new_x);
    }
    else if (numberOfScreens == 1 && suggestion.x() + sizeAsInfo.width() - 25 < 0)
    {
        valid.setX(20);
    }

    if (suggestion.y() + 10 > abs_h)
    {
        int new_y = abs_h - sizeAsInfo.height();
        if (new_y < 0)
            valid.setY(0);
        else
            valid.setY(new_y);
    }
    else if (numberOfScreens == 1 && suggestion.y() + sizeAsInfo.height() - 25 < 0)
    {
        valid.setY(30);
    }

    return valid;
}

bool Supervisor::isLastImage()
{
    return (m_current_directory_list.size() <= m_currentIndex + 1);
}

bool Supervisor::isFirstImage()
{
    return (m_currentIndex - 1 < 0);
}

bool Supervisor::anyBlendingsActive()
{
    if (m_blendingsActive > 0 || m_overlayTransitions > 0)
        return true;
    else
        return false;
}

void Supervisor::moveOpenDialogToMiddle()
{
    if (m_setDialog != NULL)
    {
        int new_x_pos = m_quickView->position().x() + (int(m_quickView->size().width()/2) - int(m_setDialog->size().width()/2));
        int new_y_pos = m_quickView->position().y() + (int(m_quickView->size().height()/2) - int((double(m_setDialog->size().height()))/2));
        m_setDialog->move(new_x_pos, new_y_pos);
    }
}

void Supervisor::printState()
{
    std::cout << "m_automaticForwardActive: " << m_automaticForwardActive << std::endl;
    std::cout << "m_blendingsActive: " << m_blendingsActive << std::endl;
    std::cout << "m_currentlyLoading: " << m_currentlyLoading << std::endl;
    std::cout << "m_currentlyWaiting: " << m_currentlyWaiting << std::endl;
    std::cout << "m_messageActive: " << m_messageActive << std::endl;
    std::cout << "m_helpActive: " << m_helpActive << std::endl;

}
