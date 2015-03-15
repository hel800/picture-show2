/* ------------------------------------------------------------------
imageprovider.cpp is part of picture-show2.
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

#include "imageprovider.h"
#include <iostream>

#include <time.h>

ImageProvider::ImageProvider(QList<QFileInfo> *list, int *ind)
    : QQuickImageProvider(QQuickImageProvider::Pixmap, QQmlImageProviderBase::ForceAsynchronousImageLoading)
{
    this->m_dirList = list;
    this->m_currentIndex = ind;

    ImageState dummy_next = IMAGE_READY;
    this->m_loadingCur = &dummy_next;
    this->m_loadingNext = &dummy_next;
    this->m_loadingPrev = &dummy_next;
    this->m_loadingJumpto = &dummy_next;

    this->m_currentCacheSize = 0;
    this->m_maxCacheSize = 209715200;

    this->m_currentLoadingType = MEMORY_OPTIMIZED;

    this->m_optimalScreenSize = QSize(2304, 1296);
}

ImageProvider::~ImageProvider()
{

}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap pixmap;
    if (id == "current")
    {
        if (*this->m_currentIndex < this->m_dirList->size() && *this->m_currentIndex >= 0)
        {
            *this->m_loadingCur = IMAGE_LOADING;
            QString fname = this->m_dirList->at(*this->m_currentIndex).absoluteFilePath();

            this->lookForCachedPixmap(fname, pixmap);
            if (pixmap.isNull())
                this->loadNewPixmap(fname, pixmap);

            // still null --> error
            if (pixmap.isNull())
            {
                *this->m_loadingCur = IMAGE_ERROR;
                pixmap = QPixmap(1,1);
                pixmap.fill(QColor(0, 0, 0, 0));
            }
            else
            {
                *this->m_loadingCur = IMAGE_READY;
            }
        }
    }
    else if (id == "next")
    {
        if (*this->m_currentIndex < this->m_dirList->size() && *this->m_currentIndex >= 0)
        {
            int correctedIndex = *this->m_currentIndex + 1;
            if (correctedIndex == this->m_dirList->size())
                correctedIndex = 0;

            *this->m_loadingNext = IMAGE_LOADING;
            QString fname = this->m_dirList->at(correctedIndex).absoluteFilePath();

            this->lookForCachedPixmap(fname, pixmap);
            if (pixmap.isNull())
                this->loadNewPixmap(fname, pixmap);

            // still null --> error
            if (pixmap.isNull())
            {
                *this->m_loadingNext = IMAGE_ERROR;
                pixmap = QPixmap(1,1);
                pixmap.fill(QColor(0, 0, 0, 0));
            }
            else
            {
                *this->m_loadingNext = IMAGE_READY;
            }
        }
    }
    else if (id == "prev")
    {
        if (*this->m_currentIndex < this->m_dirList->size() && *this->m_currentIndex >= 0)
        {
            int correctedIndex = *this->m_currentIndex - 1;
            if (correctedIndex < 0)
                correctedIndex = this->m_dirList->size() - 1;

            *this->m_loadingPrev = IMAGE_LOADING;
            QString fname = this->m_dirList->at(correctedIndex).absoluteFilePath();

            this->lookForCachedPixmap(fname, pixmap);
            if (pixmap.isNull())
                this->loadNewPixmap(fname, pixmap);

            // still null --> error
            if (pixmap.isNull())
            {
                *this->m_loadingPrev = IMAGE_ERROR;
                pixmap = QPixmap(1,1);
                pixmap.fill(QColor(0, 0, 0, 0));
            }
            else
            {
                *this->m_loadingPrev = IMAGE_READY;
            }
        }
    }
    else if (id.section("/", -1).section("_", 0, 0) == "jumpto")
    {
        QString jumpto_number = id.section("/", -1).section("_", 1);

        bool ok = false;
        int nr = jumpto_number.toInt(&ok) - 1;

        if (ok && nr >= 0 && nr < this->m_dirList->size())
        {
            *this->m_loadingJumpto = IMAGE_LOADING;
            QString fname = this->m_dirList->at(nr).absoluteFilePath();

            this->lookForCachedPixmap(fname, pixmap);
            if (pixmap.isNull())
                this->loadNewPixmap(fname, pixmap);

            // still null --> error
            if (pixmap.isNull())
            {
                *this->m_loadingJumpto = IMAGE_ERROR;
                pixmap = QPixmap(1,1);
                pixmap.fill(QColor(0, 0, 0, 0));
            }
            else
            {
                *this->m_loadingJumpto = IMAGE_READY;
            }
        }
    }

    return pixmap;
}

void ImageProvider::setLoadingPointers(ImageState *cur, ImageState *next, ImageState *prev, ImageState *jump)
{
     this->m_loadingCur = cur;
     this->m_loadingNext = next;
     this->m_loadingPrev = prev;
     this->m_loadingJumpto = jump;
}

bool ImageProvider::getJumpLoadingFinished()
{
    return (this->m_loadingJumpCounter == 0);
}

void ImageProvider::setCacheSize(size_t bytes)
{
    this->m_maxCacheSize = bytes;
}

void ImageProvider::deleteCache()
{
    for (int i = 0; i < this->m_cacheDB.size(); i++)
    {
        QPair<QString, QPixmap*> data = this->m_cacheDB.at(i);
        delete data.second;
    }

    this->m_cacheDB.clear();
    this->m_exifMap.clear();
}

void ImageProvider::setLoadingType(LoadingType type)
{
    this->m_currentLoadingType = type;
}

EXIFInfo& ImageProvider::getExifForFile(QString fname)
{
    return this->m_exifMap[fname];
}

void ImageProvider::setVirtualScreenSize(QSize size)
{
    int opt_w = size.width();
    int opt_h = size.height();
    if (size.width() > size.height() * 1.7)
        opt_w = opt_h * 1.8;
    else if (size.height() > size.width())
        opt_h = opt_w * 0.8;

    this->m_optimalScreenSize = QSize(opt_w, opt_h);
}

void ImageProvider::lookForCachedPixmap(QString fname, QPixmap & pmap)
{
    for (int i = 0; i < this->m_cacheDB.size(); i++)
    {
        QPair<QString, QPixmap*> data = this->m_cacheDB.at(i);
        if (data.first == fname)
        {
            pmap = QPixmap(*data.second);

            this->m_cacheDB.removeAt(i);
            this->m_cacheDB.prepend(data);

            return;
        }
    }
}

void ImageProvider::loadNewPixmap(QString fname, QPixmap & pmap)
{
    QPixmap image(fname);
    QPixmap * new_pixmap;

    double panorama_downscale = 0.7;  // his is necessary for older graphics cards because of limited video memory

    if (this->m_currentLoadingType == PERFORMANCE_OPTIMIZED)
    {
        new_pixmap = new QPixmap(image);
    }
    else
    {
        // panorama
        if (image.width() > image.height() * 3 && image.height() > this->m_optimalScreenSize.height() * panorama_downscale)
            new_pixmap = new QPixmap(image.scaledToHeight(this->m_optimalScreenSize.height() * panorama_downscale, Qt::SmoothTransformation));
        else if (image.height() > image.width() * 3 && image.width() > this->m_optimalScreenSize.width() * 0.8)
            new_pixmap = new QPixmap(image.scaledToWidth(this->m_optimalScreenSize.width() * 0.8, Qt::SmoothTransformation));
        else if (image.width() > image.height() && image.height() > this->m_optimalScreenSize.height())
            new_pixmap = new QPixmap(image.scaledToHeight(this->m_optimalScreenSize.height(), Qt::SmoothTransformation));
        else if (image.width() <= image.height() && image.width() > this->m_optimalScreenSize.width())
            new_pixmap = new QPixmap(image.scaledToWidth(this->m_optimalScreenSize.width(), Qt::SmoothTransformation));
        else
            new_pixmap = new QPixmap(image);
    }

    if (new_pixmap->isNull())
        return;

    //std::cout << new_pixmap->width() << " x " << new_pixmap->height() << " = " << new_pixmap->width() * new_pixmap->height() << std::endl;

    // read exif to cache and to get orientation information
    EXIFInfo exif = readExifHeader(fname);
    exif.resolution_st = QObject::tr("%1 x %2").arg(image.width()).arg(image.height());

    this->m_exifMap.insert(fname, exif);

    // adjust orientation and rotation of image
    if (exif.orientation != 1)
    {
        QTransform trans;
        QImage newImage = new_pixmap->toImage();

        switch (exif.orientation)
        {
        case 2:
            newImage = newImage.mirrored(true, false);
        break;
        case 3:
            trans.rotate(180.0);
        break;
        case 4:
            newImage = newImage.mirrored(false, true);
        break;
        case 5:
            newImage = newImage.mirrored(true, false);
            trans.rotate(270.0);
        break;
        case 6:
            trans.rotate(90.0);
        break;
        case 7:
            newImage = newImage.mirrored(true, false);
            trans.rotate(90.0);
        break;
        case 8:
            trans.rotate(270.0);
        break;
        }

        delete new_pixmap;
        new_pixmap = NULL;
        new_pixmap = new QPixmap(QPixmap::fromImage(newImage.transformed(trans)));
    }

    pmap = QPixmap(*new_pixmap);

    this->m_cacheDB.prepend(QPair<QString, QPixmap*>(fname, new_pixmap));
    this->m_currentCacheSize += (new_pixmap->width() * new_pixmap->height() * 4);
    while (this->m_cacheDB.size() != 0 && (this->m_cacheDB.size() > 15 || this->m_currentCacheSize > this->m_maxCacheSize))
    {
        QPixmap * toDelete = this->m_cacheDB.last().second;
        this->m_currentCacheSize -= (toDelete->width() * toDelete->height() * 4);
        delete toDelete;
        this->m_cacheDB.removeLast();
    }
}
