#ifdef _GNUG__
#pragma interface
#endif

// LavaPEFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLavaGUIView form view


#ifndef __LavaPEFormView
#define __LavaPEFormView


/*
#include "stdafx.h"
*/

#include "GUIProgBase.h"
#include "GUIProg.h"
#include "Syntax.h"
#include "LavaBaseDoc.h"
#include "PEBaseDoc.h"
#include "docview.h"
#include "qscrollview.h"
#include "qvbox.h"

class LAVAGUI_DLL GUIVBox : public QFrame
{
public:
  GUIVBox(QWidget* parent, bool fromPopup, QScrollView* view):QFrame(parent, "GUIVBox") 
    {View = view; FromPopup = fromPopup;}
  QSize sizeHint() { return size();}
  bool FromPopup;
  QScrollView * View;
  
private:
  Q_OBJECT
};

class LAVAGUI_DLL GUIScrollView : public QScrollView {
public:
  GUIScrollView (QWidget *parent, bool fromPopup);
  void viewportResizeEvent(QResizeEvent* ev);
  GUIVBox* qvbox;
  QRect MaxBottomRight;
  
private:
  Q_OBJECT
};

class LAVAGUI_DLL CLavaGUIView : public CLavaBaseView
{
public:
  CLavaGUIView(QWidget *parent,wxDocument *doc);           // protected constructor used by dynamic creation
  virtual ~CLavaGUIView();
  VIEWFACTORY(CLavaGUIView)
  virtual void UpdateUI();
  virtual void DisableActions();
  virtual void OnInitialUpdate();
  virtual void OnUpdate(wxView* pSender, unsigned lHint, QObject* pHint);
  virtual void OnActivateView(bool bActivate=true, wxView *deactiveView=0);
  void MessToStatusbar();

  GUIScrollView* scrollView() {return myScrv;}
  GUIVBox* qvbox;
  CLavaBaseDoc* GetDocument();
  CLavaBaseView* mainTree;
  CLavaBaseView* myTree;
  TID myID;
  LavaDECL *myDECL;
  bool inUpdate;
  CGUIProg *myGUIProg;
  CLavaThread* myThread;
  LavaVariablePtr ServicePtr;
  LavaVariablePtr IniDataPtr;
  LavaVariablePtr ResultDPtr;
  bool CurrentCategory;
  CHEFormNode* LastBrowseNode;

  void SyncForm(LavaDECL* formDECL);
  void resetLastBrowseNode();
  void OnModified();
  virtual bool event(QEvent* ev);
  bool OnKill();
  void NoteNewObj(LavaObjectPtr obj);
  void NoteLastModified();

  void OnDeleteOpt();
  void OnInsertOpt();
  void OnOK();
  void OnCancel();
	void OnTogglestate();
  void OnEditCopy();
  void OnEditCut();
  void OnEditPaste();
  void OnGotodef();

  void OnUpdateGotodef(wxAction* action);
	void OnUpdateTogglestate(wxAction* action);
  void OnUpdateEditPaste(wxAction* action);
  void OnUpdateEditCopy(wxAction* action);
  void OnUpdateEditCut(wxAction* action);
  void OnUpdateCancel(wxAction* action);
  void OnUpdateOk(wxAction* action);
  void OnUpdateDeleteopt(wxAction* action);
  void OnUpdateInsertopt(wxAction* action);
  void OnChoosefont();
  void setNewLabelFont();
//  void setNewButtonFont();
protected:
  GUIScrollView* myScrv;
  QString StatusbarMess;

private:
  Q_OBJECT
};


/*
  void OnDraw();
  void SetScreenshot();
  void GetActViewSize(QSize& size);

	CBitmap *m_pBmp;     // bitmap in the dc holding screenshot
	CBitmap *m_pOldBmp;  // old bitmap
	CDC *m_pMemDC;       // memory dc holding the screenshot
  QSize BitmapSize;


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLavaGUIView)
  public:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual bool PreCreateWindow(CREATESTRUCT& cs);
  //}}AFX_VIRTUAL


*/
/////////////////////////////////////////////////////////////////////////////

#endif
