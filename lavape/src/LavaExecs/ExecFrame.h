#ifndef __ExecFrame
#define __ExecFrame

#include "ComboBar.h"
#include "docview.h"
#include "mdiframes.h"
#include "Constructs.h"
#include "ExecView.h"
#include "ExecFrame.h"


class LAVAEXECS_DLL CExecFrame : public wxChildFrame
{
public:
  CExecFrame(QWidget *parent);
  FRAMEFACTORY(CExecFrame)
  virtual void InitialUpdate();
  void closeEvent (QCloseEvent *ev);
  void focusIn() { if (m_ExecView) m_ExecView->focusIn(); }

// Attributes
  CComboBar *m_ComboBar;
  CExecView *m_ExecView;
  bool isConstr;
  LavaDECL* myDECL;
  wxDocument* myDoc;

// Operations
  DString CalcTitle(LavaDECL *decl);
  void NewTitle(LavaDECL* decl);
  virtual ~CExecFrame();

protected:
  bool OnCreate(wxDocTemplate *temp, wxDocument *doc);
  void OnDestroy();

private:
  Q_OBJECT
};


#endif
