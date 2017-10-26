#ifndef THREADEDDATEREADER_H
#define THREADEDDATEREADER_H

#include <QRunnable>
#include <QPair>
#include <QDateTime>
#include <QFileInfo>

class threadedDateReader : public QRunnable
{
public:
    threadedDateReader();

    void setResultContainer( QPair<QFileInfo, QDateTime> * container );
    void run();

private:
    QPair<QFileInfo, QDateTime> * m_Container;

};

#endif // THREADEDDATEREADER_H
