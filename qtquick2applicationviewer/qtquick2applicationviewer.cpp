// checksum 0x56a9 version 0x80001
/*
  This file was generated by the Qt Quick 2 Application wizard of Qt Creator.
  QtQuick2ApplicationViewer is a convenience class containing mobile device specific
  code such as screen orientation handling. Also QML paths and debugging are
  handled here.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

#include "qtquick2applicationviewer.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtQml/QQmlEngine>

#include <iostream>

class QtQuick2ApplicationViewerPrivate
{
    QString mainQmlFile;
    friend class QtQuick2ApplicationViewer;
    static QString adjustPath(const QString &path);
};

QString QtQuick2ApplicationViewerPrivate::adjustPath(const QString &path)
{
#if defined(Q_OS_MAC)
    if (!QDir::isAbsolutePath(path))
        return QString::fromLatin1("%1/../Resources/%2")
                .arg(QCoreApplication::applicationDirPath(), path);
#elif defined(Q_OS_UNIX)
    const QString pathInInstallDir =
            QString::fromLatin1("%1/../%2").arg(QCoreApplication::applicationDirPath(), path);
    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#endif
    return path;
}

QtQuick2ApplicationViewer::QtQuick2ApplicationViewer(QWindow *parent)
    : QQuickView(parent)
    , d(new QtQuick2ApplicationViewerPrivate())
{
    connect(engine(), SIGNAL(quit()), SLOT(close()));
    setResizeMode(QQuickView::SizeRootObjectToView);

    this->positionBeforeFullscreen = QPoint(50, 50);
    this->sizeBeforeFullscreen = QSize(850, 550);
}

QtQuick2ApplicationViewer::~QtQuick2ApplicationViewer()
{
    delete d;
}

void QtQuick2ApplicationViewer::setMainQmlFile(const QString &file)
{
//    d->mainQmlFile = QtQuick2ApplicationViewerPrivate::adjustPath(file);
//    setSource(QUrl::fromLocalFile(d->mainQmlFile));
    setSource(QUrl(file));
}

void QtQuick2ApplicationViewer::addImportPath(const QString &path)
{
    engine()->addImportPath(QtQuick2ApplicationViewerPrivate::adjustPath(path));
}

void QtQuick2ApplicationViewer::hideCursorOnFullscreen()
{
#if defined(Q_OS_WIN32)
    if (this->windowState() == Qt::WindowMaximized)
        setCursor(Qt::BlankCursor);
#else
    if (this->windowState() == Qt::WindowFullScreen)
        setCursor(Qt::BlankCursor);
#endif
}

void QtQuick2ApplicationViewer::showExpanded(bool fullscreen)
{
#if defined(Q_OS_WIN32)
    if (fullscreen)
    {
        this->positionBeforeFullscreen = this->position();
        this->sizeBeforeFullscreen = this->size();
        this->setFlags(Qt::FramelessWindowHint | Qt::Window/* | Qt::WindowStaysOnTopHint*/);
        this->showMaximized();
        this->setCursor(Qt::BlankCursor);
    }
    else
    {
        this->setFlags(Qt::Window);
        this->unsetCursor();
        this->showNormal();
        this->setIcon(QIcon(":/img/picture-show.ico"));
        this->setPosition(this->positionBeforeFullscreen);
        this->resize(this->sizeBeforeFullscreen);
    }
#else
    if (fullscreen)
    {
        this->positionBeforeFullscreen = this->position();
        this->sizeBeforeFullscreen = this->size();
//        this->setFlags(Qt::FramelessWindowHint | Qt::Window/* | Qt::WindowStaysOnTopHint*/);
        this->showFullScreen();
        this->setCursor(Qt::BlankCursor);
    }
    else
    {
        this->setFlags(Qt::Window);
        this->unsetCursor();
        this->showNormal();
        this->setIcon(QIcon(":/img/picture-show.ico"));
        //this->setPosition(this->positionBeforeFullscreen);
        //this->resize(this->sizeBeforeFullscreen);
    }
#endif
}

bool QtQuick2ApplicationViewer::isExpanded()
{
#if defined(Q_OS_WIN32)
    return this->windowState() == Qt::WindowMaximized;
#else
    return this->windowState() == Qt::WindowFullScreen;
#endif
}

void QtQuick2ApplicationViewer::setWindowProps(QPoint pos, QSize size)
{
    this->positionBeforeFullscreen = pos;
    this->sizeBeforeFullscreen = size;
    this->setPosition(this->positionBeforeFullscreen);
    this->resize(this->sizeBeforeFullscreen);
}

QPoint QtQuick2ApplicationViewer::getLastWindowPosBeforeFullscreen()
{
    return this->positionBeforeFullscreen;
}

QSize QtQuick2ApplicationViewer::getLastWindowSizeBeforeFullscreen()
{
    return this->sizeBeforeFullscreen;
}

void QtQuick2ApplicationViewer::mousePressEvent( QMouseEvent * event)
{
//    this->lastMousePos = event->globalPos();
//    if ((event->buttons() & Qt::LeftButton) && (this->width() - (event->pos().x()) < 32) && (this->height() - (event->pos().y()) < 32) && (this->windowState() != Qt::WindowMaximized))
//    {
//        this->bottomRightHandle = true;
//    }

    emit mousePressed(event);
}

void QtQuick2ApplicationViewer::mouseMoveEvent( QMouseEvent * event)
{
    if ((!this->isExpanded()) && (event->buttons() & Qt::LeftButton))
    {
        QPoint distance = this->lastMousePos - event->globalPos();
        this->setPosition(this->position() - distance);
    }

    this->lastMousePos = event->globalPos();

    emit mouseMoved(event);
}

void QtQuick2ApplicationViewer::mouseReleaseEvent( QMouseEvent * event)
{
    emit mouseReleased(event);
}



