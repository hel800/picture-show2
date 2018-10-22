#include "xmptoolbox.h"

XMPToolBox::XMPToolBox()
{

}

QDateTime XMPToolBox::readExifDate(const QString &fileName)
{
  if (!SXMPMeta::Initialize())
  {
    qDebug("Could not initialize toolkit!");
    return QDateTime();
  }
  XMP_OptionBits options = 0;

  if (!SXMPFiles::Initialize(options))
  {
    qDebug("Could not initialize SXMPFiles.");
    return QDateTime();
  }

  QDateTime returnDt;

  try
  {
    // Options to open the file with - read only and use a file handler
    XMP_OptionBits opts = kXMPFiles_OpenForRead | kXMPFiles_OpenUseSmartHandler;

    bool ok;
    SXMPFiles myFile;
    std::string status = "";

    // First we try and open the file
    ok = myFile.OpenFile(fileName.toStdString(), kXMP_UnknownFile, opts);
    if (!ok)
    {
      status += "No smart handler available for " + fileName.toStdString() + "\n";
      status += "Trying packet scanning.\n";

      // Now try using packet scanning
      opts = kXMPFiles_OpenForUpdate | kXMPFiles_OpenUsePacketScanning;
      ok = myFile.OpenFile(fileName.toStdString(), kXMP_UnknownFile, opts);
    }

    if (ok)
    {
      SXMPMeta meta;
      myFile.GetXMP(&meta);

      bool exists = false;

      // Read a simple property
      std::string simpleValue; // Stores the value for the property
      exists = meta.GetProperty(kXMP_NS_EXIF, "DateTimeOriginal", &simpleValue, NULL);

      if (exists)
      {
        // returnDt = QDateTime::fromString(QString::fromStdString(simpleValue), QString("yyyy:MM:ddTHH:mm:ss"));
        returnDt = QDateTime::fromString(QString::fromStdString(simpleValue), Qt::ISODate);
      }
      else
        qDebug( "DateTimeOriginal not found!" );
    }

    myFile.CloseFile();
  }
  catch (XMP_Error & e)
  {
    qDebug( "XMP Error!");
  }

  return returnDt;
}
