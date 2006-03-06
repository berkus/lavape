#ifndef _WX_export__
#define _WX_export__

/* split off from docview.h since moc apparently
   doesn't like template class decls */


#include "SYSTEM.h"
#include "defs.h"
#include "qglobal.h"
#include "setup.h"
#include "qlist.h"
#include "qaction.h"
#include "qobject.h"
#include "qglobal.h"
#include "qstring.h"
#include "qmainwindow.h"
#include "qmenu.h"

class WXDLLEXPORT wxView;
class WXDLLEXPORT wxDocument;
class WXDLLEXPORT wxDocTemplate;
class WXDLLEXPORT wxDocManager;
class WXDLLEXPORT wxPrintInfo;
class WXDLLEXPORT wxCommand;
class WXDLLEXPORT wxCommandProcessor;
class WXDLLEXPORT wxHistory;
class WXDLLEXPORT QAction;

#ifdef WIN32
/*
template class WXDLLEXPORT QList<wxView>;
template class WXDLLEXPORT QList<wxDocument>;
template class WXDLLEXPORT QList<wxDocTemplate>;
template class WXDLLEXPORT QList<wxCommand>;
template class WXDLLEXPORT QList<QMenu>;
template class WXDLLEXPORT QList<QAction>;
*/
#endif

#endif //_WX_export__
