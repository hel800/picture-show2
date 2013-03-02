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
       delete[] buf;
       buf = new unsigned char[fsize];

       file.seek(0);
       qint64 newSize = file.read((char*)buf, fsize);
//       qDebug((fname + QString(" --> ") + QString::number(newSize)).toStdString().c_str());
        if (newSize != fsize)
        {
            qDebug("Can't read file2");
            delete[] buf;
            file.close();
            return originalDate;
        }

        qDebug("Try to read Date from whole file!");

        ExtractDateTime(buf, fsize, date);
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

    ParseEXIF(buf, cappedSize, headerData);

    // If IPTC caption is not empty, copy it to exif class
    IPTCInfo iptc_class;
    iptc_class.OpenFile(fname.toStdString());
    QString new_cap = QString::fromUtf8(iptc_class.m_Caption.c_str());

    if (new_cap.contains(QChar(QChar::ReplacementCharacter)))
        new_cap = QString::fromLatin1(iptc_class.m_Caption.c_str());

    if (!new_cap.isEmpty())
        headerData.imgDescription_st = new_cap;

    return headerData;
}


#endif // READEXIF_H
