#ifndef _LavaBase_PCH
#define _LavaBase_PCH

//use these if the compiler does not support _intXX

#ifdef NEEDS_INT_DEFINED
#define _int16 short
#define _int32 long
#define _int64 long long
#endif

#include "wxqDocView_all.h"

#include "idd_aboutbox.h"

#include "AboutBox.h"
#include "BAdapter.h"
#include "DumpView.h"
#include "LavaAppBase.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"
#include "LavaThread.h"
#include "PEBaseDoc.h"
#include "SafeInt.h"
#include "SylTraversal.h"
#include "SynIDTable.h"

#endif

