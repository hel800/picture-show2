/* ------------------------------------------------------------------
settingsdialog.h is part of picture-show2.
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFileDialog>
#include <QSettings>
#include <QHideEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMimeData>
#include <QMessageBox>

#include "helpwindow.h"
#include "global.h"
#include "readdirlist.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    Q_ENUMS(ScaleType)
    Q_PROPERTY(double fadeTime READ getCurrentFadeTime NOTIFY propertiesChanged)
    Q_PROPERTY(ScaleType scale READ getScaleType NOTIFY propertiesChanged)
    
public:
    enum OpenMode {
        MODE_FOLDER,
        MODE_DROPLIST
    };

    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    OpenMode getOpenMode();

    QString getCurrentDirectory();
    void setCurrentDirectory(const QString & dir);
    double getCurrentFadeTime();
    int getCurrentFadeLength();
    BlendType getBlendType();
    Sorting getDirectorySorting();
    void setDirectorySorting(Sorting sort);
    bool getIncludeSubdirs();
    void setIncludeSubdirs(bool inc);
    bool getLoopSlideShow();
    void setLoopSlideShow(bool loop);
    ScaleType getScaleType();
    QString getLanguage();
    bool getMouseControl();
    LoadingType getLoadingType();
    void setLoadingType(LoadingType type);

    QList<QFileInfo> * getDroppedItems();
    bool getDropListChanged();

    void setTimerValue(int value);
    int getTimerValue();

    void setFullScreen(bool fullscreen);
    bool getFullScreen();

    void setWindowSize(QSize size);
    QSize getWindowSize();

    void setWindowPosition(QPoint size);
    QPoint getWindowPosition();

    void setFirstStart(bool state);
    bool getFirstStart();

    size_t getMaxCacheSize();

    void updateLanguage();
    void updateHistory();

    void addDirectoryToHistory(const QString & dir);

public slots:
    Q_INVOKABLE int getScaleTypeQml();
    Q_INVOKABLE QVariant getCurrentFadeLengthForQml();
    Q_INVOKABLE QVariant getBlendTypeForQml();
    Q_INVOKABLE QVariant getMouseControlForQml();
    Q_INVOKABLE QVariant getLanguageForQml();
    Q_INVOKABLE QVariant getBackgroundColorQml();

    void resizeDialog();

signals:
    void languageChanged(QVariant lang);
    void applyClicked();
    void propertiesChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void hideEvent(QHideEvent * event);
    
private slots:
    void networkReplyReady(QNetworkReply * reply);
    void readDirListReady();
    void readDirListCanceled();

    void on_pushButton_browse_clicked();
    void on_comboBox_language_currentIndexChanged(int index);

    void on_pushButton_deleteHistory_clicked();

    void on_comboBox_effect_currentIndexChanged(int index);
    void on_pushButton_help_clicked();

    void on_comboBox_scaling_currentIndexChanged(int index);

    void on_comboBox_bgColor_currentIndexChanged(int index);

    void on_comboBox_directoryPath_currentIndexChanged(int index);

    void on_pushButton_load_clicked();

    void on_pushButton_clearZone_clicked();

private:
    Ui::SettingsDialog *ui;
    bool languageChangeSignalOff;

    HelpWindow * m_helpWindow;
    QNetworkAccessManager * m_networkManager;

    // Drag and Drop Support
    readDirList * m_dirListReader;
    QList<QFileInfo> * m_droppedItemsList;
    QList<QUrl> m_cachedDropList;
    QList<QUrl> m_current_collection;
    bool m_dropListChanged;

    void loadSettings();
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
