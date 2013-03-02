/* ------------------------------------------------------------------
iptcinfo.cpp is part of picture-show2.
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

#include "iptcinfo.h"

const char JPG_Header[] = "\xFF\xD8\xFF\xE0\x0\x10JFIF\x0";
const char Photoshop_Header[] = "Photoshop 3.0\x0";
const char Photoshop_8BIM[] = "8BIM";
const char Photoshop_1C02[] = "\x1c\x02";

IPTCInfo::IPTCInfo()
{
    m_AppHeaderLength = 0;
    m_AppHeaderStart = 0;
}

IPTCInfo::~IPTCInfo()
{

}

int IPTCInfo::OpenFile(const std::string filename)
{
    char * buffer = new char[30];	//dummy buffer
    qint64 bytesread = 0;
    unsigned long len = 0;
    qint64 filelength = 0;
    unsigned char * file_buffer = NULL;
    unsigned char* ptr2 = NULL;
    unsigned char* ptr = NULL;
    unsigned char* app = NULL;

    QFile file(QString(filename.c_str()));
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("Can't read file");
        qDebug(filename.c_str());
        return 0;
    }

    bytesread = file.read(buffer, 3);
    if(bytesread != 3)
    {
        file.close();
        qDebug("File to small --> no image!");
        return 0;
    }

    if(memcmp(buffer, JPG_Header, 3) != 0)
    {
        qDebug(buffer);
        qDebug(JPG_Header);
        file.close();
        qDebug("No JPEG image!");
        return 0;
    }

    file.seek(0);

    //read whole file into memory
    filelength = 100000;
    if (file.size() < filelength)
        filelength = file.size();
    file_buffer = (unsigned char*)malloc(filelength);
    bytesread = file.read((char*)file_buffer, filelength);

    if(bytesread != filelength)
    {
        free(file_buffer);
        file.close();
        qDebug("Error when reading whole file!");
        return 0;
    }

    len = static_cast<unsigned long>(filelength);

    if(app = GetAppHeader(file_buffer,"\xFF\xED", len))
    {
        if (app == NULL)
        {
            qDebug("IS NULL!");
            return 0;
        }

        //is this FFED header one of Photoshop?
        if(memcmp(Photoshop_Header, app+4, 14) != 0)
        {
            qDebug("no Photoshop header found");
            free(file_buffer);
            file.close();
            return 0;
        }

        if((ptr = Get8BIMHeader(app,"\x4\x4",len)) == NULL)
        {
            qDebug("no 8BIM header found");
            free(file_buffer);
            file.close();
            return 0;
        }

        if((ptr2 = GetIPTCHeader(ptr, Photoshop_1C02, len)) == NULL)
        {
            qDebug("no 1C02 header found");
            free(file_buffer);
            file.close();
            return 0;
        }

        ParseHeader(ptr2);
    }

    free(file_buffer);
    file.close();

    return 1;
}

unsigned char*  IPTCInfo::GetAppHeader(unsigned char* buf, const char* type, unsigned long &len)
{
    unsigned offs  = 0;    // current offset into buffer

    // Scan for IPTC header and do a sanity check
    for(offs = 0; offs < len-1; offs++)
      if(buf[offs] == 0xFF && buf[offs+1] == 0xED)
        break;

    if(offs == len-1)
      return NULL;

    len = len - offs;
    return buf + offs;


}

unsigned char* IPTCInfo::Get8BIMHeader(unsigned char *buffer,const char* type, unsigned long &len)
{
    unsigned long pos = 0,headerinfo = 0;

    while(pos < len)
    {
        if(memcmp(buffer+pos,Photoshop_8BIM,4) == 0)
        {
            if(memcmp(buffer+pos+4,type,2) == 0)
            {
                headerinfo = buffer[pos+6];
//                return buffer+pos;

                if (headerinfo != 0)
                {
                    //PS6 style
                    len = len - pos+4+2+1+headerinfo+2;
                    return  buffer+pos+4+2+1+headerinfo+2;//4 bytes 8BIM + 2 bytes type + 1 byte headerinfolen + headerinfo + 2 bytes 00 padding (?)
                }
                else
                {
                    //old style
                    len = len - pos+10;
                    return buffer+pos+10; //4 bytes 8BIM + 2 bytes type + 4 bytes 00 padding (?)
                }
            }
        }
        pos++;
    }
    return NULL;
}

unsigned char* IPTCInfo::GetIPTCHeader(unsigned char* buffer, const char* type, unsigned long &len)
{
    unsigned long pos = 0;

    while(pos < len)
    {
        if(buffer[pos] == 0x1C && buffer[pos+1] == 0x02)
        {
            len = len - pos;
            return buffer+pos;
        }
        pos++;
    }

    return NULL;
}

void IPTCInfo::ParseHeader(unsigned char *buffer)
{
    unsigned long ret,pos = 0,len = 0;	//first two bytes are size info
    char type;
    std::string cnt;

    //calc length
    len = ((unsigned long)(buffer[0]<<8) + ((unsigned long)(buffer[1])));

    while(pos < len)
    {
        ret = GetNextHeaderInfo(buffer+pos,&type,cnt);
        if(ret == 0)
            return;

        switch(type)
        {
        //credits
        case '\x50':
            m_Byline = cnt;
            break;
        case '\x55':
            m_BylineTitle = cnt;
            break;
        case '\x6E':
            m_Credits = cnt;
            break;
        case '\x73':
            m_Source = cnt;
            break;
        //origin
        case '\x05':
            m_ObjectName = cnt;
            break;
        case '\x37':
            m_DateCreated = cnt;
            break;
        case '\x5A':
            m_City = cnt;
            break;
        case '\x5F':
            m_State = cnt;
            break;
        case '\x65':
            m_Country = cnt;
            break;
        case '\x67':
            m_OriginalTransmissionReference = cnt;
            break;
        //copyright
        case '\x74':
            m_CopyrightNotice = cnt;
            break;
        //caption
        case '\x78':
            m_Caption = cnt;
            break;
        case '\x7A':
            m_CaptionWriter = cnt;
            break;
        case '\x69':
            m_Headline = cnt;
            break;
        case '\x28':
            m_SpecialInstructions = cnt;
            break;
        //categories
        case '\x0F':
            m_Category = cnt;
            break;
        case '\x14':
//            m_SupplementalCategories.Add(cnt);
            cnt;
            break;
        //Keywords
        case '\x19':
//            m_Keywords.Add(cnt);
            cnt;
            break;
        }
        pos += ret + 5;		//header info: marker(2) + type + size(2)
    }
}

long IPTCInfo::GetNextHeaderInfo(const unsigned char *buffer, char *type, std::string &content)
{
    unsigned long len = 0;

    if(memcmp(buffer, Photoshop_1C02, 2) != 0)
    {
        return 0;
    }

    *type = buffer[2];
    len = ((unsigned long)(buffer[3]<<8) + ((unsigned long)(buffer[4])));

    char * temp_char = new char[len];
    memcpy(temp_char, buffer+5, len);

    content.clear();
    content = std::string(temp_char, len);

    delete [] temp_char;

    return len;

}

