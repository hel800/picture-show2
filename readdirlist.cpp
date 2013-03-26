#include "readdirlist.h"

readDirList::readDirList(QObject *parent) :
    QThread(parent)
{
    this->cancel_operations = false;
    this->success = false;
}

void readDirList::setUrlList(QList<QUrl> & u_list)
{
    this->m_urlList = QList<QUrl>(u_list);
}

QList<QUrl> readDirList::getUrlList()
{
    return this->m_urlList;
}

QList<QFileInfo> readDirList::getItemList()
{
    return this->m_itemList;
}

void readDirList::run()
{
    this->success = false;
    this->m_itemList.clear();

    QStringList filters;
    filters << "jpeg" << "jpg" << "JPG" << "JPEG";
    filters << "bmp" << "BMP";
    filters << "gif" << "GIF";
    filters << "png" << "PNG";
    filters << "tif" << "tiff" << "TIF" << "TIFF";

    QStringList filters2;
    filters2 << "*.jpeg" << "*.jpg" << "*.JPG" << "*.JPEG";
    filters2 << "*.bmp" << "*.BMP";
    filters2 << "*.gif" << "*.GIF";
    filters2 << "*.png" << "*.PNG";
    filters2 << "*.tif" << "*.tiff" << "*.TIF" << "*.TIFF";


    foreach(QUrl url, this->m_urlList)
    {
        QFileInfo url_path(url.toLocalFile());
        if (url_path.isDir())
        {
            QDirIterator iterator(url_path.absoluteFilePath(), QDirIterator::Subdirectories);
            while (iterator.hasNext())
            {
                iterator.next();
                if (!iterator.fileInfo().isDir())
                {
                    if (filters.contains(iterator.fileInfo().suffix()))
                        this->m_itemList.append(iterator.fileInfo());
                }

                if (this->cancel_operations)
                {
                    this->cancel_operations = false;
                    this->success = false;
                    return;
                }
            }
        }
    }

//    foreach(QUrl url, this->m_urlList)
//    {
//        QFileInfo url_path(url.toLocalFile());
//        if (url_path.isDir())
//        {

//            QDir current_dir(url_path.absoluteFilePath());
//            this->addItemsInDir(&this->m_itemList, filters2, current_dir);
//        }
//        else if (url_path.isFile())
//        {
//            if (url_path.exists() && filters.contains(url_path.suffix()))
//            {
//                this->m_itemList.append(url_path);
//            }
//        }
//    }

//    if (this->cancel_operations)
//    {
//        this->cancel_operations = false;
//        this->success = false;
//        return;
//    }

    this->success = true;
    emit listready();
}

void readDirList::cancel()
{
    this->cancel_operations = true;
}


void readDirList::addItemsInDir(QList<QFileInfo> * t_list, QStringList t_filters, QDir t_directory)
{
    if (this->cancel_operations)
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

    if (this->cancel_operations)
        return;

    t_list->append(t_directory.entryInfoList(t_filters, QDir::Files, QDir::Name | QDir::IgnoreCase));
}
