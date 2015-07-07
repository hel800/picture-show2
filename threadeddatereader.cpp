#include "threadeddatereader.h"
#include "readExif.h"

threadedDateReader::threadedDateReader()
{
    this->m_Container = NULL;
}

void threadedDateReader::setResultContainer(QPair<QFileInfo, QDateTime> *container )
//void threadedDateReader::setResultContainer( QList< QPair<QFileInfo, QDateTime> > & container, int pos )

{
    this->m_Container = container;
}

void threadedDateReader::run()
{
    if (this->m_Container == NULL)
    {
        return;
    }

    QFileInfo info = this->m_Container->first;
    QDateTime date = readOriginalDate(info.absoluteFilePath());

    this->m_Container->second = date;
}
