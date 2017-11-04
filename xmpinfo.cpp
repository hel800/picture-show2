#include "xmpinfo.h"

#include <QTextStream>

XMPInfo::XMPInfo()
{
    m_Rating = -1;
}

XMPInfo::~XMPInfo()
{

}

bool XMPInfo::ParseImage(const QString & filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        m_LastError = "File could not be opened!";
        return false;
    }

    QByteArray rawBuffer = file.read(100000);
    unsigned fsize = rawBuffer.size();


    if (fsize <= 4)
    {
        m_LastError = "File is to small -> no image!";
        file.close();
        return false;
    }

    if (memcmp(rawBuffer.constData(), "\xFF\xD8", 2) != 0)
    {
        m_LastError = "No JPEG image!";
        file.close();
        return false;
    }

    long headerOffset = GetXMPHeaderStartPosition(rawBuffer);

    if (headerOffset == -1)
    {
        // second try

        file.reset();
        rawBuffer = file.readAll();
        file.close();

        headerOffset = GetXMPHeaderStartPosition(rawBuffer);

        if (headerOffset == -1)
        {
            m_LastError = "No XMP Information found!";
            return false;
        }
    }
    else
    {
        file.close();
    }

    long headerEnd = GetXMPHeaderEndPosition(rawBuffer, headerOffset);
    if (headerEnd == -1)
    {
        m_LastError = "No XMP Information found!";
        return false;
    }

    unsigned short headerSize = (headerEnd + 1) - headerOffset;

    QString xmlHeader;
    xmlHeader = QString::fromLatin1(rawBuffer.mid(
                                    headerOffset,
                                    headerSize ));

    QString rating = GetDescriptionValueFromXML(xmlHeader, "xmp:Rating");

    bool ok = false;
    m_Rating = rating.toShort(&ok);

    if (ok == false)
    {
        m_Rating = -1;
    }

    return true;
}

std::string XMPInfo::GetError()
{
    return m_LastError;
}

long XMPInfo::GetXMPHeaderStartPosition(const QByteArray & buffer)
{
    long offs  = 0;    // current offset into buffer

    for(offs = 0; offs < buffer.size() - 10; offs++)
        if(buffer[offs] == '\x3c'
          && buffer[offs+1] == '\x78' && buffer[offs+2] == '\x3a'
          && buffer[offs+3] == '\x78' && buffer[offs+4] == '\x6d'
          && buffer[offs+5] == '\x70' && buffer[offs+6] == '\x6d'
          && buffer[offs+7] == '\x65' && buffer[offs+8] == '\x74'
          && buffer[offs+9] == '\x61' && buffer[offs+10] == '\x20')
          break;

    if(offs >= buffer.size() - 10)
      return -1;

    return offs;
}

long XMPInfo::GetXMPHeaderEndPosition(const QByteArray & buffer, long start)
{
    long offs = start;

    for(offs = start; offs < buffer.size() - 11; offs++)
      if(buffer[offs] == '\x3c' && buffer[offs+1] == '\x2f'
        && buffer[offs+2] == '\x78' && buffer[offs+3] == '\x3a'
        && buffer[offs+4] == '\x78' && buffer[offs+5] == '\x6d'
        && buffer[offs+6] == '\x70' && buffer[offs+7] == '\x6d'
        && buffer[offs+8] == '\x65' && buffer[offs+9] == '\x74'
        && buffer[offs+10] == '\x61' && buffer[offs+11] == '\x3e')
        break;

    offs += 11;

    if(offs >= buffer.size() - 11)
      return -1;

    return offs;
}

QString XMPInfo::GetDescriptionValueFromXML(const QString xml, const QString tag)
{
    QRegExp rx(tag + "=\"(\\d)\"", Qt::CaseInsensitive);
    rx.indexIn(xml);
    QStringList capturedValues = rx.capturedTexts();

    if (capturedValues.size() != 2)
    {
        m_LastError = "tag not found in XMP header";
        return QString();
    }

    return QString(capturedValues.last());
}
