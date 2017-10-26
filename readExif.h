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
#include <stdio.h>

#include "iptcinfo.h"

static QDateTime readOriginalDate(const QString &fname)
{
    QDateTime originalDate;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        return originalDate;
    }

    QByteArray rawBuffer = file.read(100000);
    const unsigned char * buf = (const unsigned char *) rawBuffer.constData();
    unsigned fsize = rawBuffer.size();

    EXIFInfo result;
    int code = result.parseFrom(buf, fsize);

    if (code)
    {
        file.close();
        qDebug("Exif parsing failed!");
        return originalDate;
    }
    else
    {
        file.close();
    }

    QString date = QString::fromStdString(result.DateTimeOriginal);
    if(!date.isEmpty())
    {
        originalDate = QDateTime::fromString(date, QString("yyyy:MM:dd HH:mm:ss"));
    }

    return originalDate;
}

static short readRating(const QString &fname)
{
    short rating = -1;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        return rating;
    }

    QByteArray rawBuffer = file.read(100000);
    const unsigned char * buf = (const unsigned char *) rawBuffer.constData();
    unsigned fsize = rawBuffer.size();

    EXIFInfo result;
    int code = result.parseFrom(buf, fsize);

    if (code)
    {
        file.close();
        qDebug("Exif parsing failed!");
        return rating;
    }
    else
    {
        file.close();
    }

    rating = result.Rating;

    return rating;
}

static EXIFInfo readExifHeader(const QString &fname)
{
    EXIFInfo headerData;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        return headerData;
    }

    QByteArray rawBuffer = file.readAll();
    const unsigned char * buf = (const unsigned char *) rawBuffer.constData();
    unsigned int fsize = static_cast<unsigned int>(file.size());
    file.close();

    int code = headerData.parseFrom(buf, fsize);

    if (code)
    {
        qDebug("Error parsing EXIF!");
    }

    headerData.Make = QString::fromStdString(headerData.Make).trimmed().toStdString();
    headerData.Model = QString::fromStdString(headerData.Model).trimmed().toStdString();
    headerData.ImageDescription = QString::fromStdString(headerData.ImageDescription).trimmed().toStdString();

    // If IPTC caption is not empty, copy it to exif class
    IPTCInfo iptc_class;
    iptc_class.OpenFile(fname.toStdString());
    QString new_cap = QString::fromUtf8(iptc_class.m_Caption.c_str());

    if (new_cap.contains(QChar(QChar::ReplacementCharacter)))
        new_cap = QString::fromLatin1(iptc_class.m_Caption.c_str());

    if (!new_cap.isEmpty())
        headerData.ImageDescription = new_cap.trimmed().toStdString();

    return headerData;
}


#endif // READEXIF_H
