/* ------------------------------------------------------------------
loaddirectory.cpp is part of picture-show2.
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
March 2013

--------------------------------------------------------------------*/

#include "loaddirectory.h"
#include "threadeddatereader.h"

#include "xmpinfo.h"
#include "xmptoolbox.h"

#include <time.h>
#include <QThreadPool>
#include <QElapsedTimer>

loadDirectory::loadDirectory() : QThread()
{
    this->m_sorting=DATE_CREATED;
    this->m_dirList = NULL;
    this->m_subdirs = false;
    this->m_RatingFilter = -1;
    this->m_dropList = NULL;
    this->m_forcing_large_data = false;

    this->m_openMode_drop = false;
}

void loadDirectory::setDirectory(const QString &path)
{
    this->m_path = path;
}

const QString& loadDirectory::getDirectory()
{
    return this->m_path;
}

void loadDirectory::setDirectoryList(QList<QFileInfo> * list)
{
    this->m_dirList = list;
}

void loadDirectory::setDropList(QSet<QString> *d_list)
{
    this->m_dropList = d_list;
}

void loadDirectory::setSorting(Sorting sortType)
{
    this->m_sorting = sortType;
}

Sorting loadDirectory::getSorting()
{
    return this->m_sorting;
}

bool loadDirectory::getIncludeSubdirs()
{
    return this->m_subdirs;
}

void loadDirectory::setIncludeSubdirs(bool sd)
{
    this->m_subdirs = sd;
}

void loadDirectory::setRatingFilter(short rating)
{
    m_RatingFilter = rating;
}

short loadDirectory::getRatingFilter()
{
    return m_RatingFilter;
}

void loadDirectory::setOpenModeToDropList()
{
    this->m_openMode_drop = true;
}

void loadDirectory::setOpenModeToFolder()
{
    this->m_openMode_drop = false;
}

void loadDirectory::setForceLargeData(bool state)
{
    this->m_forcing_large_data = state;
}

QString& loadDirectory::getErrorMsg()
{
    return this->m_error_msg;
}

void loadDirectory::run()
{
    //QElapsedTimer timer;
    //timer.start();

    QStringList filters;
    filters << "*.jpeg" << "*.jpg" << "*.JPG" << "*.JPEG";
    filters << "*.bmp" << "*.BMP";
    filters << "*.gif" << "*.GIF";
    filters << "*.png" << "*.PNG";
    filters << "*.tif" << "*.tiff" << "*.TIF" << "*.TIFF";

    QList<QString> tempList;

    if (this->m_openMode_drop)
    {
        if (this->m_dropList != NULL && this->m_dropList->isEmpty())
        {
            this->m_error_msg = tr("Die abgelegten Bilder sind ungültig");
            emit loadDirectoryFinished(false);
            return;
        }

        this->m_dirList->clear();

        tempList.append(this->m_dropList->toList());
    }
    else
    {
        QDir current_dir = QDir(this->m_path);
        if (!current_dir.exists())
        {
            this->m_error_msg = tr("Das angegebene Verzeichnis existiert nicht");
            emit loadDirectoryFinished(false);
            return;
        }

        if (!m_forcing_large_data && this->m_subdirs)
        {
            int num_dirs = 0;
            QDirIterator iterator(current_dir.absolutePath(), QDirIterator::Subdirectories);
            while (iterator.hasNext())
            {
                iterator.next();
                if (!iterator.fileInfo().isDir())
                    num_dirs++;

                if (num_dirs > LARGE_DATA3)
                    break;
            }

            if (num_dirs > LARGE_DATA3)
            {
                this->m_error_msg = tr("LARGE_DATA3");
                emit loadDirectoryFinished(false);
                return;
            }
        }


        this->m_dirList->clear();

        if (this->m_subdirs)
        {
            this->addItemsInDir(tempList, filters, current_dir);
        }
        else
        {
            QList<QFileInfo> f_info_list = current_dir.entryInfoList(filters, QDir::Files, QDir::Name | QDir::IgnoreCase);
            foreach (QFileInfo inf, f_info_list)
                tempList.append(inf.absoluteFilePath());
        }
    }


    // CHECK FOR RATING FILTER
    if ( m_RatingFilter > 0 )
    {
      tempList.erase( std::remove_if( tempList.begin(),
                                      tempList.end(),
                                      [&]( const QString& filename ) {
                                        short rating = getRatingOfImage( filename );
                                        // even index: inclusive filter - more than
                                        if ( m_RatingFilter % 2 == 0 )
                                        {
                                          return rating < m_RatingFilter / 2;
                                        }
                                        // odd index: exclusive filter
                                        else
                                        {
                                          auto ratingFil = ( m_RatingFilter + 1 ) / 2;
                                          return rating < ratingFil || rating > ratingFil;
                                        }
                                      } ),
                      tempList.end() );
    }

    // CHECK FOR ZERO IMAGES
    if (tempList.size() == 0)
    {
        QString ratingFilterAddon = "";
        if (m_RatingFilter > 1)
        {
            ratingFilterAddon = tr(", eventuell muss die Einstellung des Bewertungsfilters überprüft werden");
        }

        this->m_error_msg = tr("Das angegebene Verzeichnis enthält keine unterstützten Bilder") + ratingFilterAddon;
        emit loadDirectoryFinished(false);
        return;
    }


    // CHECK FOR TOO LARGE IMAGE LISTS
    if (!m_forcing_large_data && tempList.size() > LARGE_DATA)
    {
        this->m_error_msg = tr("LARGE_DATA");
        emit loadDirectoryFinished(false);
        return;
    }

    if (!m_forcing_large_data && this->m_sorting == DATE_CREATED && tempList.size() > LARGE_DATA2)
    {
        this->m_error_msg = tr("LARGE_DATA2");
        emit loadDirectoryFinished(false);
        return;
    }


    // SORTING
    if (this->m_sorting == DATE_CREATED)
    {
        std::vector< std::pair<QFileInfo, QDateTime> > temp2list;

        for(const auto& file : tempList)
        {
            QFileInfo info(file);
            //QDateTime date = readOriginalDate(info.absoluteFilePath());
            QDateTime date = XMPToolBox::readExifDate( file );
            std::pair<QFileInfo, QDateTime> pair;
            pair.first = info;
            pair.second = date;
            temp2list.push_back(pair);

//            threadedDateReader *reader = new threadedDateReader();
//            reader->setResultContainer(&temp2list.last());
//            QThreadPool::globalInstance()->start(reader);
        }

        //        QThreadPool::globalInstance()->waitForDone();

        std::sort( temp2list.begin(),
                   temp2list.end(),
                   []( std::pair<QFileInfo, QDateTime> f1, std::pair<QFileInfo, QDateTime> f2 ) {
                     QDateTime f1_date = f1.second;
                     QDateTime f2_date = f2.second;

                     if ( f1_date.isValid() && f2_date.isValid() )
                       return f1_date < f2_date;
                     else if ( f2_date.isValid() && !f1_date.isValid() )
                       return false;
                     else if ( f1_date.isValid() && !f2_date.isValid() )
                       return true;
                     else
                       return f1.first.fileName() < f2.first.fileName();
                   } );

        for ( const auto& p : temp2list )
        {
          this->m_dirList->append( p.first );
        }
    }
    else
    {
      for ( int i = 0; i < tempList.size(); i++ )
        this->m_dirList->append( tempList.at( i ) );

      qSort( this->m_dirList->begin(), this->m_dirList->end(), fileNameLessThan );
    }

    //qDebug( QString::number( timer.elapsed() ).toLocal8Bit().data() );

    emit loadDirectoryFinished( true );
}

void loadDirectory::addItemsInDir(QList<QString> &t_list, QStringList t_filters, QDir t_directory)
{
    if (!m_forcing_large_data && t_list.size() > LARGE_DATA)
        return;

    QList<QFileInfo> dirList = t_directory.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
    foreach (QFileInfo str_dir, dirList)
    {
        if (str_dir.isDir())
        {
            QDir cur_dir(str_dir.absoluteFilePath());
            if (cur_dir.exists())
                this->addItemsInDir(t_list, t_filters, cur_dir);
        }
    }

    if (!m_forcing_large_data && t_list.size() > LARGE_DATA)
        return;

    QList<QFileInfo> f_info_list = t_directory.entryInfoList(t_filters, QDir::Files, QDir::Name | QDir::IgnoreCase);
    foreach (QFileInfo inf, f_info_list)
        t_list.append(inf.absoluteFilePath());
}

short loadDirectory::getRatingOfImage(const QString & filename)
{
    // first check xmp
    XMPInfo xmp;
    xmp.ParseImage(filename);
    short rating = xmp.m_Rating;

    // fallback to exif rating
    if ( rating == -1 )
    {
        rating = readRating(filename);
    }

    return rating;
}

bool fileCreateLessThan(const QPair<QFileInfo, QDateTime> &f1, const QPair<QFileInfo, QDateTime> &f2)
{
    QDateTime f1_date = f1.second;
    QDateTime f2_date = f2.second;

    if (f1_date.isValid() && f2_date.isValid())
        return f1_date < f2_date;
    else if (f2_date.isValid() && !f1_date.isValid())
        return false;
    else if (f1_date.isValid() && !f2_date.isValid())
        return true;
    else
        return f1.first.fileName() < f2.first.fileName();
}

bool fileNameLessThan(const QFileInfo &f1, const QFileInfo &f2)
{
    int cmp = qstricmp(f1.fileName().toLatin1(), f2.fileName().toLatin1());

    if (cmp < 0)
        return true;
    else
        return false;
}
