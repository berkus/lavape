/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
	 (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#ifdef __GNUC__
#pragma implementation
#endif


#include "LavaPE.h"

#include "LavaBaseStringInit.h"
#include "LavaPEStringInit.h"
#include "LavaPEView.h"
#include "VTView.h"
#include "LavaPEFrames.h"
//#include "LavaGUIView.h"
#include "ExecTree.h"
#include "DString.h"
#include "Comment.h"
#include "SynIO.h"
#include "LavaPEWizard.h"
#include "Boxes.h"
#include "FindRefsBox.h"
//#include "wxExport.h"
#include "docview.h"
#include "qlineedit.h"
#include "qobject.h"
#include "qstring.h"
#include "qstatusbar.h"
#include "qcheckbox.h"
#include "cmainframe.h"
#include "qpixmapcache.h"
#include "qclipboard.h"
#include "qdragobject.h"
#include "qdatastream.h"
#include "qmessagebox.h"

/////////////////////////////////////////////////////////////////////////////
// CLavaPEView

static int ContainTab [FormDef+1] [DragIO+1] = {  
  //ContainTab[type1, type2] = 0 type1 contains no type2,
  //                         = 1 type1 may contain some type2

               // has 
               //types,Int,Im,Ini, Fu,Pack,Comp,CSpc,CpOb,For,Ali, VT,Bas,Att,IAt,OAt,Cns,FTx,PD, DP, DD, DF,DFF,DIO
  /*NoDef*/      {  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0 }, //NoDef
  /*Interface*/  {  1,  1,  0,  0,  1,  0,  0,   0,   0,   1,  1,  1,  0,  1,  0,  0,  1,  0,  1,  1,  1,  1,  1, 1 }, //Interface
  /*Impl*/       {  1,  1,  1,  0,  1,  0,  0,   0,   0,   1,  1,  0,  0,  1,  0,  0,  0,  0,  0,  1,  1,  1,  1, 1 }, //Impl
  /*Initiator*/  {  1,  1,  1,  1,  0,  0,  0,   1,   0,   1,  1,  1,  0,  0,  1,  0,  1,  0,  1,  1,  1,  1,  1, 1 }, //Initiator
  /*Function*/   {  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  1,  0, 1 }, //Function
  /*Package*/    {  1,  1,  1,  1,  0,  1,  0,   1,   0,   1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0, 0 }, //Package
  /*Component*/  {  1,  1,  1,  1,  0,  1,  0,   1,   1,   1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0, 0 }, //Component
  /*CompObjSpec*/{  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0 }, //CompObjSpec
  /*CompObj*/    {  1,  1,  1,  1,  0,  1,  0,   1,   0,   1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0, 0 }, //CompObj
  /*FormDef*/    {  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0,  1,  0,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0, 0 }, //FormDef
};
/* 
   Note: Interfaces with Component flag and/or notification flag have no input, output and constraint.
         Functions have no fields (DeclType = Attr) and no constraint.
*/


void CMainItemData::Serialize(QDataStream& ar)
{
  if (ar.device()->isWritable()) {
    ar << (Q_ULONG)type;
    ar << ( Q_ULONG)((LavaDECL*)synEl)->DeclType;
    ar << ( Q_ULONG)((LavaDECL*)synEl)->TreeFlags.bits;
    ar << ( Q_ULONG)((LavaDECL*)synEl)->SecondTFlags.bits;
    ar << docPathName->c;
    ASN1tofromAr* cid = new ASN1tofromAr(&ar);
    cid->Release = RELEASE;
    CDPLavaDECL(PUT, cid, (address)synEl, false);
    CDPClipSyntaxDefinition(PUT, cid, (address)ClipTree, false);
    delete cid;
  }
  else {
    Q_ULONG wt;
    char* str;
    ar >> wt;
    type = (TIType)wt;
    ar >> wt;
    ar >> wt;
    ar >> wt;
    ar >> str;
    docPathName = new DString(str);
    synEl = (unsigned long) NewLavaDECL();
    ASN1tofromAr* cid = new ASN1tofromAr(&ar);
    cid->Release = RELEASE;
    CDPLavaDECL(GET, cid, (address)synEl, false);
    ClipTree = new SyntaxDefinition;
    CDPClipSyntaxDefinition(GET, cid, (address)ClipTree, false);
    delete cid;
  }
}
 
TDeclType CMainItemData::Spick( QByteArray& ar, SynFlags& treeflags, SynFlags& secondtflags)
{
  QDataStream stream(ar, IO_ReadOnly);
  Q_ULONG wt;
  char* str;
  TDeclType defType;
  stream >> wt;
  type = (TIType)wt;
  stream >> wt;
  defType = (TDeclType)wt;
  stream >> treeflags.bits;
  stream >> secondtflags.bits;
  stream >> str;
  docPathName = new DString(str);
  return (TDeclType)defType;
}

void CMainItemData::Destroy()
{
  if (ClipTree)
    delete ClipTree;
  if (synEl)
    delete (LavaDECL*)synEl;
}

CMainItemData::~CMainItemData()
{
  if (docPathName)
    delete docPathName;
}



const char * LavaSource::format (int i) const
{
  if (i==0)
    return "QLavaTreeView";
  else
    return 0;
}

bool CLavaPEView::event(QEvent *ev)
{
  if (ev->type() == IDU_LavaPE_CalledView) {
    wxDocManager::GetDocumentManager()->SetActiveView(this);
    return true;
  }
  else if (ev->type() == IDU_LavaPE_SyncTree) {
    CTreeItem* item = BrowseTree((LavaDECL*)((QCustomEvent*)ev)->data(), (CTreeItem*)GetListView()->firstChild());
    if (item && (item != (CTreeItem*)GetListView()->currentItem())) {
      inSync = true;
      GetListView()->setCurAndSel(item);
      GetListView()->ensureItemVisible(item);
      QApplication::postEvent(myFormView, new QCustomEvent(IDU_LavaPE_CalledView));
      return true;
    }
    else
      return false;
  }
  else if (ev->type() == IDU_LavaPE_SetLastHint) {
    GetDocument()->SetLastHint();
    return true;
  }
  else
		return wxView::event(ev);
}


CLavaPEView::CLavaPEView(QWidget* parent, wxDocument *doc)
: CTreeView(parent, doc, "LavaPEView")
{
  myDECL = 0;
  myFormView = 0;
  myVTView = 0;
  myInclView = 0;
  InitFinished = false;
  ItemSel = 0;
  DataSel = 0;
  ParItemSel = 0;
  ParDataSel = 0;
  lastCurrent = 0;
  m_nIDClipFormat = "QLavaTreeView";
  CollectDECL = 0;
  m_hitemDrag = 0;
  m_hitemDrop = 0;
  updateTree = false;
  drawTree = false;
  inSync = false;
  Clipdata = 0;
  if (!GetDocument()->MainView) 
    GetDocument()->MainView = this;
  connect(m_tree,SIGNAL(selectionChanged()), SLOT(OnSelchanged()));
  connect(m_tree,SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)), SLOT(OnDblclk(QListViewItem*, const QPoint&, int)));
  //connect(m_tree,SIGNAL(rightButtonClicked(QListViewItem*)), SLOT(OnRclick(QListViewItem*)));
  connect(m_tree,SIGNAL(expanded(QListViewItem*)), SLOT(OnItemexpanded(QListViewItem*)));
  connect(m_tree,SIGNAL(collapsed(QListViewItem*)), SLOT(OnItemcollapsed(QListViewItem*)));

  if (!GetDocument()->MainView) 
    GetDocument()->MainView = this;
//  if (LBaseData->m_lfDefTreeFont.lfHeight != 0)
  setFont(LBaseData->m_TreeFont);
  GetListView()->setAcceptDrops(true);
}

CLavaPEView::~CLavaPEView()
{
	setFocusProxy(0);
  destroy();
}

void CLavaPEView::UpdateUI()
{
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  frame->expandAction->setEnabled(true);
  frame->collapseAction->setEnabled(true);
  frame->showOptsAction->setEnabled(true);
  frame->showAllOptsAction->setEnabled(true);
  frame->nextCommentAction->setEnabled(true);
  frame->hideEmptyOptsAction->setEnabled(true);
  frame->prevCommentAction->setEnabled(true);
  frame->nextErrorAction->setEnabled(true);
  frame->prevErrorAction->setEnabled(true);

  OnUpdateEditCopy(frame->editCopyAction);
  OnUpdateEditCut(frame->editCutAction);
  OnUpdateEditCut(frame->deleteAction);
  OnUpdateEditPaste(frame->editPasteAction);

  OnUpdateNewenum(frame->newEnumAction);
  OnUpdateNewfunction(frame->newFuncAction);
  OnUpdateNewinitiator(frame->newInitiatorAction);
  OnUpdateNewmember(frame->newMemVarAction);
  OnUpdateNewNamespace(frame->newPackageAction);
  OnUpdateNewclassimpl(frame->newImplAction);
  OnUpdateCSpec(frame->newCOspecAction);
  OnUpdateComponent(frame->newCOimplAction);
  OnUpdateNewVirtualType(frame->newVTAction);
  OnUpdateNewclassintf(frame->new_InterfaceAction);
  OnUpdateNewset(frame->newSetAction);
	OnUpdateNewEnumItem(frame->newEnumItemAction);

  OnUpdateShowformview(frame->openFormViewAction);
  OnUpdateOverride(frame->showOverridablesAction);
  OnUpdateShowOptionals(frame->showOptsAction);
  OnUpdateComment(frame->editCommentAction);
  OnUpdateEditSel(frame->editSelItemAction);
  OnUpdateLitStr(frame->insertTextAction);
  OnUpdateGotodef(frame->gotoDeclAction);
	OnUpdateGotoImpl(frame->gotoImplAction);
	OnUpdateFindreferences(frame->findRefsAction);
}


void CLavaPEView::DisableActions()
{
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  frame->editPasteAction->setEnabled(false);
  frame->openFormViewAction->setEnabled(false);
  frame->gotoDeclAction->setEnabled(false);
  frame->editCopyAction->setEnabled(false);
  frame->showOverridablesAction->setEnabled(false);
  frame->newEnumAction->setEnabled(false);
  frame->newFuncAction->setEnabled(false);
  frame->newInitiatorAction->setEnabled(false);
  frame->newMemVarAction->setEnabled(false);
  frame->newPackageAction->setEnabled(false);
  frame->newImplAction->setEnabled(false);
  frame->editCutAction->setEnabled(false);
  frame->editCommentAction->setEnabled(false);
  frame->newCOspecAction->setEnabled(false);
  frame->newCOimplAction->setEnabled(false);
  frame->editSelItemAction->setEnabled(false);
  frame->newVTAction->setEnabled(false);
  frame->new_InterfaceAction->setEnabled(false);
  frame->newSetAction->setEnabled(false);
  frame->insertTextAction->setEnabled(false);
	frame->newEnumItemAction->setEnabled(false);
	frame->gotoImplAction->setEnabled(false);
	frame->findRefsAction->setEnabled(false);
  frame->expandAction->setEnabled(false);
  frame->collapseAction->setEnabled(false);
  frame->showOptsAction->setEnabled(false);
  frame->showAllOptsAction->setEnabled(false);
  frame->nextCommentAction->setEnabled(false);
  frame->hideEmptyOptsAction->setEnabled(false);
  frame->prevCommentAction->setEnabled(false);
  frame->nextErrorAction->setEnabled(false);
  frame->prevErrorAction->setEnabled(false);
}


QPixmap* CLavaPEView::GetPixmap(bool isParent, bool isAttr, TDeclType deftype, const SynFlags flag) 
{  
  QPixmap *pm;
  if (isParent) { 
    switch (deftype) {
    case VirtualType:
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[27];//QPixmapCache::find("l_params"); //27;
    case IAttr:
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[17];//QPixmapCache::find("l_inputs"); //17;
    case OAttr:
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[20];//QPixmapCache::find("l_outputs"); //20;
    case Attr:
    case FormText:
    case Function:
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[19];//QPixmapCache::find("l_members"); //19;
    case ExecDef:
      if (flag.bits)
        return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[8];//QPixmapCache::find("l_constrai"); //8;
      else
        return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[7];//QPixmapCache::find("l_constrempty"); //7;
    default: 
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[9];//QPixmapCache::find("l_definitions"); //9; //all types
    }
  }
  switch (deftype) {
  case NoDef:
  case UnDef:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[26];//QPixmapCache::find("l_undefine"); //26;
  case VirtualType:
    if (isAttr)
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[2];//QPixmapCache::find("l_paramatt"); //2;
    else
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[3];//QPixmapCache::find("l_patpara"); //3;
  case Impl:
    if (flag.Contains(isGUI))
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[30];//QPixmapCache::find("l_formimpl"); //30;
    else
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[4];//QPixmapCache::find("l_classimpl"); //4; 
  case Interface:
    if (flag.Contains(isSet))
      if (isAttr)
        return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[29];//QPixmapCache::find("l_setatt"); //29;
      else
        return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[28];//QPixmapCache::find("l_settyp"); //28;
    else
      if (isAttr)
        return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[5];//QPixmapCache::find("l_classAttr"); //5;
      else
        if (flag.Contains(isGUI))
          return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[13];//QPixmapCache::find("l_form"); //13;
        else
          return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[6];//QPixmapCache::find("l_classint"); //6;
  case CompObjSpec:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[23];//QPixmapCache::find("l_cospec"); //23;
  case CompObj:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[24];//QPixmapCache::find("l_coimpl"); //24;

  case Initiator:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[16];//QPixmapCache::find("l_initiato"); //16;
  case Function:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[15];//QPixmapCache::find("l_function"); //15;
  case Package:
  case Component:
    pm = ((CLavaPEApp*)wxTheApp)->LavaPixmaps[22];//QPixmapCache::find("l_Namespace"); //22;
    return pm;
  case Attr:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[0];//QPixmapCache::find("l_basicatt"); //0;
  case FormText:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[18];//QPixmapCache::find("l_literal"); //18;
  case IAttr:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[0];//QPixmapCache::find("l_basicatt"); //0;
  case OAttr:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[0];//QPixmapCache::find("l_basicatt"); //0;
  case FormDef:
    if (isAttr)
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[14];//QPixmapCache::find("l_formAt"); //14;
    else
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[30];//QPixmapCache::find("l_formimpl"); //30;
  case ExecDef:
    if (flag.bits)
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[8];//QPixmapCache::find("l_constrai"); //8;
    else
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[7];//QPixmapCache::find("l_constrempty"); //7;
  case BasicDef:
    if (isAttr)
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[0];//QPixmapCache::find("l_basicatt"); //0;
    else
      return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[1];//QPixmapCache::find("l_BasicType"); //1;
  default:
    return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[26];//QPixmapCache::find("l_undefine"); //26;
  }
  return ((CLavaPEApp*)wxTheApp)->LavaPixmaps[26];//QPixmapCache::find("l_undefine"); //26;
}

/*
bool CLavaPEView::OnCreate()//LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
  //if (CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext)) {
    if (!GetDocument()->MainView) 
      GetDocument()->MainView = this;
    /*
    if (LBaseData->m_lfDefTreeFont.lfHeight != 0)
      SetFont(&LBaseData->m_TreeFont);
    m_dropTarget.Register(this);
    PopupMenu.CreatePopupMenu();
    PopupMenu.AppendMenu(MF_STRING, ID_EDIT_CUT, "Cut");
//    bitmap.LoadBitmap(IDB_MenuTest); //0
    PopupMenu.CheckMenuItem(0, MF_BYPOSITION | MF_CHECKED);
    PopupMenu.SetMenuItemBitmaps( 0, MF_BYPOSITION, 0, &bitmap);
    PopupMenu.CheckMenuItem(0, MF_BYPOSITION | MF_CHECKED);
    PopupMenu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");
    PopupMenu.AppendMenu(MF_STRING, ID_EDIT_PASTE, "Paste");
    PopupMenu.AppendMenu(MF_STRING, ID_DELETE, "Delete");
    PopupMenu.AppendMenu(MF_SEPARATOR, 0, "");
    PopupMenu.AppendMenu(MF_SEPARATOR, 0, "");
    PopupMenu.AppendMenu(MF_STRING, ID_ExpandAll, "Expand subtree");
    PopupMenu.AppendMenu(MF_STRING, ID_CollapsAll, "Collaps subtree");
    PopupMenu.AppendMenu(MF_STRING, ID_SHOW_OPTIONALS, "Show/hide optionals");
    PopupMenu.AppendMenu(MF_SEPARATOR, 0, "");
    PopupMenu.AppendMenu(MF_STRING, ID_COMMENT, "Add/edit comment");
    PopupMenu.AppendMenu(MF_STRING, ID_EDIT_SEL, "Show/edit item details");
    PopupMenu.AppendMenu(MF_SEPARATOR, 0, "");
    PopupMenu.AppendMenu(MF_STRING, ID_GOTODEF, "Go to declaration");
    PopupMenu.AppendMenu(MF_SEPARATOR, 0, "");
    PopupMenu.AppendMenu(MF_STRING, ID_GOTOIMPL, "Go to implememtation");
    PopupMenu.AppendMenu(MF_SEPARATOR, 0, "");
    PopupMenu.AppendMenu(MF_STRING, ID_Override, "Show virtual table view");
    PopupMenu.AppendMenu(MF_STRING, ID_SHOWFORMVIEW, "Show form view");
//    PopupMenu.AppendMenu(MF_STRING, ID_DefaultForm, "Make default form");

    return true;
  }
  else
    return false;
}

bool CLavaPEView::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.style = cs.style | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_EDITLABELS;
  return CTreeView::PreCreateWindow(cs);
}
*/


void CLavaPEView::OnInitialUpdate()
{
  SelItem =0;
  SelType = NoDef;
  VisibleDECL = 0;
  if (((CLavaPEApp*)wxTheApp)->LBaseData.actHint 
      && (((CLavaPEApp*)wxTheApp)->LBaseData.actHint->com == CPECommand_OpenFormView)
      && !myInclView) {
    CLavaPEHint* pHint = ((CLavaPEApp*)wxTheApp)->LBaseData.actHint;
    myDECL = (LavaDECL*)pHint->CommandData1;
    p_myDECL = (LavaDECL**)pHint->CommandData4;
    myID = TID(myDECL->OwnID, 0);
    myMainView = (CLavaPEView*)pHint->CommandData2;
    InitFinished = true;
    OnUpdate(myMainView, 0, pHint);
  }
  else { 
    ((CTreeFrame*)GetParentFrame())->SetModified(false);
    OnUpdate(NULL, CHLV_fit, 0);
    if (GetDocument()->modified) {
      QMessageBox::critical(this, qApp->name(), "This Lava file was modified by automatically generated corrections after opening",QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      GetDocument()->Modify(true);
    }
    CExecChecks* ch = new CExecChecks(GetDocument(), true);
    delete ch;
    if (/*GetDocument()->nTreeErrors ||*/ GetDocument()->nErrors || GetDocument()->nPlaceholders)
      GetDocument()->ShowErrorBox(true);

  }
}


void CLavaPEView::OnUpdate(wxView* pSender, unsigned undoRedoCheck, QObject* pHint) 
{ 
  if (m_hitemDrag) //in drag drop no tree drawing
    return;
  CLavaPEHint* hint = (CLavaPEHint*) pHint;
  bool lastUpdate = (undoRedoCheck == 3) ||
                     hint && ((undoRedoCheck == 1) && hint->FirstLast.Contains(firstHint)
                            || (undoRedoCheck != 1) && hint->FirstLast.Contains(lastHint));
  bool foreignHint = hint 
                    && (hint->com != CPECommand_OpenFormView)
                    && (hint->com != CPECommand_Insert)
                    && (hint->com != CPECommand_Delete)
                    && (hint->com != CPECommand_Change)
                    && (hint->com != CPECommand_Include)
                    && (hint->com != CPECommand_ChangeInclude)
                    && (hint->com != CPECommand_Exclude)
                    && (hint->com != CPECommand_Comment)
                    && (hint->com != CPECommand_FromOtherDoc);
  bool inUndoRedo = (undoRedoCheck == 1) || (undoRedoCheck == 2);
  updateTree = updateTree || !foreignHint;
  drawTree = drawTree || (undoRedoCheck == CHLV_fit)
                      || hint && (hint->com == CPECommand_FromOtherDoc) && !hint->FirstLast.Contains(noDrawHint);
  if (!updateTree)
    return;
  updateTree = !lastUpdate;
  CLavaPEDoc* pDoc = GetDocument();
  if (pHint && hint->FirstLast.Contains(firstHint) && !foreignHint) {
//    CTreeItem* firstVisible = GetListView()->GetFirstVisibleItem();
    VisibleDECL = 0;
//    SelEnumSel = 0;
    /*
    if (firstVisible) {
      CMainItemData* visData = (CMainItemData*)GetItemData(firstVisible);
      if (visData->type == TIType_CHEEnumSel) {
        firstVisible = (CTreeItem*)firstVisible->parent();
        firstVisible = (CTreeItem*)firstVisible->parent();
      }
      VisibleDECL = *(LavaDECL**)((CMainItemData*)GetItemData(firstVisible))->synEl;
    }*/
    if (!pDoc || !pDoc->mySynDef)
      return;
    if (pHint) {
      switch (hint->com) {
        case CPECommand_FromOtherDoc:
          if (VisibleDECL)
            VisibleDECL->WorkFlags.INCL(selAfter);
          break;
        case CPECommand_Comment:
          if (hint->CommandData3) {
            ((CHEEnumSelId*)hint->CommandData1)->data.selItem = true;
          }
          else 
            ((LavaDECL*)hint->CommandData1)->WorkFlags.INCL(selAfter);
          break;        
        default: ;
      }
    }
  }
  if (!myInclView) {
    if (hint) {
      if (hint->com == CPECommand_OpenFormView) {
        if (myDECL != (LavaDECL*)hint->CommandData1)
          return;
        drawTree = true;
      }
      else {
        if (!foreignHint) {
          DString *pHintName=0;
          if ((hint->com == CPECommand_Delete)
              && myDECL->isInSubTree((LavaDECL*)hint->CommandData1) ) {
            delete GetParentFrame();
            return;
          }
          if (( hint->com == CPECommand_Comment)  && hint->CommandData3)
            pHintName = &((LavaDECL*)hint->CommandData3)->FullName;
          else
            if (hint->CommandData1)
              pHintName = &((LavaDECL*)hint->CommandData1)->FullName;
          if (pHintName && isPartOf(myDECL->FullName, *pHintName)) {
            TIDType tt;
            p_myDECL = (LavaDECL**)GetDocument()->IDTable.GetVar(myID, tt);
            if ((tt != globalID) || !p_myDECL || !*p_myDECL)
              return;
            else {
              myDECL = *p_myDECL;
            }
          }
        }
      }
    }
    CTreeView::OnUpdate(pSender, undoRedoCheck, pHint);
    if ( hint && (hint->com != CPECommand_Comment)
              && ( (undoRedoCheck == 1) && hint->FirstLast.Contains(firstHint)
                 || (undoRedoCheck != 1) && hint->FirstLast.Contains(lastHint))
          || (undoRedoCheck == 3) || (undoRedoCheck >= CHLV_noCheck) ) {
      myDECL->DECLError2.Destroy();
      drawTree = drawTree || GetDocument()->UndoMem.DrawTree;
      DrawTree(GetDocument()->GetFormpDECL(myDECL), inUndoRedo);
      if (!drawTree) {
        CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
        if (item) {
          QApplication::postEvent(myFormView, new QCustomEvent(IDU_LavaPE_SyncForm, (void*)*(LavaDECL**)((CMainItemData*)item->getItemData())->synEl));
          GetDocument()->openWizardView(this, (LavaDECL**)((CMainItemData*)item->getItemData())->synEl/*, (unsigned long)1*/);
        }
      }
    }
  } //forms
  else {
    if (hint && ((hint->com == CPECommand_OpenFormView)
         || (hint->com == CPECommand_OpenSelView)))
      return;
    p_myDECL = &((CHESimpleSyntax*)pDoc->mySynDef->SynDefTree.first)->data.TopDef.ptr;
    myDECL = *p_myDECL;
    if ((undoRedoCheck == 1) && hint && hint->FirstLast.Contains(firstHint)
        || (undoRedoCheck != 1) && hint && hint->FirstLast.Contains(lastHint)
        || (undoRedoCheck == 3) || (undoRedoCheck >= CHLV_noCheck)) {
      if (undoRedoCheck < CHLV_noCheck)
        undoRedoCheck = CHLV_inUpdateLowFinal;
      CTreeView::OnUpdate(pSender, undoRedoCheck, pHint);
      ((CTreeFrame*)GetParentFrame())->showIt = ((CTreeFrame*)GetParentFrame())->showIt
                                                || !((CLavaPEApp*)wxTheApp)->inTotalCheck
                                                || GetDocument()->hasErrorsInTree
                                                || GetDocument()->nErrors
                                                || GetDocument()->nPlaceholders;
      drawTree = drawTree || GetDocument()->UndoMem.DrawTree;
      DrawTree(0, inUndoRedo, true, undoRedoCheck);
      //if ((undoRedoCheck == CHLV_fit)  //the initial open
      //  && (GetDocument()->nTreeErrors || GetDocument()->nErrors || GetDocument()->nPlaceholders))
      //  GetDocument()->ShowErrorBox(true);
    }
  }
}//OnUpdate

void CLavaPEView::DrawTree(LavaDECL ** pDECL, bool inUndoRedo, bool finalUpdate, int checkLevel)
{
  CTreeItem *selItem, *firstVisible = 0;
  DString str;
  CMainItemData *data;
  bool drawed = drawTree;

  setUpdatesEnabled(false);
  if (drawTree) {
    DeleteItemData();
    delete GetListView()->firstChild();
    ItemSel = 0;
    SelItem = 0;
    SelType = NoDef;
  }
  else
    SelItem = (CTreeItem*)GetListView()->currentItem();
  CExecTree* execTree = new CExecTree(this, !inUndoRedo, finalUpdate, checkLevel);
  execTree->Travers->FillOut = (pDECL == 0);
  execTree->Travers->DownTree(pDECL,0,str);
  drawTree = false;
  GetDocument()->UpdateNo++;
  delete execTree;
  if (finalUpdate) {
    lastCurrent = 0;
    ExpandTree();
    if (SelItem) {
      if (SelType != NoDef)
        SelItem = getSectionNode(SelItem, SelType);
    }
    else {
      SelItem = (CTreeItem*)GetListView()->firstChild();
      firstVisible = SelItem;
      data = (CMainItemData*)SelItem->getItemData();
      if ( (*((LavaDECL**)data->synEl))->NestedDecls.first) {
        selItem = (CTreeItem*)SelItem->firstChild();
        if (selItem) {
          data = (CMainItemData*)selItem->getItemData();
          if (data->type != TIType_Defs)
            selItem = (CTreeItem*)selItem->nextSibling();
          if (selItem)
            SelItem = selItem;
        }
      }
    }
    if (drawed) {
      GetListView()->setCurAndSel(SelItem);
      if (VisibleDECL)
        firstVisible = BrowseTree(VisibleDECL, (CTreeItem*)GetListView()->firstChild());
      if (firstVisible)
        GetListView()->ensureItemVisible(firstVisible);
      else
        GetListView()->ensureItemVisible(SelItem);
    }
    setUpdatesEnabled(true);
    GetListView()->update();
  }
}//DrawTree



CTreeItem* CLavaPEView::getSectionNode(CTreeItem* parent, TDeclType ncase)
{
  if (!parent)
    return NULL;
  CMainItemData *data = (CMainItemData*)parent->getItemData();
  TDeclType parentType;
//  if ((parent != GetListView()->firstChild()) || !myInclView) {
    parentType = (*(LavaDECL**)data->synEl)->DeclType;
    if ((data->type == TIType_DECL)
        //&& (  (*(LavaDECL**)data->synEl)->DeclDescType == DefDesc)
        && (parentType != IAttr) && (parentType != OAttr) && (parentType != FormDef) ) {
      if (( (*(LavaDECL**)data->synEl)->DeclDescType != StructDesc)
          && ( (*(LavaDECL**)data->synEl)->DeclDescType != EnumType))
        return parent;
    }
    else
      if ((parentType != FormDef) || myInclView) 
        return parent;
//  }
//  else 
//    parentType = Package;
  DWORD synEl = data->synEl;
  CTreeItem* node = (CTreeItem*)parent->firstChild();
  if (node)
    if (!ContainTab[parentType] [ncase])
      return node;
    while (node) {
      data = (CMainItemData*)node->getItemData();
      if ( (ncase == VirtualType) && (data->type == TIType_VTypes) 
         || (ncase == IAttr) && (data->type == TIType_Input)
         || (ncase == OAttr) && (data->type == TIType_Output)
         || ((ncase == Attr) || (ncase == Function) || (ncase == FormText)) && (data->type == TIType_Features)
         || (ncase == ExecDef) && (data->type == TIType_Constraint)
         || (ncase != NoDef) && (ncase != VirtualType) && (ncase != IAttr)
             && (ncase != OAttr) && (ncase != Attr) && (ncase != Function) && (ncase != FormText) && (ncase != ExecDef)
             &&  (data->type == TIType_Defs))
        return node;
      else 
        node = (CTreeItem*)node->nextSibling();
    }
  QPixmap* bm;
  DString Fields, Constraint;
  if (parentType == Function)
    Fields = DString("Fields");
  else
    Fields = DString("Features");
  if ((parentType == Initiator) || (parentType == Function)
      || (parentType == Impl) ) 
    Constraint = DString("Exec");
  else
    Constraint = DString("Constraint");
  if (!node) {
    if ((*(LavaDECL**)synEl)->TreeFlags.Contains(hasEmptyOpt)) {
      TDeclType gpt = Impl;
      if ((parentType == Function) && (parent != GetListView()->firstChild()) ) {
        CTreeItem* gp = (CTreeItem*)parent->parent();
        gp = (CTreeItem*)gp->parent();
        CMainItemData *gpdata = (CMainItemData*)gp->getItemData();
        gpt = (*(LavaDECL**)gpdata->synEl)->DeclType;
      }
      if ((ncase == VirtualType) 
        || ContainTab[parentType] [VirtualType]
           && GetDocument()->IDTable.okPatternLevel(*(LavaDECL**)synEl)
           && !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent) ) {

        data = new CMainItemData(TIType_VTypes, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(ParaExpanded));
        bm = GetPixmap(true, true, VirtualType);
        node = InsertItem("Virtual types", bm, parent);
        node->setItemData( data);
      }
      if ((ncase == IAttr) 
        || ContainTab[parentType] [ IAttr]
          && !(*(LavaDECL**)synEl)->TypeFlags.Contains(defaultInitializer)) {

        data = new CMainItemData(TIType_Input, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(InExpanded));
        bm = GetPixmap(true, true, IAttr);
        node = InsertItem("Inputs", bm, parent);
        node->setItemData( data);
      }
      if ((ncase == OAttr) 
        || ContainTab[parentType] [OAttr]
          && !(*(LavaDECL**)synEl)->TypeFlags.Contains(execIndependent)
          && !(*(LavaDECL**)synEl)->TypeFlags.Contains(defaultInitializer)) {
        data = new CMainItemData(TIType_Output, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(OutExpanded));
        bm = GetPixmap(true, true, OAttr);
        node = InsertItem("Outputs", bm, parent);
        node->setItemData( data);
      }
      if (ContainTab[parentType] [ 0]) {
        data = new CMainItemData(TIType_Defs, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(DefsExpanded));
        bm = GetPixmap(true, true, Interface);
        node = InsertItem("Declarations", bm, parent);
        node->setItemData( data );
      }
      if ((ncase == Attr) 
        || ContainTab[parentType] [ Attr] || ContainTab[parentType] [ Function] ) {
        if ((parentType != Function) 
          || (gpt != Interface) ) {  
          data = new CMainItemData(TIType_Features, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(MemsExpanded));
          bm = GetPixmap(true, true, Attr);
          node = InsertItem(Fields.c, bm, parent);
          node->setItemData( data );
        }
      }
      if (ContainTab[parentType] [ ExecDef]
          && (gpt != Interface) 
          && ((parentType != /*S*/Interface) || !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent) )) {
         data = new CMainItemData(TIType_Constraint, synEl);
         bm = GetPixmap(true, true, ExecDef);
         node = InsertItem(Constraint.c ,bm, parent);
         node->setItemData(  data);
      }
    }
    else {
      CTreeItem *afterItem, *naItem;
      CMainItemData * afterData;
      switch (ncase) {
        case VirtualType:
          afterItem = (CTreeItem*)parent->firstChild();
          if (afterItem) {
            afterData = (CMainItemData*)afterItem->getItemData();
            if (afterData->type != TIType_EnumItems)
              afterItem = TVI_FIRST;
          }
          else
            afterItem = TVI_FIRST;
          data = new CMainItemData(TIType_VTypes, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(ParaExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem("Virtual types", bm, parent, afterItem);
          node->setItemData( data);
          break;
        case IAttr:
          naItem = (CTreeItem*)parent->firstChild();
          afterItem = 0;
          if (naItem)
            afterData = (CMainItemData*)naItem->getItemData();
          while (naItem && (afterData->type != TIType_VTypes) 
                        && (afterData->type == TIType_EnumItems)) {
            afterItem = naItem;
            naItem = (CTreeItem*)afterItem->nextSibling();
            if (naItem)
              afterData = (CMainItemData*)naItem->getItemData();
          }
          if (naItem && (afterData->type == TIType_VTypes))
            afterItem = naItem;
          else
            if (!afterItem)
              afterItem = TVI_FIRST;  
          data = new CMainItemData(TIType_Input, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(InExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem("Inputs", bm, parent, afterItem);
          node->setItemData( data );
          break;
        case OAttr:
          naItem = (CTreeItem*)parent->firstChild();
          afterItem = 0;
          if (naItem) 
            afterData = (CMainItemData*)naItem->getItemData();
          while (naItem && (afterData->type != TIType_Input) 
                        && ((afterData->type == TIType_EnumItems)
                            || (afterData->type == TIType_VTypes))) {
            afterItem = naItem;
            naItem = (CTreeItem*)afterItem->nextSibling();
            if (naItem)
              afterData = (CMainItemData*)naItem->getItemData();
          }
          if (naItem && (afterData->type == TIType_Input))
            afterItem = naItem;
          else
            if (!afterItem)
              afterItem = TVI_FIRST;
          data = new CMainItemData(TIType_Output, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(OutExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem("Outputs", bm, parent, afterItem);
          node->setItemData( data );
          break;
        case Attr:
        case Function:
        case FormText:
          afterItem = TVI_FIRST;
          naItem = (CTreeItem*)parent->firstChild();
          while (naItem) {
            afterData = (CMainItemData*)naItem->getItemData();
            if (afterData->type != TIType_Constraint) {
              afterItem = naItem;
              naItem = (CTreeItem*)afterItem->nextSibling();
            }
            else
              naItem = 0;
          }
          data = new CMainItemData(TIType_Features, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(MemsExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem(Fields.c, bm, parent, afterItem);
          node->setItemData( data );
          break;
        case ExecDef:
          data = new CMainItemData(TIType_Constraint, synEl);
          bm = GetPixmap(true,true,ExecDef);
          node = InsertItem(Constraint.c ,bm, parent, TVI_LAST);
          node->setItemData(  data);
          break;

        default: 
          naItem = (CTreeItem*)parent->firstChild();
          afterItem = TVI_FIRST;
          naItem = (CTreeItem*)parent->firstChild();
          while (naItem) {
            afterData = (CMainItemData*)naItem->getItemData();
            if ((afterData->type != TIType_Features) && (afterData->type != TIType_Constraint)) {
              afterItem = naItem;
              naItem = (CTreeItem*)afterItem->nextSibling();
            }
            else
              naItem = 0;
          }
          data = new CMainItemData(TIType_Defs, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(DefsExpanded));
          bm = GetPixmap(true, true, ncase);
          node = InsertItem("Declarations", bm, parent, afterItem);
          node->setItemData( data);
      }
    }
    node = (CTreeItem*)parent->firstChild();
  }
  if (node)
    while (node) {
      data = (CMainItemData*)node->getItemData();
      if ( (ncase == VirtualType) && (data->type == TIType_VTypes)
         || (ncase == IAttr) && (data->type == TIType_Input)
         || (ncase == OAttr) && (data->type == TIType_Output)
         || (ncase != NoDef) && (ncase != Attr) && (ncase != Function) && (ncase != FormText) && (ncase != IAttr) && (ncase != OAttr)
             && (ncase != ExecDef) && (data->type == TIType_Defs)
         || ((ncase == Attr) || (ncase == Function) || (ncase == FormText)) && (data->type == TIType_Features)
         || (ncase == ExecDef) && (data->type == TIType_Constraint))
        return node;
      else 
        node = (CTreeItem*)node->nextSibling();
  }
  return NULL;
}


bool CLavaPEView::DrawEmptyOpt(CTreeItem* parent, bool down)
{
  CTreeItem *pitem, *collItem, *declItem, *gp, *item = TVI_FIRST;
  CMainItemData *itd, *dataD;
  TIType  ptype = TIType_NoType;
  TDeclType gpt = Impl;
  DString Fields, Constraint;
  QPixmap* bm;

  itd = (CMainItemData*)parent->getItemData();
  DWORD synEl = itd->synEl;
  TDeclType parentType = (*(LavaDECL**)itd->synEl)->DeclType;
  if ((parentType > FormDef) || (parentType == FormDef) && myInclView)
    return false;
  pitem = (CTreeItem*)parent->firstChild();
  if (down) {
    collItem = pitem;
    while (collItem) {
      declItem = (CTreeItem*)collItem->firstChild();
      while (declItem) {
        dataD = (CMainItemData*)declItem->getItemData();
        if (dataD->type == TIType_DECL) {
          DrawEmptyOpt(declItem, true);
          (*(LavaDECL**)dataD->synEl)->TreeFlags.INCL(hasEmptyOpt);
          declItem = (CTreeItem*)declItem->nextSibling();
        }
        else
          declItem = 0;
      }
      collItem = (CTreeItem*)collItem->nextSibling();
    }
  }
  if (pitem)
    ptype = ((CMainItemData*)pitem->getItemData())->type;
  if ((ptype != TIType_EnumItems) && ((*(LavaDECL**)synEl)->DeclDescType == EnumType) ) {
    itd = new CMainItemData(TIType_EnumItems, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(ItemsExpanded));
    bm = ((CLavaPEApp*)wxTheApp)->LavaPixmaps[enumBM];
    item = InsertItem("Enumeration",  bm, parent, item);
    item->setItemData(itd);
    if (!down)
      GetListView()->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_EnumItems) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_VTypes) && ContainTab[parentType] [ VirtualType] 
      && !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent)
      && GetDocument()->IDTable.okPatternLevel(*(LavaDECL**)synEl)) {
    itd = new CMainItemData(TIType_VTypes, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(ParaExpanded));
    bm = GetPixmap(true, true, VirtualType);
    item = InsertItem("Virtual types",  bm, parent, item);
    item->setItemData( itd);
    if (!down)
      GetListView()->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_VTypes) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Input) && ContainTab[parentType] [ IAttr]
        && !(*(LavaDECL**)synEl)->TypeFlags.Contains(defaultInitializer)
        /*&& ((parentType != Function) || !(*(LavaDECL**)synEl)->SecondTFlags.Contains(funcImpl))*/ ) {
    itd = new CMainItemData(TIType_Input, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(InExpanded));
    bm = GetPixmap(true, true, IAttr);
    item = InsertItem("Input",  bm, parent, item);
    item->setItemData( itd);
    if (!down)
      GetListView()->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_Input) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Output) && ContainTab[parentType] [ OAttr]
        && !(*(LavaDECL**)synEl)->TypeFlags.Contains(execIndependent)
        && !(*(LavaDECL**)synEl)->TypeFlags.Contains(defaultInitializer)
        /*&& ((parentType != Function) || !(*(LavaDECL**)synEl)->SecondTFlags.Contains(funcImpl)) */
        && ((parentType != Interface) 
              || !(*(LavaDECL**)synEl)->TypeFlags.Contains(isAbstract) 
                && !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent) ) ) {
    itd = new CMainItemData(TIType_Output, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(OutExpanded));
    bm = GetPixmap(true, true, OAttr);
    item = InsertItem("Outputs",  bm, parent, item);
    item->setItemData( itd);
    if (!down)
      GetListView()->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_Output) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Defs) && ContainTab[parentType] [ 0]) {
    itd = new CMainItemData(TIType_Defs, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(DefsExpanded) );
    bm = GetPixmap(true, true, Interface);
    item = InsertItem("Declarations",  bm, parent, item);
    item->setItemData( itd);
    if (!down)
      GetListView()->ensureItemVisible(item);
  }
  else {
    if (ptype == TIType_Defs) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if (parentType == Function)
    Fields = DString("Fields");
  else
    Fields = DString("Features");
  if ((parentType == Initiator) || (parentType == Function)
      || (parentType == Impl) ) 
    Constraint = DString("Exec");
  else
    Constraint = DString("Constraint");
  if ((parentType == Function) && (parent != GetListView()->firstChild())) {
    gp = (CTreeItem*)parent->parent();
    gp = (CTreeItem*)gp->parent();
    CMainItemData *gpdata = (CMainItemData*)gp->getItemData();
    gpt = (*(LavaDECL**)gpdata->synEl)->DeclType;
  }
  if ((ptype != TIType_Features) && (ContainTab[parentType] [ Attr] || ContainTab[parentType] [ Function] )) {
    if ((parentType != Function) || (gpt != Interface) ) { 
      itd = new CMainItemData(TIType_Features, synEl, (*(LavaDECL**)synEl)->TreeFlags.Contains(MemsExpanded));
      bm = GetPixmap(true, true, Attr);
      item = InsertItem(Fields.c,  bm, parent, item);
      item->setItemData( itd);
      if (!down)
        GetListView()->ensureItemVisible(item);
    }
  }
  else {
    if (ptype == TIType_Features) {
      item = pitem;
      pitem = (CTreeItem*)item->nextSibling();
      if (pitem)
        ptype = ((CMainItemData*)pitem->getItemData())->type;
      else
        ptype = TIType_NoType;
    }
  }
  if ((ptype != TIType_Constraint) && ContainTab[parentType] [ ExecDef]
      && (gpt != Interface) 
      && ((parentType != /*S*/Interface) || !(*(LavaDECL**)synEl)->TypeFlags.Contains(isComponent) )) {
    itd = new CMainItemData(TIType_Constraint, synEl);
    bm = GetPixmap(true,true,ExecDef);
    item = InsertItem(Constraint.c ,bm, parent, item);
    item->setItemData( itd);
    if (!down)
      GetListView()->ensureItemVisible(item);
  }
  return true;
}

void CLavaPEView::RemoveEmptyOpt(CTreeItem* startItem, bool down)
{
  CTreeItem* pitem = (CTreeItem*)startItem->firstChild();
  if (!pitem)
    return;
  CTreeItem* item;
  CMainItemData *itd, *dataD;
  LavaDECL *decl;
  bool hasC = false;
  CHE* che;
  itd = (CMainItemData*)pitem->getItemData();
  if (itd->type == TIType_CHEEnumSel)
    return;

  while (pitem) {
    itd = (CMainItemData*)pitem->getItemData();
    hasC = (itd->type == TIType_Constraint);
    if (hasC) {
      decl = *(LavaDECL**)itd->synEl;
      che = (CHE*)decl->NestedDecls.last;
      hasC = (che != 0);
    }
    if (hasC) 
      hasC =  ((((LavaDECL*)che->data)->DeclDescType == ExecDesc)
              && ((LavaDECL*)che->data)->Exec.ptr);
    item = (CTreeItem*)pitem->firstChild();
    if (item || hasC) {
      if (down) {
        while (item) {
          RemoveEmptyOpt(item, true);
          dataD = (CMainItemData*)item->getItemData();
          if (dataD->type == TIType_DECL)
            (*(LavaDECL**)dataD->synEl)->TreeFlags.EXCL(hasEmptyOpt);
          item = (CTreeItem*)item->nextSibling();
        }
      }
      pitem = (CTreeItem*)pitem->nextSibling();
    }
    else {
      delete itd;
      item = (CTreeItem*)pitem->nextSibling();
      delete pitem; 
      pitem = item;
    }
  }
}


void CLavaPEView::OnShowAllEmptyOpt() 
{
  CTreeItem* startItem = (CTreeItem*)GetListView()->currentItem();
  if (!startItem)
    return;
  CMainItemData *dataD = (CMainItemData*)startItem->getItemData();
  if (dataD->type == TIType_DECL) {
    DrawEmptyOpt(startItem, true);
    (*(LavaDECL**)dataD->synEl)->TreeFlags.INCL(hasEmptyOpt);
  }
  else {
    CTreeItem* item = (CTreeItem*)startItem->firstChild();
    while (item) {
      dataD = (CMainItemData*)item->getItemData();
      DrawEmptyOpt(item, true);
      (*(LavaDECL**)dataD->synEl)->TreeFlags.INCL(hasEmptyOpt);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}

void CLavaPEView::OnRemoveAllEmptyOpt() 
{
  CTreeItem* startItem = (CTreeItem*)GetListView()->currentItem();
  if (!startItem)
    return;
  CMainItemData *dataD = (CMainItemData*)startItem->getItemData();
  if (dataD->type == TIType_DECL) {
    RemoveEmptyOpt(startItem, true);
    (*(LavaDECL**)dataD->synEl)->TreeFlags.EXCL(hasEmptyOpt);
  }
  else {
    CTreeItem* item = (CTreeItem*)startItem->firstChild();
    while (item) {
      dataD = (CMainItemData*)item->getItemData();
      RemoveEmptyOpt(item, true);
      (*(LavaDECL**)dataD->synEl)->TreeFlags.EXCL(hasEmptyOpt);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}

CTreeItem* CLavaPEView::BrowseTree(LavaDECL* decl, CTreeItem* startItem, DString* enumID)
{
  CTreeItem* item;
  if (!startItem)
    return NULL;
  CMainItemData *itd = (CMainItemData*)startItem->getItemData();
  if (itd && (itd->type == TIType_DECL) && (decl == *(LavaDECL**)itd->synEl))
    return startItem;
  CTreeItem* topItem = (CTreeItem*)startItem->firstChild();
  while (topItem) {
    item = (CTreeItem*)topItem->firstChild();
    while (item) {
      itd = (CMainItemData*)item->getItemData();
      if (decl == *(LavaDECL**)itd->synEl)
        if (enumID && enumID->l && decl->DeclDescType == EnumType) {
          item = (CTreeItem*)item->firstChild();
          item = (CTreeItem*)item->firstChild();
          while (item) {
            itd = (CMainItemData*)item->getItemData();
            if (((CHEEnumSelId*)itd->synEl)->data.Id == *enumID)
              return item;
            item = (CTreeItem*)item->nextSibling();
          }
        }
        else
          return item;
      else
        if (decl->isInSubTree(*(LavaDECL**)itd->synEl))
          return BrowseTree(decl, item, enumID);
      item = (CTreeItem*)item->nextSibling();
    }
    topItem = (CTreeItem*)topItem->nextSibling();
  }
  return NULL;
}


bool CLavaPEView::VerifyItem(CTreeItem* item, CTreeItem* topItem)
{
  if (!topItem)
    topItem = (CTreeItem*)GetListView()->firstChild();
  CTreeItem* nitem;
  bool isValid = false;
  while (topItem && (topItem != item) && !isValid) {
    nitem = (CTreeItem*)topItem->firstChild();
    if (nitem)
      isValid = VerifyItem(item, nitem);
    if (!isValid)
      topItem = (CTreeItem*)topItem->nextSibling();
  }
  return isValid || (topItem == item);
}


CLavaPEDoc* CLavaPEView::GetDocument() // non-debug version is inline
{
  return (CLavaPEDoc*)m_viewDocument;
}


void CLavaPEView::OnDblclk(QListViewItem* itemHit, const QPoint&, int) 
{
  if (itemHit && (((CMainItemData*)((CTreeItem*)itemHit)->getItemData())->type == TIType_Constraint))
    OnShowConstraint();
    //OnEditSelItem((CTreeItem*)itemHit, false);
}


CMainItemData* CLavaPEView::Navigate(bool all, CTreeItem*& item, int& level)
{
  CTreeItem *nextItem=0, *parItem;

  if (item) {
    if (all || item->isOpen())
      nextItem = (CTreeItem*)item->firstChild();
    if (nextItem)
      level = level+1;
    else
      if ((level > 0) || !all)
        nextItem = (CTreeItem*)item->nextSibling();
    while (!nextItem && (level > 0)) {
      parItem = (CTreeItem*)item->parent();
      if (parItem) {
        level = level-1;
        if (level) {
          nextItem = (CTreeItem*)parItem->nextSibling();
          if (!nextItem)
            item = parItem;
        }
      }
      else
        level = 0;
    }
  }
  else {
    level = 0;
    nextItem = (CTreeItem*)GetListView()->firstChild();
  }
  if (nextItem) {
    item = nextItem;
    return (CMainItemData*)item->getItemData();
  }
  else {
    item = 0;
    return 0;
  }
}//Navigate

void CLavaPEView::OnExpandAll() 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item )
    ExpandItem(item);
  GetListView()->ensureItemVisible(item);
}


bool CLavaPEView::ExpandItem(CTreeItem* item, int level/*=-1*/)
{
  if (level == 0)
    return false;
  int count = level-1;
  CTreeItem* htr = item;
  htr = (CTreeItem*)item->firstChild();
  if (htr) {
    item->setOpen(true);
    SetTreeFlags(item, true);
  }
  else
    return false;
  while (htr) {
    ExpandItem(htr, count);
    htr = (CTreeItem*)htr->nextSibling();
  }
  return true;
}

void CLavaPEView::ExpandTree(CTreeItem* top)
{
  CTreeItem* item;
  LavaDECL *itemDECL = 0;
  CMainItemData* itd; 
  if (top) {
    item = (CTreeItem*)top->firstChild();
    while (item) {
      itd = (CMainItemData*)item->getItemData();
      if ((itd->type == TIType_DECL) || (itd->type == TIType_Constraint)) {
        if (itd->type == TIType_Constraint) {
          if ((*((LavaDECL**)itd->synEl))->NestedDecls.last) {
            itemDECL = (LavaDECL*)((CHE*)(*((LavaDECL**)itd->synEl))->NestedDecls.last)->data;
            if (itemDECL->DeclType != ExecDef)
              itemDECL = 0;
          }
        }
        else
          itemDECL = *((LavaDECL**)itd->synEl);
        item->SetItemMask(itemDECL && (itemDECL->DECLError1.first || itemDECL->DECLError2.first),
                          itemDECL && itemDECL->DECLComment.ptr && itemDECL->DECLComment.ptr->Comment.l);
      }
      else
        if (itd->type == TIType_CHEEnumSel) 
          item->SetItemMask(false, ((CHEEnumSelId*)itd->synEl)->data.DECLComment.ptr!=0);
      item->setOpen(((CMainItemData*)item->getItemData())->toExpand);
      ExpandTree(item);
      item = (CTreeItem*)item->nextSibling();
    }
  }
  else {
    item = (CTreeItem*)GetListView()->firstChild();
    itd = (CMainItemData*)item->getItemData();
    itemDECL = *((LavaDECL**)itd->synEl);
    item->SetItemMask(itemDECL->DECLError1.first || itemDECL->DECLError2.first,
              itemDECL->DECLComment.ptr && itemDECL->DECLComment.ptr->Comment.l);
    item->setOpen(true);
    item = (CTreeItem*)item->firstChild();
    while (item) {
      item->setOpen(true);
      SetTreeFlags(item, true);
      ExpandTree(item);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}

/*
bool CLavaPEView::EditNames(CTreeItem* top)
{
  CTreeItem* item;
  CMainItemData* itd;
  item = (CTreeItem*)top->firstChild();//GetListView()->GetChildItem(top);
  CEdit* Ed;
  while (item) {
    itd = (CMainItemData*)item->getItemData();
    if ((itd->type == TIType_DECL) && (*(LavaDECL**)itd->synEl)->SecondTFlags.Contains(editName)) {
      GetListView()->setSelected(item, true);
      GetListView()->ensureItemVisible(item);
      QMessageBox::critical(this, qApp->name(),IDP_NameInUse,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      Ed = GetListView()->EditLabel(item);
      Ed->SetFocus();
      Ed->SetSel(0, -1);
      return true;
    }
    if (EditNames(item))
      return true;
    item = (CTreeItem*)item->nextSibling();
  }
  return false;
}
*/

void CLavaPEView::OnCollapsAll() 
{
  CTreeItem* htr0 = (CTreeItem*)GetListView()->currentItem();
  CTreeItem* htr = htr0;
  int level = 0;
  while (htr) {
    htr->setOpen(false);
    SetTreeFlags(htr, false);
    Navigate(true, htr, level);
  } 
  GetListView()->ensureItemVisible(htr0);
}

void CLavaPEView::SetTreeFlags(CTreeItem* item, bool exp)
{
  CMainItemData *data = (CMainItemData*)item->getItemData();
  data->toExpand = exp;
  switch (data->type) {
    case TIType_DECL:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(isExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(isExpanded);
      break;
    case TIType_EnumItems:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(ItemsExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(ItemsExpanded);
      break;
    case TIType_VTypes:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(ParaExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(ParaExpanded);
      break;
    case TIType_Input:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(InExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(InExpanded);
      break;
    case TIType_Output:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(OutExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(OutExpanded);
      break;
    case TIType_Defs:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(DefsExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(DefsExpanded);
      break;
    case TIType_Features:
      if (exp)
        (*(LavaDECL**)data->synEl)->TreeFlags.INCL(MemsExpanded);
      else
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(MemsExpanded);
      break;
    default: ;
  }
}

void CLavaPEView::OnItemexpanded(QListViewItem* item) 
{
  SetTreeFlags((CTreeItem*)item, true); 
}
void CLavaPEView::OnItemcollapsed(QListViewItem* item) 
{
  SetTreeFlags((CTreeItem*)item, false); 
}


int CLavaPEView::GetPos(CTreeItem* item, CTreeItem* prev)
{
  /*
    Calculate positioning .
    If item != 0, calculate pos from item.
    If item = 0, calculate pos where a new item will be inserted:
      if prev != 0 : the new item will be inserted as sibling after the prev item,
      otherwise result = 1, i.e. the new item will be inserted as first child of the parent item.
  */
  CTreeItem* htree;
  int ii;
  if (item)
    htree = item;
  else
    if (prev)
      htree = prev;
    else 
      return 1;
  CTreeItem* parhtree = (CTreeItem*)htree->parent();
  if (prev)
    ii = 2;
  else
    ii = 1;
  CTreeItem* elh = (CTreeItem*)parhtree->firstChild();
  while (elh && (elh != htree)) {
    ii +=1;
    elh = (CTreeItem*)elh->nextSibling();
  }
  return ii;

}//GetPos


bool CLavaPEView::IsChildNodeOf(CTreeItem* hitemChild, CTreeItem* hitemSuspectedParent)
{
  do {
    if (hitemChild == hitemSuspectedParent)
      break;
  } while (hitemChild = (CTreeItem*)hitemChild->parent());
  return (hitemChild != NULL);
}


QDragObject*  CLavaPEView::OnBegindrag()
{
  CMainItemData *ddrag;

  m_hitemDrop = 0;
  m_hitemDrag = (CTreeItem*)GetListView()->currentItem();
  ddrag = (CMainItemData*)m_hitemDrag->getItemData();
  if ((ddrag->type != TIType_DECL) && (ddrag->type != TIType_CHEEnumSel)
      || (m_hitemDrag == (CTreeItem*)GetListView()->firstChild())
      || (ddrag->type == TIType_DECL)
          && ((*(LavaDECL**)ddrag->synEl)->TypeFlags.Contains(thisComponent)
              || (*(LavaDECL**)ddrag->synEl)->TypeFlags.Contains(thisCompoForm) ) ) {
    m_hitemDrag = 0;
    return 0;
  }
  if (!CollectDECL) {
    CollectDECL = NewLavaDECL();
    CollectDECL->DeclType = NoDef;
    CanDelete = true;
    AddToDragChain(m_hitemDrag);
    CollectPos = GetPos(m_hitemDrag, 0);
  }
  else
    if (CollectDECL->TreeFlags.Contains(mixedDrag)
        && (CollectDECL->TreeFlags.Contains(dragFuncDef) || CollectDECL->TreeFlags.Contains(dragFuncImpl))) {
      m_hitemDrag = 0;
      if (CollectDECL)
        DeleteDragChain();
      return 0;
    }
  declDrag = CollectDECL;
  m_hitemDragP = (CTreeItem*)m_hitemDrag->parent();
  m_hitemDragPP = 0;
  if (m_hitemDragP)
    m_hitemDragPP = (CTreeItem*)m_hitemDragP->parent();
  if (m_hitemDragPP)
    pDeclDragP = (LavaDECL**)((CMainItemData*)m_hitemDragPP->getItemData())->synEl;

	QByteArray ba;
  QDataStream ar(ba,IO_WriteOnly);
  Clipdata  = new CMainItemData(ddrag->type, 0);
  Clipdata->synEl = (unsigned long) NewLavaDECL();  //the Clipdata synEl is LavaDECL*
  Clipdata->ClipTree = &GetDocument()->mySynDef->SynDefTree;
  *(LavaDECL*)Clipdata->synEl = *declDrag;
  Clipdata->docPathName = new DString(GetDocument()->GetAbsSynFileName());
  Clipdata->Serialize(ar);
  dragSource = new LavaSource(GetListView());
  dragSource->setEncodedData(ba);
  GetDocument()->DragView = this;
  DragDoc = GetDocument();
  dragSource->drag();
  m_hitemDrag = 0;
  if (m_hitemDrop)
    GetListView()->setSelected(m_hitemDrop, false);
  m_hitemDrop = 0;
  GetDocument()->DragView = 0;
  if (CollectDECL)
    DeleteDragChain();
  if (Clipdata) {
    delete Clipdata;
    Clipdata = 0;
  }
  return 0;
}


void CLavaPEView::OnDragEnter(QDragEnterEvent* ev) 
{
  CLavaPEDoc *dropDoc = GetDocument();
  CHESimpleSyntax *cheSyn;
  SynFlags treeflags, secondtflags;
  DString /*lowRelName,*/ dropAbsName;

  if (ev->provides(m_nIDClipFormat)) {
    QByteArray ba = ev->encodedData(m_nIDClipFormat);
		QDataStream ar(ba,IO_ReadOnly);
    if (Clipdata)
      delete Clipdata;
    Clipdata = new CMainItemData;
    Clipdata->Serialize(ar);
    if ((((LavaDECL*)Clipdata->synEl)->DeclType == DragFeature)
        || (((LavaDECL*)Clipdata->synEl)->DeclType == DragFeatureF)) {
      dropAbsName = dropDoc->GetAbsSynFileName();
      DragINCL = 0;
      DropINCL = 0;
      if (SameFile(dropAbsName, *Clipdata->docPathName) )
        DragDoc = dropDoc;
      else {
        DragDoc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(Clipdata->docPathName->c);
        if (DragDoc) {
          //lowRelName = *Clipdata->docPathName;
          //RelPathName(lowRelName, dropDoc->IDTable.DocDir);
          cheSyn = (CHESimpleSyntax*)dropDoc->mySynDef->SynDefTree.first;
          if (cheSyn) {
            cheSyn = (CHESimpleSyntax*)cheSyn->successor; //is DragDoc included?
            for ( ;cheSyn
                && !SameFile(cheSyn->data.SyntaxName,  dropDoc->IDTable.DocDir, *Clipdata->docPathName);//lowRelName, dropDoc->IDTable.DocDir);
                   cheSyn = (CHESimpleSyntax*)cheSyn->successor);
            if (cheSyn) 
              DragINCL = cheSyn->data.nINCL;
          }
          //lowRelName = dropAbsName;
          //RelPathName(lowRelName, DragDoc->IDTable.DocDir);
          cheSyn = (CHESimpleSyntax*)DragDoc->mySynDef->SynDefTree.first;
          if (cheSyn) {
            cheSyn = (CHESimpleSyntax*)cheSyn->successor; //is DropDoc included?
            for ( ;cheSyn && !SameFile(cheSyn->data.SyntaxName, DragDoc->IDTable.DocDir, dropAbsName);//lowRelName, DragDoc->IDTable.DocDir);
                   cheSyn = (CHESimpleSyntax*)cheSyn->successor);
            if (cheSyn) 
              DropINCL = cheSyn->data.nINCL;
          }
          if (!DragINCL && !DropINCL) 
            DragDoc = 0;
        }//DragDoc
        else 
          DragDoc = 0;
      }//DragDoc != dropDoc
    }//Features
  }
}

void CLavaPEView::OnDragLeave(QDragLeaveEvent* ev) 
{
  if (m_hitemDrop)
    GetListView()->setSelected(m_hitemDrop, false);
  m_hitemDrop = 0;
  if (Clipdata) {
    Clipdata->Destroy();
    delete Clipdata;
    Clipdata = 0;
    DragDoc = 0;
  }
}

void CLavaPEView::OnDragOver(QDragMoveEvent* ev)
{
  CLavaPEView* dragView;
  TIType tiType;
  CTreeItem* item;

  if (ev->provides(m_nIDClipFormat)) {
    item = (CTreeItem*)GetListView()->itemAt(GetListView()->contentsToViewport(ev->pos()));
    if (m_hitemDrop && (item != m_hitemDrop))
      GetListView()->setSelected(m_hitemDrop, false);
    m_hitemDrop = item;
    if (m_hitemDrop) {
      RefacCase = noRefac;
      tiType = CanPaste(((LavaDECL*)Clipdata->synEl)->DeclType, ((LavaDECL*)Clipdata->synEl)->TreeFlags, ((LavaDECL*)Clipdata->synEl)->SecondTFlags, m_hitemDrop);
      if (tiType != TIType_NoType) {
        DString fn = GetDocument()->GetAbsSynFileName();
        if (fn == *Clipdata->docPathName) {
          dragView = (CLavaPEView*)GetDocument()->DragView;
          if ((dragView == this)
             && ((m_hitemDrag == m_hitemDrop)
                || (m_hitemDrag == (CTreeItem*)GetListView()->firstChild())
                || IsChildNodeOf(m_hitemDrop, m_hitemDrag)
                || (CollectPos == 1) && (m_hitemDrop == m_hitemDrag->parent())
                || (m_hitemDrop == GetPrevSiblingItem(m_hitemDrag) ))) {
            m_hitemDrop->setDropEnabled(false);
            ev->ignore(GetListView()->itemRect(m_hitemDrop));
            ev->acceptAction(false);
            m_hitemDrop = 0;
            return;
          }
          else {
            if (ev->action() == QDropEvent::Copy) {
              GetListView()->setCurAndSel(m_hitemDrop, false);
              ev->accept(GetListView()->itemRect(m_hitemDrop));
              m_hitemDrop->setDropEnabled(true);
              ev->acceptAction(true);
              return;
            }
            if ((((LavaDECL*)Clipdata->synEl)->DeclType == DragFeature)
                || (((LavaDECL*)Clipdata->synEl)->DeclType == DragFeatureF)
                || (((LavaDECL*)Clipdata->synEl)->DeclType == DragIO)
                || (((LavaDECL*)Clipdata->synEl)->DeclType == DragParam)
                || (((LavaDECL*)Clipdata->synEl)->DeclType == DragEnum)) {
              if ( (m_hitemDrop == m_hitemDrag->parent())
                 || (m_hitemDrop->parent() == m_hitemDrag->parent())) {
                GetListView()->setCurAndSel(m_hitemDrop, false);
                ev->accept(GetListView()->itemRect(m_hitemDrop));
                m_hitemDrop->setDropEnabled(true);
                ev->acceptAction(true);
                return;
              }
              else {
                if ( ((((LavaDECL*)Clipdata->synEl)->DeclType == DragFeature)
                    || (((LavaDECL*)Clipdata->synEl)->DeclType == DragFeatureF))
                    && RefacMove(m_hitemDrop)) {
                  GetListView()->setCurAndSel(m_hitemDrop, false);
                  m_hitemDrop->setDropEnabled(true);
                  ev->accept(GetListView()->itemRect(m_hitemDrop));
                  ev->acceptAction(true);
                  return;
                }
                else {
                  m_hitemDrop->setDropEnabled(false);
                  ev->ignore(GetListView()->itemRect(m_hitemDrop));
                  ev->acceptAction(false);
                  m_hitemDrop = 0;
                  return;
                }
              }
            }
            else { //DragDef
              m_hitemDrop->setDropEnabled(true);
              GetListView()->setCurAndSel(m_hitemDrop, false);
              ev->accept(GetListView()->itemRect(m_hitemDrop));
              ev->acceptAction(true);
              return;
            }
          }
        }
        else { //in other doc
          if ((((LavaDECL*)Clipdata->synEl)->DeclType == DragDef)
            || (((LavaDECL*)Clipdata->synEl)->DeclType == DragFText)) {
            m_hitemDrop->setDropEnabled(true);
            GetListView()->setCurAndSel(m_hitemDrop);
            ev->accept(GetListView()->itemRect(m_hitemDrop));
            ev->acceptAction(true);
            return;
          }
          else {
            if ( (((LavaDECL*)Clipdata->synEl)->DeclType != DragFeature)
                && (((LavaDECL*)Clipdata->synEl)->DeclType != DragFeatureF)
                || RefacMove(m_hitemDrop)) {
              m_hitemDrop->setDropEnabled(true);
              GetListView()->setCurAndSel(m_hitemDrop);
              ev->accept(GetListView()->itemRect(m_hitemDrop));
              ev->acceptAction(true);
              return;
            }
            else {
              m_hitemDrop->setDropEnabled(false);
              ev->ignore(GetListView()->itemRect(m_hitemDrop));
              m_hitemDrop = 0;
              ev->acceptAction(false);
              return;
            }
          }
        }
      }
      else {
        m_hitemDrop->setDropEnabled(false);
        ev->ignore(GetListView()->itemRect(m_hitemDrop));
        m_hitemDrop = 0;
        ev->acceptAction(false);
        return;
      }
    }//CanPaste
  }//HitTest
  ev->ignore();
  ev->acceptAction(false);
  m_hitemDrop = 0;
  return;
}



void CLavaPEView::OnDrop(QDropEvent* ev) 
{
  CTreeItem *itemDragParent, *itemDropP;
  CLavaPEView* dragView = 0;
  bool canDrag = false, canDrop, hasDragged = false, mdh = false;
  LavaDECL *declClip, *dragParent = 0, *dropDECL;
  TDeclType clipDefType;
  DString fn, *str2;
  CMainItemData *ddrop;
  TIType dropType, pasteType;
  TIDType idType;
  CHE *che, *vtHints = 0;
  SynFlags firstlast;
  DWORD d4;
  CLavaPEHint* hint=0, *delHint=0;
  CPECommand com;
  int pos;
  
  if (ev->provides(m_nIDClipFormat)) {
    m_hitemDrop = (CTreeItem*)GetListView()->itemAt(GetListView()->contentsToViewport(ev->pos()));
    if (m_hitemDrop) {
      GetListView()->setSelected(m_hitemDrop, false);
      declClip =  (LavaDECL*)Clipdata->synEl;
      clipDefType = declClip->DeclType;
      fn = GetDocument()->GetAbsSynFileName();
      canDrag = (fn == *Clipdata->docPathName);
      if (canDrag) {
        dragView = (CLavaPEView*)GetDocument()->DragView;
        itemDragParent = (CTreeItem*)dragView->m_hitemDrag->parent();
        if ((this == dragView) && ((m_hitemDrop == m_hitemDrag) 
          || (CollectPos == 1) && (m_hitemDrop == itemDragParent)
          || (m_hitemDrop == GetPrevSiblingItem(m_hitemDrag) ))) {
          m_hitemDrop = 0;
          m_hitemDrag = 0;
          if (CollectDECL)
            DeleteDragChain();
          return;
        }
        dragParent = *dragView->pDeclDragP;
      }
      else
        if (!canDrag)
          declClip->inINCL = 10000;
      ddrop = (CMainItemData*)m_hitemDrop->getItemData();
      dropType = ddrop->type;
      canDrop = true;
      dropDECL = *(LavaDECL**)ddrop->synEl;
      if (RefacCase == noRefac)  {
        pasteType = CanPaste(clipDefType, declClip->TreeFlags, declClip->SecondTFlags, m_hitemDrop);
        if (pasteType != TIType_NoType) {
          if ((clipDefType == DragIO) || (clipDefType == DragFeature) ) {
            TDeclType elType;
            switch (pasteType) {
            case TIType_Features:
              elType = Attr;
              break;
            case TIType_Input:
              elType = IAttr;
              break;
            case TIType_Output:
              elType = OAttr;
              break;
            default: ;
            }
            che = (CHE*)declClip->NestedDecls.first;
            while (che) {
              if ((((LavaDECL*)che->data)->DeclType != Attr) && (elType == Attr))
                ((LavaDECL*)che->data)->TypeFlags = SET(acquaintance, -1);
              ((LavaDECL*)che->data)->DeclType = elType;
              che = (CHE*)che->successor;
            }
          }
        }
      }
      else {
        if (ev->action() == QDropEvent::Move)
          canDrop = DoRefac(dropDECL, mdh, vtHints);
      }

      if (canDrag && canDrop) {
        if (dragView->CollectDECL) {
          if (dragView->CollectDECL->DeclType == DragEnum) {
            hint = ChangeEnum(CollectDECL, m_hitemDrop, ev->action() == QDropEvent::Move, true);
            GetDocument()->hasHint = true;
            Clipdata->docPathName = 0;
            m_hitemDrag = 0;
            if (hint) {
              hint->FirstLast.EXCL(lastHint);
              GetDocument()->UndoMem.AddToMem(hint);
              GetDocument()->UpdateDoc(this, false, hint);
              GetDocument()->ConcernForms(hint);
              GetDocument()->ConcernExecs(hint);
              m_hitemDrop = 0;
              GetDocument()->SetLastHint();
            }
            ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = false;
            return;
          }
          else {
            if (ev->action() == QDropEvent::Move) {
              d4 = ((CMainItemData*)itemDragParent->getItemData())->synEl;
              if (GetDocument()->hasHint)
                d4 = GetDocument()->IDTable.GetVar(TID((*(LavaDECL**)d4)->OwnID,0),idType);
              if (dragView->CollectDECL->FullName.l)
                str2 = new DString(dragView->CollectDECL->FullName);
              else
                str2 = 0;
              FIRSTLAST(GetDocument(), firstlast);
              if (mdh)
                firstlast.INCL(multiDocHint);
              hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), firstlast, (DWORD)dragView->CollectDECL, (DWORD)str2, (DWORD)dragView->CollectPos, d4, (DWORD)dropDECL->ParentDECL);
              GetDocument()->UpdateDoc(this, false, hint);
              drawTree = true;
              DrawTree(p_myDECL, false, false);
              GetDocument()->UpdateNo++; //the virtual tables are calculated in DrawTree
              hasDragged = true;
              delete hint; //??
            }
          }
        }
        if (dropDECL->ParentDECL) {
          m_hitemDrop = BrowseTree(dropDECL, (CTreeItem*)GetListView()->firstChild());
          if (!m_hitemDrop) {
            m_hitemDrop = BrowseTree(dropDECL->ParentDECL, (CTreeItem*)GetListView()->firstChild());
            m_hitemDrop = getSectionNode(m_hitemDrop, dropDECL->DeclType);
            if ( CollectPos > 1) {
              m_hitemDrop = (CTreeItem*)m_hitemDrop->firstChild();
              pos = CollectPos-2;
              while (pos) {
                m_hitemDrop = (CTreeItem*)m_hitemDrop->nextSibling();
                pos--;
              }
            }
          }
        }
        else
          m_hitemDrop = getSectionNode((CTreeItem*)GetListView()->firstChild(), dropDECL->DeclType);
      }//canDrag && canDrop
      if (canDrop) {
        if (clipDefType == DragEnum) {
          hint = ChangeEnum(declClip, m_hitemDrop, false, true);
          hint->FirstLast.EXCL(lastHint);
          GetDocument()->UndoMem.AddToMem(hint);
        }
        else {
          if (dropType == TIType_DECL) {
            pos = GetPos(0, m_hitemDrop);
            itemDropP = (CTreeItem*)m_hitemDrop->parent();
            d4 = ((CMainItemData*)itemDropP->getItemData())->synEl;
          }
          else {
            //as child inserted
            pos = GetPos(0, 0);
            d4 = ((CMainItemData*)m_hitemDrop->getItemData())->synEl;
          }
          if (GetDocument()->hasHint)
            d4 = GetDocument()->IDTable.GetVar(TID((*(LavaDECL**)d4)->OwnID,0),idType);
          if ((*(LavaDECL**)d4)->FullName.l) {
            str2 = new DString( (*(LavaDECL**)d4)->FullName);
            declClip->FullName = *str2;
          }
          else {
            str2 = 0;
            declClip->FullName.Reset(0);
          }
          if (ev->action() == QDropEvent::Move)
            com = CPECommand_Move;
          else
            com = CPECommand_Insert;
          DWORD d6 = 0;
          if (!canDrag || mdh) 
            d6 = (DWORD)Clipdata->ClipTree;
          if (!canDrag && (ev->action() == QDropEvent::Move) || mdh) {
            firstlast.INCL(multiDocHint);
            if (RefacCase == noRefac)
              ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
          }
          FIRSTLAST(GetDocument(), firstlast);
          hint = new CLavaPEHint(com, GetDocument(), firstlast, (DWORD)declClip, (DWORD)str2, (DWORD)pos, d4, (DWORD)dragParent,
                                d6, (DWORD)Clipdata->docPathName);
          delHint = hint;
        }
        Clipdata->docPathName = 0;
        if (dragView)
          dragView->m_hitemDrag = 0;
        m_hitemDrop = 0;
        GetDocument()->UpdateDoc(this, false, hint);
        while(vtHints) {
          hint = (CLavaPEHint*)vtHints->data;
          if (DragDoc != GetDocument())
            GetDocument()->IDTable.ChangeIDFromTab(((CHETID*)((LavaDECL*)hint->CommandData1)->Supports.first)->data.nID);
          FIRSTLAST(hint->fromDoc, firstlast); 
          hint->FirstLast = firstlast;
          ((CLavaPEDoc*)hint->fromDoc)->UndoMem.AddToMem(hint);
          ((CLavaPEDoc*)hint->fromDoc)->UpdateDoc(this, false, hint);
          che = (CHE*)vtHints->successor;
          vtHints->data = 0;
          delete vtHints;
          vtHints = che;
        }
        if ((clipDefType == DragIO) || (clipDefType == DragEnum) ) 
          GetDocument()->ConcernForms(hint);
        if (clipDefType == DragIO)
          if (d4) 
            GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
          else
            GetDocument()->ConcernImpls(hint, 0);
        GetDocument()->ConcernExecs(hint);
        GetDocument()->SetLastHints(true, (ev->action() == QDropEvent::Move) || !hasDragged
                              || (RefacCase != privToPublic) && (RefacCase != noRefac));
        if (delHint)
          delete delHint;
      }//canDrop
    }
  }
  ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = false;
  if (dragView)
    dragView->m_hitemDrag = 0;
  if (m_hitemDrop)
    GetListView()->setSelected(m_hitemDrop, false);
  m_hitemDrop = 0;
  if (!canDrop) {
    if (CollectDECL)
      DeleteDragChain();
    if (Clipdata)
      Clipdata->Destroy();
  }
  delete Clipdata;
  Clipdata = 0;
}


bool CLavaPEView::RefacMove(CTreeItem* dropItem)
{ 
  //call this function only for dragged functions or member
  if (!DragDoc)
    return false;

  int hDropINCL;
  CHE *clipEl, *dropEl;
  CLavaPEDoc *highDoc=0, *dropDoc = GetDocument();
  CMainItemData *ddrop;
  LavaDECL *dragParent, *dropParent;

  ddrop = (CMainItemData*)dropItem->getItemData();
  dropParent = *(LavaDECL**)ddrop->synEl;
  if (ddrop->type == TIType_DECL)
    dropParent = dropParent->ParentDECL;
  if (DragINCL != 0) {
    highDoc = dropDoc;
    hDropINCL = 0;
  }
  else {
    highDoc = DragDoc;
    hDropINCL = DropINCL;
  }
  dragParent = highDoc->IDTable.GetDECL(0, ((LavaDECL*)((CHE*)((LavaDECL*)Clipdata->synEl)->NestedDecls.first)->data)->OwnID, DragINCL);
  dragParent = dragParent->ParentDECL;
  RefacCase = noRefac;
  if (highDoc->IDTable.IsAn(TID(dropParent->OwnID, hDropINCL), 0,TID(dragParent->OwnID, DragINCL), 0)) {
    //move from base to ex: publicToPriv or baseToEx
    if (dropParent->DeclType == Impl) {
      if (((CHETID*)dropParent->Supports.first)->data == TID(dragParent->OwnID, DragINCL))
        RefacCase = publicToPriv;
    }
    else {
        //no derivate of the moved function in dropParent?
      for (clipEl = (CHE*)((LavaDECL*)Clipdata->synEl)->NestedDecls.first;
           clipEl;
           clipEl = (CHE*)clipEl->successor) {
        for (dropEl = (CHE*)dropParent->NestedDecls.first;
             dropEl
               && (!((LavaDECL*)dropEl->data)->Supports.first
               || !highDoc->IDTable.EQEQ(TID(((LavaDECL*)clipEl->data)->OwnID, DragINCL),0, ((CHETID*)((LavaDECL*)dropEl->data)->Supports.first)->data, hDropINCL));
             dropEl = (CHE*)dropEl->successor);
        if (dropEl) 
          return false;
      }
      RefacCase = baseToEx;
    }
  }
  else {
    if (highDoc->IDTable.IsAn(TID(dragParent->OwnID, DragINCL), 0,TID(dropParent->OwnID, hDropINCL), 0)) 
      //move from ex to base: privToPublic or exToBase
      if (dragParent->DeclType == Impl) {
        if (highDoc->IDTable.EQEQ(((CHETID*)dragParent->Supports.first)->data, DragINCL, TID(dropParent->OwnID, hDropINCL),0))
          RefacCase = privToPublic;
      }
      else
        RefacCase = exToBase;
  }
  return RefacCase != noRefac;
}

void CLavaPEView::CorrVT_BaseToEx(LavaDECL *dropParent, LavaDECL *dragParent, LavaDECL *clipDecl, bool& mdh)
{
  CLavaPEDoc *mDoc;
  TID mId;
  int mINCL = 0;
  TIDType type;
  LavaDECL *newmDecl, **p_mDecl, *mDecl;
  CHETVElem* cheElVT;
  DString *str2, absName;
  SynFlags firstlast;
  CHETID *cheID;
  CLavaPEHint *hint;
  
  cheElVT = GetDocument()->IDTable.FindElemInVT(dropParent, dragParent, clipDecl, DragINCL);
  if (TID(clipDecl->OwnID, DragINCL) == cheElVT->data.VTEl)
    return;
  mId = cheElVT->data.VTEl;
  mDecl = GetDocument()->IDTable.GetDECL(mId, mINCL);
  while (!GetDocument()->IDTable.EQEQ(mId, mINCL, TID(clipDecl->OwnID, DragINCL),0)) {
    mDecl = GetDocument()->IDTable.GetDECL(mId, mINCL);
    mId = ((CHETID*)mDecl->Supports.first)->data;
    mINCL = mDecl->inINCL;
  }
  if (mDecl->inINCL) {
    absName = GetDocument()->IDTable.IDTab[mDecl->inINCL]->FileName;
    AbsPathName(absName, GetDocument()->IDTable.DocDir);
    mDoc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(absName.c);
    firstlast.INCL(multiDocHint);
    if (!mdh) {
      mdh = true;
      ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
    }
  }
  else
    mDoc = GetDocument();
  p_mDecl = (LavaDECL**)mDoc->IDTable.GetVar(TID(mDecl->OwnID, 0),type, 0);
  newmDecl = NewLavaDECL();
  *newmDecl = *(*p_mDecl);
  if (clipDecl->Supports.first) {
    ((CHETID*)newmDecl->Supports.first)->data.nID = ((CHETID*)clipDecl->Supports.first)->data.nID;
    for (mINCL = 0; 
         mINCL < GetDocument()->IDTable.IDTab[mDecl->inINCL]->maxTrans;
         mINCL++)
      if (GetDocument()->IDTable.IDTab[mDecl->inINCL]->nINCLTrans[mINCL].isValid
         && (GetDocument()->IDTable.IDTab[mDecl->inINCL]->nINCLTrans[mINCL].nINCL == DragINCL)) {
        ((CHETID*)newmDecl->Supports.first)->data.nINCL = mDoc->IDTable.IDTab[mINCL]->nINCLTrans[((CHETID*)clipDecl->Supports.first)->data.nINCL].nINCL;
        mINCL = GetDocument()->IDTable.IDTab[mDecl->inINCL]->maxTrans;
      }
  }
  else {
    cheID = new CHETID;
    cheID->data = cheElVT->data.VTEl;
    clipDecl->Supports.Append(cheID);
    clipDecl->WorkFlags.INCL(SupportsReady);
    clipDecl->SecondTFlags.INCL(overrides);
    newmDecl->Supports.Destroy();
  }
  str2 = new DString(newmDecl->FullName);  //bisheriger Name
  FIRSTLAST(mDoc, firstlast);
  hint = new CLavaPEHint(CPECommand_Change, mDoc, firstlast, (DWORD)newmDecl, (DWORD)str2, 0, (DWORD)p_mDecl);
  mDoc->UndoMem.AddToMem(hint);
  mDoc->UpdateDoc(this, false, hint);
}

void CLavaPEView::CorrVT_ExToBase(LavaDECL *dragParent, LavaDECL *dropParent, LavaDECL *clipDecl, bool& mdh, CHE*& vtHints)
{
  CHETVElem* cheElVT;
  DString *str2, absName;
  TIDType type;
  LavaDECL *newmDecl, **p_mDecl, *mDecl, *dropParentInDragDoc;
  CLavaPEDoc *mDoc;
  TID mId;
  int mINCL;
  SynFlags firstlast;
  CHETID *cheID;
  CHE* cheHint;
  CLavaPEHint *mHint;
  
  dropParentInDragDoc = DragDoc->IDTable.GetDECL(0, dropParent->OwnID, DropINCL);
  cheElVT = DragDoc->IDTable.FindElemInVT(dropParentInDragDoc, dragParent, clipDecl, 0);
  if (cheElVT && ((CHETID*)clipDecl->Supports.first)->data == cheElVT->data.VTEl)
    return;

  if (cheElVT) {
    mId = ((CHETID*)clipDecl->Supports.first)->data;
    mINCL = 0;
    while (!DragDoc->IDTable.EQEQ(mId, mINCL, cheElVT->data.VTEl,0)) {
      mDecl = DragDoc->IDTable.GetDECL(mId, mINCL);
      mId = ((CHETID*)mDecl->Supports.first)->data;
      mINCL = mDecl->inINCL;
    }
  }
  else {
    mDecl = clipDecl;
    while (mDecl->Supports.first)
      mDecl = DragDoc->IDTable.GetDECL(((CHETID*)mDecl->Supports.first)->data, mDecl->inINCL);
  }
  if (mDecl->inINCL) {
    absName = DragDoc->IDTable.IDTab[mDecl->inINCL]->FileName;
    AbsPathName(absName, DragDoc->IDTable.DocDir);
    mDoc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(absName.c);
    firstlast.INCL(multiDocHint);
    if (!mdh) {
      mdh = true;
      ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
    }
  }
  else
    mDoc = DragDoc;
  p_mDecl = (LavaDECL**)mDoc->IDTable.GetVar(TID(mDecl->OwnID, 0),type, 0);
  newmDecl = NewLavaDECL();
  *newmDecl = *(*p_mDecl);
  if (!newmDecl->Supports.first) {
    cheID = new CHETID;
    newmDecl->Supports.Append(cheID);
    newmDecl->SecondTFlags.INCL(overrides);
  }
  ((CHETID*)newmDecl->Supports.first)->data.nID = clipDecl->OwnID; //the new OwnID is not yet available
  for (mINCL = 0; 
       mINCL < DragDoc->IDTable.IDTab[mDecl->inINCL]->maxTrans;
       mINCL++)
    //DropDoc in mDoc
    if (DragDoc->IDTable.IDTab[mDecl->inINCL]->nINCLTrans[mINCL].isValid
       && (DragDoc->IDTable.IDTab[mDecl->inINCL]->nINCLTrans[mINCL].nINCL == DropINCL)) {
      ((CHETID*)newmDecl->Supports.first)->data.nINCL = mINCL;
      mINCL = DragDoc->IDTable.IDTab[mDecl->inINCL]->maxTrans;
    }

  str2 = new DString(newmDecl->FullName); 
  mHint = new CLavaPEHint(CPECommand_Change, mDoc, firstlast, (DWORD)newmDecl, (DWORD)str2, 0, (DWORD)p_mDecl);
  cheHint = new CHE;
  cheHint->data = (DObject*)mHint;
  cheHint->successor = vtHints;
  vtHints = cheHint;
  if (cheElVT)
    ((CHETID*)clipDecl->Supports.first)->data = cheElVT->data.VTEl;
  else {
    clipDecl->Supports.Destroy();
    clipDecl->SecondTFlags.EXCL(overrides);
  }
}


bool CLavaPEView::DoRefac(LavaDECL* dropDECL, bool& mdh, CHE*& vtHints)
{
  LavaDECL *dropParent, **pDragParent, *clipDECL, *clipElDECL, **del_d4, **d4, *execDECL,
    *implDECL, *implElDECL, **pDragImplDecl=0, **pDropImplDecl=0, **pDragP, *collectDECL;
  CHE *clipEl, *dropEl, *ioElImpl, *execEl, *implEl, *nextEl, *clipImplEl;
  TID tid;
  CHETID *che;
  int del_pos, pos, collectPos;
  CLavaPEHint *hint, *hintCol=0;
  DString *str2, *docName;
  CTreeItem *itemDropParent, *itemP;
  CLavaPEView* dragView;
  wxDocument *dragImplDoc=0, *dropImplDoc=0;
  CMainItemData *itemData;
  SynFlags firstlast;
  DWORD d6;
  TIDType idType;

  if (RefacCase == noRefac)
    return false;
  if ((dropDECL->DeclType == Attr) || (dropDECL->DeclType == Function))
    dropParent = dropDECL->ParentDECL;
  else
    dropParent = dropDECL;
  itemDropParent = (CTreeItem*)m_hitemDrop->parent();
  del_d4 = (LavaDECL**)((CMainItemData*)itemDropParent->getItemData())->synEl;
  clipDECL = (LavaDECL*)Clipdata->synEl;  
  dragView = (CLavaPEView*)DragDoc->DragView;
  itemData = (CMainItemData*)m_hitemDrag->getItemData();
  if (itemData->type == TIType_DECL)
    pos = dragView->GetPos(0, m_hitemDrag);
  else
    pos = dragView->GetPos(0, 0);
  itemP = (CTreeItem*)dragView->m_hitemDrag->parent();
  pDragParent = (LavaDECL**)((CMainItemData*)itemP->getItemData())->synEl;
  if ((DragDoc != GetDocument()) || mdh) {
    firstlast.INCL(multiDocHint);
    if (!mdh) {
      ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
      mdh = true;
    }
  }
  switch (RefacCase) {
  case publicToPriv:
    for (clipEl = (CHE*)clipDECL->NestedDecls.first;
         clipEl; clipEl = (CHE*)clipEl->successor) {
      clipElDECL = (LavaDECL*)clipEl->data;
      if (!clipElDECL->TypeFlags.Contains(defaultInitializer)) {
        if (clipElDECL->DeclType == Attr) 
          clipElDECL->Supports.Destroy();
        else {
          del_pos = 1;
          tid = TID(clipElDECL->OwnID, DragINCL);
          for (dropEl = (CHE*)dropParent->NestedDecls.first;
               dropEl && (!((LavaDECL*)dropEl->data)->Supports.first
                || (((CHETID*)((LavaDECL*)dropEl->data)->Supports.first)->data != tid));
               dropEl = (CHE*)dropEl->successor) {
            if ((((LavaDECL*)dropEl->data)->DeclType == Attr) || (((LavaDECL*)dropEl->data)->DeclType == Function))
              del_pos++;
          }
          if (dropEl) {
            dropDECL = (LavaDECL*)dropEl->data;
            execDECL = NewLavaDECL();
            *execDECL = *(LavaDECL*)((CHE*)((LavaDECL*)dropEl->data)->NestedDecls.last)->data;
            execEl = NewCHE(execDECL);
            clipElDECL->NestedDecls.Append(execEl);
            clipElDECL->Supports.Destroy();
            clipElDECL->SecondTFlags.EXCL(overrides);
            clipElDECL->WorkFlags.INCL(skipOnCopy);
            dropDECL->WorkFlags.INCL(skipOnDeleteID);
            str2 = new DString(dropParent->FullName);
            FIRSTLAST(GetDocument(), firstlast);
            hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), firstlast, (DWORD)dropEl->data, (DWORD)str2, (DWORD)del_pos, (DWORD)del_d4, (DWORD)dropParent);
            GetDocument()->UndoMem.AddToMem(hint);
            GetDocument()->UpdateDoc(this, false, hint);
            for (ioElImpl = (CHE*)dropDECL->NestedDecls.first;
                 ioElImpl && ioElImpl->successor;
                 ioElImpl = (CHE*)ioElImpl->successor) {
              che = new CHETID;
              che->data = TID(-((LavaDECL*)ioElImpl->data)->OwnID, ((CHETID*)((LavaDECL*)ioElImpl->data)->Supports.first)->data.nID); //old ID, new ID
              GetDocument()->IDTable.ChangeTab.Append(che);
            }
          }
        }
      }
    }
    break;

  case privToPublic:
    implDECL = NewLavaDECL();
    *implDECL = *clipDECL;
    implDECL->WorkFlags.INCL(fromPrivToPub);
    implEl = (CHE*)implDECL->NestedDecls.first;
    for (clipEl = (CHE*)clipDECL->NestedDecls.first; clipEl;
         clipEl = (CHE*)clipEl->successor) {
      implElDECL = (LavaDECL*)implEl->data;
      if (implElDECL->DeclType == Attr) {
        nextEl = (CHE*)implEl->successor;
        implDECL->NestedDecls.Delete(implEl);
        implEl = nextEl;
      }
      else {
        che = new CHETID;
        che->data = TID(implElDECL->OwnID, DragINCL);
        ((LavaDECL*)implEl->data)->Supports.Append(che);
        for (ioElImpl = (CHE*)implElDECL->NestedDecls.first;
             ioElImpl && ioElImpl->successor;
             ioElImpl = (CHE*)ioElImpl->successor) {
          che = new CHETID;
          che->data = TID(((LavaDECL*)ioElImpl->data)->OwnID, DragINCL); //old ID, new ID
          ((LavaDECL*)ioElImpl->data)->Supports.Append(che);
          ((LavaDECL*)ioElImpl->data)->SecondTFlags.INCL(funcImpl);
          ((LavaDECL*)ioElImpl->data)->WorkFlags.INCL(fromPrivToPub);
        }
        implElDECL->SecondTFlags.INCL(funcImpl);
        implElDECL->WorkFlags.INCL(fromPrivToPub);
        ((LavaDECL*)clipEl->data)->NestedDecls.Delete(((LavaDECL*)clipEl->data)->NestedDecls.last);
        implEl = (CHE*)implEl->successor;
      }
    }
    dragView = (CLavaPEView*)DragDoc->DragView;
    itemData = (CMainItemData*)m_hitemDrag->getItemData();
    if (itemData->type == TIType_DECL)
      pos = dragView->GetPos(0, m_hitemDrag);
    else
      pos = dragView->GetPos(0, 0);
    itemP = (CTreeItem*)dragView->m_hitemDrag->parent();
    d4 = pDragParent;
    if ((*d4)->FullName.l) {
      str2 = new DString( (*(LavaDECL**)d4)->FullName);
      implDECL->FullName = *str2;
    }
    else {
      str2 = 0;
      implDECL->FullName.Reset(0);
    }
    FIRSTLAST(DragDoc, firstlast);
    hintCol = new CLavaPEHint(CPECommand_Insert, DragDoc, firstlast, (DWORD)implDECL, (DWORD)str2, pos, (DWORD)d4, 0, (DWORD)Clipdata->ClipTree, (DWORD)Clipdata->docPathName);
    DragDoc->UpdateDoc(dragView, false, hintCol);
    break;
  case baseToEx:
  case exToBase:
    for (clipEl = (CHE*)clipDECL->NestedDecls.first;
         clipEl && (
          ( ((LavaDECL*)clipEl->data)->DeclType == Function)
             && (((LavaDECL*)clipEl->data)->TypeFlags.Contains(isAbstract)
                || ((LavaDECL*)clipEl->data)->TypeFlags.Contains(isNative))
             || (((LavaDECL*)clipEl->data)->DeclType == Attr)
                && !((LavaDECL*)clipEl->data)->TypeFlags.Contains(hasSetGet));
         clipEl = (CHE*)clipEl->successor);
    ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = true;
    if (clipEl && ((CLavaPEApp*)wxTheApp)->Browser.FindImpl(DragDoc, *pDragParent, dragImplDoc, pDragImplDecl)
      && ((CLavaPEApp*)wxTheApp)->Browser.FindImpl(GetDocument(), dropParent, dropImplDoc, pDropImplDecl)) {
      ((CLavaPEDoc*)dragImplDoc)->DragView = ((CLavaPEDoc*)dragImplDoc)->MainView;
      pDragP = ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->pDeclDragP;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->pDeclDragP = pDragImplDecl;
      implDECL = NewLavaDECL();
      implDECL->WorkFlags.INCL(ImplFromBaseToEx);
      implDECL->DeclType = clipDECL->DeclType;
      implDECL->FullName = clipDECL->FullName;
      implDECL->ParentDECL = clipDECL->ParentDECL;
      implDECL->TreeFlags = clipDECL->TreeFlags;
      implDECL->DeclDescType = clipDECL->DeclDescType;
      implDECL->SecondTFlags = clipDECL->SecondTFlags;
      pos = 1;
      if ((dropImplDoc != dragImplDoc)
         || (dropImplDoc != DragDoc)
         || (dropImplDoc != GetDocument())
         || (dragImplDoc != DragDoc)
         || (dragImplDoc != GetDocument())
         || mdh) {
         firstlast.INCL(multiDocHint);
         if (!mdh) {
           mdh = true;
           ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
         }
      }
      for (clipEl = (CHE*)clipDECL->NestedDecls.first;
           clipEl;
           clipEl = (CHE*)clipEl->successor) {
        clipElDECL = (LavaDECL*)clipEl->data;
        for (implEl = (CHE*)(*pDragImplDecl)->NestedDecls.first;
             implEl
               && (!((LavaDECL*)implEl->data)->Supports.first
                  || (clipElDECL->OwnID != ((CHETID*)((LavaDECL*)implEl->data)->Supports.first)->data.nID));
             implEl = (CHE*)implEl->successor) {
          if (!implDECL->NestedDecls.first && ((((LavaDECL*)implEl->data)->DeclType == Attr) || (((LavaDECL*)implEl->data)->DeclType == Function)))
            pos++;
        }
        if (implEl) {
          clipImplEl = NewCHE(NewLavaDECL());
          *((LavaDECL*)clipImplEl->data) = *((LavaDECL*)implEl->data);
          implDECL->NestedDecls.Append(clipImplEl);
          if ((clipElDECL->DeclType == Attr) && clipElDECL->TypeFlags.Contains(hasSetGet)) {
            for (implEl = (CHE*)implEl->successor;
                 implEl
                   && (!((LavaDECL*)implEl->data)->Supports.first
                      || (clipElDECL->OwnID != ((CHETID*)((LavaDECL*)implEl->data)->Supports.first)->data.nID));
                 implEl = (CHE*)implEl->successor);
            if (implEl) {
              clipImplEl = NewCHE(NewLavaDECL());
              *((LavaDECL*)clipImplEl->data) = *((LavaDECL*)implEl->data);
              implDECL->NestedDecls.Append(clipImplEl);
            }
          }
        }
        if ((RefacCase == baseToEx)
          && !GetDocument()->IDTable.EQEQ(((CHETID*)dropParent->Supports.first)->data, 0, TID((*pDragParent)->OwnID, DragINCL), 0))
          CorrVT_BaseToEx(dropParent, *pDragParent, (LavaDECL*)clipEl->data, mdh);
        else
          if ((RefacCase == exToBase)
            && !DragDoc->IDTable.EQEQ(((CHETID*)(*pDragParent)->Supports.first)->data, 0, TID(dropParent->OwnID, DropINCL), 0))
            CorrVT_ExToBase(*pDragParent, dropParent, (LavaDECL*)clipEl->data, mdh, vtHints);
      }
      collectDECL = ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL;
      collectPos = ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectPos;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL = NewLavaDECL();
      *((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL = *implDECL;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectPos = pos;
      if (dragImplDoc == dropImplDoc) {
        d4 = pDragImplDecl;
        str2 = new DString((*pDragImplDecl)->FullName);
        FIRSTLAST(dragImplDoc, firstlast);
        hint = new CLavaPEHint(CPECommand_Delete, dragImplDoc, firstlast, (DWORD)((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL, (DWORD)str2, (DWORD)pos, (DWORD)d4, (DWORD)pDropImplDecl);
        ((CLavaPEDoc*)dragImplDoc)->UpdateDoc(this, false, hint);
        delete hint;
        d6 = 0;
      }
      else
        d6 = (DWORD)&((CLavaPEDoc*)dragImplDoc)->mySynDef->SynDefTree;
      
      pos = GetPos(0, 0);
      d4 = pDropImplDecl;
      str2 = new DString((*pDropImplDecl)->FullName);
      docName = new DString(((CLavaPEDoc*)dragImplDoc)->GetAbsSynFileName());
      FIRSTLAST(dropImplDoc, firstlast);
      hint = new CLavaPEHint(CPECommand_Move, dropImplDoc, firstlast, (DWORD)implDECL,
                 (DWORD)str2, (DWORD)pos, (DWORD)d4, (DWORD)*pDragImplDecl, d6, (DWORD)docName);
      ((CLavaPEDoc*)dropImplDoc)->UpdateDoc(this, false, hint);
      delete hint;
      delete implDECL;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->pDeclDragP = pDragP;
      delete ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectDECL = collectDECL;
      ((CLavaPEView*)((CLavaPEDoc*)dragImplDoc)->DragView)->CollectPos = collectPos;
      ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = true;
    }
    else {
      ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = false;
      if (clipEl) {
        QMessageBox::critical(this, qApp->name(),"Cannot move feature because the implementation is not available",QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        ;//AfxMessageBox("Cannot move feature because the implementation is not available");
        return false;
      }
      else
        return true;
    }
    GetParentFrame()->Activate(true);
    break;
  default:
    return false;
  }
  if (hintCol) {
    hintCol->CommandData7 = 0;
    delete hintCol;
  }
  if (DragDoc->hasHint && DragDoc->DragView)
    ((CLavaPEView*)DragDoc->DragView)->pDeclDragP = (LavaDECL**)DragDoc->IDTable.GetVar(TID((*((CLavaPEView*)DragDoc->DragView)->pDeclDragP)->OwnID,0), idType);

  return true;
}


void CLavaPEView::OnEditCopy()
{
  IOnEditCopy();
  if (CollectDECL) {
    DeleteDragChain();
    GetListView()->selectAll(false);
    if (lastCurrent) {
      GetListView()->setCurAndSel(lastCurrent);
    }

  }
}

void CLavaPEView::IOnEditCopy() 
{
  CMainItemData *data, *newClipdata = 0;
  LavaDECL *decl = 0;
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    data = (CMainItemData*)item->getItemData();
    if (data && (data->type == TIType_DECL))
      decl = *(LavaDECL**)data->synEl;
    if (data && (decl && !decl->TypeFlags.Contains(thisComponent) && !decl->TypeFlags.Contains(thisCompoForm)
                 || (data->type == TIType_CHEEnumSel))) {

      //CSharedFile globFile;
      //CArchive ar(&globFile,CArchive::store);
      QByteArray ba;
			QDataStream ar(ba,IO_WriteOnly);
      CMainItemData clipdata(data->type, 0);
      clipdata.synEl = (unsigned long) NewLavaDECL();  //the Clipdata synEl is LavaDECL*
      if (!CollectDECL) {
        CollectDECL = NewLavaDECL();
        CollectDECL->DeclType = NoDef;
        AddToDragChain(item);
      }
      *(LavaDECL*)clipdata.synEl = *CollectDECL;
      clipdata.ClipTree = &GetDocument()->mySynDef->SynDefTree;
      clipdata.docPathName = new DString(GetDocument()->GetAbsSynFileName());
      clipdata.Serialize(ar);
      LavaSource *ls = new LavaSource();
      ls->setEncodedData(ba);
      wxTheApp->clipboard()->setData(ls, QClipboard::Clipboard);
      /*
      COleDataSource *srcItem = new COleDataSource;

      srcItem->CacheGlobalData(m_nIDClipFormat,globFile.Detach());
      srcItem->FlushClipboard();
      srcItem->SetClipboard();
      */
    }
  }

}

void CLavaPEView::OnEditCut()
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (!item || (item == GetListView()->firstChild()))
    return;
  int pos;
  if (!CollectDECL) 
    pos = GetPos(item, 0);
  else
    pos = CollectPos;
  IOnEditCopy();
  CLavaPEHint* hint;
  if (CollectDECL->DeclType == DragEnum) {
    hint = ChangeEnum(CollectDECL, item, true, false);
    if (hint) {
      hint->FirstLast.EXCL(lastHint);
      GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, false, hint);
      GetDocument()->ConcernForms(hint);
      GetDocument()->ConcernExecs(hint);
      GetDocument()->SetLastHint();
    }
  }
  else {
    CTreeItem* pItem = (CTreeItem*)item->parent();
    if (pItem) {
      DWORD d4 = ((CMainItemData*)pItem->getItemData())->synEl;
      DString* str2 = 0;
      if (CollectDECL->FullName.l)
        str2 = new DString(CollectDECL->FullName);
      hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), (const unsigned long)1, (DWORD)CollectDECL, (DWORD)str2, (DWORD)pos, d4, 0);
      GetDocument()->UpdateDoc(this, false, hint);
      if (myInclView) {
        GetDocument()->ConcernForms(hint);
        if (d4) 
          GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
        else
          GetDocument()->ConcernImpls(hint, 0);
        GetDocument()->ConcernExecs(hint);
      }
      GetDocument()->SetLastHint();
    }
  }
  if (CollectDECL) 
    DeleteDragChain();
}



void CLavaPEView::OnDelete() 
{
  CTreeItem *item, *par;
  CMainItemData *data, *parData;
  DWORD d4;
  DString *str2;
  CLavaPEHint *hint;
  LavaDECL *cutDECL, *newDECL;
  int pos;
  CHEEnumSelId *delEl, *newEl;

  item = (CTreeItem*)GetListView()->currentItem();
  if (!item || (item == GetListView()->firstChild()))
    return;
  data = (CMainItemData*)item->getItemData();
  if (data && (data->type != TIType_DECL)
           && ((data->type != TIType_CHEEnumSel) || !myInclView))
    return;
  par = (CTreeItem*)item->parent();

  if (CollectDECL) {
    if (CollectDECL->DeclType == DragEnum) {
      hint = ChangeEnum(CollectDECL, item, true, false);
      if (hint) {
        hint->FirstLast.EXCL(lastHint);
        GetDocument()->UndoMem.AddToMem(hint);
        GetDocument()->UpdateDoc(this, false, hint);
        GetDocument()->ConcernForms(hint);
        GetDocument()->ConcernExecs(hint);
        GetDocument()->SetLastHint();
      }
    }
    else {
      if (CanDelete && par) {
        d4 = ((CMainItemData*)par->getItemData())->synEl;
        str2 = 0;
        if (CollectDECL->FullName.l)
          str2 = new DString(CollectDECL->FullName);
        hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), (const unsigned long)1, (DWORD)CollectDECL, (DWORD)str2, (DWORD)CollectPos, d4, 0);
        GetDocument()->UpdateDoc(this, false, hint);
        if (myInclView) {
          GetDocument()->ConcernForms(hint);
          if (d4) 
            GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
          else
            GetDocument()->ConcernImpls(hint, 0);
          GetDocument()->ConcernExecs(hint);
        }
        GetDocument()->SetLastHint();
      }
    }
    DeleteDragChain();
  }
  else {  
    par = (CTreeItem*)par->parent();
    parData = (CMainItemData*)par->getItemData();
    if (data  && (data->type == TIType_DECL)) {
      cutDECL = *(LavaDECL**)data->synEl;
      if (EnableDelete(cutDECL)) {
        d4 = parData->synEl;
        if (par != GetListView()->firstChild()) {
          str2 = new DString(cutDECL->FullName);
          pos = GetPos(item, 0);
        }
        else {
          str2 = 0;
          if (!myInclView) {
            if (cutDECL->DeclType == FormText) 
              pos = GetPos(item);
            else {
              item = myMainView->BrowseTree(cutDECL, (CTreeItem*)myMainView->GetListView()->firstChild());
              pos = myMainView->GetPos(item);
            }
          }
          else
            pos = GetPos(item, 0);
        }
        hint = new CLavaPEHint(CPECommand_Delete, GetDocument(), (const unsigned long)1, (DWORD)cutDECL, (DWORD)str2, (DWORD)pos, d4/*, (DWORD)dropParent*/);
        GetDocument()->UndoMem.AddToMem(hint);
        GetDocument()->UpdateDoc(this, false, hint);
        if (myInclView) {
          GetDocument()->ConcernForms(hint);
          if (d4) 
            GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
          else
            GetDocument()->ConcernImpls(hint, 0);
          GetDocument()->ConcernExecs(hint);
        }
        GetDocument()->SetLastHint();
      }
    }
    else {
      if (data && (data->type == TIType_CHEEnumSel)) {
        delEl = (CHEEnumSelId*)data->synEl;

        par = (CTreeItem*)item->parent();
        par = (CTreeItem*)par->parent();
        parData = (CMainItemData*)par->getItemData();
        cutDECL = *(LavaDECL**)parData->synEl;
        newDECL = NewLavaDECL();
        *newDECL = *cutDECL;
        CHAINANY* newItems = &((TEnumDescription*)newDECL->EnumDesc.ptr)->EnumField.Items;
        newEl = (CHEEnumSelId*)newItems->first;
        while (newEl && (newEl->data.Id != delEl->data.Id))
          newEl = (CHEEnumSelId*)newEl->successor;
        if (newEl) {
          newEl = (CHEEnumSelId*)newItems->Uncouple(newEl);
          if (newEl->successor)
            ((CHEEnumSelId*)newEl->successor)->data.selItem = true;
          delEl->data.selItem = true;
          delete newEl;
          str2 = new DString((*(LavaDECL**)parData->synEl)->FullName);
          newDECL->WorkFlags.INCL(selEnum);
          newDECL->TreeFlags.INCL(ItemsExpanded);
          hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)1, (DWORD)newDECL, (DWORD)str2, 0, parData->synEl);
          GetDocument()->UndoMem.AddToMem(hint);
          GetDocument()->UpdateDoc(this, false, hint);
          GetDocument()->ConcernForms(hint);
          GetDocument()->ConcernExecs(hint);
          GetDocument()->SetLastHint();
        }
      }
    }
  }
}

void CLavaPEView::OnNewEnumItem() 
{
  CTreeItem *item, *par;
  CMainItemData *data, *parData;
  LavaDECL *oldDECL, *newDECL;
  CHEEnumSelId *newEl, *che;
  CHAINANY* newItems;
  DString *str;
  QString iT;
  CLavaPEHint *hint;

  
  item = (CTreeItem*)GetListView()->currentItem();
  if (!item)
    return;
  data = (CMainItemData*)item->getItemData();
  if (!data || (data->type != TIType_CHEEnumSel) 
               && (data->type != TIType_EnumItems))
    return;
  par = (CTreeItem*)item->parent();
  if (data->type == TIType_CHEEnumSel)
    par = (CTreeItem*)par->parent();
  parData = (CMainItemData*)par->getItemData();
  oldDECL = *(LavaDECL**)parData->synEl;
 
  CEnumItem *cm = new CEnumItem(&iT, 0, &((TEnumDescription*)oldDECL->EnumDesc.ptr)->EnumField.Items, true, this);
  if (cm->exec() == QDialog::Accepted) {
    newDECL = NewLavaDECL();
    *newDECL = *oldDECL;
    newEl = new CHEEnumSelId;
    newEl->data.Id = DString(iT);
    newItems = &((TEnumDescription*)newDECL->EnumDesc.ptr)->EnumField.Items;
    newEl->data.selItem = true;
    if  (data->type == TIType_EnumItems)
      newItems->Prepend(newEl);
    else { 
      che = (CHEEnumSelId*)newItems->first;
      while (che && (che->data.Id != ((CHEEnumSelId*)data->synEl)->data.Id))
        che = (CHEEnumSelId*)che->successor;
      if (che) 
        newItems->Insert(che, newEl);
      ((CHEEnumSelId*)data->synEl)->data.selItem = true;
    }
    str = new DString((*(LavaDECL**)parData->synEl)->FullName);
    newDECL->WorkFlags.INCL(selEnum);
    newDECL->TreeFlags.INCL(ItemsExpanded);
    hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)1, (DWORD)newDECL, (DWORD)str, 0, parData->synEl);
    GetDocument()->UndoMem.AddToMem(hint);
    GetDocument()->UpdateDoc(this, false, hint);
    GetDocument()->ConcernForms(hint);
    GetDocument()->ConcernExecs(hint);
    GetDocument()->SetLastHint();
  }
  delete cm;
}

void CLavaPEView::OnUpdateNewEnumItem(wxAction* action) 
{
  action->setEnabled((ItemSel != 0)
        && ( (GroupType == TIType_EnumItems) || (GroupType == TIType_CHEEnumSel))); 
	
}


CLavaPEHint* CLavaPEView::ChangeEnum(LavaDECL* clipDECL, CTreeItem* item, bool cut, bool paste)
{
  //makes the CPECommand_Change-hint for a cut, paste and drag and drop operation with enum-items
  CMainItemData* itemData = (CMainItemData*)item->getItemData();
  CTreeItem* itemP = (CTreeItem*)item->parent();
  LavaDECL** poldDECL;
  CHEEnumSelId* relEl, *afterElem;
  if (itemData->type == TIType_EnumItems) {
    poldDECL = (LavaDECL**)itemData->synEl;
    afterElem = 0;
  }
  else {
    CMainItemData* itemPData = (CMainItemData*)itemP->getItemData();
    poldDECL = (LavaDECL**)itemPData->synEl;
    afterElem = (CHEEnumSelId*)itemData->synEl;
  }
  if ((*poldDECL)->DeclDescType != EnumType)
    return 0;
  LavaDECL* newDECL = NewLavaDECL();
  *newDECL = **poldDECL;

  CHAINANY* newItems = &((TEnumDescription*)newDECL->EnumDesc.ptr)->EnumField.Items;
  relEl = (CHEEnumSelId*)newItems->first;
  if (afterElem) {
    while (relEl && (relEl->data.Id != afterElem->data.Id))
      relEl = (CHEEnumSelId*)relEl->successor;
    afterElem = relEl;
  }
  CHAINANY* clipItems = &((TEnumDescription*)clipDECL->EnumDesc.ptr)->EnumField.Items;
  CHEEnumSelId* clipEl = (CHEEnumSelId*)clipItems->first;
  while (clipEl) {
    clipEl = (CHEEnumSelId*)clipItems->Uncouple(clipEl);
    if (cut) {
      relEl = (CHEEnumSelId*)newItems->first;
      while (relEl && (relEl->data.Id != clipEl->data.Id))
        relEl = (CHEEnumSelId*)relEl->successor;
      if (relEl) {
        relEl = (CHEEnumSelId*)newItems->Uncouple(relEl);
        if (relEl) {
          if (relEl == afterElem)
            afterElem = (CHEEnumSelId*)afterElem->predecessor;
          delete relEl;
        }
      }
    }
    if (paste) {
      relEl = (CHEEnumSelId*)newItems->first;
      while (relEl) {
        while (relEl && (relEl->data.Id != clipEl->data.Id))
          relEl = (CHEEnumSelId*)relEl->successor;
        if (relEl) {
          clipEl->data.Id.Insert(DString("CopyOf_"), 0);
          relEl = (CHEEnumSelId*)newItems->first;
        }
      }
      newItems->Insert(afterElem, clipEl);
      afterElem = clipEl;
    }
    clipEl = (CHEEnumSelId*)clipItems->first;
  }
  if (afterElem)
    afterElem->data.selItem = true;
  DString* str2 = new DString((*poldDECL)->FullName);
  newDECL->WorkFlags.INCL(selEnum);
  newDECL->TreeFlags.INCL(ItemsExpanded);
  return new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)3, (DWORD)newDECL, (DWORD)str2, 0, (DWORD)poldDECL);
  
}

void CLavaPEView::OnEditPaste()
{
  //remember: the CMainItemData on the Clipboard has LavaDECL* in synEl
  //COleDataObject pDataObject;
  CTreeItem *item, *itemP;
  TIType pasteType;
  LavaDECL *declClip, *decl;
  CMainItemData clipdata, *itemData;
  TDeclType elType;
  CHE* che;
  CLavaPEHint *hint;
  DString *str2;
  DWORD d4;

  if (wxTheApp->clipboard()->data(QClipboard::Clipboard)->provides(m_nIDClipFormat)) {
    QByteArray ba = ((LavaSource*)wxTheApp->clipboard()->data(QClipboard::Clipboard))->encodedData(m_nIDClipFormat);
		QDataStream ar(ba,IO_ReadOnly);
    item = (CTreeItem*)GetListView()->currentItem();
    clipdata.Serialize(ar);
    declClip = (LavaDECL*)clipdata.synEl;
    pasteType = CanPaste(declClip->DeclType, declClip->TreeFlags, declClip->SecondTFlags, item);
    if (pasteType != TIType_NoType) {
      if ((declClip->DeclType == DragIO) || (declClip->DeclType == DragFeature)) {
        switch (pasteType) {
        case TIType_Features:
          elType = Attr;
          break;
        case TIType_Input:
          elType = IAttr;
          break;
        case TIType_Output:
          elType = OAttr;
          break;
        default: ;
        }
        che = (CHE*)declClip->NestedDecls.first;
        while (che) {
          decl = (LavaDECL*)che->data;
          if ((decl->DeclType == Attr) || (decl->DeclType == IAttr) || (decl->DeclType == OAttr)) 
            decl->DeclType = elType;
          che = (CHE*)che->successor;
        }
      }
      item = (CTreeItem*)GetListView()->currentItem();
      itemData = (CMainItemData*)item->getItemData();
      DString fn = GetDocument()->GetAbsSynFileName();
      if (fn != *clipdata.docPathName) 
        declClip->inINCL = 10000;
      itemP = (CTreeItem*)item->parent();
      if (declClip->DeclType == DragEnum) {
        hint = ChangeEnum(declClip, item, false, true);
        if (hint) {
          hint->FirstLast.EXCL(lastHint);
          GetDocument()->UndoMem.AddToMem(hint);
          GetDocument()->UpdateDoc(this, false, hint);
          GetDocument()->ConcernForms(hint);
          GetDocument()->ConcernExecs(hint);
          GetDocument()->SetLastHint();
        }
      }
      else { 
        int pos;
        if (itemData->type == TIType_DECL)
          pos = GetPos(0, item);
        else
          pos = GetPos(0, 0);
        d4 = ((CMainItemData*)itemP->getItemData())->synEl;
        if ((*(LavaDECL**)d4)->FullName.l) {
          str2 = new DString( (*(LavaDECL**)d4)->FullName);
          declClip->FullName = *str2;
        }
        else {
          str2 = 0;
          declClip->FullName.Reset(0);
        }
        hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)1, (DWORD)declClip, (DWORD)str2, pos, d4, 0, (DWORD)clipdata.ClipTree, (DWORD)clipdata.docPathName);
        GetDocument()->UpdateDoc(this, false, hint);
        if (myInclView) {
          GetDocument()->ConcernForms(hint);
          if (d4) 
            GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
          else
            GetDocument()->ConcernImpls(hint, 0);
          GetDocument()->ConcernExecs(hint);
        }
        GetDocument()->SetLastHint();
      }
      clipdata.docPathName = 0;
    }
  }
}

void CLavaPEView::OnUpdateEditPaste(wxAction* action) 
{
  bool bEnable = false;
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    bEnable = wxTheApp->clipboard()->data(QClipboard::Clipboard)->provides(m_nIDClipFormat);
		if (bEnable) {
      QByteArray ar = ((LavaSource*)wxTheApp->clipboard()->data(QClipboard::Clipboard))->encodedData(m_nIDClipFormat);
      CMainItemData clipdata;
      SynFlags treeflags, secondtflags;
      TDeclType deftype= clipdata.Spick(ar, treeflags, secondtflags);
      bEnable = (CanPaste(deftype, treeflags, secondtflags, item) != TIType_NoType);
    }
  }
  action->setEnabled(bEnable);
  /*
  if (bEnable)
    PopupMenu.EnableMenuItem(ID_EDIT_PASTE, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_EDIT_PASTE, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
  */
}


void CLavaPEView::OnUpdateEditSel(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  bool enable = false;
  if (myInclView && item) {
    CMainItemData *itd = (CMainItemData*)item->getItemData();
    enable = (itd->type == TIType_DECL)
//             || (itd->type == TIType_CHEEnumSel)
//             || (itd->type == TIType_EnumItems)
             || (itd->type == TIType_Constraint);
  }
  action->setEnabled(enable);
  /*
  if (enable)
    PopupMenu.EnableMenuItem(ID_EDIT_SEL, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_EDIT_SEL, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
  */
}

void CLavaPEView::OnEditSel() 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (!item ) { //||  (myInclView && (item == GetListView()->firstChild()))) {
    QMessageBox::critical(this, qApp->name(),IDP_NoElemSel,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return;
  }
  else
    OnEditSelItem(item, false);
}

void CLavaPEView::OnEditSelItem(CTreeItem* item, bool clickedOnIcon) 
{
  if (!item)
    return;
  CLavaPEHint *hint;
  TisOnWhat isonwhat;
  LavaDECL *itdDECL, *decl, *ppDECL;
  CMainItemData *itd = (CMainItemData*)item->getItemData();
  int okBox;
  DString *name;
  bool onEnumsel = false;
  if (itd->type == TIType_CHEEnumSel) {
    item = (CTreeItem*)item->parent();
    item = (CTreeItem*)item->parent();
    itd = (CMainItemData*)item->getItemData();
    onEnumsel = true;
  } 
  else
    if (itd->type == TIType_EnumItems) {
      item = (CTreeItem*)item->parent();
      itd = (CMainItemData*)item->getItemData();
      onEnumsel = true;
    }
  if ((itd->type != TIType_DECL) && (itd->type != TIType_Constraint)) 
    return;
  if (itd->type == TIType_Constraint) {
    OnShowConstraint();
    return;
  }
  if (itd->type == TIType_DECL) {
    itdDECL = *(LavaDECL**)itd->synEl;
    if (clickedOnIcon) {
      Gotodef(item);
      return;
    }
    else
      if (!myInclView)
        isonwhat = isOnFormProps;
      else
        isonwhat = isOnEdit;
    decl = NewLavaDECL();
    *decl = *itdDECL;
    decl->DECLError1.Destroy();
    ppDECL = decl->ParentDECL;
    GetListView()->ensureItemVisible(item);
    if (isonwhat == isOnFormProps) {
      if (decl->DeclType == FormText) {
        CFormTextBox* fbox = new CFormTextBox(decl, this);
        okBox = fbox->exec();
        delete fbox;
      }
      else {
       // CLavaPEWizard* dtwz = new CLavaPEWizard(GetDocument(), (LavaDECL**)itd->synEl/*itdDECL*/, "LavaPE Wizard", isonwhat, decl, this);
       // okBox = dtwz->exec();
       // delete dtwz;
       // okBox = QDialog::Rejected;
      }
    }
    else {
      bool build=false;
      if (onEnumsel) {
        decl->DeclType = EnumDef; //call the EnumBox!
        decl->TreeFlags.INCL(ItemsExpanded);
      }
      okBox = CallBox(decl, itdDECL, GetDocument(), false, build, this);
    }
    if (okBox == QDialog::Accepted) {
      if (decl->Annotation.ptr && decl->Annotation.ptr->FA.ptr)
          ((TAnnotation*)decl->Annotation.ptr->FA.ptr)->BType =
                                  ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(GetDocument()->mySynDef, decl);
      name = new DString(itdDECL->FullName);  //bisheriger Name

      hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (const unsigned long)1, (DWORD)decl, (DWORD)name, 0, itd->synEl);
      GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, false, hint);
      if (myInclView) {
        GetDocument()->ConcernForms(hint);
        GetDocument()->ConcernImpls(hint, ppDECL);
        GetDocument()->ConcernExecs(hint);
      }
      GetDocument()->SetLastHint();
    }
    else
      delete decl;
  }
}//OnEditSel


void CLavaPEView::OnNewmember() 
{
  OnInsert(Attr); 
}

void CLavaPEView::OnNewclassimpl() 
{
  OnInsert(Impl);
}

void CLavaPEView::OnNewVirtualType() 
{
  OnInsert(VirtualType);
}

void CLavaPEView::OnNewclassintf() 
{
  OnInsert(Interface);
}

void CLavaPEView::OnNewenum() 
{
  OnInsert(EnumDef);
}

void CLavaPEView::OnNewfunction() 
{
  OnInsert(Function);
}

void CLavaPEView::OnNewinitiator() 
{
  OnInsert(Initiator);
}

void CLavaPEView::OnNewNamespace() 
{
  OnInsert(Package);
}


void CLavaPEView::OnNewset() 
{
  OnInsert(SetDef);
}


void CLavaPEView::OnComponent() 
{
  OnInsert(CompObj);
}

void CLavaPEView::OnCSpec() 
{
  OnInsert(CompObjSpec);
}

TIType CLavaPEView::CanPaste (TDeclType defType, SynFlags treeFlags, SynFlags secondtflags, CTreeItem* toItem)
{
  if (toItem == GetListView()->firstChild())
    return TIType_NoType;
  CMainItemData *pardata, *toData;
  CTreeItem* parItem=0;
  LavaDECL *ppdecl;

  toData = (CMainItemData*)toItem->getItemData();
  if (defType == DragFText) {
    if (!myInclView && (toItem != GetListView()->firstChild()))
      return TIType_Features;
    else
      return TIType_NoType;
  }
  else
    if (!myInclView)
      return TIType_NoType;
  if (defType == DragEnum) {
    if ((toData->type == TIType_EnumItems)
        || (toData->type == TIType_CHEEnumSel))
      return TIType_CHEEnumSel;
    else
      return TIType_NoType;
  }
  else {
    if (toData->type == TIType_DECL) {
      parItem = (CTreeItem*)toItem->parent();
      toData = (CMainItemData*)parItem->getItemData();
      parItem = (CTreeItem*)parItem->parent();
    }
    else 
      if ((toData->type == TIType_EnumItems)
          || (toData->type == TIType_CHEEnumSel))
        return TIType_NoType;
      else
        parItem = (CTreeItem*)toItem->parent();
    if (!parItem)
      return TIType_NoType;
    pardata = (CMainItemData*)parItem->getItemData();
    ppdecl = *(LavaDECL**)pardata->synEl;
    if (!ContainTab[ppdecl->DeclType] [defType])
      return TIType_NoType;
    if ((ppdecl->DeclType == Function) 
      && ( ppdecl->SecondTFlags.Contains(overrides) || ppdecl->SecondTFlags.Contains(funcImpl)
         || ppdecl->TypeFlags.Contains(isPropGet) || ppdecl->TypeFlags.Contains(isPropSet)
         || (ppdecl->op != OP_noOp)))
    return TIType_Refac; //TIType_NoType;
    switch (defType) {
      case DragEnum:
        break;
      case DragDef:
      case DragParam:
        if ((defType == DragDef) && (toData->type != TIType_Defs))
          return TIType_NoType;
        else
          if ((defType == DragParam) && (toData->type != TIType_VTypes))
            return TIType_NoType;
        if (treeFlags.Contains(dragVT) && !ContainTab[ppdecl->DeclType] [VirtualType])
          return TIType_NoType;
        if (treeFlags.Contains(dragInt) && !ContainTab[ppdecl->DeclType] [Interface]) 
          return TIType_NoType;
        if (treeFlags.Contains(dragImpl) && !ContainTab[ppdecl->DeclType] [Impl]) 
          return TIType_NoType;
        if (treeFlags.Contains(dragIni) && !ContainTab[ppdecl->DeclType] [Initiator])
          return TIType_NoType;
        if (treeFlags.Contains(dragFuncDef) && !ContainTab[ppdecl->DeclType] [Function]) 
          return TIType_NoType;
        if (treeFlags.Contains(dragFuncImpl)
            && (!ContainTab[ppdecl->DeclType] [Function]
           || (ppdecl->DeclType == Interface) || (ppdecl->DeclType == Package)))
          return TIType_NoType;
        if (treeFlags.Contains(dragPack) && !ContainTab[ppdecl->DeclType] [Package]) 
          return TIType_NoType;
        if (treeFlags.Contains(dragCompo) && !ContainTab[ppdecl->DeclType] [Component]) 
          return TIType_NoType;
        if (treeFlags.Contains(dragCOS) && !ContainTab[ppdecl->DeclType] [CompObjSpec]) 
          return TIType_NoType;
        if (treeFlags.Contains(dragCO) && !ContainTab[ppdecl->DeclType] [CompObj]) 
          return TIType_NoType;
        if (treeFlags.Contains(dragInput) && !ContainTab[ppdecl->DeclType] [IAttr]) 
          return TIType_NoType;
        break;
      case DragFeatureF:
        if (toData->type != TIType_Features)
          return TIType_NoType;
        if (secondtflags.Contains(funcImpl))
          if (ppdecl->DeclType != Impl)
            return TIType_Refac; //TIType_NoType;
          /*
          if ((ppdecl->DeclType == Interface)
              && ppdecl->TypeFlags.Contains(isComponent)
              && !secondtflags.Contains(inComponent))
            return TIType_NoType;
              */
        break;
      case DragFeature:
      case DragIO:
        if( (toData->type != TIType_Features)
              && (toData->type != TIType_Input)
              && (toData->type != TIType_Output))
          return TIType_NoType;
        /*
        if ((ppdecl->DeclType == Interface)
            && ppdecl->TypeFlags.Contains(isComponent)
            && !secondtflags.Contains(inComponent))
          return TIType_NoType;
          ??wozu war das da??*/

        break;
  //    case ExecDef:
  //      if (toData->type != TIType_Constraint)
  //        return TIType_NoType;
  //      break;
    }
    return toData->type;
  }

}


bool CLavaPEView::OnInsert(TDeclType eType, LavaDECL *iDECL)
{
  CMainItemData *data, *ppdata;
  CTreeItem *par, *ppar, *collNode = NULL, *item;
  SynFlags first = (const unsigned long)1;
  bool asChild, asSibling, callBox = false, isOnNew;
  LavaDECL *decl, *parDECL, *refDECL = 0, *parentT, *setDECL; //*inEl
  CLavaPEHint *hint, *Sethint;
  CHE *cheIOEl, *che;
  DWORD d4 = 0;
  DString *str2 = 0, newName, *setStr2;
  int pos;
  TID setID;

  item = (CTreeItem*)GetListView()->currentItem();
  if (!item) {
    QMessageBox::critical(this, qApp->name(),IDP_NoInsertionPos,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
  if (myInclView && (item == GetListView()->firstChild())) {
    QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
  if (!myInclView && (eType != FormText)) {
    QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
  data = (CMainItemData*)item->getItemData();
  if (data->type == TIType_DECL) {
    switch ((*(LavaDECL**)data->synEl)->DeclType) {
    case FormText:
    case Attr:
    case Function:
      asSibling = (eType == Attr) || (eType == Function) || (eType == FormText);
      break;
    case IAttr:
      asSibling = (eType == Attr);
      if (asSibling)
        eType = IAttr;
      break;
    case OAttr:
      asSibling = (eType == Attr);
      if (asSibling)
        eType = OAttr;
      break;
    default:
      asSibling = (eType != ExecDef) && (eType != Attr) && (eType != FormText);
    }
  }
  else
    asSibling = false;
  if (!asSibling) {
    switch (data->type) {
    case TIType_Defs:
      asChild = (eType != Attr) && (eType != Function) && (eType != ExecDef);
      break;
    case TIType_Features:
      asChild = (eType == Attr) || (eType == Function) || (eType == FormText);
      break;
    case TIType_VTypes:
      asChild = (eType == VirtualType);
      break;
    case TIType_Input:
      asChild = (eType == Attr);
      if (asChild) {
        eType = IAttr;
      }
      break;
    case TIType_Output:
      asChild = (eType == Attr);
      if (asChild)
        eType = OAttr;
      break;
    case TIType_Constraint:
      asChild = (eType == ExecDef);
    default:
      asChild = false;
    }
  }
  else
    asChild = false;
  if (asChild) 
    par = item;
  else {
    if (asSibling) {
      par= (CTreeItem*)item->parent();
      data = (CMainItemData*)par->getItemData();
    }
    else {
      par = item;
      while ((par != GetListView()->firstChild()) 
            && !((data->type == TIType_DECL)
            && ( ( (*(LavaDECL**)data->synEl)->DeclDescType == StructDesc)
               || ( (*(LavaDECL**)data->synEl)->DeclDescType == EnumType)
               || (eType == ExecDef) && ( (*(LavaDECL**)data->synEl)->DeclType == FormDef) ))) {
        par= (CTreeItem*)par->parent();
        data = (CMainItemData*)par->getItemData();
      }
      if (myInclView && (eType == ExecDef)
          && ( (*(LavaDECL**)data->synEl)->DeclType == FormDef)) {
        GetListView()->setCurAndSel(par);
        OnShowformview();
        return true;
      }
      par = getSectionNode(par, eType);
      collNode = par;
      data = (CMainItemData*)par->getItemData();
      if (par) {
        item = par;
        asChild = true;
      }
      else {
        QMessageBox::critical(this, qApp->name(),IDP_NoInsertionPos,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return false;
      }
    }
  }
  if (iDECL)
    decl = iDECL;
  else {
    decl = NewLavaDECL();
    if ((eType <= FormDef) || (eType == SetDef) || (eType == EnumDef))
      decl->TreeFlags.INCL(hasEmptyOpt);
    decl->TreeFlags.INCL(isExpanded);
  }
  if (eType == SetDef) {
    eType = Interface;
    decl->SecondTFlags.INCL(isSet);
  }
  decl->DeclType = eType;

  ppar= (CTreeItem*)par->parent();
  ppdata = (CMainItemData*)ppar->getItemData();

  if (iDECL  && decl->SecondTFlags.Contains(funcImpl) /*&& !dragParent*/) {
    // a function implementation or it's IO-members cannot be inserted in this way
    decl->SecondTFlags.EXCL(funcImpl);
    decl->TypeFlags.EXCL(isPropGet);
    decl->TypeFlags.EXCL(isPropSet);
    decl->Supports.Destroy();
    //decl->RefID.nID = -1;
    if (decl->DeclType == Function)  {
      if (decl->op != OP_noOp) {
        decl->LocalName = DString("copy_of_operator_") + LBaseData->OpFuncNames[decl->op];
        decl->op = OP_noOp;
      }
      cheIOEl = (CHE*)decl->NestedDecls.first;
      while (cheIOEl) {
        if ( ((LavaDECL*)cheIOEl->data)->SecondTFlags.Contains(funcImpl)) {
          ((LavaDECL*)cheIOEl->data)->SecondTFlags.EXCL(funcImpl);
          //((LavaDECL*)cheIOEl->data)->RefID.nID = -1;
          ((LavaDECL*)cheIOEl->data)->Supports.Destroy();
        }
        cheIOEl = (CHE*)cheIOEl->successor;
      }
    }
  }
  if (((decl->DeclType == IAttr) || (decl->DeclType == OAttr))
    && (*(LavaDECL**)ppdata->synEl)->SecondTFlags.Contains(funcImpl)) {
    //no insertion of io into a function implementation
    delete decl;
    QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }

  if (data->type == TIType_Constraint) {
    if (iDECL) {
      hint = new CLavaPEHint(CPECommand_Constraint, GetDocument(), first, (DWORD)decl);
      GetDocument()->UndoMem.AddToMem(hint);
    }
    else {
      delete decl;
      GetListView()->setCurAndSel(par);
      OnShowConstraint();
    }
    return true;
  }
  else {
    if ((data->type == TIType_Defs)
        || (data->type == TIType_VTypes)
        || (data->type == TIType_Features)
        || (data->type == TIType_Input)
        || (data->type == TIType_Output)
      ) {
      decl->DeclType = eType;
    }
    else {
      delete decl;
      QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      return false;
    }
  }
  if ((ppdata->type != TIType_DECL) && (ppar != GetListView()->firstChild())) {
      delete decl;
      QMessageBox::critical(this, qApp->name(),IDP_NoFieldInsertion,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      return false;
  }
  decl->ParentDECL = *(LavaDECL**)ppdata->synEl;
  if (!myInclView || (ppar != GetListView()->firstChild())
       && (ppar->parent() != GetListView()->firstChild())) {
    decl->FullName = (*(LavaDECL**)ppdata->synEl)->FullName;
    if (iDECL) {
      decl->FullName += ddppkt;
      decl->FullName += iDECL->LocalName;
    }
  }
  else {
    if (iDECL) {
      decl->FullName = decl->LocalName;
    }
  }
  if (iDECL && !decl->SecondTFlags.Contains(funcImpl)) {
    isOnNew = false;
  }
  else {
    isOnNew = true;
    switch (eType) {
    case EnumDef:
      decl->DeclDescType = EnumType;
      decl->SecondTFlags.INCL(isEnum);
      decl->ParentDECL->TreeFlags.INCL(DefsExpanded);
      break;
    case CompObj:
    case FormDef:
    case Impl:
    case Interface:
    case CompObjSpec:
    case Initiator:
    case Package:
      decl->DeclDescType = StructDesc;
      decl->ParentDECL->TreeFlags.INCL(DefsExpanded);
      break;
    case Function:
      decl->DeclDescType = StructDesc;
      decl->ParentDECL->TreeFlags.INCL(MemsExpanded);
      break;
    case Attr:
      decl->DeclDescType = Undefined;
      decl->ParentDECL->TreeFlags.INCL(MemsExpanded);
      break;
    case IAttr:
      decl->DeclDescType = Undefined;
      decl->ParentDECL->TreeFlags.INCL(InExpanded);
      break;
    case OAttr:
      decl->DeclDescType = Undefined;
      decl->ParentDECL->TreeFlags.INCL(OutExpanded);
      break;
     case VirtualType:
      decl->DeclDescType = Undefined;
      decl->ParentDECL->TreeFlags.INCL(ParaExpanded);
      break;
     case FormText:
      decl->DeclDescType = LiteralString;
        break;
   default:
      decl->DeclDescType = Undefined;
    }
  }
  bool buildSet=false; 
  if (!iDECL || callBox) {
    int okBox = CallBox(decl, 0, GetDocument(), isOnNew, buildSet, this, asChild);
    if (okBox == QDialog::Accepted) {
      if (decl->Annotation.ptr && decl->Annotation.ptr->FA.ptr)
         ((TAnnotation*)decl->Annotation.ptr->FA.ptr)->BType = 
            ((CLavaPEApp*)wxTheApp)->Browser.GetBasicType(GetDocument()->mySynDef, decl);
    }
    else {
      if (collNode) 
        if ((CTreeItem*)collNode->firstChild() == NULL) {
          DeleteItemData(collNode);
          delete collNode;
        }
      delete decl;
      return false;
    }
  }
  if (asChild) 
    pos = GetPos(0, 0);
  else
    pos = GetPos(0, item);
  d4 = ppdata->synEl;
  if ((*(LavaDECL**)ppdata->synEl)->FullName.l)
    str2 = new DString((*(LavaDECL**)ppdata->synEl)->FullName);
  parDECL = *(LavaDECL**)ppdata->synEl;
  decl->ParentDECL = parDECL;
  hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), first, (DWORD) decl, (DWORD)str2, (DWORD)pos, d4/*, (DWORD)dragParent, (DWORD)clipTree, (DWORD)docPathName*/);
  GetDocument()->UndoMem.AddToMem(hint);
  GetDocument()->UpdateDoc(this, false, hint);
  if (buildSet) {
    che = (CHE*)parDECL->NestedDecls.first;
    while (che && ( (LavaDECL*)che->data != decl))
      che = (CHE*)che->successor;
    if (che) {
      if (decl->DeclType == Interface) {
        d4 = (DWORD)&che->data;
        pos = 0;
        parentT = decl;
      }
      else
        parentT = decl->ParentDECL;
      setID = TID(((LavaDECL*)hint->CommandData1)->OwnID, 0);
      setDECL = GetDocument()->MakeSet(setID);
      setDECL->ParentDECL = parentT;
      newName = DString("Set");
        /*;
      while (CheckNewName(newName.c, setDECL, GetDocument(), 0))
        newName += DString("_S");*/
      setDECL->LocalName = newName;
      setDECL->TreeFlags.INCL(isExpanded);
      setDECL->TreeFlags.INCL(ParaExpanded);
      pos++;
      setStr2 = new DString(decl->FullName);
      Sethint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)0, (DWORD) setDECL, (DWORD)setStr2, (DWORD)pos, d4);
      GetDocument()->UndoMem.AddToMem(Sethint);
      GetDocument()->UpdateDoc(this, false, Sethint);
    }
  }
  if (myInclView) {
    GetDocument()->ConcernForms(hint);
    if (d4) 
      GetDocument()->ConcernImpls(hint, *(LavaDECL**)d4);
    GetDocument()->ConcernExecs(hint);
  }
  GetDocument()->SetLastHint();
  return true;
}

void CLavaPEView::OnShowformview() 
{  //show the form view
  OnShowSpecialView(FormDef);
}

void CLavaPEView::OnUpdateShowformview(wxAction* action) 
{
  bool enable = false;
  if (!myInclView) 
    enable = false;
  else {
    CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
    if (item) {
      CMainItemData* data = (CMainItemData*)item->getItemData();
      enable = (data->type == TIType_DECL) 
                   && ((*(LavaDECL**)data->synEl)->DeclType == /*S*/Impl)
                   && (*(LavaDECL**)data->synEl)->TypeFlags.Contains(isGUI);
    }
  }
  action->setEnabled(enable);
  /*
  if (enable) 
    PopupMenu.EnableMenuItem(ID_SHOWFORMVIEW, MF_ENABLED | MF_BYCOMMAND); 
  else 
    PopupMenu.EnableMenuItem(ID_SHOWFORMVIEW, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
    */
}


void CLavaPEView::OnShowConstraint() 
{
  OnShowSpecialView(ExecDef);
}

void CLavaPEView::OnShowSpecialView(TDeclType exprType) 
{  
  bool hasView = false;
  CMainItemData * itd;
  CTreeItem* root = (CTreeItem*)GetListView()->firstChild();
  CTreeItem* item;
  if (!myInclView && (exprType == FormDef)) {
    item = root;
    hasView = true;
  }
  else {
    item = (CTreeItem*)GetListView()->currentItem();
    if (!item && !myInclView)
      item = root;
  }
  if (item) 
    itd = (CMainItemData*)item->getItemData();
  if ((exprType == ExecDef) && (itd->type == TIType_Constraint))
    hasView = GetDocument()->OpenCView(GetDocument()->GetConstrDECL(*(LavaDECL**)itd->synEl));
  else {
    if ((itd->type == TIType_DECL) 
        && (((*(LavaDECL**)itd->synEl)->DeclType == /*S*/Impl)
       && (*(LavaDECL**)itd->synEl)->TypeFlags.Contains(isGUI)
          || ((*(LavaDECL**)itd->synEl)->DeclType == FormDef)))
        hasView = GetDocument()->openFView((LavaDECL**)itd->synEl);
  }
  if (!hasView) 
    if (exprType == FormDef)
      QMessageBox::critical(this, qApp->name(),IDP_NoFormView,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    else 
      QMessageBox::critical(this, qApp->name(),IDP_NoConstraintView,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
}


void CLavaPEView::OnGotodef() 
{
  //wxDocManager::GetDocumentManager()->SetActiveView(this, true);
  Gotodef((CTreeItem*)GetListView()->currentItem());
}

void CLavaPEView::Gotodef(CTreeItem* item) 
{
  LavaDECL *DECL, *decl;
  TID id;
  CGotoBox *box;
  CMainItemData * itd;
  
  if (item) {
    itd = (CMainItemData*)item->getItemData();
    if (itd->type == TIType_Constraint)
      OnShowConstraint();
    else {
      if (itd->type == TIType_DECL) {
        DECL = *(LavaDECL**)itd->synEl;
        id.nID = -1;
        ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, DECL);//item, item->parent());
        id = DECL->RefID;
        if ((id.nID >= 0) && (DECL->Supports.first || DECL->Inherits.first)
            || (DECL->Supports.first != DECL->Supports.last)
            || DECL->Supports.first && DECL->Inherits.first
            || (DECL->Inherits.first != DECL->Inherits.last)) {
         //select ID
          box = new CGotoBox(DECL, GetDocument(), this);
          if (box->exec() == QDialog::Accepted)
            id = box->SelID;
          else
            id.nID = -1;
          delete box;
          if (id.nID < 0)
            return;
        }
        else 
          if (id.nID < 0) {
            if (DECL->Supports.first)
              id = ((CHETID*)DECL->Supports.first)->data;
            else 
              if (DECL->Inherits.first)
                id = ((CHETID*)DECL->Inherits.first)->data;
          }
        decl = ((CLavaPEApp*)wxTheApp)->Browser.BrowseDECL(GetDocument(), id);
      }
    }
  }
}

void CLavaPEView::OnUpdateGotodef(wxAction* action) 
{
  bool canBrowse = false;
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    CMainItemData * itd = (CMainItemData*)item->getItemData();
    canBrowse = ( (itd->type == TIType_Constraint)
                 || (itd->type == TIType_DECL)
                 && ((CLavaPEApp*)wxTheApp)->Browser.CanBrowse(*(LavaDECL**)itd->synEl));
  }
  action->setEnabled(canBrowse);  
  /*
  if (canBrowse)
    PopupMenu.EnableMenuItem(ID_GOTODEF, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_GOTODEF, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
    */
}


void CLavaPEView::OnGotoImpl() 
{
  LavaDECL *DECL;
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  TID id;
  if (item) {
    CMainItemData * itd = (CMainItemData*)item->getItemData();
    if (itd->type == TIType_DECL) {
      DECL = *(LavaDECL**)itd->synEl;
      if ((DECL->DeclType == Interface)
          || (DECL->DeclType == CompObjSpec)
          || (DECL->ParentDECL->DeclType == Interface) 
             && ((DECL->DeclType == Function) || (DECL->DeclType == Attr) && DECL->TypeFlags.Contains(hasSetGet))) {
        if (((DECL->DeclType == Function) || (DECL->DeclType == Attr))
          && (DECL->TypeFlags.Contains(isAbstract) || DECL->TypeFlags.Contains(isNative)) )
           QMessageBox::information(this, qApp->name(),ERR_NoImplForAbstract,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        else
          //find implemetation id
          ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, DECL);//item, item->parent());
          if (!((CLavaPEApp*)wxTheApp)->Browser.GotoImpl(GetDocument(), DECL)) {
            ((CLavaPEApp*)wxTheApp)->Browser.DestroyLastBrsContext();
            if (DECL->DeclType == Interface)
              if (DECL->TypeFlags.Contains(isComponent))
                QMessageBox::critical(this, qApp->name(),ERR_LookForCompoObj,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
              else
                QMessageBox::critical(this, qApp->name(),ERR_NoClassImpl,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
            else if (DECL->DeclType == Function)
              if (DECL->ParentDECL->TypeFlags.Contains(isComponent))
                QMessageBox::critical(this, qApp->name(),ERR_LookForCompoObjF,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
              else
                QMessageBox::critical(this, qApp->name(),ERR_NoFuncImpl,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
            else if (DECL->DeclType == Attr)
              if (DECL->ParentDECL->TypeFlags.Contains(isComponent))
                QMessageBox::critical(this, qApp->name(),ERR_LookForCompoObjF,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
              else
                QMessageBox::critical(this, qApp->name(),ERR_SetGetError,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        
            else if (DECL->DeclType == CompObjSpec)
              QMessageBox::critical(this, qApp->name(),ERR_LookForCompoObj,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          }
        }
      else
        if ((DECL->ParentDECL->DeclType == Impl) && (DECL->DeclType == Function)
          || (DECL->DeclType == Initiator)) {
          ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext = new CBrowseContext(this, DECL);//item, item->parent());
          GetDocument()->OpenCView((LavaDECL*)((CHE*)DECL->NestedDecls.last)->data);
        }
    }
  }
}

void CLavaPEView::OnUpdateGotoImpl(wxAction* action) 
{
  bool canBrowse = false;
  LavaDECL* DECL;
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    CMainItemData * itd = (CMainItemData*)item->getItemData();
    if (itd->type == TIType_DECL) {
      DECL = *(LavaDECL**)itd->synEl;
      canBrowse = (DECL->DeclType == Interface)
        || (DECL->DeclType == Function)
        || (DECL->DeclType == CompObjSpec)
        || (DECL->DeclType == Initiator)
        || (DECL->DeclType == Attr) && (DECL->ParentDECL->DeclType == Interface)
             && DECL->TypeFlags.Contains(hasSetGet);
    }
  }
  action->setEnabled(canBrowse);  
  /*
  if (canBrowse)
    PopupMenu.EnableMenuItem(ID_GOTOIMPL, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_GOTOIMPL, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
    */
}


void CLavaPEView::OnFindreferences() 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  CFindData fw;
  fw.FWhere = findInThisDoc;
  if (item) {
    CMainItemData * itd = (CMainItemData*)item->getItemData();
    if (itd->type == TIType_DECL) {
      LavaDECL* DECL = *(LavaDECL**)itd->synEl;
      ((CLavaPEApp*)wxTheApp)->Browser.OnFindRefs(GetDocument(), DECL, fw);
    }
    else
    if (itd->type == TIType_CHEEnumSel) {
      fw.enumID = ((CHEEnumSelId*)itd->synEl)->data.Id;
      item = (CTreeItem*)item->parent();
      itd = (CMainItemData*)item->getItemData();
      LavaDECL* DECL = *(LavaDECL**)itd->synEl;
      ((CLavaPEApp*)wxTheApp)->Browser.OnFindRefs(GetDocument(), DECL, fw);
    }
  }
}

void CLavaPEView::OnUpdateFindreferences(wxAction* action) 
{
  bool canBrowse = false;
  CMainItemData * itd;
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    itd = (CMainItemData*)item->getItemData();
    canBrowse = (itd->type == TIType_DECL) || (itd->type == TIType_CHEEnumSel);
    /*
    if (itd->type == TIType_DECL) {
      DECL = *(LavaDECL**)itd->synEl;
      canBrowse = (DECL->DeclType == Interface)
        || (DECL->DeclType == Function)
        || (DECL->DeclType == Attr) && DECL->TypeFlags.Contains(hasSetGet);
    }
    */
  }
  action->setEnabled(canBrowse); 
  /*
  if (canBrowse)
    PopupMenu.EnableMenuItem(ID_FINDREFERENCES, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_FINDREFERENCES, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 	
    */
}


void CLavaPEView::DeleteItemData(CTreeItem* parent)
{
  if (!parent)
    parent = (CTreeItem*)GetListView()->firstChild();
  if (parent) {
    CMainItemData * itd = (CMainItemData*)parent->getItemData();
    parent->setItemData(0);
    if (itd)
      delete itd;
    CTreeItem* item = (CTreeItem*)parent->firstChild();
    while (item) {
      DeleteItemData(item);
      item = (CTreeItem*)item->nextSibling();
    }
  }
}


/*
void CLavaPEView::OnDefaultForm() 
{
  DString *str2, newFName, newLName;
  CLavaPEHint *hint;
  DWORD d4;
  int pos;
  LavaDECL * parDECL, *decl, *exDECL;
  CTreeItem *item, *par;
  CMainItemData *data, *pdata;

  item = (CTreeItem*)GetListView()->currentItem();
  data = (CMainItemData*)item->getItemData();
  newLName = DString("F__") + (*(LavaDECL**)data->synEl)->LocalName;
  par = (CTreeItem*)item->parent();
  pdata = (CMainItemData*)par->getItemData();
  parDECL = *(LavaDECL**)pdata->synEl;
  if (parDECL->FullName.l)
    newFName = parDECL->FullName + ddppkt + newLName;
  else 
    newFName = newLName;
  exDECL = GetDocument()->FindInSupports(newLName, 0, parDECL);
  if (exDECL) {
    QMessageBox::critical(this, qApp->name(),IDP_DefaultFormName2,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return;
  }
  decl = NewLavaDECL();
  *decl = **(LavaDECL**)data->synEl;
  decl->DeclType = FormDef;
  decl->RefID = TID((*(LavaDECL**)data->synEl)->OwnID, 0);
  decl->FullName = newFName;
  decl->LocalName = newLName;
  if (((CLavaPEApp*)wxTheApp)->Browser.HasDefaultForm(decl, *(LavaDECL**)data->synEl, GetDocument()->mySynDef) > 0) {
    pos = GetPos(0, item);
    d4 = pdata->synEl;
    if ((*(LavaDECL**)pdata->synEl)->FullName.l) {
      str2 = new DString((*(LavaDECL**)pdata->synEl)->FullName);
    }
    else {
      str2 = 0;
    }
    hint = new CLavaPEHint(CPECommand_Insert, GetDocument(), (const unsigned long)3, (DWORD) decl, (DWORD)str2, (DWORD)pos, d4);
    GetDocument()->UndoMem.AddToMem(hint);
    GetDocument()->UpdateDoc(this, false, hint);
  }
  else {
    delete decl;
    QMessageBox::critical(this, qApp->name(),IDP_NoDefaultForm,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return;
  }
}

void CLavaPEView::OnUpdateDefaultForm(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  bool enable = false;
  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    enable = (data->type == TIType_DECL) 
              && ( (*(LavaDECL**)data->synEl)->DeclType == Interface);
  }
  pCmdUI->Enable(enable);
  if (enable)
    PopupMenu.EnableMenuItem(ID_DefaultForm, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_DefaultForm, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
}
*/

void CLavaPEView::destroy() 
{
  DisableActions();
  if (((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext)
    ((CLavaPEApp*)wxTheApp)->Browser.LastBrowseContext->RemoveView(this);
  DeleteItemData();
 
  if (myInclView && GetDocument()->MainView) {
    GetDocument()->OnDestroyMainView(this);
    GetDocument()->MainView = 0;
  }
  CTreeView::destroy();
}


void CLavaPEView::OnUpdateEditCopy(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  CMainItemData* data;
  LavaDECL *decl = 0;
  if (item != 0) {
    data = (CMainItemData*)item->getItemData();
    if (data && (data->type == TIType_DECL))
      decl = *(LavaDECL**)data->synEl;
  }
  bool enable = (myInclView && (item != 0)
       && (decl && !decl->TypeFlags.Contains(thisComponent)
                && !decl->TypeFlags.Contains(thisCompoForm)
           || (data->type == TIType_CHEEnumSel))
           || decl && (decl->DeclType == FormText));
  action->setEnabled(enable);
  /*
  if (enable)
    PopupMenu.EnableMenuItem(ID_EDIT_COPY, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_EDIT_COPY, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
    */
}

void CLavaPEView::OnUpdateEditCut(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  CMainItemData* data;
  bool enable = false;
  if (item != 0) {
    data = (CMainItemData*)item->getItemData();
    if (data  && (data->type == TIType_DECL)) 
      enable = EnableDelete(*(LavaDECL**)data->synEl)
                && (myInclView 
                  || ((*(LavaDECL**)data->synEl)->DeclType == FormText)) ;
    else
      enable = data && (data->type == TIType_CHEEnumSel) && myInclView;
  }
  action->setEnabled(enable);
  /*
  if (enable) {
    PopupMenu.EnableMenuItem(ID_EDIT_CUT, MF_ENABLED | MF_BYCOMMAND); 
    PopupMenu.EnableMenuItem(ID_DELETE, MF_ENABLED | MF_BYCOMMAND); 
  }
  else {
    PopupMenu.EnableMenuItem(ID_EDIT_CUT, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
    PopupMenu.EnableMenuItem(ID_DELETE, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
  }
  */
}

/*
void CLavaPEView::OnShowPattern() 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    CMainItemData* itd = (CMainItemData*)item->getItemData();
    if (itd &&  (itd->type == TIType_DECL)
      && ((*(LavaDECL**)itd->synEl)->DeclType == VirtualType)) {
      LavaDECL *paramDECL = *(LavaDECL**)itd->synEl;
//      if (CollectDECL) 
//        delete CollectDECL;
//      CollectDECL = NewLavaDECL();
//      GetDocument()->CollectPattern(paramDECL, CollectDECL);
      GetDocument()->CollectPattern(paramDECL, 0);
      CTreeItem* pitem = (CTreeItem*)item->parent();
      pitem = (CTreeItem*)pitem->parent();
      CTreeItem* citem = (CTreeItem*)GetNextItem(pitem, true);
      while (citem) {
        item = (CTreeItem*)citem->firstChild();
        while (item) {
          itd = (CMainItemData*)item->getItemData();
          if ((*(LavaDECL**)itd->synEl)->WorkFlags.Contains(checkmark))
            ;//SetAllStates(item, nState, nStateMask, true);
          item = (CTreeItem*)item->nextSibling();
        }
        citem->setOpen(true);
        SetTreeFlags(citem, true);
        citem =(CTreeItem*) citem->nextSibling();
      }
      paramDECL->ParentDECL->ResetCheckmarks();
    }
  }
}


void CLavaPEView::OnUpdateShowPattern(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  bool enable;
  if (item) {
    CMainItemData* itd = (CMainItemData*)item->getItemData();
    enable = itd &&  (itd->type == TIType_DECL)
                 && ((*(LavaDECL**)itd->synEl)->DeclType == VirtualType);
  }
  else
    enable = false;
  action->setEnabled(enable);
}
*/

void CLavaPEView::OnShowOverridables() 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item != 0) {
    CMainItemData* itd = (CMainItemData*)item->getItemData();
    if (itd->type == TIType_DECL) {
      LavaDECL* decl = *(LavaDECL**)itd->synEl;
      if ((decl->DeclType == Interface)
        || (decl->DeclType == Package) && decl->LocalName.l && decl->Supports.first) {
        GetDocument()->MakeVElems(decl);
        GetDocument()->openVTView((LavaDECL**)itd->synEl, (unsigned long)0);
      }
    }
  }
}


void CLavaPEView::OnUpdateOverride(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  bool ena = item != 0;
  if (ena) {
    CMainItemData* itd = (CMainItemData*)item->getItemData();
    ena = (itd->type == TIType_DECL);
    if (ena) {
      LavaDECL* decl = *(LavaDECL**)itd->synEl;
      ena = ((decl->DeclType == Interface)
            || (decl->DeclType == Package) && decl->LocalName.l && decl->Supports.first);
    }
  }
  action->setEnabled(ena);
  /*
  if (ena)
    PopupMenu.EnableMenuItem(ID_Override, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_Override, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
    */

}

void CLavaPEView::OnUpdateNewVirtualType(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_VTypes) && (ContainTab[DefTypeSel] [ VirtualType] != 0)); 
}


void CLavaPEView::OnUpdateNewclassintf(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Interface] != 0));
}

void CLavaPEView::OnUpdateNewenum(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Interface] != 0));
}

/*
void CLavaPEView::OnUpdateNewform(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ FormDef] != 0)); 
}
*/
void CLavaPEView::OnUpdateNewfunction(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_Features) && (ContainTab[DefTypeSel] [ Function] != 0));
}

void CLavaPEView::OnUpdateNewinitiator(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Initiator] != 0));
}

void CLavaPEView::OnUpdateNewmember(wxAction* action) 
{
  action->setEnabled( (ItemSel != 0) 
                 && ( (GroupType == TIType_Features)
                      && ( (ContainTab[DefTypeSel] [ Attr]  != 0) 
                         || (*(LavaDECL**)DataSel->synEl)->SecondTFlags.Contains(funcImpl))
                    || ( (GroupType == TIType_Input) && (ContainTab[DefTypeSel] [ IAttr] != 0) 
                    || (GroupType == TIType_Output) && (ContainTab[DefTypeSel] [ OAttr] != 0) )
                      && !(*(LavaDECL**)DataSel->synEl)->SecondTFlags.Contains(funcImpl)
                      && !(*(LavaDECL**)DataSel->synEl)->SecondTFlags.Contains(overrides)
                      && ( (*(LavaDECL**)DataSel->synEl)->op == OP_noOp)));

}

void CLavaPEView::OnUpdateNewNamespace(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Package] != 0));
}

void CLavaPEView::OnUpdateNewclassimpl(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Impl] != 0));
}

void CLavaPEView::OnUpdateNewset(wxAction* action) 
{
  action->setEnabled((ItemSel != 0) && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ Interface] != 0));
}

void CLavaPEView::OnUpdateCSpec(wxAction* action) 
{
  action->setEnabled((ItemSel != 0)  && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ CompObjSpec] != 0));
}

void CLavaPEView::OnUpdateComponent(wxAction* action) 
{
  action->setEnabled((ItemSel != 0)  && (GroupType == TIType_Defs) && (ContainTab[DefTypeSel] [ CompObj] != 0));
}

void CLavaPEView::SetErrAndCom(CTreeItem* item)
{
  CHE* che;
  bool hasC;
  LavaDECL *decl = 0; 
  DString str0;

  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    if (data->type == TIType_CHEEnumSel) {
      if (((CHEEnumSelId*)data->synEl)->data.DECLComment.ptr)
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetComment(((CHEEnumSelId*)data->synEl)->data.DECLComment.ptr->Comment, true);
      else {
        DString str0;
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetComment(str0, true);
      }
      return;
    }
    if (data->type == TIType_DECL)
      decl = *(LavaDECL**)data->synEl;
    else {
      if (data->type == TIType_Constraint) {
        decl = *(LavaDECL**)data->synEl;
        che = (CHE*)decl->NestedDecls.last;
        hasC = (che != 0);
        if (hasC) 
          hasC =  ((((LavaDECL*)che->data)->DeclDescType == ExecDesc)
                && ((LavaDECL*)che->data)->Exec.ptr);
        if (hasC)
          decl = (LavaDECL*)che->data;
      }
      else
        decl = 0; //no constraint node present
    }
    if (decl) {
      if (decl->DECLComment.ptr)
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetComment(decl->DECLComment.ptr->Comment, !decl->DECLError1.first && !decl->DECLError2.first);
      else 
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetComment(str0, !decl->DECLError1.first && !decl->DECLError2.first);
      CheckAutoCorr(decl);
      ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetErrorOnBar(decl);
    }
    else {
      ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetComment(str0, true);
      ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->ResetError();
    }
  }
}

bool CLavaPEView::AddToDragChain(CTreeItem* itemDrag, bool vkControl, bool sameCat)
{
  CTreeItem *itemP, *itemPP = 0;
  CMainItemData *itdP = 0, *itd, *itdPP = 0;
  LavaDECL *itemDECL, *refDECL, *El;
  CHE *che, *cheIO, *cheTree, *cheCol;
  bool canComponent;
  CHEEnumSelId* cheId;
  
  if (itemDrag) {
    itd = (CMainItemData*)itemDrag->getItemData();
    itemP = (CTreeItem*)itemDrag->parent();
    if (itemP) {
      itdP = (CMainItemData*)itemP->getItemData();
      itemPP = (CTreeItem*)itemP->parent();
    }
    if (itemPP)
      itdPP = (CMainItemData*)itemPP->getItemData();
    if (itd && (itd->type == TIType_DECL)) {
      itemDECL = *(LavaDECL**)itd->synEl;
      if (CollectDECL->DeclType == NoDef) {
        CollectDECL->DeclDescType = StructDesc;
        if (itemP)
          itemPP = (CTreeItem*)itemP->parent();
        if (itemPP) 
          itdPP = (CMainItemData*)itemPP->getItemData();
        if (itemDrag == GetListView()->firstChild())
          CollectDECL->DeclType = DragDef;
        else {
          if (itemDECL->DeclType == FormText)
            CollectDECL->DeclType = DragFText;
          else
            switch (itdP->type) {
            case TIType_VTypes:
              CollectDECL->DeclType = DragParam;
              break;
            case TIType_Defs:
              CollectDECL->DeclType = DragDef;
              CollectDECL->TreeFlags.INCL(mixedDrag);
              break;
            case TIType_Features:
              if (!CollectDECL->TreeFlags.Contains(mixedDrag))
                CollectDECL->DeclType = DragFeature;
              break;
            case TIType_Output:
            case TIType_Input:
              CollectDECL->DeclType = DragIO;
              break;
            }
        }
      }
      else
        if ( CollectDECL->NestedDecls.first
            && (itemDECL->ParentDECL
              != ((LavaDECL*)((CHE*)CollectDECL->NestedDecls.first)->data)->ParentDECL))
          return false;
      if (CollectDECL->DeclType == NoDef) 
        return false;
      if (vkControl && !sameCat)
        CollectDECL->TreeFlags.INCL(mixedDrag);
      El = NewLavaDECL();
      che = NewCHE(El);
      *El = *itemDECL;
      if (El->DeclType == VirtualType)
        CollectDECL->TreeFlags.INCL(dragVT);
      else if (El->DeclType == Interface) 
        CollectDECL->TreeFlags.INCL(dragInt);
      else if (El->DeclType == Impl) 
        CollectDECL->TreeFlags.INCL(dragImpl);
      else if (El->DeclType == Initiator)
        CollectDECL->TreeFlags.INCL(dragIni);
      else if ((El->DeclType == Function) || (El->DeclType == Attr)) {
        if (El->NestedDecls.last && (((LavaDECL*)((CHE*)El->NestedDecls.last)->data)->DeclType == ExecDef))
          CollectDECL->TreeFlags.INCL(dragFuncImpl);
        else
          CollectDECL->TreeFlags.INCL(dragFuncDef);
        if (El->SecondTFlags.Contains(overrides))
          CollectDECL->TreeFlags.INCL(dragOverrides);
      }
      else if (El->DeclType == Package) 
        CollectDECL->TreeFlags.INCL(dragPack);
      else if (El->DeclType == Component) 
        CollectDECL->TreeFlags.INCL(dragCompo);
      else if (El->DeclType == CompObjSpec) 
        CollectDECL->TreeFlags.INCL(dragCOS);
      else if (El->DeclType == CompObj) 
        CollectDECL->TreeFlags.INCL(dragCO);  
      else if (El->DeclType == IAttr) 
        CollectDECL->TreeFlags.INCL(dragInput);
      if (itemDECL->ParentDECL) {
        cheTree = (CHE*)itemDECL->ParentDECL->NestedDecls.first;
        cheCol = (CHE*)CollectDECL->NestedDecls.first;
        while (cheCol) {
          while (cheTree && ((LavaDECL*)cheCol->data)->OwnID != ((LavaDECL*)cheTree->data)->OwnID)
            if (((LavaDECL*)cheTree->data)->OwnID == ((LavaDECL*)che->data)->OwnID) {
              if (!cheCol->predecessor) {
                CollectPos = GetPos(itemDrag, 0);
                if ((CollectDECL->DeclType != PatternDef) && (itemDECL->DeclType == VirtualType))
                  CollectDECL->DeclType = DragParam;
              }
              CollectDECL->NestedDecls.Insert(cheCol->predecessor, che);
              cheTree = 0;
            }
            else
              cheTree = (CHE*)cheTree->successor;
          if (cheTree)
            cheCol = (CHE*)cheCol->successor;
          else
            cheCol = 0;
        }
        if (cheTree)
          CollectDECL->NestedDecls.Append(che);
      }
      else
        CollectDECL->NestedDecls.Append(che);
      if ((CollectDECL->DeclType != PatternDef)
        && ((itemDECL->DeclType == Function) || (itemDECL->DeclType == Attr))) {
        if (!CollectDECL->TreeFlags.Contains(mixedDrag))
          CollectDECL->DeclType = DragFeature;
      }
      if ((CollectDECL->DeclType == DragFeature) && (itemDECL->DeclType == Function)) {
        CollectDECL->DeclType = DragFeatureF;
        if (itemDECL->ParentDECL->DeclType == Impl)
          CollectDECL->SecondTFlags.INCL(funcImpl);
      }
      canComponent = !CollectDECL->SecondTFlags.Contains(funcImpl);
      if (canComponent && 
          ((CollectDECL->DeclType == DragFeature) || (CollectDECL->DeclType == DragFeatureF))) {
        canComponent = !itemDECL->ParentDECL || !itemDECL->ParentDECL->NestedDecls.first || CollectDECL->SecondTFlags.Contains(inComponent);
        if (canComponent) {
          if (itemDECL->DeclType == Function) {
            cheIO = (CHE*)itemDECL->NestedDecls.first;
            while (cheIO && canComponent) {
              refDECL = GetDocument()->IDTable.GetDECL(((LavaDECL*)((CHE*)cheIO)->data)->RefID);
              canComponent = refDECL && ((refDECL->DeclType != /*S*/Interface) || refDECL->TypeFlags.Contains(isComponent));
              cheIO = (CHE*)cheIO->successor;
            }
          }
          else {
            refDECL = GetDocument()->IDTable.GetDECL(itemDECL->RefID);
            canComponent = refDECL && ((refDECL->DeclType != /*S*/Interface) || refDECL->TypeFlags.Contains(isComponent));
          }
        }
        if (canComponent)
          CollectDECL->SecondTFlags.INCL(inComponent);
        else
          CollectDECL->SecondTFlags.EXCL(inComponent);
      }
      CanDelete = CanDelete && EnableDelete(itemDECL);
//      SetAllStates(itemDrag, nState, nStateMask, false);
      return true;
    }//TITypeDECL
    else {
      if (itd && (itd->type == TIType_CHEEnumSel)) {
        cheId = (CHEEnumSelId*)itd->synEl;
        if (CollectDECL->DeclType == NoDef) {
          CollectDECL->DeclType = DragEnum;
          itemP = (CTreeItem*)itemP->parent();
          itdP = (CMainItemData*)itemP->getItemData();
          CollectDECL->ParentDECL = *(LavaDECL**)itdP->synEl;
          CollectDECL->DeclDescType = EnumType;
          CollectDECL->EnumDesc.ptr = new TEnumDescription;
          ((TEnumDescription*)CollectDECL->EnumDesc.ptr)->EnumField.DeclDescType = BasicType;
          ((TEnumDescription*)CollectDECL->EnumDesc.ptr)->EnumField.BType = Enumeration;
        }
        else 
          if ((CollectDECL->DeclType != DragEnum)
              || (CollectDECL->ParentDECL != *(LavaDECL**)itdP->synEl))
            return false;
        CHEEnumSelId* newId = new CHEEnumSelId;
        newId->data = cheId->data;
        ((TEnumDescription*)CollectDECL->EnumDesc.ptr)->EnumField.Items.Append(newId);
//        SetAllStates(itemDrag, nState, nStateMask, false);
        return true;
      }
    }
  }
  return false;
} 

bool CLavaPEView::EnableDelete(LavaDECL* decl)
{
  return (!decl->ParentDECL || ( decl->ParentDECL->op == OP_noOp) )
         && (!decl->SecondTFlags.Contains(funcImpl) || decl->WorkFlags.Contains(allowDEL))
         && (!decl->SecondTFlags.Contains(overrides)
            || (decl->DeclType != IAttr) && (decl->DeclType != OAttr) )
         && (!decl->TypeFlags.Contains(thisCompoForm) 
            || (decl->DeclType != Function) && (decl->ParentDECL->DeclType != Function) )
         && !decl->TypeFlags.Contains(thisComponent);
}

void CLavaPEView::DeleteDragChain()
{
//  CTreeItem* item = (CTreeItem*)GetListView()->firstChild();
//  unsigned nStateMask=0;// = TVIS_DROPHILITED; 
//  unsigned nState = 0;
//  SetAllStates(item, nState, nStateMask, false);
  if (CollectDECL) {
    delete CollectDECL;
    CollectDECL = 0;
  }
}

/*
void CLavaPEView::SetAllStates(CTreeItem* item1, unsigned nState, unsigned nStateMask, bool inPattern)
{
  if (inPattern) {
    CMainItemData* itd = (CMainItemData*)GetItemData(item1);
    if (itd && (itd->type == TIType_DECL) && (*(LavaDECL**)itd->synEl)->WorkFlags.Contains(checkmark))  
      ;//GetListView()->SetItemState(item1, nState, nStateMask);
  }
  else
    ;//GetListView()->SetItemState(item1, nState, nStateMask);
  CTreeItem* item = (CTreeItem*)GetNextItem(item1, true);
  while(item) {
    SetAllStates(item, nState, nStateMask, inPattern);
    item = (CTreeItem*)item->nextSibling();
  }
}


void CLavaPEView::OnLButtonDown(UINT nFlags, CPoint point) 
{ 
  if (CollectDECL && (GetAsyncKeyState(VK_SHIFT) < 0)) {
    CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
    CTreeItem* itemDrag = GetListView()->HitTest(point, &nFlags);
    if (item != itemDrag) {
      while (item && (item != itemDrag)) {
        item = (CTreeItem*)item->nextSibling();
      }
      if (!item)
        DeleteDragChain();
    }
  }
  CTreeView::OnLButtonDown(nFlags, point);
}
*/

void CLavaPEView::OnSelchanged()  
{
  bool ok, sameCat, collected = false;
  CTreeItem *itemDrag, *item;
  LavaDECL *itemDECL;

  QListViewItem* selItem = GetListView()->currentItem();
  if (!selItem)
    return;
  if (!selItem->isSelected())
    return;
  if (lastCurrent && (GetListView()->withControl && !GetListView()->withShift || GetListView()->withShift && !GetListView()->withControl)) {
    if (!CollectDECL) {
      CanDelete = true;
      CollectDECL = NewLavaDECL();
      CollectDECL->DeclType = NoDef;
      ok = AddToDragChain(lastCurrent);
      CollectPos = GetPos(lastCurrent, 0);
    }
    if (GetListView()->withShift) {
      itemDrag = lastCurrent;
      while (itemDrag && (itemDrag != selItem)) {
        itemDrag = (CTreeItem*)itemDrag->nextSibling();
        if (!AddToDragChain(itemDrag))
          itemDrag = 0;
      }
      if (!itemDrag || (itemDrag != selItem)
          || !CollectDECL->NestedDecls.first
          && (!CollectDECL->EnumDesc.ptr || !((TEnumDescription*)CollectDECL->EnumDesc.ptr)->EnumField.Items.first)) {
        DeleteDragChain();
        GetListView()->withShift = false;
        GetListView()->selectAll(false);
        GetListView()->setCurAndSel(selItem);
      }
      else
        collected = true;
    }
    else {//withControl
      sameCat = lastCurrent->parent() == selItem->parent();
      if (!AddToDragChain((CTreeItem*)selItem, true, sameCat)) {
        DeleteDragChain();
        GetListView()->withControl = false;
        GetListView()->selectAll(false);
        GetListView()->setCurAndSel(selItem);
      }
      else
        collected = true;
    }
  }
  else
    if (!m_hitemDrag)
      DeleteDragChain();
  if (m_hitemDrop)
    return;
  ParItemSel = 0;
  ItemSel = (CTreeItem*)selItem; 
  if (ItemSel) {
    if (myVTView)
      ((CVTView*)myVTView)->activeInt = false;
    DataSel = (CMainItemData*)ItemSel->getItemData();
    if (DataSel) {
      item = ItemSel;
      if (myInclView) {
        if (DataSel->type == TIType_DECL) {
          itemDECL = *(LavaDECL**)DataSel->synEl;
          if ((itemDECL->DeclType == Interface)
              || (itemDECL->DeclType == Package)
              && itemDECL->LocalName.l && itemDECL->Supports.first) {
            ((CVTView*)myVTView)->activeInt = true;
            GetDocument()->openVTView((LavaDECL**)DataSel->synEl, (unsigned long)1);
            if (itemDECL->DECLError1.first)
              ItemSel->SetItemMask(itemDECL->DECLError1.first || itemDECL->DECLError2.first,
                itemDECL->DECLComment.ptr && itemDECL->DECLComment.ptr->Comment.l);
          }
          else 
            ((CVTView*)myVTView)->activeInt = itemDECL->isInSubTree(((CVTView*)myVTView)->myDECL);
          if (ItemSel != GetListView()->firstChild()) {
            ItemSel = (CTreeItem*)ItemSel->parent();
            DataSel = (CMainItemData*)ItemSel->getItemData();
          }
        }
        if (ItemSel != GetListView()->firstChild()) {
          GroupType = DataSel->type;
          ItemSel = (CTreeItem*)ItemSel->parent();
          DataSel = (CMainItemData*)ItemSel->getItemData();
          if (DataSel) {
            if (!((CVTView*)myVTView)->activeInt)
              ((CVTView*)myVTView)->activeInt = (*(LavaDECL**)DataSel->synEl)->isInSubTree(((CVTView*)myVTView)->myDECL);
            DefTypeSel = (*(LavaDECL**)DataSel->synEl)->DeclType;
            if (ItemSel != GetListView()->firstChild()) {
              ParItemSel = (CTreeItem*)ItemSel->parent();
              ParItemSel = (CTreeItem*)ParItemSel->parent();
              if (ParItemSel)
                ParDataSel = (CMainItemData*)ParItemSel->getItemData();
            }
          }
          else 
            ItemSel = 0;
        }
        else
          ItemSel = 0;
      }
      else { //form
        if ((DataSel->type == TIType_DECL))
          GetDocument()->openWizardView(this, (LavaDECL**)DataSel->synEl/*, (unsigned long)1*/);
        if (!inSync && myFormView && (DataSel->type == TIType_DECL)) {
          //if (ItemSel != GetListView()->firstChild()) 
            ((CLavaGUIView*)myFormView)->SyncForm(*(LavaDECL**)DataSel->synEl);
        }
        else
          inSync = false;
        ItemSel = 0;
      }
      if (this == (CLavaBaseView*)wxDocManager::GetDocumentManager()->GetActiveView())
        SetErrAndCom(item);
    }
    else
      ItemSel = 0;
  }
  else
    ItemSel = 0;
  lastCurrent = (CTreeItem*)selItem;
}

void CLavaPEView::OnVkreturn() 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    TDeclType etype;
    switch (data->type) {
    case TIType_Constraint:
      OnEditSelItem(item, false);
      break;
    case TIType_DECL:
      if (myInclView) {
        etype = (*(LavaDECL**)data->synEl)->DeclType;
        if ((etype == IAttr) || (etype == OAttr))
          etype = Attr;
      }
      else
        etype = FormText;
      OnInsert(etype);
      break;
    case TIType_Features:
    case TIType_Input:
    case TIType_Output:
      if (myInclView) 
        OnInsert(Attr);
      else
        OnInsert(FormText);
      break;
    case TIType_VTypes:
      if (myInclView) 
        OnInsert(VirtualType);
      break;
    case TIType_CHEEnumSel:
    case TIType_EnumItems:
      if (myInclView) 
        OnNewEnumItem();
      break;
    default: ;
    }
  } 
}

void CLavaPEView::OnShowOptionals() 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    if (data->type == TIType_DECL) {
      if ((*(LavaDECL**)data->synEl)->TreeFlags.Contains(hasEmptyOpt)) {
        RemoveEmptyOpt(item, false);
        (*(LavaDECL**)data->synEl)->TreeFlags.EXCL(hasEmptyOpt);
      }
      else {
        if (DrawEmptyOpt(item, false)) {
          (*(LavaDECL**)data->synEl)->TreeFlags.INCL(hasEmptyOpt);
          if (!data->toExpand) 
            ExpandItem(item, 2);
        }
      }
      GetListView()->ensureItemVisible(item);
    }
  }
}


void CLavaPEView::OnUpdateShowOptionals(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    if (data->type == TIType_DECL) 
      if ((*(LavaDECL**)data->synEl)->TreeFlags.Contains(hasEmptyOpt)
         && (!myInclView || (*(LavaDECL**)data->synEl)->DeclType != FormDef)) {
        action->setOn(true);
/*        if (pCmdUI->m_pOther)
          ((CToolBar*)pCmdUI->m_pOther)->GetToolBarCtrl().PressButton(ID_SHOW_OPTIONALS, true);*/
        return;
      }
  }
  action->setOn(false);
  /*if (pCmdUI->m_pOther)
    ((CToolBar*)pCmdUI->m_pOther)->GetToolBarCtrl().PressButton(ID_SHOW_OPTIONALS, false);*/
}


/*
void CLavaPEView::OnChoosefont() 
{
   LOGFONT lf;
   if (((CLavaPEApp*)wxTheApp)->LBaseData.m_lfDefTreeFont.lfHeight == 0) {
     CFont* pFont = GetFont();
     if (pFont != NULL)
       pFont->GetObject(sizeof(LOGFONT), &lf);
     else
       ::GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
   }
   else
     lf = ((CLavaPEApp*)wxTheApp)->LBaseData.m_lfDefTreeFont;
  CFontDialog dlg(&lf, CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT);
  if (dlg.DoModal() == IDOK) {
    // switch to new font.
    ((CLavaPEApp*)wxTheApp)->LBaseData.m_TreeFont.DeleteObject();
    if (((CLavaPEApp*)wxTheApp)->LBaseData.m_TreeFont.CreateFontIndirect(&lf)) {
      SetFont(&((CLavaPEApp*)wxTheApp)->LBaseData.m_TreeFont);
      if (myInclView)  
        myInclView->SetFont(&((CLavaPEApp*)wxTheApp)->LBaseData.m_TreeFont);
      if (myVTView)  
        myVTView->SetFont(&((CLavaPEApp*)wxTheApp)->LBaseData.m_TreeFont);
      ((CLavaPEApp*)wxTheApp)->LBaseData.m_lfDefTreeFont = lf;
      wxTheApp->m_appWindow->m_OutputBar->ChangeFont();
    }
  } 
}
*/

void CLavaPEView::OnComment() 
{
  CTreeItem *pItem, *item = (CTreeItem*)GetListView()->currentItem();
  TDECLComment* ptrComment=0, *ptr = 0;
  DWORD d1, d3 = 0;
  CComment *pComment;
  CMainItemData* data;
  CHE* che;
  bool hasC, hasErr = false;
  CLavaPEHint* hint;

  if (item) {
    data = (CMainItemData*)item->getItemData();
    pComment = new CComment(this, "Comment", true);

    if (data->type == TIType_DECL) {
      ptrComment = (*(LavaDECL**)data->synEl)->DECLComment.ptr;
      d1 = (DWORD)*(LavaDECL**)data->synEl;
      hasErr = ((LavaDECL*)d1)->DECLError1.first || ((LavaDECL*)d1)->DECLError2.first;
    }
    else
      if (data->type == TIType_CHEEnumSel) {
        ptrComment = ((CHEEnumSelId*)data->synEl)->data.DECLComment.ptr;
        d1 = data->synEl;
        pItem = (CTreeItem*)item->parent();
        pItem = (CTreeItem*)pItem->parent();
        pItem = (CTreeItem*)pItem->parent();
        d3 = (DWORD)*(LavaDECL**)((CMainItemData*)item->getItemData())->synEl;
      }
      else
        if (data->type == TIType_Constraint) {
          che = (CHE*)(*(LavaDECL**)data->synEl)->NestedDecls.last;
          hasC = (che != 0);
          if (hasC) 
            hasC =  ((((LavaDECL*)che->data)->DeclDescType == ExecDesc)
                    && ((LavaDECL*)che->data)->Exec.ptr);
          if (hasC) {
            ptrComment = ((LavaDECL*)che->data)->DECLComment.ptr;
            d1 = (DWORD)che->data;
          }
          else
            return;
        }
        else
          return;
    if (ptrComment && ptrComment->Comment.l) {
      //pComment->inline_comment->setChecked(false);
      //pComment->trailing_comment->setChecked(false);
      pComment->trailing_comment->hide();
      pComment->inline_comment->hide();
      //pComment->m_inline = ptrComment->PrintFlags.Contains(inLineComment);
      //pComment->m_trailing = ptrComment->PrintFlags.Contains(trailingComment);
      pComment->text->setText(ptrComment->Comment.c);
    }
    if (pComment->exec() == QDialog::Accepted) {  
      if (pComment->text->text().length()) {
        ptr = new TDECLComment;
        //if (pComment->m_inline)
        //  ptr->PrintFlags.INCL(inLineComment);
        //else
          ptr->PrintFlags.EXCL(inLineComment);
        //if (pComment->m_trailing)
        //  ptr->PrintFlags.INCL(trailingComment);
        //else
          ptr->PrintFlags.EXCL(trailingComment);
        ptr->Comment = STRING(pComment->text->text());
      }
      hint = new CLavaPEHint(CPECommand_Comment, GetDocument(), (const unsigned long)3, d1, (DWORD)ptr, d3);
      GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, false, hint);
     // if (!hasErr)
     //   wxTheApp->m_appWindow->m_wndStatusBar.message(pComment->m_comment);

      delete pComment;
    }
    else
      delete pComment;
  }
  
}

void CLavaPEView::OnUpdateComment(wxAction* action) 
{
  CTreeItem* item = (CTreeItem*)GetListView()->currentItem();
  bool enable = false;
  TDECLComment* ptrComment;

  if (item) {
    CMainItemData* data = (CMainItemData*)item->getItemData();
    if (data->type == TIType_DECL) {
      enable = true;
      ptrComment = (*(LavaDECL**)data->synEl)->DECLComment.ptr;
      if (ptrComment)
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->statusBar()->message(ptrComment->Comment.c);
    }
    else {
      if (data->type == TIType_CHEEnumSel)
        enable = true;
      else 
        if (data->type == TIType_Constraint) {
          LavaDECL* decl = *(LavaDECL**)data->synEl;
          CHE* che = (CHE*)decl->NestedDecls.last;
          enable = (che != 0) && ( ((LavaDECL*)che->data)->DeclDescType == ExecDesc)
                    && ((LavaDECL*)che->data)->Exec.ptr;
        }
    }
  }
  action->setEnabled(enable);
  /*
  if (enable)
    PopupMenu.EnableMenuItem(ID_COMMENT, MF_ENABLED | MF_BYCOMMAND); 
  else
    PopupMenu.EnableMenuItem(ID_COMMENT, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND); 
    */
}

void CLavaPEView::startRename(CTreeItem *item)
{
  if (!item->renameEnabled(0))
    return;
  CMainItemData* itd = (CMainItemData*)item->getItemData();
  LavaDECL* decl;
  QString str;
  if (item->actLab.l) {
    item->setText(0, item->actLab.c);
    item->actLab.Reset(0);
  }
  else {
    item->completeText = item->text(0);
    if (itd->type == TIType_DECL) {
      decl = *(LavaDECL**)itd->synEl;
      item->setText(0, decl->LocalName.c);
    }
    else if (itd->type == TIType_CHEEnumSel)
      item->setText(0, ((CHEEnumSelId*)itd->synEl)->data.Id.c);
  }
}

void CLavaPEView::cancelRename(CTreeItem *item)
{
  item->setText(0, item->completeText);
}

void CLavaPEView::okRename(CTreeItem *item)
{
  CMainItemData *pdata, *itd = (CMainItemData*)item->getItemData();
  LavaDECL *decl, *labelDECL, *ppDECL;
  CLavaPEHint *hint=0;
  CTreeItem *pi;
  CHAINANY* enumItems;
  CHEEnumSelId *relEl;
  QString *err = 0;
  DString *name,  *newLab, *oldLab, lab = DString(item->text(0)) ;

  if (lab.l) {
    if (!((CLavaPEApp*)wxTheApp)->LBaseData.isIdentifier(lab.c)) {
      if ((item != GetListView()->firstChild())
          || (*(LavaDECL**)itd->synEl)->Supports.first)
        err = &IDP_IsNoID;
      else {
        item->setText(0, item->completeText);
        return;
      }
    }
    else {
      if (itd->type == TIType_DECL) {
        labelDECL = *(LavaDECL**)itd->synEl;
        if (lab != labelDECL->LocalName) {
          if (GetDocument()->FindInSupports(lab, labelDECL, labelDECL->ParentDECL))
            err = &ERR_NameInUse;
          else {
            decl = NewLavaDECL();
            *decl = *labelDECL; 
            decl->LocalName = lab;
            if (decl->ParentDECL)
              decl->FullName = decl->ParentDECL->FullName + decl->LocalName;
            else
              decl->FullName = decl->LocalName;
            name = new DString(labelDECL->FullName);  //bisheriger Name
            ppDECL = decl->ParentDECL;
            hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (unsigned long)1, (DWORD)decl, (DWORD)name, 0, itd->synEl);
          }
        }
        else {
          item->setText(0, item->completeText);
          return;
        }
      }
      else
        if (itd->type == TIType_CHEEnumSel) {
          pi = (CTreeItem*)item->parent();
          pi = (CTreeItem*)pi->parent();
          pdata = (CMainItemData*)pi->getItemData();
          enumItems = &((TEnumDescription*)(*(LavaDECL**)pdata->synEl)->EnumDesc.ptr)->EnumField.Items;
          relEl = (CHEEnumSelId*)enumItems->first;
          if (lab != ((CHEEnumSelId*)itd->synEl)->data.Id) {
            while (relEl && (relEl->data.Id != lab))
              relEl = (CHEEnumSelId*)relEl->successor;
            if (relEl)
              err = &ERR_NameInUse;
            else {
              decl = NewLavaDECL();
              *decl = **(LavaDECL**)pdata->synEl;
              relEl = (CHEEnumSelId*)((TEnumDescription*)decl->EnumDesc.ptr)->EnumField.Items.first;
              while (relEl && (relEl->data.Id != ((CHEEnumSelId*)itd->synEl)->data.Id))
                relEl = (CHEEnumSelId*)relEl->successor;
              if (relEl && (relEl->data.Id != lab)) {
                oldLab = new DString(relEl->data.Id);
                newLab = new DString(lab);
                relEl->data.Id = lab;
                ppDECL = decl->ParentDECL;
                name = new DString((*(LavaDECL**)pdata->synEl)->FullName);  //bisheriger Name
                hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (unsigned long)1, (DWORD)decl, (DWORD)name, 0, pdata->synEl, (DWORD)newLab, (DWORD)oldLab);
              }
            }
          }//new label
          else { //do nothing
            item->setText(0, item->completeText);
            return;
          }
        }
    }//isIdentifier
  }//len > 0
  else {
    if ((item == GetListView()->firstChild()) && !(*(LavaDECL**)itd->synEl)->Supports.first) {
      decl = NewLavaDECL();
      *decl = **(LavaDECL**)itd->synEl;
      decl->FullName.Destroy();
      decl->LocalName.Destroy();
      name = new DString((*(LavaDECL**)itd->synEl)->FullName);  //bisheriger Name
      if (name->l) {
        ppDECL = 0;
        hint = new CLavaPEHint(CPECommand_Change, GetDocument(), (unsigned long)1, (DWORD)decl, (DWORD)name, 0, itd->synEl);
      }
    }
    else {
      item->setText(0, item->completeText);
      return;
    }
  }//len == 0
  if (hint) {
    GetDocument()->UndoMem.AddToMem(hint);
    GetDocument()->UpdateDoc(this, false, hint);
    GetDocument()->ConcernForms(hint);
    GetDocument()->ConcernImpls(hint, ppDECL);
    GetDocument()->ConcernExecs(hint);
    if (itd->type == TIType_CHEEnumSel) {
      QApplication::postEvent(this, new QCustomEvent(IDU_LavaPE_SetLastHint));
    }
    else
      GetDocument()->SetLastHint();
  }
  else {
    if (err) {
      QMessageBox::critical(this, qApp->name(),*err,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      item->actLab = lab;
      item->startRename(0);
    }
    else {
      item->setText(0, item->completeText);
    }
  }
}

void CLavaPEView::OnActivateView(bool bActivate, wxView *deactiveView) 
{
  CTreeItem* sel;
  if (GetDocument()->mySynDef) {
    if (bActivate) {
      sel = (CTreeItem*)GetListView()->currentItem();
      SetErrAndCom(sel);
      if (!GetListView()->hasFocus())
        GetListView()->setFocus();
      sel->repaint();
    }
    else
      DisableActions();
  }
}


void CLavaPEView::OnRclick(QListViewItem* itemHit)  
{
  
  if (itemHit) {
    GetListView()->setSelected(itemHit, true);
    //PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, ptAction.x, ptAction.y, this);
  }
}

  
 void CLavaPEView::OnNextEC(CTreeItem* itemStart, bool onErr) 
{
  if (!itemStart)
    return;
  CTreeItem* item2 = itemStart;
  if (((CLavaMainFrame*)wxTheApp->m_appWindow)->OutputBarHidden
      || onErr && (((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->ActTab != tabError)
      || !onErr && (((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->ActTab != tabComment))
    if (PutEC(item2, onErr))
      return;
  
  CTreeItem* item1 = (CTreeItem*)item2->firstChild();
  if (!item1)
    item1 = (CTreeItem*)item2->nextSibling();
  while (!item1 && item2) {
    item2 = (CTreeItem*)item2->parent();
    if (item2)
      item1 = (CTreeItem*)item2->nextSibling();
  }
  if (!item1)
    item1 = (CTreeItem*)GetListView()->firstChild();
  while (item1) {
    if (item1 == itemStart) {
      if (onErr)
          QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      else
          QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      return;
    }
    if (PutEC(item1, onErr))
      return;

    item2 = (CTreeItem*)item1->firstChild();
    while (item2) {
      if (item2 == itemStart) {
        if (onErr)
          QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        else
          QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return;
      }
      if (PutEC(item2, onErr))
        return;

      item1 = item2;
      item2 = (CTreeItem*)item2->firstChild();
    }
    item2 = item1;    
    item1 = (CTreeItem*)item2->nextSibling();
    while (!item1 && item2) {
      item2 = (CTreeItem*)item2->parent();
      if (item2)
        item1 = (CTreeItem*)item2->nextSibling();
    }
    if (!item1) 
      item1 = (CTreeItem*)GetListView()->firstChild();
  }

}


void CLavaPEView::OnPrevEC(CTreeItem* itemStart, bool onErr) 
{
  CTreeItem *item1 = itemStart, *item2 = 0, *item3; 
  while (!item2 && item1) {
    item2 = GetPrevSiblingItem(item1);
    if (!item2) { //if first child then go up to parent
      if (item1 != GetListView()->firstChild()) {
        item1 = (CTreeItem*)item1->parent();
        if (item1 == itemStart) {
          if (onErr)
            QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          else
            QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
          return;
        }
        if (item1) {
          if (PutEC(item1, onErr))
            return;
          item2 = GetPrevSiblingItem(item1);
        }
      }
      else
        item2 = item1;
    }
    item3 = item2;
    while (item3) { //first child ..
      item2 = item3;
      item3 = (CTreeItem*)item2->firstChild();
      item1 = item3;
      while (item1) { //then it's last sibling
        item2 = item1;
        item3 = item1;
        item1 = (CTreeItem*)item1->nextSibling();
      }
    }
    if (item2) {
      item1 = item2;
      if (item1 == itemStart) {
        if (onErr)
          QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        else
          QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return;
      }
      item2 = 0;
      if (PutEC(item1, onErr))
        return;
    }
  }
}

bool CLavaPEView::PutEC(CTreeItem* item, bool onErr)
{
  CMainItemData *dataD = (CMainItemData*)item->getItemData();
  LavaDECL* errDECL;
  if ((dataD->type == TIType_DECL) || (dataD->type == TIType_Constraint)) {
    errDECL = *(LavaDECL**)dataD->synEl;
    if (dataD->type == TIType_Constraint) {
      if (errDECL->NestedDecls.last) {
         errDECL = (LavaDECL*)((CHE*)errDECL->NestedDecls.last)->data;
         if (errDECL->DeclType != ExecDef)
           errDECL = 0;
      }
      else
        errDECL = 0;
    }
    if (errDECL) {
      if (onErr && (errDECL->DECLError1.first || errDECL->DECLError2.first)) {
        GetListView()->ensureItemVisible(item);
        GetListView()->setCurAndSel(item);
        CheckAutoCorr(errDECL);
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetErrorOnBar(errDECL);
        ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetTab(tabError);
        return true;
      }
      else
        if (!onErr && errDECL->DECLComment.ptr) {
          ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetComment(errDECL->DECLComment.ptr->Comment, !errDECL->DECLError1.first && !errDECL->DECLError2.first);
          ((CLavaMainFrame*)wxTheApp->m_appWindow)->m_OutputBar->SetTab(tabComment);
          GetListView()->setCurAndSel(item);
          GetListView()->ensureItemVisible(item);
          return true;
        }
    }
  }
  return false;
}


void CLavaPEView::OnNextError() 
{
  CTreeItem* itemStart = (CTreeItem*)GetListView()->currentItem();
  OnNextEC(itemStart, true);
}

void CLavaPEView::OnPrevError() 
{
  CTreeItem* itemStart = (CTreeItem*)GetListView()->currentItem();
  OnPrevEC(itemStart, true);
}

void CLavaPEView::OnNextComment() 
{
  CTreeItem* itemStart = (CTreeItem*)GetListView()->currentItem();
  OnNextEC(itemStart, false);
  
}

void CLavaPEView::OnPrevComment() 
{
  CTreeItem* itemStart = (CTreeItem*)GetListView()->currentItem();
  OnPrevEC(itemStart, false); 
}

void CLavaPEView::OnLitStr() 
{
  OnInsert(FormText);
}

void CLavaPEView::OnUpdateLitStr(wxAction* action) 
{
  action->setEnabled(!myInclView && (GetListView()->currentItem() != GetListView()->firstChild()));
}

/*
void CLavaPEView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if (nChar == VK_RETURN)
    OnVkreturn();
  else  if (nChar == VK_DELETE)
    OnDelete();
  else
    CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}*/



void CLavaPEView::resizeEvent (QResizeEvent * e)
{
  CTreeView::resizeEvent(e);
}

/*
LRESULT CLavaPEView::OnAutoCorr(WPARAM wparam, LPARAM lparam)
{
  LavaDECL* decl = GetDocument()->IDTable.GetDECL(0, wparam, 0);
  if (decl)
    GetDocument()->AutoCorr(decl);
  return 0;
}
*/

void CLavaPEView::CheckAutoCorr(LavaDECL* decl)
{
  CHE *che = (CHE*)decl->DECLError1.first;
  while (che) {
    if (((CLavaError*)che->data)->showAutoCorrBox)  {
      //PostMessage(MESS_AUTOCORR, decl->OwnID);
      GetDocument()->AutoCorr(decl);
      return;
    }
    if (che == decl->DECLError1.last)
      che = (CHE*)decl->DECLError2.first;
    else
      che = (CHE*)che->successor;
  }
}

