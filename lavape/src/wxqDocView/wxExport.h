#ifndef _WX_export__
#define _WX_export__

/* split off from docview.h since moc apparently
   doesn't like template class decls */


#include "SYSTEM.h"
#include "defs.h"
#include "qglobal.h"
#include "setup.h"
#include "qptrlist.h"
#include "qaction.h"
#include "qobject.h"
#include "qglobal.h"
#include "qstring.h"
#include "qmainwindow.h"
#include "qpopupmenu.h"

class WXDLLEXPORT wxView;
class WXDLLEXPORT wxDocument;
class WXDLLEXPORT wxDocTemplate;
class WXDLLEXPORT wxDocManager;
class WXDLLEXPORT wxPrintInfo;
class WXDLLEXPORT wxCommand;
class WXDLLEXPORT wxCommandProcessor;
class WXDLLEXPORT wxHistory;
class WXDLLEXPORT wxAction;

#ifdef WIN32
template class WXDLLEXPORT QPtrList<wxView>;
template class WXDLLEXPORT QPtrList<wxDocument>;
template class WXDLLEXPORT QPtrList<wxDocTemplate>;
template class WXDLLEXPORT QPtrList<wxCommand>;
template class WXDLLEXPORT QPtrList<QPopupMenu>;
template class WXDLLEXPORT QPtrList<wxAction>;
#endif

#endif //_WX_export__
