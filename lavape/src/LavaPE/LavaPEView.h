// LavaPEView.h : interface of the CLavaPEView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_LAVAPEVIEW_H__)
#define _LAVAPEVIEW_H__


#include "TreeView.h"
#include "LavaGUIView.h"
#include "LavaPEDoc.h"
#include "Syntax.h"
//#include "wxExport.h"
#include "qcombobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
#include "qclipboard.h"
#include "qdragobject.h"


enum TIType {
  TIType_DECL,         // LavaDECL node
  TIType_CHEEnumSel,   // enum item node 
  TIType_EnumItems,    // enum items node    //this is not a node in syntax
  TIType_VTypes,       // virtual type node  //this is not a node in syntax
  TIType_Input,        // input node         //this is not a node in syntax
  TIType_Output,       // output node        //this is not a node in syntax
  TIType_Defs,         // local pattern node //this is not a node in syntax
  TIType_Features,     // local member node  //this is not a node in syntax
  TIType_Constraint,   // invariant node     //this is not a node in syntax
  TIType_Refac,        // only used as return value
  TIType_NoType
};

enum TRefacMove {
  privToPublic,
  publicToPriv,
  baseToEx,
  exToBase,
  noRefac
};


enum TisOnWhat {  //CLavaPEWizard creation cause
  isOnNew,
  isOnEdit,
  isOnFormProps
};


class LavaSource: public QStoredDrag
{
public:
  LavaSource(QWidget* dragSource) :QStoredDrag ( "QLavaTreeView", dragSource, "LavaSource_Drag") {}
  LavaSource() :QStoredDrag ( "QLavaTreeView",0,"LavaSource_Copy") {}

  virtual const char* format( int i = 0 ) const;
private:
  Q_OBJECT
};


class CMainItemData : public TItemData {
public:
  CMainItemData() { docPathName = 0; ClipTree = 0;}
  CMainItemData(TIType tt, unsigned long synel, bool exp = false)
       {type = tt; synEl = synel; toExpand = exp; docPathName = 0; ClipTree = 0;}
  TIType type;
  unsigned long synEl;
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
  VIEWFACTORY(CLavaPEView)
  CLavaPEDoc* GetDocument();
  virtual ~CLavaPEView();
  virtual void UpdateUI();
  void DisableActions();
  virtual void OnInitialUpdate(); // called first time after construct
  virtual void OnUpdate(wxView* pSender, unsigned lHint, QObject* pHint);
  virtual void OnActivateView(bool bActivate=true, wxView *deactiveView=0);

  QPixmap* GetPixmap(bool isParent, bool isAttr, TDeclType deftype, const SynFlags flag=(const unsigned long)0);

  //CImageList  *m_pimagelist;
  //CBitmap bitmap;
  //CMenu PopupMenu;
  CTreeItem* lastCurrent;
  CTreeItem* ItemSel; //
  CTreeItem* ParItemSel;
  CMainItemData* DataSel;
  CMainItemData* ParDataSel;
  TDeclType DefTypeSel;
  CTreeItem* SelItem; //item that should be selected
  TDeclType SelType;
  TIType GroupType;
  LavaDECL* VisibleDECL;
  bool InitFinished;
  bool updateTree;
  bool inSync;
  bool drawTree;
  LavaDECL * myDECL;
  TID myID;
  LavaDECL ** p_myDECL;
  CLavaGUIView* myFormView;  //myFormView != 0 :  -> FormView
  CTreeView* myInclView;    //myInclView != 0: -> main view
  CTreeView *myVTView;      //myVTView != 0: -> main view
  CLavaPEView *myMainView;  //myMainView != 0: -> FormView

  void ExpandTree(CTreeItem* top=NULL);
  void DrawTree(LavaDECL ** pDECL, bool inUndoRedo = false, bool finalUpdate = true, int checkLevel = 0);
  bool DrawEmptyOpt(CTreeItem* startItem, bool down);
  void RemoveEmptyOpt(CTreeItem* startItem, bool down);
  bool ExpandItem(CTreeItem* item, int level = -1); //level = -1: expand the complete subtree
  void SetTreeFlags(CTreeItem* item, bool exp);
  void OnShowSpecialView(TDeclType exprType); 
  CTreeItem* BrowseTree(LavaDECL* decl, CTreeItem* startItem, DString* enumID=0);
  bool VerifyItem(CTreeItem* item, CTreeItem* topItem = 0);
  CMainItemData* Navigate(bool all, CTreeItem*& item, int& level);
  bool IsChildNodeOf(CTreeItem* hitemChild, CTreeItem* hitemSuspectedParent);
  int GetPos(CTreeItem* item, CTreeItem* prev=0);
  CTreeItem* getSectionNode(CTreeItem* parent, TDeclType ncase);
  void SetErrAndCom(CTreeItem* item);
  void DeleteItemData(CTreeItem* parent = NULL);
  void Gotodef(CTreeItem* item);
//  void SetAllStates(CTreeItem* item1, unsigned nState, unsigned nStateMask, bool inPattern);
  bool PutEC(CTreeItem* item, bool onErr);
  void OnNextEC(CTreeItem* itemStart, bool onErr);
  void OnPrevEC(CTreeItem* itemStart, bool onErr);
  virtual void OnVkreturn();
  void CheckAutoCorr(LavaDECL* decl);
  void GenHTML(LavaDECL *pnode,TDeclType &category, bool &firstChild);
  void OnGenHtmlI();
  void whatNext();

  //Document update actions----------------------------------------

  bool CanDelete;
  //COleDropTarget m_dropTarget;
  char* m_nIDClipFormat;
//  UINT m_nIDDragBackFormat;
  int CollectPos;
  CMainItemData* Clipdata;
  bool clipboard_lava_notEmpty;
  TDeclType defTypeSpicked;
  SynFlags treeflagsSpicked;
  SynFlags secondtflagsSpicked;

  LavaDECL* CollectDECL;
  CTreeItem* m_hitemDrag;
  CTreeItem* m_hitemDragP;
  CTreeItem* m_hitemDragPP;
  CTreeItem* m_hitemDrop;
  LavaSource* dragSource;
  LavaDECL* declDrag, **pDeclDragP; //defined in drag-view
  TRefacMove RefacCase;   //defined in drop-view
  CLavaPEDoc* DragDoc;    //defined in drop-view
  int DragINCL, DropINCL; //defined in drop-view

  bool EnableDelete(LavaDECL* decl);
  CLavaPEHint* ChangeEnum(LavaDECL* clipDECL,  CTreeItem* item, bool cut, bool paste);
  bool OnInsert(TDeclType eType, LavaDECL* iDECL = NULL);
  void OnEditSelItem(CTreeItem* item, bool clickedOnIcon); 

  void IOnEditCopy();
  TIType CanPaste (TDeclType defType, SynFlags typeFlags, SynFlags secondtflags, CTreeItem* toItem/*, bool& toInter*/);

  QDragObject*  OnBegindrag();
  void OnDragOver(QDragMoveEvent* ev);
  void OnDragEnter(QDragEnterEvent* ev);
  void OnDragLeave(QDragLeaveEvent* ev);
  virtual void OnDrop(QDropEvent* ev);
  bool RefacMove(CTreeItem* dropItem);
  void CorrVT_BaseToEx(LavaDECL *dropParent, LavaDECL *dragParent, LavaDECL *clipDecl, bool& mdh);
  void CorrVT_ExToBase(LavaDECL *dragParent, LavaDECL *dropParent, LavaDECL *clipDecl, bool& mdh, CHE*& vtHints);
  bool DoRefac(LavaDECL* dropDECL, bool& mdh, CHE*& vtHints);
  bool AddToDragChain(CTreeItem* itemDrag, bool vkControl = false, bool sameCat = true);
  void DeleteDragChain();
  virtual bool event(QEvent* ev);
protected:
  virtual void destroy();

    //Toolbutton update handler
  void OnUpdateLitStr(wxAction* action);
  void OnUpdateOverride(wxAction* action);
  void OnUpdateEditCopy(wxAction* action);
  void OnUpdateGotodef(wxAction* action);
  void OnUpdateShowformview(wxAction* action);
  void OnUpdateEditPaste(wxAction* action);
  void OnUpdateNewenum(wxAction* action);
  void OnUpdateNewfunction(wxAction* action);
  void OnUpdateNewinitiator(wxAction* action);
  void OnUpdateNewmember(wxAction* action);
  void OnUpdateNewNamespace(wxAction* action);
  void OnUpdateNewclassimpl(wxAction* action);
  void OnUpdateEditCut(wxAction* action);
  void OnUpdateShowOptionals(wxAction* action);
  void OnUpdateComment(wxAction* action);
  void OnUpdateCSpec(wxAction* action);
  void OnUpdateComponent(wxAction* action);
  void OnUpdateEditSel(wxAction* action);
  void OnUpdateNewVirtualType(wxAction* action);
  void OnUpdateNewclassintf(wxAction* action);
  void OnUpdateNewset(wxAction* action);
	void OnUpdateNewEnumItem(wxAction* action);
	void OnUpdateGotoImpl(wxAction* action);
	void OnUpdateFindreferences(wxAction* action);
  //void OnUpdateNewalias(wxAction* action);
  //void OnUpdateNewform(wxAction* action);
  //void OnUpdateShowPattern(wxAction* action);
  //void OnUpdateDefaultForm(wxAction* action);

public:
//Toolbutton/menu item handler  
  void OnExpandAll();//
  void OnEditCopy(); //
  void OnEditCut(); //
  void OnEditPaste(); //
  void OnEditSel();//
  void OnShowConstraint();
  void OnShowformview();//
  virtual void OnDelete();//
  void OnGotodef();//
  void OnNewmember();//
  void OnNewclassimpl();//
  void OnShowOverridables();//
  void OnNewclassintf();//
  void OnNewenum();//
  void OnNewfunction();//
  void OnNewinitiator();//
  void OnNewNamespace();//
  void OnShowOptionals();//
  void OnCollapsAll();//
  void OnComment();//
  void OnComponent();//
  void OnCSpec();//
  void OnNewVirtualType();//
  void OnShowAllEmptyOpt();//
  void OnRemoveAllEmptyOpt();//
  void OnNewset();//
  void OnNextError();//
  void OnPrevError();//
  void OnNextComment();//
  void OnPrevComment();//
  void OnLitStr();//
//  void OnShowPattern();
	void OnNewEnumItem();//
	void OnGotoImpl();//
	void OnFindreferences();//

  void OnGenHtml();
  void OnGenHtmlS();
  virtual void resizeEvent(QResizeEvent * );

  virtual void startRename(CTreeItem* item);
  virtual void okRename(CTreeItem* item);
  virtual void cancelRename(CTreeItem* item);


public slots: 
  void OnDblclk(QListViewItem* onItem, const QPoint&, int);
  void OnSelchanged();
  void OnRclick(QListViewItem* item);
  void OnItemexpanded(QListViewItem* item);
  void OnItemcollapsed(QListViewItem* item);

  //void OnLButtonDown(UINT nFlags, QPoint point);
  //void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //LRESULT OnCalledView(WPARAM, LPARAM lparam);
  //LRESULT OnAutoCorr(WPARAM wparam, LPARAM lparam);
  //LRESULT OnSyncTree(WPARAM wparam, LPARAM lparam);


private:
  Q_OBJECT
};

#endif
