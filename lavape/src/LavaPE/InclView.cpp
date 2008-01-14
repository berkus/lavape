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


#include "LavaPE.h"
#include "SynIO.h"
#include "InclView.h"
#include "SylTraversal.h"
#include "LavaPEFrames.h"
#include "qfileinfo.h"
//Added by qt3to4:
#include <QPixmap>
#include "Boxes.h"
#include "qdir.h"
#include "qpixmapcache.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "LavaBaseStringInit.h"
#include "LavaPEStringInit.h"
#include "LavaAppBase.h"

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
// CInclView

CInclView::CInclView(QWidget* parent, wxDocument *doc)
: CTreeView(parent, doc, "InclView")
{
  InitComplete = false;
  connect(Tree,SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(OnSelchanged(QTreeWidgetItem*, QTreeWidgetItem*)));
  connect(Tree,SIGNAL(itemDoubleClicked( QTreeWidgetItem *, int  )), SLOT(OnDblclk( QTreeWidgetItem *, int  )));
  setFont(LBaseData->m_TreeFont);
//  new InclWhatsThis(Tree);
  Tree->setSelectionMode(QAbstractItemView::SingleSelection);
  Tree->viewport()->installEventFilter(this);
}

CInclView::~CInclView()
{
	setFocusProxy(0);
}

void CInclView::DisableActions()
{
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  frame->editSelItemAction->setEnabled(false);
  frame->newIncludeAction->setEnabled(false);
}



CLavaPEDoc* CInclView::GetDocument() // non-debug version is inline
{
  return (CLavaPEDoc*)m_viewDocument;
}

void CInclView::UpdateUI()
{
  if (!InitComplete)
    return;
  CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
  OnUpdateDelete(frame->deleteAction);
  OnUpdateEditSel(frame->editSelItemAction);
}


void CInclView::OnInitialUpdate()
{
  CLavaPEDoc* pDoc = GetDocument();
  if (pDoc && pDoc->mySynDef) {
    Expanded = true;
    OnUpdate(NULL, 0, 0);
    Tree->expandItem(Tree->RootItem);//firstChild()->setOpen(true);
    Tree->setCurrentItem(Tree->RootItem);
    Tree->RootItem->setSelected(true);
    InitComplete = true;
  }
}

void CInclView::OnUpdate(wxView* pSender, unsigned lHint, QObject* pHint)
{
  CTreeItem* parent, *item;
  DString lab;
  CHESimpleSyntax* cheSyn;
  int bm = lavafileBM;//QPixmapCache::find("l_lavafile");
  CLavaPEDoc *doc;
  bool hasErr= false;

  if (pHint && ( ((CLavaPEHint*)pHint)->com != CPECommand_Include)
     && ( ((CLavaPEHint*)pHint)->com != CPECommand_Exclude)
     && ( ((CLavaPEHint*)pHint)->com != CPECommand_ChangeInclude)
     && ( ((CLavaPEHint*)pHint)->com != CPECommand_FromOtherDoc)
     && ( ( ((CLavaPEHint*)pHint)->com != CPECommand_Change)
          || ((CLavaPEHint*)pHint)->CommandData1 ))
    return;
  parent = (CTreeItem*)Tree->RootItem;
  if (parent) {
    Expanded = parent->isExpanded();//parent->isOpen();
    delete Tree->RootItem;//DGetListView()->DeleteAllItems();
    Tree->RootItem = 0;
  }
  doc = GetDocument();
  parent = InsertItem(doc->IDTable.IDTab[0]->FileName.c, bm, MY_TVI_ROOT);
    //TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE,
    //                                doc->IDTable.IDTab[0]->FileName.c,
    //                               bm, bm, TVIS_EXPANDED, TVIS_EXPANDED,
    //                                0, MY_TVI_ROOT, MY_TVI_LAST);
  cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first;
  parent->setItemData( (TItemData*)cheSyn);
  if (cheSyn)
    cheSyn = (CHESimpleSyntax*)cheSyn->successor;
  while (cheSyn) {
    lab = cheSyn->data.UsersName;
    if (cheSyn->data.LocalTopName.l)
      lab = lab + DString(" used as ") + cheSyn->data.LocalTopName;
    item = InsertItem(lab.c, bm, parent, MY_TVI_LAST);
    item->setItemData( (TItemData*)cheSyn );
    if (cheSyn->data.notFound) {
      item->SetItemMask(true, false);
      //Tree->SetItemState(item, INDEXTOOVERLAYMASK(2), TVIS_OVERLAYMASK );
      hasErr = true;
    }
    cheSyn = (CHESimpleSyntax*)cheSyn->successor;
  }
  if (hasErr) {
    Tree->expandItem(Tree->RootItem);//child(0)->setOpen(true);
    ((CTreeFrame*)GetParentFrame())->CalcSplitters(false, true);
  }
  else
    if (Expanded)
      Tree->expandItem(Tree->RootItem);//->setOpen(Expanded);
    else
      Tree->collapseItem(Tree->RootItem);
  Tree->update();

}


void CInclView::OnNewInclude()
{
  QString strFilter, fileExt, title;
  DString *pfn, *pUsers, openfilename;
  CHESimpleSyntax* cheSyn;
  CLavaPEHint* hint;
  QFileInfo finfo;
  int ii;
  unsigned long firstLast = 1;
  QStringList fileNames = L_GetOpenFileNames(GetDocument()->IDTable.DocDir.c, this,
				                                      "Select additional include files", "Lava.File (*.lava)");

  if (fileNames.isEmpty())
    return;
  for ( ii = 0; ii < fileNames.count(); ii++) {
    wxTheApp->SetLastFileOpen(fileNames[ii]);
    finfo.setFile(wxTheApp->GetLastFileOpen());
    openfilename = qPrintable(ResolveLinks(finfo));
    pfn = new DString(openfilename);
    pUsers = new DString(qPrintable(wxTheApp->GetLastFileOpen()));
    //RelPathName(rpfn, GetDocument()->IDTable.DocDir);
    for (cheSyn = (CHESimpleSyntax*)GetDocument()->mySynDef->SynDefTree.first;
         cheSyn && !SameFile(cheSyn->data.SyntaxName, GetDocument()->IDTable.DocDir,openfilename);
            //is the mySynDef really new?
         cheSyn = (CHESimpleSyntax*)cheSyn->successor);
    if (!cheSyn || !cheSyn->data.TopDef.ptr) {
      hint = new CLavaPEHint(CPECommand_Include, GetDocument(), firstLast,  cheSyn, pfn, GetDocument()->mySynDef->SynDefTree.last, pUsers);
      GetDocument()->UndoMem.AddToMem(hint);
      if (GetDocument()->UpdateDoc(this, FALSE, hint))
        firstLast = 0;
    }
    else
      if (firstLast == 1)
        QMessageBox::critical(this, qApp->applicationName(),IDP_AlreadyIncluded,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
 }
 if (!firstLast)
   GetDocument()->SetLastHint();
}

void CInclView::OnDelete()
{
  int in;
  QString errStr;
  if (GetDocument()->changeNothing)
    return;
  CTreeItem* item = (CTreeItem*)Tree->currentItem();
  if (item && (item != Tree->RootItem)) {
    CHESimpleSyntax* cheSyn = (CHESimpleSyntax*)item->getItemData();
    if (cheSyn->data.nINCL == 1) {
      QMessageBox::critical(this, qApp->applicationName(),ERR_StdNotRemovable, QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
      return;
    }
    in = GetDocument()->IDTable.IsFileInherited(cheSyn->data.SyntaxName);
    if (in) {
      errStr = IDP_InclInherited + QString(GetDocument()->IDTable.IDTab[in]->FileName.c);
      QMessageBox::critical(this, qApp->applicationName(), errStr, QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
      return;
    }
    else {
      DString *pfn = new DString(cheSyn->data.SyntaxName);
      CLavaPEHint* hint = new CLavaPEHint(CPECommand_Exclude, GetDocument(), (const unsigned long)3, cheSyn, pfn, cheSyn->predecessor);
      GetDocument()->UndoMem.AddToMem(hint);
      GetDocument()->UpdateDoc(this, FALSE);
    }
  }
}


void CInclView::OnDblclk( QTreeWidgetItem * item, int col )
{
 //CTreeItem* item = (CTreeItem*)Tree->itemAtIndex(index);
 if (item && (item != Tree->RootItem)) {
    DString *fn = new DString(((CHESimpleSyntax*)((CTreeItem*)item)->getItemData())->data.UsersName); //.SyntaxName);
    QApplication::postEvent(this, new CustomEvent(UEV_LavaPE_CalledView,(void*)fn));

  }
}

void CInclView::customEvent(QEvent *ev0)
{
  CustomEvent *ev=(CustomEvent*)ev0;

  if (ev->type() == UEV_LavaPE_CalledView) {
    DString* usersFn = (DString*)ev->data();
    QDir dir(GetDocument()->IDTable.DocDir.c);
	  QFileInfo fi(dir, usersFn->c);
    QString fn = ResolveLinks(fi), abs_fn;
#ifdef WIN32
    QString driveLetter = QString(fn[0].toUpper());
    fn.replace(0,1,driveLetter);
#endif
    CLavaPEDoc* doc = (CLavaPEDoc*)wxDocManager::GetDocumentManager()->FindOpenDocument(fn);
    //doc->SetUserFilename( fn);
    delete usersFn;
    if (doc) {
      doc->MainView->ActivateView(true);
      doc->SetTitle( fi.absoluteFilePath());
			abs_fn = fi.absoluteFilePath();
      doc->MainView->GetParentFrame()->SetTitle(abs_fn);
    }
  }
}

void CInclView::OnUpdateDelete(QAction* action)
{
  CTreeItem* item = (CTreeItem*)Tree->currentItem();
  action->setEnabled((!GetDocument()->changeNothing) &&
                     item && (item != Tree->RootItem));
//                 && !((CHESimpleSyntax*) item->getItemData())->data.Inherited);
}

void CInclView::OnEditSel()
{
  CLavaPEHint* hint;
  CHESimpleSyntax *che, *newChe, *cheSyn;
  DString newAbsName, oldAbsName, newRelName, oldRelName, docAbsName;
  CLavaPEDoc *doc, *nDoc;
  int pos;//, prevPos;
  //wxDocTemplate* pTemplate;
  SynFlags firstlast;
  bool multi = false;

  CTreeItem* item = (CTreeItem*)Tree->currentItem();
  if (item && (item != Tree->RootItem)) {
    che = (CHESimpleSyntax*)item->getItemData();
    if (che->data.nINCL != 1) {
      newChe = new CHESimpleSyntax;
      newChe->data = che->data;
      newChe->data.notFound = false;
      CIncludeBox* box = new CIncludeBox(GetDocument(), newChe, che, this);
      box->setWindowFlags(box->windowFlags() ^ Qt::WindowContextHelpButtonHint);
      if (box->exec() == QDialog::Accepted) {
        firstlast.INCL(firstHint);
        hint = new CLavaPEHint(CPECommand_ChangeInclude, GetDocument(), firstlast,  newChe, 0, che->predecessor);
        if (newChe->data.UsersName != che->data.UsersName) {
          //find all open docs and included files which include the changed file
          //if any then make a multi doc hint
          newAbsName = newChe->data.SyntaxName;
          AbsPathName(newAbsName, GetDocument()->IDTable.DocDir);
          oldAbsName = che->data.SyntaxName;
          AbsPathName(oldAbsName, GetDocument()->IDTable.DocDir);
          ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = true;
          wxDocManager* mana = wxDocManager::GetDocumentManager();
          //pos = mana->GetFirstDocPos();
          //while (pos) {
          for (pos = 0; pos < mana->m_docs.size(); pos++) {
            //prevPos = pos;
            doc = (CLavaPEDoc*)mana->m_docs[pos];
            oldRelName = oldAbsName;
            RelPathName(oldRelName, doc->IDTable.DocDir);
            if (doc == GetDocument()) {
              cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first->successor;
              for ( ;cheSyn; cheSyn = (CHESimpleSyntax*)cheSyn->successor) {
                if (doc->IDTable.IsFileInherited(oldRelName, cheSyn->data.nINCL)) {
                  docAbsName = cheSyn->data.SyntaxName;
                  AbsPathName(docAbsName, ((CLavaPEDoc*)doc)->IDTable.DocDir);
                  nDoc = (CLavaPEDoc*)mana->FindOpenDocument(docAbsName.c);
                }
              }
            }
            else {
              if (!doc->hasHint) {
                cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first;
                if (cheSyn) {
                  cheSyn = (CHESimpleSyntax*)cheSyn->successor; //is fromDoc included?
                  for ( ;cheSyn && !SameFile(cheSyn->data.SyntaxName, doc->IDTable.DocDir, oldRelName, doc->IDTable.DocDir);
                         cheSyn = (CHESimpleSyntax*)cheSyn->successor);
                  if (cheSyn) {
                    newRelName = newAbsName;
                    RelPathName(newRelName, doc->IDTable.DocDir);
                    CHESimpleSyntax* newChe = new CHESimpleSyntax;
                    newChe->data = cheSyn->data;
                    newChe->data.notFound = false;
                    newChe->data.SyntaxName = newRelName;
                    if (!multi) {
                      ((CLavaPEApp*)wxTheApp)->LBaseData.MultiUndoMem.StartMultiDocUpdate();
                      GetDocument()->hasHint = true;
                      hint->FirstLast.INCL(multiDocHint);
                      GetDocument()->UndoMem.AddToMem(hint);
                      GetDocument()->UpdateDoc(this, FALSE, hint);
                      firstlast.INCL(multiDocHint);
                      multi = true;
                      ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = true;
                    }
                    hint = new CLavaPEHint(CPECommand_ChangeInclude, doc, firstlast,  newChe, 0, cheSyn->predecessor);
                    doc->hasHint = true;
                    doc->UndoMem.AddToMem(hint);
                    doc->UpdateDoc(this, FALSE, hint);
                    cheSyn = (CHESimpleSyntax*)doc->mySynDef->SynDefTree.first->successor;
                    for ( ;cheSyn; cheSyn = (CHESimpleSyntax*)cheSyn->successor) {
                      if (doc->IDTable.IsFileInherited(oldRelName, cheSyn->data.nINCL)) {
                        docAbsName = cheSyn->data.SyntaxName;
                        AbsPathName(docAbsName, ((CLavaPEDoc*)doc)->IDTable.DocDir);
                        nDoc = (CLavaPEDoc*)mana->FindOpenDocument(docAbsName.c);
                      }
                    }
                  }
                }
              }
            }
          }
        }
        ((CLavaPEApp*)wxTheApp)->LBaseData.inMultiDocUpdate = false;
        if (multi)
          GetDocument()->SetLastHints(false, false);
        else {
          GetDocument()->UndoMem.AddToMem(hint);
          GetDocument()->UpdateDoc(this, FALSE, hint);
        }

      }
      else
        delete newChe;
      delete box;
    }
  }
}

void CInclView::OnUpdateEditSel(QAction* action)
{
  CTreeItem* item = (CTreeItem*)Tree->currentItem();
  bool enable = (item && (item != Tree->RootItem));
  if (enable) {
    CHESimpleSyntax* che = (CHESimpleSyntax*)item->getItemData();
    enable = che->data.nINCL != 1;
  }
  action->setEnabled(enable);
}

/*
void CInclView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == VK_DELETE)
    OnDelete();
  else
    CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/

void CInclView::OnActivateView(bool bActivate, wxView *deactiveView)
{
  DString str0;
  if (GetDocument()->mySynDef) {
    CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;
    if (bActivate) {
      active = true;
      frame->newIncludeAction->setEnabled(!GetDocument()->changeNothing);
      frame->m_UtilityView->SetComment(str0, true);
      frame->m_UtilityView->ResetError();
      if (!Tree->hasFocus())
        Tree->setFocus();
      wxTheApp->updateButtonsMenus();
    }
    else {
      active = false;
      DisableActions();
    }
  }
}

void CInclView::OnSelchanged(QTreeWidgetItem* selItem, QTreeWidgetItem* )
{
  if (!selItem)
    return;
  if (!selItem->isSelected()) 
    return;
  wxTheApp->updateButtonsMenus();
}

void CInclView::on_whatNextAction_triggered()
{
  QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),tr("\"What next?\" help not yet available for the include view"),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
}

bool CInclView::event(QEvent *ev) {
  QWhatsThisClickedEvent *wtcEv;
  QString href;

  if (ev->type() == QEvent::WhatsThisClicked) {
    wtcEv = (QWhatsThisClickedEvent*)ev;
    href = wtcEv->href();
    ShowPage(QString("whatsThis/")+href);
    return true;
  }
  else
    return QWidget::event(ev);
}

bool CInclView::eventFilter(QObject *, QEvent *ev) {
  QHelpEvent *hev;

  if (ev->type() == QEvent::WhatsThis) {
    hev = (QHelpEvent*)ev;
    setWhatsThis(text(hev->pos()));
    return QWidget::event(ev);
  }
  else
    return false;
}

QString CInclView::text(const QPoint &point) {
  CTreeItem *item=(CTreeItem*)Tree->itemAt(point);

  if (item == Tree->RootItem)
    return QString(QObject::tr("This is the current <b><i><font color=red>Lava</font></i></b> file; it may <a href=\"../Packages.htm#include\">include</a> other <b><i><font color=red>Lava</font></i></b> files"));
  else
    return QString(QObject::tr("This is an <a href=\"../Packages.htm#include\">included</a> <b><i><font color=red>Lava</font></i></b> file"));
}
