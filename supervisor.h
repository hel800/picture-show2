/* ------------------------------------------------------------------
supervisor.h is part of picture-show2.
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

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <QObject>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlContext>
#include <QScreen>
#include <QDesktopWidget>
#include <QStyleHints>
#include <QTranslator>
#include <QTimer>
#include <QQueue>
#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QApplication>

#include "qtquick2applicationviewer.h"
#include "settingsdialog.h"
#include "imageprovider.h"
#include "loaddirectory.h"
#include "readExif.h"
#include "global.h"

class Supervisor : public QObject
{
    Q_OBJECT

    enum WaitingTask {
        START_TASK,
        PREV_TASK,
        NEXT_TASK,
        HIDE_HELP,
        HIDE_MESSAGE,
        STOP_TIMER,
        JUMP_FADE_OUT,
        DIR_LOAD,
        NO_TASK
    };

    enum InputMode {
        MODE_JUMPTO,
        MODE_TIMER_ON,
        MODE_TIMER_OFF,
        MODE_LARGE_DATA,
        NO_MODE
    };

public:
    explicit Supervisor(QObject *parent = 0);
    ~Supervisor();

    void setView(QtQuick2ApplicationViewer * view);
    void setDialog(SettingsDialog * dialog);

    void bindings();
    bool isQmlReady();
    
signals:
    void loadingFinished();
    void informationRequest();
    void refresh();
    void quit();

    // SIGNAL intended for QML
    void blendImage(QVariant slot_start, QVariant slot_end, QVariant forward);
    void blendOutShow(QVariant slot_current);
    void loadImage(QVariant image, QVariant slot);
    void showMessage(QVariant image, QVariant title, QVariant text, QVariant info);
    void waiting(QVariant state);
    void showHelp();
    void startTheShow(QVariant slot_current);
    void languageChanged(QVariant lang);
    void startPanorama();
    void stopPanorama();
    void infoBox();
    void bubbleBox(QVariant text, QVariant timeout);
    void blendJumpToPreview();
    void fadeToJumpToPreview(QVariant slot_current);

    
public slots:
    void applyNewOptions();

    Q_INVOKABLE void directoryLoadingFinished(bool success);
    Q_INVOKABLE void blendingFinished();
    Q_INVOKABLE int blendingCount();
    Q_INVOKABLE void overlayTransitionFinished();
    Q_INVOKABLE int overlayTransitionCount();
    Q_INVOKABLE bool showLoaded();
    Q_INVOKABLE void waitingFinished();
    Q_INVOKABLE void imageLoadingFinished(QVariant slot);
    Q_INVOKABLE void startShowFinished();
    Q_INVOKABLE void jumtoPreviewReady();
    Q_INVOKABLE bool isFullscreen();
    Q_INVOKABLE bool isInfoActive();
    Q_INVOKABLE QVariant getImageNumSlashTotalNumber();
    Q_INVOKABLE QVariant getExifTagOfCurrent(QVariant tagname);

    Q_INVOKABLE void panoramaError(QVariant title, QVariant text);
    Q_INVOKABLE QVariant getAppVersion();

    // INPUT EVENTS
    void keyPressEvent( QKeyEvent * event );
//    void keyReleaseEvent ( QKeyEvent * event );
//    void mouseMoveEvent ( QMouseEvent * event );
    void mouseDoubleClickEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );
    void mouseWheelEvent ( QWheelEvent * event );
    void leftMousePress ();
    void resizeEvent( QResizeEvent * event );

private slots:
    void startDirectoryLoading(bool forceLargeData = false);
    void nextImagePressed();
    void prevImagePressed();
    void jumpToImage(bool fromPreview = false);

    void processWaitingQueue();
    void changeLanguage(QVariant language);

    void numberPressed(const QString & number);
    void upOrDownPressed(bool up);
    void showJumpToPreview();
    bool overlayActive();
    void hideOverlays();

    void showHelpOverlay();
    void hideHelpOverlay();

    void showCustomMessage(QString imageUrl, QString title, QString text, int timeout = 0, bool info = false);
    void hideMessage();

    void queueTask(WaitingTask task);

    void startInputMode(InputMode mode, int timeout = 5000);
    void inputModeTimeout();
    void timerInputValueTimeout();

    void errorOccurred(const QList<QQmlError> &warnings);

private:
    int giveFreeSlot(int occupied1, int occupied2);
    QPoint checkValidPosition(QPoint suggestion, QSize sizeAsInfo);
    bool isLastImage();
    bool isFirstImage();
    bool anyBlendingsActive();
    void printState();

    // HELPER CLASSES
    SettingsDialog * m_setDialog;
    QTranslator m_translator;
    QtQuick2ApplicationViewer * m_quickView;
    loadDirectory * m_dirLoader;
    ImageProvider * m_imgProvider;

    // Automatic Timer and Wait Timer
    QTimer * m_automaticForward;
    bool m_automaticForwardActive;
    QTimer * m_messageTimeout;
    QTimer * m_inputMessageTimeout;
    QTimer * m_inputTimeout;
    QTimer * m_mousePressTimer;
    QTimer * m_jumptoPreview;
    QQueue<WaitingTask> m_wTask;

    // LIST containing all images
    QList<QFileInfo> m_current_directory_list;

    // Image Slots
    int m_cur;
    int m_prev;
    int m_next;

    // LOADING STATES
    bool m_showLoaded;
    bool m_currentlyLoading;
    ImageState m_loadingStateCurrent;
    ImageState m_loadingStateNext;
    ImageState m_loadingStatePrev;
    ImageState m_loadingStateJumpto;

    // Overlay STATES
    bool m_helpActive;
    bool m_messageActive;
    bool m_infoActive;

    // STATE VALUES
    bool m_qml_ready;
    int m_currentIndex;
    int m_timerValue;
    QString m_timerInputValue;
    QString m_jumpToImageValue;
    int m_blendingsActive;
    int m_overlayTransitions;
    bool m_waitingActive;
    bool m_currentlyWaiting;
    bool m_panoramaModeActive;
    bool m_exitRequested;
    InputMode m_activeInputMode;
    bool m_jumptoPreviewVisible;
    bool m_largeDataDetected;
};

#endif // SUPERVISOR_H
