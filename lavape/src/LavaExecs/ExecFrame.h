#ifndef __ExecFrame
#define __ExecFrame

#include "ComboBar.h"
#include "docview.h"
#include "mdiframes.h"


#ifdef LAVAEXECS_EXPORT
#define LAVAEXECS_DLL __declspec( dllexport )
#undef AFX_DATA
#define AFX_DATA
#elif defined(LAVAEXECS_IMPORT)
#define LAVAEXECS_DLL __declspec( dllimport )
#undef AFX_DATA
#define AFX_DATA
#endif


class LAVAEXECS_DLL CExecFrame : public wxMDIChildFrame
{
//  DECLARE_DYNCREATE(CExecFrame)
public:
  CExecFrame(QWidget *parent);
  FRAMEFACTORY(CExecFrame)
  virtual void InitialUpdate();
  void closeEvent (QCloseEvent *ev);

// Attributes
public:
   CComboBar *m_ComboBar;
   bool isConstr;
   LavaDECL* myDECL;
   wxDocument* myDoc;
   DString CalcTitle(LavaDECL *decl);
   void NewTitle(LavaDECL* decl);
// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  // {{AFX_VIRTUAL(CExecFrame)
  public:
//  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, wxMainFrame* pParentWnd = NULL, CCreateContext* pContext = NULL);
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CExecFrame() {}

protected:
  bool OnCreate(wxDocTemplate *temp, wxDocument *doc);
  void OnDestroy();
};


#endif
