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

#include <time.h>

loadDirectory::loadDirectory() : QThread()
{
    this->m_dirList = NULL;
    this->m_subdirs = false;
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

QString& loadDirectory::getErrorMsg()
{
    return this->m_error_msg;
}

void loadDirectory::run()
{
//    clock_t start, end;
//    start = clock();

    QStringList filters;
    filters << "*.jpeg" << "*.jpg" << "*.JPG" << "*.JPEG";
    filters << "*.bmp" << "*.BMP";
    filters << "*.gif" << "*.GIF";
    filters << "*.png" << "*.PNG";
    filters << "*.tif" << "*.tiff" << "*.TIF" << "*.TIFF";

    QDir current_dir = QDir(this->m_path);
    if (!current_dir.exists())
    {
        this->m_error_msg = tr("Das angegebene Verzeichnis existiert nicht!");
        emit loadDirectoryFinished(false);
        return;
    }

    this->m_dirList->clear();

    QList<QFileInfo> tempList;

    if (this->m_subdirs)
        this->addItemsInDir(tempList, filters, current_dir);
    else
        tempList.append(current_dir.entryInfoList(filters, QDir::Files, QDir::Name | QDir::IgnoreCase));

    if (this->m_sorting == DATE_CREATED)
    {
        QList< QPair<QFileInfo, QDateTime> > temp2list;

        foreach(QFileInfo info, tempList)
        {
            QDateTime date = readOriginalDate(info.absoluteFilePath());
            QPair<QFileInfo, QDateTime> pair;
            pair.first = info;
            pair.second = date;
            temp2list << pair;
        }

        qSort(temp2list.begin(), temp2list.end(), fileCreateLessThan);

        for (int i = 0; i < temp2list.size(); i++)
            this->m_dirList->append(temp2list.at(i).first);
    }
    else
    {
        for (int i = 0; i < tempList.size(); i++)
            this->m_dirList->append(tempList.at(i));
    }

//    end = clock();
//    qDebug(QString::number(end - start).toStdString().c_str());

    emit loadDirectoryFinished(true);
}

void loadDirectory::addItemsInDir(QList<QFileInfo> & t_list, QStringList t_filters, QDir t_directory)
{
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

    t_list.append(t_directory.entryInfoList(t_filters, QDir::Files, QDir::Name | QDir::IgnoreCase));
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
