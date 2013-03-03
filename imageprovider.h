/* ------------------------------------------------------------------
imageprovider.h is part of picture-show2.
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

#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QFileInfo>

#include "exif.h"
#include "readExif.h"
#include "global.h"

class ImageProvider : public QQuickImageProvider
{


public:
    explicit ImageProvider(QList<QFileInfo> * list, int * ind);

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

    void setLoadingPointers(ImageState * cur, ImageState * next, ImageState * prev);

    void setCacheSize(size_t bytes);
    void deleteCache();

    void setLoadingType(LoadingType type);
    void setVirtualScreenSize(QSize size);

    EXIFInfo & getExifForFile(QString fname);

//signals:
//    void finishedLoading(const QString & id);

private:
    QList<QFileInfo> * m_dirList;
    int * m_currentIndex;

    ImageState * m_loadingCur;
    ImageState * m_loadingNext;
    ImageState * m_loadingPrev;

    // caching
    QList< QPair<QString, QPixmap *> > m_cacheDB;
    QMap< QString, EXIFInfo > m_exifMap;
    size_t m_currentCacheSize;
    size_t m_maxCacheSize;

    LoadingType m_currentLoadingType;

    QSize m_optimalScreenSize;

    void lookForCachedPixmap(QString fname, QPixmap & pmap);
    void loadNewPixmap(QString fname, QPixmap & pmap);
};

#endif // IMAGEPROVIDER_H
