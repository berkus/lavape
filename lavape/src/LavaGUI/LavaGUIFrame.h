#ifndef __LavaGUIFrame
#define __LavaGUIFrame


#include "GUIProgBase.h"
#include "qstring.h"
#include "docview.h"
#include "mdiframes.h"
#include "qframe.h"
#include "qtoolbar.h"


/////////////////////////////////////////////////////////////////////////////
// CLavaGUIFrame frame
//
// This is the Childframe class of the Splitter - View containing
// a CLavaGUIView - View and a CLavaPEView - View
//
/////////////////////////////////////////////////////////////////////////////

class LAVAGUI_DLL CLavaGUIFrame : public wxMDIChildFrame
{

public:
  CLavaGUIFrame(QWidget *parent);
  virtual ~CLavaGUIFrame();
  FRAMEFACTORY(CLavaGUIFrame)
  bool OnCreate(wxDocTemplate *temp, wxDocument *doc);
  virtual void InitialUpdate();

  DString CalcTitle(LavaDECL* decl, const DString& lavaName);
  void NewTitle(LavaDECL* decl, const DString& lavaName);
  void SetModified(const QString& lavaName, bool changed);
  virtual void closeEvent(QCloseEvent *e);
  //QToolBar* m_GUIBar;
  wxView* myView;
  CLavaBaseDoc* myDoc;
  bool onClose;
private:
	Q_OBJECT
};


#endif
