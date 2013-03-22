#ifndef READDIRLIST_H
#define READDIRLIST_H

#include <QThread>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QUrl>

class readDirList : public QThread
{
    Q_OBJECT
public:
    explicit readDirList(QObject *parent = 0);

    void setUrlList(QList<QUrl> & u_list);
    QList<QUrl> getUrlList();
    QList<QFileInfo> getItemList();

    void run();
    void cancel();

    bool success;
    
signals:
    void listready();
    void canceled();
    
public slots:

private:
    QList<QUrl> m_urlList;
    QList<QFileInfo> m_itemList;
    bool cancel_operations;

    void addItemsInDir(QList<QFileInfo> *t_list, QStringList t_filters, QDir t_directory);
    
};

#endif // READDIRLIST_H
