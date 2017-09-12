/* ------------------------------------------------------------------
xmpinfo.h is part of picture-show2.
.....................................................................

picture-show 2 is free software: you can redistribute it and/or modify
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
Juli 2015

--------------------------------------------------------------------*/
#ifndef BLUBBINFO_H
#define BLUBBINFO_H

#include <string>
#include <QFile>
#include <QString>
#include <QVariant>

class XMPInfo
{
public:
    short m_Rating;

    XMPInfo();
    virtual ~XMPInfo();

    bool ParseImage(const QString &filename);
    std::string GetError();

protected:
    long m_AppHeaderLength;
    long m_AppHeaderStart;

    std::string m_LastError;

    long GetXMPHeaderStartPosition(const QByteArray &buffer);
    long GetXMPHeaderEndPosition(const QByteArray &buffer, long start);

    QString GetDescriptionValueFromXML(const QString xml, const QString tag);

//    long GetNextHeaderInfo(const unsigned char * buffer,char* type,std::string &content);
//    void ParseHeader(unsigned char* buffer);
//    unsigned char* Get8BIMHeader(unsigned char* buffer, const char* type, unsigned long &len);
//    unsigned char* GetIPTCHeader(unsigned char* buffer,const char* type, unsigned long &len);
//    unsigned char* GetAppHeader(unsigned char* file, const char* type, unsigned long &len);
};


#endif // XMPINFO_H
