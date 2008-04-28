#ifndef __LavaPEFormView
#define __LavaPEFormView


#include "GUIProgBase.h"
#include "GUIProg.h"
#include "Syntax.h"
#include "LavaBaseDoc.h"
#include "PEBaseDoc.h"
#include "docview.h"
#include "qscrollarea.h"
//#include "q3vbox.h"
#include "qdialog.h" 
//Added by qt3to4:
#include <QVBoxLayout>
#include <QFrame>
#include <QResizeEvent>
#include <QEvent>
#include <QCloseEvent>

class LAVAGUI_DLL GUIVBox : public QFrame
{
public:
  GUIVBox(QWidget* parent, bool fromPopup, QScrollArea* view)
    :QFrame(parent) 
    {View = view; FromPopup = fromPopup; setObjectName("GUIVBox"); }
  QSize sizeHint() { return size();}
  bool FromPopup;
  QScrollArea * View;
  
private:
  Q_OBJECT
};

class LAVAGUI_DLL GUIScrollView : public QScrollArea {
public:
  GUIScrollView (QWidget *parent, bool fromPopup);
  void resizeEvent(QResizeEvent* ev);
  //virtual void focusInEvent(QFocusEvent *ev);
  GUIVBox* qvbox;
  QRect MaxBottomRight;
  
private:
  Q_OBJECT
};

class LAVAGUI_DLL LavaGUIDialog : public QDialog
{
public:
  LavaGUIDialog(QWidget *parent,CLavaPEHint *pHint);
    // protected constructor used by dynamic creation
  virtual ~LavaGUIDialog();

  
  void UpdateUI();
  void OnUpdateDeleteopt(QAction* action);
  void OnUpdateInsertopt(QAction* action);
  void OnDeleteOpt();
  void OnInsertOpt();

  GUIScrollView* scrollView() {return myScrv;}
  void NewTitle(LavaDECL *decl, const DString& lavaName);
  void setpropSize(QSize& scrSize);

  GUIVBox* qvbox;
  CLavaBaseDoc* myDoc;
  TID myID;
  LavaDECL *myDECL;
  CGUIProg *myGUIProg;
  CLavaThread* myThread;
  LavaVariablePtr ServicePtr;
  LavaVariablePtr IniDataPtr;
  LavaVariablePtr ResultDPtr;
  QVBoxLayout *qvbl;
  bool returned;
  bool inInit;

  void closeEvent(QCloseEvent *e);

protected:
  GUIScrollView* myScrv;

public slots:
  void OnOK();
  void OnReset();
  void OnCancel();

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
//  void AddButtons();
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
  bool released;
  CGUIProg *myGUIProg;
//  CLavaThread* myThread;
  LavaVariablePtr ServicePtr;
  LavaVariablePtr IniDataPtr;
  LavaVariablePtr ResultDPtr;
  bool CurrentCategory;
  CHEFormNode* LastBrowseNode;
  bool clipboard_text_notEmpty;

  void SyncForm(LavaDECL* formDECL);
  void resetLastBrowseNode();
  void OnModified();
  virtual bool event(QEvent* ev);
  bool OnKill();
  void NoteNewObj(LavaObjectPtr obj);
  void NoteLastModified();

  void OnDeleteOpt();
  void OnInsertOpt();
	//void OnTogglestate();
  void OnEditCopy();
  void OnEditCut();
  void OnEditPaste();
  void OnGotoDecl();
  //void OnNewHandler();

  //void OnUpdateNewHandler(QAction* action);
  void OnUpdateGotodef(QAction* action);
	void OnUpdateTogglestate(QAction* action);
  void OnUpdateEditPaste(QAction* action);
  void OnUpdateEditCopy(QAction* action);
  void OnUpdateEditCut(QAction* action);
  void OnUpdateCancel(QAction* action);
  void OnUpdateOk(QAction* action);
  void OnUpdateDeleteopt(QAction* action);
  void OnUpdateInsertopt(QAction* action);
  void OnChoosefont();
  void setNewLabelFont();
//  void setNewButtonFont();
protected:
  GUIScrollView* myScrv;
  QString StatusbarMess;

public slots:
  void OnOK();
  void OnReset();
  void OnCancel();

private:
  Q_OBJECT
};


#endif
