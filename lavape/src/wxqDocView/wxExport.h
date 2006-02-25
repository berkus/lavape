#ifndef _WX_export__
#define _WX_export__

/* split off from docview.h since moc apparently
   doesn't like template class decls */


#include "SYSTEM.h"
#include "defs.h"
#include "qglobal.h"
#include "setup.h"
#include "q3ptrlist.h"
#include "qaction.h"
#include "qobject.h"
#include "qglobal.h"
#include "qstring.h"
#include "qmainwindow.h"
#include "q3popupmenu.h"

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
template class WXDLLEXPORT Q3PtrList<wxView>;
template class WXDLLEXPORT Q3PtrList<wxDocument>;
template class WXDLLEXPORT Q3PtrList<wxDocTemplate>;
template class WXDLLEXPORT Q3PtrList<wxCommand>;
template class WXDLLEXPORT Q3PtrList<Q3PopupMenu>;
template class WXDLLEXPORT Q3PtrList<QAction>;
#endif

#endif //_WX_export__
