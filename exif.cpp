/**************************************************************************
  exif.cpp  -- A simple ISO C++ library to parse basic EXIF
               information from a JPEG file.

  Copyright (c) 2010 Mayank Lahiri
  mlahiri@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  -- Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  -- Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
   NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
   OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include"exif.h"
#include<string.h>      // included for memcpy() and memset()

static unsigned int parse32(unsigned char *buf, bool intel) {
  return intel ?
                (((unsigned)buf[3]<<24) + ((unsigned)buf[2]<<16) + ((unsigned)buf[1]<<8) + buf[0])
        :
          (((unsigned)buf[0]<<24) + ((unsigned)buf[1]<<16) + ((unsigned)buf[2]<<8) + buf[3]);
}
static unsigned short parse16(unsigned char *buf, bool intel) {
  return intel ? (((unsigned)buf[1]<<8) + buf[0]) : (((unsigned)buf[0]<<8) + buf[1]);
}
static void copyEXIFString(char **dest, unsigned ncomp, unsigned base, unsigned offs, unsigned char *buf) {
  *dest = new char[ncomp+1];
  memset(*dest, 0, ncomp+1);
  if(ncomp > 4)
    memcpy(*dest, (char*) (buf+base+offs), ncomp);
  else
    memcpy(*dest, (char*) &offs, 4);
}
static float parseEXIFrational(unsigned char *buf, bool align, float * num = NULL, float * den = NULL) {
  float numerator   = 0;
  float denominator = 1;

//  numerator  = (float) *((unsigned *)buf);
//  denominator= (float) *(((unsigned *)buf)+1);

  numerator = (float) parse32(buf, align);
  denominator = (float) parse32(buf+4, align);

  if(denominator < 1e-20)
    return 0;

  if (num != NULL)
      *num = numerator;
  if (den != NULL)
      *den = denominator;

  return numerator/denominator;
}



int ParseEXIF(unsigned char *buf, unsigned len, EXIFInfo &result) {
  bool alignIntel = true;    // byte alignment
  unsigned offs  = 0;    // current offset into buffer

  // Prepare return structure
  //memset(&result, 0, sizeof(result));

  // Scan for EXIF header and do a sanity check
  for(offs = 0; offs < len-1; offs++)
    if(buf[offs] == 0xFF && buf[offs+1] == 0xE1)
      break;
  if(offs+18 >= len-1)
    return PARSE_EXIF_ERROR_NO_EXIF;

  offs += 4;
  if(buf[offs] != 0x45 || buf[offs+1] != 0x78 || buf[offs+2] != 0x69)
    return PARSE_EXIF_ERROR_NO_EXIF;

  // Get byte alignment (Motorola or Intel)
  offs += 6;
  if(buf[offs] == 0x49 && buf[offs+1] == 0x49)
    alignIntel = true;
  else {
    if(buf[offs] == 0x4d && buf[offs+1] == 0x4d)
      alignIntel = false;
    else
      return PARSE_EXIF_ERROR_UNKNOWN_BYTEALIGN;
  }
  result.byteAlign = alignIntel;

  // Get offset into first IFD
  offs += 4;
  unsigned x = parse32(buf+offs, alignIntel);
  if(offs + x >= len) {
    return PARSE_EXIF_ERROR_CORRUPT;
  }

  // Jump to the first IFD, scan tags there.
  offs += x-4;
  int nentries = parse16(buf+offs, alignIntel);
  offs += 2;
  unsigned ifdOffset = offs-10;
  unsigned exifSubIFD = 0;
  for(int j = 0; j < nentries; j++)  {
    if(offs+12 >= len) {
        return PARSE_EXIF_ERROR_CORRUPT;
    }

    unsigned short tag = parse16(buf+offs, alignIntel);
    unsigned ncomp = parse32(buf+offs+4, alignIntel);
    unsigned coffs = parse32(buf+offs+8, alignIntel);

//    std::cout << std::dec << "@" << offs << ": tag: " << std::hex << tag << std::dec << " ncomp: " << ncomp << " coffs: " << coffs << std::endl;

    switch(tag) {    
      case 0x112:
        {
            unsigned short orient = parse16((unsigned char*) &coffs, alignIntel);
            result.orientation = orient;
        }
        break;
      case 0x8769:
        // EXIF subIFD offset
        exifSubIFD = ifdOffset + coffs;
        break;

      case 0x10F:
      {
        // Digicam manufacturer
        char * cm = (char*)0;
        copyEXIFString(&cm, ncomp, ifdOffset, coffs, buf);
        result.cameraMake_st = QString(cm);
        delete cm;
      }
        break;

      case 0x110:
      {
        // Digicam model
        char * dm = (char*)0;
        copyEXIFString(&dm, ncomp, ifdOffset, coffs, buf);
        result.cameraModel_st = QString(dm);
        delete dm;
      }
        break;

      case 0x132:
      {
        // EXIF/TIFF date/time of image
        char * dateTime = (char*)0;
        copyEXIFString(&dateTime, ncomp, ifdOffset, coffs, buf);
        result.dateTimeModified_st = QString(dateTime);
        delete dateTime;
      }
        break;

      case 0x10E:
      {
        // image description
        char * desc = (char*)0;
        copyEXIFString(&desc, ncomp, ifdOffset, coffs, buf);
        result.imgDescription_st = QString(desc);
        delete desc;
      }
        break;

    }
    offs += 12;
  }

  if(!exifSubIFD)
    return 0;

  // At the EXIF SubIFD, read the rest of the EXIF tags
  offs = exifSubIFD;

  if(offs+2 >= len) {
      return 0;
  }

  nentries = parse16(buf+offs, alignIntel);
  offs += 2;
  for(int j = 0; j < nentries; j++)  {
    if(offs+12 >= len) {
        return 0;
    }
    unsigned short tag = parse16(buf+offs, alignIntel);
    unsigned ncomp = parse32(buf+offs+4, alignIntel);
    unsigned coffs = parse32(buf+offs+8, alignIntel);

    switch(tag) {
      case 0x9003:
      {
        // original image date/time string
        char * dateTimeOrig = (char*)0;
        copyEXIFString(&dateTimeOrig, ncomp, ifdOffset, coffs, buf);
//        std::cout << "j: " << j << " offs: " << offs << std::endl;
        result.dateTimeOriginal_st = QString(dateTimeOrig);
        delete dateTimeOrig;
      }
        break;

      case 0x920a:
      {
        // Focal length in mm
        // result.focalLength = *((unsigned*)(buf+ifdOffset+coffs));
        result.focalLength = parseEXIFrational(buf+ifdOffset+coffs, alignIntel);
      }
        break;

      case 0x829D:
      {
        // F-stop
        result.FStop = parseEXIFrational(buf+ifdOffset+coffs, alignIntel);
      }
        break;

      case 0x829A:
      {
        // Exposure time
        float num, den = 0.0;
        result.exposureTime = parseEXIFrational(buf+ifdOffset+coffs, alignIntel, &num, &den);
        result.exposureTime_st = QString::number(num) + "/" + QString::number(den);
      }
        break;
    }
    offs += 12;
  }

  return 0;
}

int ExtractDateTime(unsigned char *buf, unsigned len, QString &dateString)
{
    bool alignIntel = true;    // byte alignment
    unsigned offs  = 0;    // current offset into buffer

    // Scan for EXIF header and do a sanity check
    for(offs = 0; offs < len-1; offs++)
      if(buf[offs] == 0xFF && buf[offs+1] == 0xE1)
        break;
    if(offs+18 >= len-1)
      return PARSE_EXIF_ERROR_NO_EXIF;
    offs += 4;
    if(buf[offs] != 0x45 || buf[offs+1] != 0x78 || buf[offs+2] != 0x69)
      return PARSE_EXIF_ERROR_NO_EXIF;

    // Get byte alignment (Motorola or Intel)
    offs += 6;
    if(buf[offs] == 0x49 && buf[offs+1] == 0x49)
      alignIntel = true;
    else {
      if(buf[offs] == 0x4d && buf[offs+1] == 0x4d)
        alignIntel = false;
      else
        return PARSE_EXIF_ERROR_UNKNOWN_BYTEALIGN;
    }

    // Get offset into first IFD
    offs += 4;
    unsigned x = parse32(buf+offs, alignIntel);
    if(offs + x >= len) {
      return PARSE_EXIF_ERROR_CORRUPT;
    }

    // Jump to the first IFD, scan tags there.
    offs += x-4;
    int nentries = parse16(buf+offs, alignIntel);
    offs += 2;
    unsigned ifdOffset = offs-10;
    unsigned exifSubIFD = 0;

    unsigned tryoff = 298;

    if(tryoff+12 >= len) {
      return PARSE_EXIF_ERROR_CORRUPT;
    }

    unsigned short tag = parse16(buf+tryoff, alignIntel);
    if (tag == 0x9003)
    {
        char * date = (char*)0;
        unsigned ncomp = parse32(buf+tryoff+4, alignIntel);
        unsigned coffs = parse32(buf+tryoff+8, alignIntel);
        copyEXIFString(&date, ncomp, ifdOffset, coffs, buf);
        dateString = date;
        return 0;
    }
    tryoff = 288;

    if(tryoff+12 >= len) {
      return PARSE_EXIF_ERROR_CORRUPT;
    }

    tag = parse16(buf+tryoff, alignIntel);
    if (tag == 0x9003)
    {
        char * date = (char*)0;
        unsigned ncomp = parse32(buf+tryoff+4, alignIntel);
        unsigned coffs = parse32(buf+tryoff+8, alignIntel);
        copyEXIFString(&date, ncomp, ifdOffset, coffs, buf);
        dateString = date;
        return 0;
    }

    for(int j = 0; j < nentries; j++)  {
        if (offs+32 >= len)
            return PARSE_EXIF_ERROR_CORRUPT;

      unsigned short tag = parse16(buf+offs, alignIntel);
      unsigned coffs = parse32(buf+offs+8, alignIntel);

      switch(tag) {
          case 0x8769:
            // EXIF subIFD offset
            exifSubIFD = ifdOffset + coffs;
            break;
      }
      offs += 12;
    }

    if(!exifSubIFD)
      return 0;

    // At the EXIF SubIFD, read the rest of the EXIF tags
    offs = exifSubIFD;
    nentries = parse16(buf+offs, alignIntel);
    offs += 2;
    for(int j = 0; j < nentries; j++)  {
        if (offs+32 >= len)
            return PARSE_EXIF_ERROR_CORRUPT;

      unsigned short tag = parse16(buf+offs, alignIntel);
      unsigned ncomp = parse32(buf+offs+4, alignIntel);
      unsigned coffs = parse32(buf+offs+8, alignIntel);

      switch(tag) {
        case 0x9003:
          // original image date/time string
          char * date = (char*)0;
          copyEXIFString(&date, ncomp, ifdOffset, coffs, buf);
          dateString = date;
          return 0;
          break;
      }
      offs += 12;
    }

    return 0;
}
