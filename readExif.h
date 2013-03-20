/* ------------------------------------------------------------------
readExif.h is part of picture-show2.
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

#ifndef READEXIF_H
#define READEXIF_H

#include <QString>
#include <QDateTime>
#include <QFile>
#include <stdio.h>
#include "exif.h"

#include <iostream>

#include "iptcinfo.h"

static QDateTime readOriginalDate(const QString &fname)
{
    QDateTime originalDate;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("Can't read file");
        qDebug(fname.toStdString().c_str());
        return originalDate;
    }

    qint64 fsize = file.size();
    qint64 cappedSize = 1500;

    if (fsize < cappedSize)
        cappedSize = fsize;

    unsigned char *buf = new unsigned char[cappedSize];

    if (file.read((char*)buf, cappedSize) != cappedSize)
    {
        qDebug("Can't read file");
        delete[] buf;
        file.close();
        return originalDate;
    }

    QString date;
    ExtractDateTime(buf, cappedSize, date);

    if(!date.isEmpty())
    {
        originalDate = QDateTime::fromString(date, QString("yyyy:MM:dd HH:mm:ss"));
    }
    else
    {
       qDebug("CANNOT READ DATE!!!");
       delete[] buf;

       qint64 newCappedSize = 6000;

       if (fsize < newCappedSize)
           newCappedSize = fsize;

       buf = new unsigned char[newCappedSize];

       file.seek(0);
       qint64 newSize = file.read((char*)buf, newCappedSize);
        if (newSize != newCappedSize)
        {
            qDebug("Can't read file");
            delete[] buf;
            file.close();
            return originalDate;
        }

        qDebug("Try to read Date with larger crop value!");

        ExtractDateTime(buf, newCappedSize, date);
        qDebug(fname.toStdString().c_str());

        if(!date.isEmpty())
            originalDate = QDateTime::fromString(date, QString("yyyy:MM:dd HH:mm:ss"));
        else
            qDebug("CANNOT READ DATE!!!");
    }

    delete[] buf;
    file.close();

    return originalDate;
}

static EXIFInfo readExifHeader(const QString &fname)
{
    EXIFInfo headerData;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("Can't read file");
        qDebug(fname.toStdString().c_str());
        return headerData;
    }

    qint64 fsize = file.size();
    qint64 cappedSize = 3000;
    unsigned char * buf = new unsigned char[cappedSize];

    file.seek(0);
    qint64 newSize = file.read((char*)buf, cappedSize);
    if (newSize != cappedSize)
    {
        qDebug("Can't read file");
        delete[] buf;
        file.close();
        return headerData;
    }

    int ret = ParseEXIF(buf, cappedSize, headerData);
    QDateTime test = QDateTime::fromString(headerData.dateTimeOriginal_st, QString("yyyy:MM:dd HH:mm:ss"));

    if (ret != 0 || !test.isValid())
    {
        qDebug("CANNOT READ EXIF --> second try");
        delete[] buf;

        buf = new unsigned char[fsize];

        file.seek(0);
        qint64 newSize = file.read((char*)buf, fsize);
         if (newSize != fsize)
         {
             qDebug("Can't read file");
             delete[] buf;
             file.close();
             return headerData;
         }

         qDebug("Try to read EXIF from whole file!");

         int ret2 = ParseEXIF(buf, fsize, headerData);

         if(ret2 != 0)
             qDebug("CANNOT READ EXIF - final!!!");
    }

    headerData.cameraMake_st = headerData.cameraMake_st.trimmed();
    headerData.cameraModel_st = headerData.cameraModel_st.trimmed();
    headerData.imgDescription_st = headerData.imgDescription_st.trimmed();

    // If IPTC caption is not empty, copy it to exif class
    IPTCInfo iptc_class;
    iptc_class.OpenFile(fname.toStdString());
    QString new_cap = QString::fromUtf8(iptc_class.m_Caption.c_str());

    if (new_cap.contains(QChar(QChar::ReplacementCharacter)))
        new_cap = QString::fromLatin1(iptc_class.m_Caption.c_str());

    if (!new_cap.isEmpty())
        headerData.imgDescription_st = new_cap.trimmed();

    return headerData;
}


#endif // READEXIF_H
