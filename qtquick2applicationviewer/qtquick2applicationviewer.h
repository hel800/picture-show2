// checksum 0xfde6 version 0x80001
/*
  This file was generated by the Qt Quick 2 Application wizard of Qt Creator.
  QtQuick2ApplicationViewer is a convenience class containing mobile device specific
  code such as screen orientation handling. Also QML paths and debugging are
  handled here.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

#ifndef QTQUICK2APPLICATIONVIEWER_H
#define QTQUICK2APPLICATIONVIEWER_H

#include <QtQuick/QQuickView>

class QtQuick2ApplicationViewer : public QQuickView
{
    Q_OBJECT

public:
    explicit QtQuick2ApplicationViewer(QWindow *parent = 0);
    virtual ~QtQuick2ApplicationViewer();

    void setMainQmlFile(const QString &file);
    void addImportPath(const QString &path);

public slots:
    void hideCursorOnFullscreen();
    void showExpanded(bool fullscreen = false);
    bool isExpanded();
    void setWindowProps(QPoint pos, QSize size);
    QPoint getLastWindowPosBeforeFullscreen();
    QSize getLastWindowSizeBeforeFullscreen();

signals:
    void keyPressed( QKeyEvent * event );
    void keyReleased( QKeyEvent * event );
    void mousePressed( QMouseEvent * event );
    void mouseMoved( QMouseEvent * event );
    void mouseReleased( QMouseEvent * event );
    void mouseDoubleClicked( QMouseEvent * event );
    void mouseWheelTurned( QWheelEvent * event );
    void windowResized( QResizeEvent * event );

protected:
    void keyPressEvent( QKeyEvent * event ) { emit keyPressed(event); }
    void keyReleaseEvent( QKeyEvent * event ) { emit keyReleased(event); }
    void mousePressEvent( QMouseEvent * event);
    void mouseMoveEvent( QMouseEvent * event);
    void mouseReleaseEvent( QMouseEvent * event);
    void mouseDoubleClickEvent( QMouseEvent * event) { emit mouseDoubleClicked(event); }
    void wheelEvent( QWheelEvent * event ) { emit mouseWheelTurned(event); }
    void resizeEvent(QResizeEvent *event);

private:
    class QtQuick2ApplicationViewerPrivate *d;

    QPoint lastMousePos;
    QPoint positionBeforeFullscreen;
    QSize sizeBeforeFullscreen;
};

#endif // QTQUICK2APPLICATIONVIEWER_H
