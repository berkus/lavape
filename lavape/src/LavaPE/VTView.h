// VTView.h : header file
//

#ifndef __VTView
#define __VTView

#include "LavaPEView.h"
#include "ExecTree.h"
//#include "wxExport.h"
#include "qcombobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"


enum TNodeType { TNodeType_Class, TNodeType_VT, TNodeType_Func, TNodeType_Mem };

class CVTItemData : public TItemData {
public:
  CVTItemData() {}
  CVTItemData(TNodeType t, CHETVElem* V, int A) {type = t; VTEl = V; AmbgNo = A;}
  TNodeType type;
  CHETVElem* VTEl;
  int AmbgNo;
};


/////////////////////////////////////////////////////////////////////////////
// CVTView view

class CVTView : public CTreeView
{
public:
  CVTView(QWidget* parent, wxDocument* doc);
  virtual void UpdateUI();
  void DisableActions();
  virtual void OnInitialUpdate();
  virtual bool OnCreate();//LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  virtual void OnUpdate(wxView* pSender, unsigned lHint, QObject* pHint);
  //virtual bool PreCreateWindow(CREATESTRUCT& cs);
  virtual void OnActivateView(bool bActivate=true, wxView *deactiveView=0);

  CLavaPEView* myMainView;
  LavaDECL * myDECL; //=the decl from which it was initiated
  TID myID;
  ChainAny myVT;
  LavaDECL* CollectDECL;
  CTreeItem *collectParent, *lastCurrent;
  bool activeInt;

  CLavaPEDoc* GetDocument();

  void DeleteItemData(CTreeItem* parent);
  void ExpandItem(CTreeItem* item, int level);
  void PrepareDECL(LavaDECL* decl, CVTItemData* itd);
  void OnOverrideI(CTreeItem* item);
  bool EnableOverride(CTreeItem* item);
  bool AddToExChain(CTreeItem* itemOver);
  void DeleteExChain();
  void SetVTError(CTreeItem* item);
  bool VerifyItem(CTreeItem* item, CTreeItem* topItem = 0);
  bool DrawTreeAgain();
//  void SetAllStates(CTreeItem* item1, unsigned nState, unsigned nStateMask);
  void gotoDef(CTreeItem* item);
  CTreeItem* BrowseTree(TID id, CTreeItem* start);


protected:
  virtual ~CVTView();
  virtual void destroy();

  void OnUpdateGotodef(wxAction* action);
  void OnUpdateOverride(wxAction* action);

public:
  void OnGotodef();
  void OnOverride();

public slots:
  void OnSelchanged();
  //void OnDblclk(QListViewItem* onItem, const QPoint&, int);
private:
  Q_OBJECT
};

extern QPixmap* findPixmap(const QString& lab);

#endif


/////////////////////////////////////////////////////////////////////////////
