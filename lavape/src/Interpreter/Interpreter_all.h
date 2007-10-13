#ifndef _Interpreter_PCH
#define _Interpreter_PCH

#include "disco_all.h"
#include "wxqDocView_all.h"
#include "LavaBase_all.h"
#include "LavaGUI_all.h"
#include "Lava_UI_all.h"

#include "Constructs.h"
#include "ConstructsX.h"
#include "Check.h"
#include "Debugger.h"
#include "LavaProgram.h"
#include "Tokens.h"
#include "LavaMainFrameBase.h"
#include "LavaThread.h"
#include "SafeInt.h"

#ifndef WIN32
#include <setjmp.h>
#endif

#ifdef __MINGW32
#include <excpt.h>
#endif
#ifndef WIN32
#include <unistd.h>
#endif

#endif
