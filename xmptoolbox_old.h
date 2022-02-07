#ifndef XMPTOOLBOX_H
#define XMPTOOLBOX_H

// Must be defined to instantiate template classes
#define TXMP_STRING_TYPE std::string

// Must be defined to give access to XMPFiles
#define XMP_INCLUDE_XMPFILES 1

#define WIN_ENV true

// Ensure XMP templates are instantiated
#include "XMP.incl_cpp"

// Provide access to the API
#include "XMP.hpp"

#include <QDateTime>

class XMPToolBox
{
public:
    XMPToolBox();

    static QDateTime readExifDate(const QString& fileName);
};

#endif // XMPTOOLBOX_H
