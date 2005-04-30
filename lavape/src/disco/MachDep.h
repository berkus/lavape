#ifndef __MachDep
#define __MachDep
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include <ctype.h>


const unsigned BytesPerWORD = 4;
const unsigned BitsPerWORD = 32;
const unsigned BytesPerCARDINAL = 4;
const unsigned BitsPerCARDINAL = 32;
const unsigned BytesPerTSIZEUnit = 1;
const bool CHARisASCII = true;
const char MaxCC = '\37'; /* maximum control character (not printable!) */
const char MaxPC = '\176'; /* maximum printable character */
const char EOL = '\12'; /* end of line character */
const char CR = '\15'; /* carriage return character */

#undef EOF
const char EOF = '\3'; /* end of file character */

const char HT = '\11'; /* horizontal tabulator */

const unsigned BytesPerADDRESS = 4;
const unsigned MinusZero = 0X80000000;
const unsigned BlanksPerTab = 8;

#define hibyte(a) (unsigned char)((unsigned)(a) >> 24)


typedef unsigned BYTEorWORD;

/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/



/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class DISCO_DLL MachDepCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  unsigned char ToASCII[256], FromASCII[256];


  bool IsLetter (unsigned char ch) { return (isalpha(ch)?true:false); }


  bool IsIdChar (unsigned char ch) { return (isalnum(ch)?true:false); }
};

extern DISCO_DLL MachDepCLASS MachDep;

#endif
