#ifndef __Scanner
#define __Scanner


#include "ComboBar.h"
#include "Tokens.h"
#include "qstring.h"


void Scanner_INIT ();


bool IsOK (QString &txt,TToken &token,int &pos,QString *&msg, CComboBar *comboBar);

#endif
