#ifndef __Convert
#define __Convert
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "DString.h"


class DISCO_DLL ConvertCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();


  static void CardToString (long unsigned x,
                     DString& str);

  static void StringToCard (const DString& str,
                     long unsigned& x);


  static void IntToString (long int x,
                           DString& str);

  static void StringToDate (const DString& str,
                     unsigned& year,
                     unsigned& month,
                     unsigned& day);
                     
  static void StringToTime (const DString& str,
                     unsigned& hour,
                     unsigned& minute,
                     unsigned& second);
                   
  static void DateToString (unsigned year,
                     unsigned month,
                     unsigned day,
                     DString& str);
 
  static void TimeToString(unsigned hour,
                    unsigned minute,
                    unsigned second,
                    DString& str);

};

extern DISCO_DLL ConvertCLASS Convert;

#endif
