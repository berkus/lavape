#ifdef _GNUG__
#pragma interface
#endif
// LavaGUIFrames.h : 
//
/////////////////////////////////////////////////////////////////////////////


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

  DString CalcTitle(LavaDECL* decl, const DString& lavaName);
  void NewTitle(LavaDECL* decl, const DString& lavaName);
  void SetModified(const QString& lavaName, bool changed);
  virtual void closeEvent(QCloseEvent *e);
  //QToolBar* m_GUIBar;
  wxView* myView;
  bool onClose;

  /*
// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLavaGUIFrame)
	public:
  virtual bool Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
  virtual bool PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CLavaGUIFrame();

  // Generated message map functions
  //{{AFX_MSG(CLavaGUIFrame)
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam );
  afx_msg void OnClose();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  */
private:
	Q_OBJECT
};

#endif
