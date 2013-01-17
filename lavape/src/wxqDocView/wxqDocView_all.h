#ifndef _wxqDocView_PCH
#define _wxqDocView_PCH


#include "disco_all.h"
#include "docview.h"
#include "mdiframes.h"
#include "wxExport.h"
#include "defs.h"
#include "setup.h"
#include <QList>
#include <QEvent>
#include <QSplitter>
#include <QStyle>
#include <QTabBar>
#include <QToolButton>
#include <QTimer>
#include <QAbstractEventDispatcher>
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#ifdef WIN32
#include <QProcessEnvironment>
#endif
#ifdef MINGW32
#include <shlobj.h>
#include <windows.h>
#include <winbase.h>
#endif
/*
#include "qgtkstyle_p.h"
#include "qfusionstyle_p.h"
#include "qmacstyle_mac_p.h"
#include "qwindowsstyle_p.h"
#include "qwindowsvistastyle_p.h"
#include "qwindowsxpstyle_p.h"
*/
#endif
