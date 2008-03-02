#ifndef __ExecFrame
#define __ExecFrame

#include "ComboBar.h"
#include "docview.h"
#include "mdiframes.h"
#include "Constructs.h"


class LAVAEXECS_DLL CExecFrame : public wxChildFrame
{
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

  public:

// Implementation
public:
  virtual ~CExecFrame();

protected:
  bool OnCreate(wxDocTemplate *temp, wxDocument *doc);
  void OnDestroy();
};


#endif
