/* ------------------------------------------------------------------
iptcinfo.h is part of picture-show2.
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

IPTCInfo class has been adapted from code
of Christian Tratz (cat@cat-software.de)

author: Christian Tratz, Sebastian Schäfer
February 2013

--------------------------------------------------------------------*/

#ifndef IPTCINFO_H
#define IPTCINFO_H

#include <string>
#include <QFile>
#include <QString>

class IPTCInfo
{
public:
    //credits
    std::string m_Byline;
    std::string m_BylineTitle;
    std::string m_Credits;
    std::string m_Source;
    //caption
    std::string m_CaptionWriter;
    std::string m_Caption;
    std::string m_Headline;
    std::string m_SpecialInstructions;
    //Origin
    std::string m_ObjectName;
    std::string m_DateCreated;
    std::string m_City;
    std::string m_State;
    std::string m_Country;
    std::string m_OriginalTransmissionReference;
    //categories
    std::string m_Category;
//    std::stringArray m_SupplementalCategories;
    //keywords
//    std::stringArray m_Keywords;
    //copyright
    std::string m_CopyrightNotice;

//    int SaveHeaders(const std::string filename);
    int OpenFile(const std::string& filename);
    IPTCInfo();
    virtual ~IPTCInfo();

protected:
    long m_AppHeaderLength;
    long m_AppHeaderStart;
    long GetNextHeaderInfo(const unsigned char * buffer,char* type,std::string &content);
    void ParseHeader(unsigned char* buffer);
    unsigned char* Get8BIMHeader(unsigned char* buffer, const char* type, unsigned long &len);
    unsigned char* GetIPTCHeader(unsigned char* buffer,const char* type, unsigned long &len);
    unsigned char* GetAppHeader(unsigned char* file, const char* type, unsigned long &len);
};

#endif // GLOBAL_H
