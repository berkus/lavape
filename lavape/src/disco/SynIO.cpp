/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
	 (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#ifdef __GNUC__
#pragma implementation
#endif



/**********************************************************************

               implementation of class SynIOCLASS

 **********************************************************************/

#include "SYSTEM.h"
#include "UNIX.h"
#include "SynIO.h"
#include "ASN1File.h"
#include "CDPpp.h"
#include "DIO.h"
//#include "disco.h"


SynIOCLASS SynIO;
static bool __INITstarted=false;


void SynIOCLASS::error (QString errorMsg)

{
  if (!Silent) {
    QString mess = QString("++++ SynIO: ") + errorMsg;
    qDebug(mess);
  }

  Done = false;
} // END OF error

void SynIOCLASS::InitSyntax(SynDef *syntax, QString name)
{
  CHESimpleSyntax *simpleSyntax = (CHESimpleSyntax*)syntax->SynDefTree.first;
  if (!simpleSyntax) {
    simpleSyntax = new CHESimpleSyntax;
    simpleSyntax->data.SyntaxName = DString(name);
    simpleSyntax->data.nINCL = 0;
    syntax->SynDefTree.Insert(0, simpleSyntax);
    syntax->IDTable = 0;
    syntax->FreeID = 0;
    syntax->FreeINCL = 0;
  }

}

int SynIOCLASS::ReadSynDef (const QString& fileName, SynDef *&syntax, ASN1* strgCid)
{
  //returns -1 : read failed, 0: read ok, write ok, 1: read ok write failed
  ASN1* pcid;
  int ret = 0;

  if (strgCid)
    pcid = strgCid;
  else {
    ASN1InFile* icid = new ASN1InFile (fileName);
    if (!icid->Done) {
      syntax = 0;
      delete icid;
      return -1;
    }
    else {
      if (access(fileName,W_OK) != 0) //FILE_ATTRIBUTE_READONLY & GetFileAttributes(fileName.c))
        ret = 1;
    }
    pcid = icid;
  }
  Done = true;
  syntax = new SynDef;
	DString fName;
  CDPSTRING(GET, pcid, (address)&fName);
  CDPint(GET,pcid,(address)&syntax->Release);
  pcid->Release = syntax->Release;
  CDPSyntaxDefinition(GET,pcid,(address)&syntax->SynDefTree);
  CDPint(GET,pcid,(address)&syntax->FreeID);
  CDPint(GET,pcid,(address)&syntax->FreeINCL);
  fromOldStyle = false;
  if (pcid->Skip())
    ret = -1;
  else {
    int ii;
    CHESimpleSyntax *simpleSyntax;
    for (simpleSyntax = (CHESimpleSyntax*)syntax->SynDefTree.first;
         simpleSyntax;
         simpleSyntax = (CHESimpleSyntax*)simpleSyntax->successor) {
      for (ii = 0; ii < simpleSyntax->data.SyntaxName.l; ii++)
        if (simpleSyntax->data.SyntaxName[ii] == '\\')
          simpleSyntax->data.SyntaxName[ii] = '/';

    }
  }
  if (!strgCid)
    delete pcid;
  return ret;
}//ReadSynDef



bool SynIOCLASS::WriteSynDef (const QString& fileName, SynDef *syntax, ASN1* strgCid)
{
  if (syntax) {
    if (strgCid)
      syntax->PathName = fileName;
    syntax->Release = RELEASE;
    return WriteSynDef(fileName, syntax->SynDefTree , syntax->FreeID, syntax->FreeINCL, syntax->Release, strgCid);
  }
  return false;
}

bool SynIOCLASS::WriteSynDef (const QString& fileName,
                              SyntaxDefinition &synDef,
                              int freeID,
                              int freeINCL,
                              int release,
                              ASN1* strgCid)

{
  ASN1* pcid;
  DString str=fileName.latin1();

  if (synDef.first == 0) {
    Done = false;
    error("WriteSynDef: syntax definition = NIL");
    return false;
  }

  if (strgCid) {
    pcid = strgCid;
    CDPSTRING(PUT, pcid, (address)&str);
  }
  else {
    ASN1OutFile* ocid = new ASN1OutFile(fileName);
    if (!((ASN1OutFile*)ocid)->Done) {
      error("syntax file couldn't be opened for writing");
      delete ocid;
      Done = false;
      return false;
    }
    pcid = ocid;
    CDPSTRING(PUT, pcid, (address)&str);
  }
  Done = true;
  pcid->Release = release;
  CDPint(PUT,pcid,(address)&release);
  CDPSyntaxDefinition(PUT,pcid,(address)&synDef);
  CDPint(PUT,pcid,(address)&freeID);
  CDPint(PUT,pcid,(address)&freeINCL);
  if (!strgCid)
    delete pcid;
  return true;
} // END OF WriteSynDef


void SynIOCLASS::DeleteSynDef (SynDef *&syntax)

{
  delete syntax;
  syntax = 0;
} // END OF DeleteSynDef

/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/

void SynIOCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  CDPpp.INIT();
  ASN1pp.INIT();
  IO.INIT();

  Silent = false;
  Done = true;

}

