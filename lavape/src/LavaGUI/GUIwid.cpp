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


#include "MACROS.h"
#include "SYSTEM.h"
#include "GUIwid.h"
#include "GUIProgBase.h"
#include "LavaMenu.h"
#include "FormWid.h"
#include "TButton.h"
#include "TComboBox.h"
#include "TEdit.h"
#include "TStatic.h"
#include "mdiframes.h"
#include "LavaGUIView.h"
#include "LavaGUIPopup.h"
#include "GUIProg.h"
#include <QFrame>
#include <QTextCursor>

#pragma hdrstop


void GUIwidCLASS::ReallyDestroyWidget(QWidget* widget)
{
  delete widget;
}

void GUIwidCLASS::PopupWindow (QWidget* widget)
{
  widget->show();
}

void GUIwidCLASS::PopdownWindow (QWidget* widget)
{
  widget->hide();
}

void GUIwidCLASS::CreatePopupWindow(bool isTextPopup, QWidget*& popupShell, QWidget*& pane, DString& label, CHEFormNode* data)
{
  DString str = ("TextPopupwidget");
  QWidget* parent;
  if (!popupShell) {
    if (GUIProg->isView) {
      parent = ((wxMainFrame*)wxTheApp->m_appWindow)->GetClientWindow();
      popupShell = new CLavaGUIPopup(parent, GUIProg, data);
    }
    else {
      parent = wxTheApp->m_appWindow;
      popupShell = new CLavaGUIPopupD(parent, GUIProg, data);
    }
  }
  QString cap = GUIProg->winCaption();
  if (label.l)
    cap = cap + " - " + QString(label.c);
  popupShell->setWindowTitle(cap);
  if (GUIProg->isView) 
    if (isTextPopup) 
      CreateFormWidget(pane, ((CLavaGUIPopup*)popupShell)->view->qvbox, 0, str, data);
    else
      pane = ((CLavaGUIPopup*)popupShell)->view->qvbox;
  else
    if (isTextPopup) 
      CreateFormWidget(pane, ((CLavaGUIPopupD*)popupShell)->view->qvbox, 0, str, data);
    else
      pane = ((CLavaGUIPopupD*)popupShell)->view->qvbox;

}

void GUIwidCLASS::AddOptionMenuItem (QWidget*& pos, QWidget* menu, DString& text, CHEFormNode* data)
{ 
  DString str;
  if (!text.l)
    str = DString(" ");
  else
    str = text;
  pos = (QWidget*)((CTComboBox*)menu)->count();
  ((CTComboBox*)menu)->AddItem (str.c, data);
  ((CTComboBox*)menu)->isEnter = (data != 0);
}


void GUIwidCLASS::AddPopupMenuItem (QWidget*& pos, QWidget* menu, DString& text, CHEFormNode* data)
{ 
  DString str;
  int ipos;

  if (!text.l)
    str = DString(" ");
  else
    str = text;
  if (((CLavaMenu*)menu)->myMenu) {
    ipos = ((CLavaMenu*)menu)->myMenu->actions().count();
    ((CLavaMenu*)menu)->myMenu->addAction(str.c);
    if (data)
     ((CLavaMenu*)menu)->SetItemData(ipos, data);
    ((CLavaMenu*)menu)->isEnter = (data != 0);
    pos = (QWidget*)ipos;
  }
}


void GUIwidCLASS::CreateFormWidget (QWidget*& formWidget, QWidget* parentWidget, int box, DString& widgetName, CHEFormNode* data)

{
  formWidget = new CFormWid(GUIProg, data, parentWidget, box, widgetName.c);
}

void GUIwidCLASS::CreateTextWidget (QWidget*& textWidget, QWidget* parentWidget, DString& widgetName,
                                    int rows, int cols,
                                    bool echo, CHEFormNode* data)

{
  QSize size(cols, rows);
  if (rows > 1)
    textWidget = new CMultiLineEdit(GUIProg, data, parentWidget, widgetName.c, size, echo);
  else
    textWidget = new CTEdit(GUIProg, data, parentWidget, widgetName.c, size, echo);
}
  

void GUIwidCLASS::CreateLabelWidget (QWidget*& labelWidget, QWidget* parentWidget, DString& text,
                                     DString& widgetName, CHEFormNode* data)
{
  labelWidget = new CTStatic(GUIProg, data, parentWidget, text.c, widgetName.c);
}


void GUIwidCLASS::CreateToggleWidget (QWidget*& toggleWidget, QWidget* parentWidget, DString& widgetName, DString& label, bool leftLabel, CHEFormNode* data)
{
  if (!widgetName.l) widgetName = DString("booleanField");
  toggleWidget = new CToggleButton(GUIProg, data, parentWidget, widgetName.c, label.c, leftLabel);
}


void GUIwidCLASS::CreateButtonMenuButton (QWidget*& toggleWidget, QWidget* parentWidget, DString& widgetName,
                             DString& text, QWidget*& radioBox,
                             bool enterFlag, CHEFormNode* node)
{
  if (!widgetName.l)
    if (enterFlag)
      widgetName = DString("PushButton");
    else
      widgetName = DString("RadioButton");
  Q_ASSERT(radioBox);
  if (((CFormWid*)radioBox)->myFormNode->data.BasicFlags.Contains(RadioButtons)) 
    toggleWidget =  new CRadioButton(GUIProg, node, parentWidget, widgetName.c, text.c,
                                          radioBox, ((CFormWid*)radioBox)->nRadio+1);
  else 
    if (node->data.Pixmap && node->data.Pixmap)
      toggleWidget =  new CPushButton(true, GUIProg, node, parentWidget, widgetName.c, text.c,
                                           radioBox, ((CFormWid*)radioBox)->nRadio+1);
    else
      toggleWidget =  new CPushButton(GUIProg, node, parentWidget, widgetName.c, text.c,
                                           radioBox, ((CFormWid*)radioBox)->nRadio+1);
  ((CFormWid*)radioBox)->AddRadio((QPushButton*)toggleWidget);
}



void GUIwidCLASS::CreateEllipsisWidget (QWidget*& ellipsisWidget, QWidget* parentWidget,
                           DString& widgetName, DString& label, unsigned cmnd, CHEFormNode* data)

{
  if (!label.l)
    label = DString("...");
  if (!widgetName.l)
    widgetName = DString("EllipsisButton");
  ellipsisWidget = new CPushButton(GUIProg, data, parentWidget, widgetName.c, label.c , cmnd);
}


void GUIwidCLASS::CreatePopupWidget (QWidget*& popupWidget, QWidget* parentWidget, DString& widgetName,
                        DString& label, CHEFormNode* data)
{
  if (!label.l)
    label = DString("-->");
  if (!widgetName.l) 
    widgetName = DString("PopupButton");
  popupWidget = new CPushButton(GUIProg, data, parentWidget, widgetName.c, label.c, WINPOPUP);
}


void GUIwidCLASS::CreateOptionMenuWidget (QWidget*& widget, QWidget*& menu, QWidget* parentWidget, DString& widgetName,
                                          DString& defaultSel, unsigned width, CHEFormNode* data)
{
  if (!widgetName.l) 
    widgetName = DString("menuButton");
  widget = new CTComboBox(GUIProg, data, parentWidget, widgetName.c, defaultSel, width);
  menu = widget;
}

void GUIwidCLASS::CreatePopupMenuWidget (QWidget*& widget,QWidget*& popupMenu,QWidget* parentWidget,
                       DString& widgetName, DString& label, unsigned width, CHEFormNode* data)
{

  if (!widgetName.l)
    widgetName = DString("menuButton");
      
  if (!label.l)
    label = DString(" ");
  widget = new CLavaMenu(GUIProg, data, parentWidget, widgetName.c, label.c, width);
  popupMenu = widget;
}

void GUIwidCLASS::SetText (QWidget* widget, DString& text)
{
  if (!widget)
    return;
  if (widget->inherits( "CTEdit")) {
    ((CTEdit*)widget)->setModified(false);
    if (text.l)
      ((CTEdit*)widget)->setText(text.c);
  }
  else if (widget->inherits( "CMultiLineEdit")) {
    ((CMultiLineEdit*)widget)->document()->setModified(false);
    if (text.l)
      ((CMultiLineEdit*)widget)->setPlainText(text.c);
  }
}


void GUIwidCLASS::SetInsertPos (QWidget* widget, unsigned pos)
{
  if (!widget->inherits("CTEdit") && !widget->inherits("CMultiLineEdit"))
    return;
  if (widget->inherits( "CTEdit"))
    ((CTEdit*)widget)->setCursorPosition(pos);
}


void GUIwidCLASS::SetTopLeft (QWidget* widget, QWidget* leftNeighbour, int horizDist,
                              QWidget* upperNeighbour,int vertDist)
{
  //calculates the position of a child window relative to its parent frame (a CFormWid-window)
  //and recalculates the size of the parent frame and grandparent widget
  
  QWidget* parent = widget->parentWidget();
  if (widget->inherits("GUIVBox"))
    return;
  QRect wrect, neighbourRect;
  QSize size;
  int x, y, disx, disy, bord, pbordx=0, pbordy=0;
  wrect = widget->geometry(); //calc the displacement of the widget
  if (widget->parentWidget()->inherits("QGroupBox")) {
    pbordx = 1+ GUIProg->xMargin;
      //((QFrame*)widget->parentWidget())->lineWidth() + GUIProg->xMargin;
    size = GUIProg->CalcStringRect(((QGroupBox*)widget->parentWidget())->title(),
                                     ((QGroupBox*)widget->parentWidget())->font());
      pbordy = size.height();
  }
  else 
    if (widget->parentWidget()->inherits("QFrame")) {
      pbordx = ((QFrame*)widget->parentWidget())->lineWidth();
      pbordy = ((QFrame*)widget->parentWidget())->lineWidth();
      if (widget->parentWidget()->inherits("GUIVBox")) {
        pbordx += GUIProg->xMargin;
        pbordy += GUIProg->yMargin;
      }
    }
  /*
  if (widget->inherits("QFrame"))
    bord = ((QFrame*)widget)->lineWidth();
  else
    bord = 0;
    */
  bord = GUIProg->GetLineWidth(widget);
  disx = wrect.left();
  disy = wrect.top();
  if(leftNeighbour)
    if (upperNeighbour)
      return;
    else {
      neighbourRect = leftNeighbour->geometry();
      x = neighbourRect.right() + 1 + disx + GUIProg->aveCharWidth(widget->font()) * horizDist;
      y = disy + vertDist * GUIProg->itemHeight(widget->font()) + pbordy; 
    }
  else
    if (upperNeighbour) {
      neighbourRect = upperNeighbour->geometry();
      x = disx + pbordx + horizDist * GUIProg->aveCharWidth(widget->font());
      y = neighbourRect.bottom() + 1 +  disy + vertDist * GUIProg->itemHeight(widget->font());
    }
    else {  
      x = disx + pbordx + horizDist * GUIProg->aveCharWidth(widget->font());
      y = disy + pbordy + vertDist * GUIProg->itemHeight(widget->font());
   }
  //now set the final coordinates of the widget relative to its parent
  widget->move(x, y);
  
}

void GUIwidCLASS::GrowParent(QWidget* widget)
{
  //calculates the position of a child window relative to its parent frame (a CFormWid-window)
  //and recalculates the size of the parent frame and grandparent widget
  
  QRect rect, wrect, newParentRect;
  QSize sz, wsz, strSz;
  QScrollArea* view;
  QWidget *grandparent, *parent;
  QGroupBox *gb;
  int bord, wid;
  parent = widget->parentWidget();
  if (!parent->inherits("GUIVBox")) {
    wrect = widget->geometry();
    wsz = widget->size();
    if (parent->inherits("QGroupBox")) {
      gb = (QGroupBox*)parent;
      sz = gb->size();
      strSz = GUIProg->CalcStringRect(gb->title(), gb->font()) + GUIProg->CalcTextRect(2,1, gb->font());
      bord = 1;//((QFrame*)gb)->lineWidth();
      wid = lmax(sz.width(), wrect.left()+wsz.width()+2*bord + (int)GUIProg->xMargin);
      wid = lmax(wid, strSz.width()+2*bord );
      gb->resize(wid, lmax(sz.height(), wrect.top()+wsz.height()+2*bord + (int)GUIProg->yMargin));
      wrect = gb->geometry();
      wsz = gb->size();
      parent = gb->parentWidget();
    }
    sz = parent->size();
    bord = GUIProg->GetLineWidth(parent); //((QFrame*)parent)->lineWidth();
    if (parent->parentWidget()->inherits("GUIVBox")) 
      parent->resize(lmax(sz.width(), wrect.left()+wsz.width()+2*bord + (int)GUIProg->xMargin), lmax(sz.height(), wrect.top()+wsz.height()+2*bord + (int)GUIProg->yMargin));
    else
      parent->resize(lmax(sz.width(), wrect.left()+wsz.width()+2*bord), lmax(sz.height(), wrect.top()+wsz.height()+2*bord));
    newParentRect = parent->geometry();
    grandparent = parent->parentWidget();
    if (!grandparent->inherits("GUIVBox")) {
      rect = parent->geometry(); 
      wsz = parent->size();
      if (grandparent->inherits("QGroupBox")) {
        gb = (QGroupBox*)grandparent;
        rect = parent->geometry();
        wsz = parent->size();
        sz = gb->size();
        strSz = GUIProg->CalcStringRect(gb->title(), gb->font()) + GUIProg->CalcTextRect(2,1, gb->font());
        bord = 1;//((QFrame*)gb)->lineWidth();
        wid = lmax(sz.width(), rect.left()+wsz.width()+2*bord + (int)GUIProg->xMargin);
        wid = lmax(wid, strSz.width()+2*bord);
        gb->resize(wid, lmax(sz.height(), rect.top()+wsz.height() + (int)GUIProg->yMargin));
        rect = gb->geometry();
        wsz = gb->size();
        grandparent = gb->parentWidget();
      }
      bord = GUIProg->GetLineWidth(grandparent);//((QFrame*)grandparent)->lineWidth();
      sz = grandparent->size();
      if (grandparent->parentWidget()->inherits("GUIVBox")) 
        grandparent->resize(lmax(sz.width(), rect.left()+wsz.width()+2*bord + (int)GUIProg->xMargin), lmax(sz.height(), rect.top()+wsz.height()+2*bord + (int)GUIProg->yMargin));
      else
        grandparent->resize(lmax(sz.width(), rect.left()+wsz.width()+2*bord), lmax(sz.height(), rect.top()+wsz.height()+2*bord));
      newParentRect = grandparent->geometry();
    }
    else if (((GUIVBox*)grandparent)->FromPopup) {
      rect = parent->geometry(); 
      wsz = parent->size();
      bord = GUIProg->GetLineWidth(grandparent) + GUIProg->globalIndent;
      grandparent->resize(rect.left()+wsz.width()+2*bord, rect.top()+wsz.height()+2*bord);
      view = (QScrollArea*)grandparent->parent();
      view->resize(rect.left()+wsz.width()+2*bord, rect.top()+wsz.height()+2*bord);
    }
    rect = *pMaxBottomRight;
    *pMaxBottomRight = rect.unite(newParentRect);
  }
}

void GUIwidCLASS::SetLabel (QWidget* widget, DString& label)
{
  SetText(widget, label);
}

void GUIwidCLASS::SetOptionDefault (QWidget* menuWidget, const QWidget* entry, DString& labelText)
{
  int ientry = int(entry);
  QAbstractItemModel *mod=((CTComboBox*)menuWidget)->model();
  QModelIndexList matches;

  if (!menuWidget->inherits("CTComboBox"))
    return;
  if (ientry) 
    ((CTComboBox*)menuWidget)->setCurrentIndex(ientry);
  else {
    matches = mod->match(mod->index(0,0),Qt::DisplayRole,QVariant(labelText.c),1,Qt::MatchFlags(Qt::MatchCaseSensitive|Qt::MatchExactly));
    if (!matches.isEmpty()) 
      ((CTComboBox*)menuWidget)->setItemText(((CTComboBox*)menuWidget)->currentIndex(),mod->data(matches.first()).toString());
    else
      ((CTComboBox*)menuWidget)->setCurrentIndex(0);
  }
}  

void GUIwidCLASS::SetToggleState (QWidget* widget, bool state)
{
  if (widget->inherits("CToggleButton"))
    ((CToggleButton*)widget)->setChecked(state);
  else if (widget->inherits("CRadioButton"))
    ((CRadioButton*)widget)->setChecked(state);
}

void GUIwidCLASS::ClearFocus ()
{
  if (GUIProg->newFocus)
    GUIProg->newFocus->clearFocus();
  GUIProg->oldFocus = 0;
  GUIProg->newFocus = 0;
}

void GUIwidCLASS::SetFocus (QWidget*& oldWidget,QWidget* newWidget)
{
  GUIProg->newFocus = newWidget;
  if (oldWidget)
    GUIProg->oldFocus = oldWidget;
  if (GUIProg->newFocus)
    GUIProg->newFocus->setFocus();
}

void GUIwidCLASS::SetSensitive (QWidget* widget, int on)
{
  widget->setEnabled(on != 0);
}

void GUIwidCLASS::SetPointer (QWidget* newWidget, unsigned isWindow)
{ 
  int len;
  if (isWindow) {
    if (newWidget->inherits("CTEdit")) {
      len = ((CTEdit*)newWidget)->text().length();
      ((CTEdit*)newWidget)->setCursorPosition(len);
      return;
    }
    if (newWidget->inherits("CMultiLineEdit")) {
      QTextCursor tc=((CMultiLineEdit*)newWidget)->textCursor();
      tc.movePosition(QTextCursor::End);
      ((CMultiLineEdit*)newWidget)->setTextCursor(tc);
    }
  }
}



