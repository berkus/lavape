#ifndef __LavaPEView
#define __LavaPEView


#include "TreeView.h"
#include "LavaGUIView.h"
#include "LavaPEDoc.h"
#include "Syntax.h"
#include "qcombobox.h"
//Added by qt3to4:
#include <QPixmap>
#include <QDrag>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include "docview.h"
#include "qobject.h"
#include "qevent.h"
#include "qstring.h"
#include "qclipboard.h"
//#include "q3dragobject.h"

/*
class TreeWhatsThis// : public WhatsThis
{
public:
  TreeWhatsThis(MyListView *lv);

  QString text(const QPoint&);

private:
  MyListView *listView;
};
*/

enum TIType {
  TIType_DECL,         // LavaDECL node
  TIType_CHEEnumSel,   // enum item node
  TIType_EnumItems,    // enum items node    //this is not a node in syntax
  TIType_VTypes,       // virtual type node  //this is not a node in syntax
  TIType_Input,        // input node         //this is not a node in syntax
  TIType_Output,       // output node        //this is not a node in syntax
  TIType_Defs,         // local pattern node //this is not a node in syntax
  TIType_Features,     // local member node  //this is not a node in syntax
  TIType_Require,
  TIType_Ensure,
  TIType_Exec,   // invariant/exec node
  TIType_Refac,        // only used as return value
  TIType_NoType
};

enum TisOnWhat {  //CLavaPEWizard creation cause
  isOnNew,
  isOnEdit,
  isOnFormProps
};

enum PMFlag {emptyPM, invariantPM, GUIPM, SetPM};


class LavaSource: public QMimeData//Q3StoredDrag
{
public:
  LavaSource(QWidget* dragSource) :QMimeData() {}//!!Q3StoredDrag ( "QLavaTreeView", dragSource, "LavaSource_Drag") {}
  LavaSource() :QMimeData (){}//!! "QLavaTreeView",0,"LavaSource_Copy") {}

  //virtual const char* format( int i = 0 ) const;
private:
  Q_OBJECT
};


class CMainItemData : public TItemData {
public:
  CMainItemData() { docPathName = 0; synEl = 0; ClipTree = 0;}
  CMainItemData(TIType tt, void *synel, bool exp = false)
       {type = tt; synEl = synel; toExpand = exp; docPathName = 0; ClipTree = 0;}
  TIType type;
  /*unsigned long*/ void *synEl;
  bool toExpand;
  DString *docPathName;
  SyntaxDefinition* ClipTree; //only for clipboard and drag and drop operation

  virtual void Serialize(QDataStream& ar);
  TDeclType Spick( QByteArray& ar, SynFlags& treeflags, SynFlags& secondtflags);

  void Destroy();
  ~CMainItemData();
};


class CLavaPEView : public CTreeView
{
public:
  CLavaPEView(QWidget* parent, wxDocument* doc);
  VIEWFACTORY(CLavaPEView);
  virtual ~CLavaPEView();
  void CleanListView();

  bool InitFinished;
  bool updateTree;
  bool inSync;
  bool drawTree;
  bool DropPosted;
  bool CanDelete;
  //bool clipboard_lava_notEmpty;
  //const char* m_nIDClipFormat;

  CLavaGUIView* myFormView;  //myFormView != 0 :  -> FormView
  CLavaPEDoc* DragDoc;    //defined in drop-view
  CLavaPEView *myMainView;  //myMainView != 0: -> FormView
  CMainItemData* DataSel; //used in OnUpdate...
  CMainItemData* Clipdata;
  CTreeItem* lastCurrent;
  CTreeItem* ItemSel;     //used in OnUpdate...
  CTreeItem* m_hitemDrag;
  CTreeItem* lastPressed;
  CTreeItem* lastDropped;
  CTreeItem* m_hitemDragP;
  CTreeItem* m_hitemDragPP;
  CTreeItem* m_hitemDrop;
  CTreeItem* SelItem; //item that should be selected
  CTreeView* myInclView;    //myInclView != 0: -> main view
  CTreeView *myVTView;      //myVTView != 0: -> main view
  int CollectPos;
  int DragINCL, DropINCL; //defined in drop-view
  LavaDECL* declDrag, **pDeclDragP; //defined in drag-view
  LavaDECL * myDECL;
  LavaDECL ** p_myDECL;
  LavaDECL* VisibleDECL;
  LavaSource* dragSource;
  //SynFlags treeflagsSpicked;
  //SynFlags secondtflagsSpicked;
  //TDeclType defTypeSpicked;
  TDeclType DefTypeSel;   //used in OnUpdate...
  TDeclType SelType;
  TID myID;
  TIType GroupType;       //used in OnUpdate...
  TRefacMove RefacCase;   //defined in drop-view

  void StoreSelection();
  bool AddToDragChain(CTreeItem* itemDrag, bool vkControl = false, bool sameCat = true);
  CTreeItem* BrowseTree(LavaDECL* decl, CTreeItem* startItem, DString* enumID=0);
  TIType CanPaste (TDeclType defType, SynFlags typeFlags, SynFlags secondtflags, CTreeItem* toItem/*, bool& toInter*/);
  CLavaPEHint* ChangeEnum(LavaDECL* clipDECL,  CTreeItem* item, bool cut, bool paste);
  void CheckAutoCorr(LavaDECL* decl);
  void CorrVT_BaseToEx(LavaDECL *dropParent, LavaDECL *dragParent, LavaDECL *clipDecl, bool& mdh);
  void CorrVT_ExToBase(LavaDECL *dragParent, LavaDECL *dropParent, LavaDECL *clipDecl, bool& mdh, CHE*& vtHints);
  void DeleteDragChain();
  void DeleteItemData(CTreeItem* parent = NULL);
  void DisableActions();
  bool DrawEmptyOpt(CTreeItem* startItem, bool down);
  void DrawTree(LavaDECL ** pDECL, bool inUndoRedo = false, bool finalUpdate = true, int checkLevel = 0);
  bool EnableDelete(LavaDECL* decl);
  virtual bool event(QEvent* ev);
  bool ExpandItem(CTreeItem* item, int level = -1); //level = -1: expand the complete subtree
  void ExpandTree(CTreeItem* top=NULL);
  void GenHTML(LavaDECL *pnode,TDeclType &category, bool &firstChild);
  CLavaPEDoc* GetDocument();
  LavaDECL* GetExecDECL(CTreeItem* item);
  int GetPixmap(bool isParent, bool isAttr, TDeclType deftype, const SynFlags flag=(const unsigned long)0);
  int GetPos(CTreeItem* item, CTreeItem* prev=0);
  CTreeItem* getSectionNode(CTreeItem* parent, TDeclType ncase);
  void Gotodef(CTreeItem* item);
  void IOnEditCopy();
  bool IsChildNodeOf(CTreeItem* hitemChild, CTreeItem* hitemSuspectedParent);
  CMainItemData* Navigate(bool all, CTreeItem*& item, int& level);
  QDrag*  OnDragBegin();
  void OnDragEnter(QDragEnterEvent* ev);
  void OnDragLeave(QDragLeaveEvent* ev);
  void OnDragOver(QDragMoveEvent* ev);
  virtual void OnDrop(QDropEvent* ev);
  void OnDropPost(void* act);
  void OnEditSelItem(CTreeItem* item, bool clickedOnIcon);
  void OnGenHtmlI();
  virtual void OnInitialUpdate(); // called first time after construct
  bool OnInsert(TDeclType eType, LavaDECL* iDECL = NULL);
  void OnNextEC(CTreeItem* itemStart, bool onErr);
  void OnPrevEC(CTreeItem* itemStart, bool onErr);
  void OnShowSpecialView(TDeclType exprType);
  virtual void OnUpdate(wxView* pSender, unsigned lHint, QObject* pHint);
  virtual void OnVkreturn();
  bool PutEC(CTreeItem* item, bool onErr);
  bool Refac(LavaDECL* dropDECL, bool& mdh, CHE*& vtHints);
  bool RefacMove(CTreeItem* dropItem);
  void RemoveEmptyOpt(CTreeItem* startItem, bool down);
  virtual void RenameCancel(CTreeItem* item);
  virtual void RenameOk(QLineEdit* editor, CTreeItem* item);
  virtual void RenameStart(QLineEdit* editor, CTreeItem* item);
  void AttachHandler(CAttachData* attachData);
//  void SetAllStates(CTreeItem* item1, unsigned nState, unsigned nStateMask, bool inPattern);
  void setSelPost(QTreeWidgetItem* selItem);
  void SetErrAndCom(CTreeItem* item);
  void SetTreeFlags(CTreeItem* item, bool exp);
  virtual void UpdateUI();
  bool VerifyItem(CTreeItem* item, CTreeItem* topItem = 0);
  void on_whatNextAction_triggered();
  bool eventFilter(QObject *watched, QEvent *ev);
  QString text(const QPoint &point);

protected:
  virtual void destroy();

    //Toolbutton update handler
  void OnUpdateComment(QAction* action);
  void OnUpdateEditCopy(QAction* action);
  void OnUpdateEditCut(QAction* action);
  void OnUpdateEditPaste(QAction* action);
  void OnUpdateEditSel(QAction* action);
	void OnUpdateFindreferences(QAction* action);
  void OnUpdateGotodef(QAction* action);
	void OnUpdateGotoImpl(QAction* action);
  void OnUpdateMakeGUI(QAction* action);
  void OnUpdateNewComponent(QAction* action);
  void OnUpdateNewCSpec(QAction* action);
  void OnUpdateNewenum(QAction* action);
	void OnUpdateNewEnumItem(QAction* action);
  void OnUpdateNewfunction(QAction* action);
  void OnUpdateNewinitiator(QAction* action);
  void OnUpdateNewInterface(QAction* action);
  void OnUpdateNewImpl(QAction* action);
  void OnUpdateNewLitStr(QAction* action);
  void OnUpdateNewmember(QAction* action);
  void OnUpdateNewPackage(QAction* action);
  void OnUpdateNewVirtualType(QAction* action);
  void OnUpdateNewset(QAction* action);
  void OnUpdateOverride(QAction* action);
  void OnUpdateShowformview(QAction* action);
  void OnUpdateShowOptionals(QAction* action);
  //void OnUpdateNewalias(QAction* action);
  //void OnUpdateNewform(QAction* action);
  //void OnUpdateShowPattern(QAction* action);
  //void OnUpdateDefaultForm(QAction* action);

public:
//Toolbutton/menu item handler
  void OnCollapseAll();//
  void OnComment();//
  virtual void OnDelete();//
  void OnExpandAll();//
  void OnEditCopy(); //
  void OnEditCut(); //
  void OnEditPaste(); //
  void OnEditSel();//
	void OnFindReferences();//
  void OnGenHtml();
  void OnGenHtmlS();
  void OnGotoDecl();//
	void OnGotoImpl();//
	void OnMakeGUI();//
  void OnNewComponent();//
  void OnNewCSpec();//
  void OnNewenum();//
	void OnNewEnumItem();//
  void OnNewfunction();//
  void OnNewinitiator();//
  void OnNewInterface();//
  void OnNewImpl();//
  void OnNewLitStr();//
  void OnNewmember();//
  void OnNewPackage();//
  void OnNewset();//
  void OnNewVirtualType();//
  void OnNextComment();//
  void OnNextError();//
  void OnPrevComment();//
  void OnPrevError();//
  void OnRemoveAllEmptyOpt();//
  void OnShowAllEmptyOpt();//
  void OnShowExec(TDeclType type);
  void OnShowGUIview();//
  void OnShowOptionals();//
  void OnShowOverridables();//
//  void OnShowPattern();

public slots:
  void OnDblclk( QTreeWidgetItem* item, int col);//QTreeWidgetItem* onItem, const QPoint&, int);
  void OnSelchanged(QTreeWidgetItem* selItem, QTreeWidgetItem* );
  void OnRclick(QTreeWidgetItem* item);
  void OnItemexpanded(QTreeWidgetItem* item);
  void OnItemcollapsed(QTreeWidgetItem* item);

  //void OnLButtonDown(UINT nFlags, QPoint point);
  //void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //LRESULT OnCalledView(WPARAM, LPARAM lparam);
  //LRESULT OnAutoCorr(WPARAM wparam, LPARAM lparam);
  //LRESULT OnSyncTree(WPARAM wparam, LPARAM lparam);


private:
  Q_OBJECT
};

#endif
