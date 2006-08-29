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


#include "DIO.h"

#include "MACROS.h"
#include "GUIwid.h"
#include "FormWid.h"
#include "GUIProg.h"
#include "LavaGUIView.h"
#include "MakeGUI.h"
#include "docview.h"
#include "mdiframes.h"
#include "LavaGUIPopup.h"
#include "qlayout.h"

#pragma hdrstop


void MakeGUICLASS::DisplayScreen (bool suppressed)
{
  CHEFormNode* suffix, *formnode, *firstnode;
  bool empty;
  DString str0;
  QRect rect;
  QSize size;
  //QPoint point = QPoint(0,0);

  str0.Reset(0);
  if (GUIProg->refresh) {
    refreshWidgets = GUIProg->selDECL == 0;

    if (!suppressed) {
      firstnode = 0;
      firstnode = ((CGUIProg*)GUIProg)->TreeSrch.FirstNode();
      size = ((GUIScrollView*)GUIProg->scrView)->MaxBottomRight.size();
      if (size.width() && size.height())
        //((GUIScrollView*)GUIProg->scrollView)->resizeContents(size.width(),size.height());
        ((GUIScrollView*)GUIProg->scrView)->widget()->resize(size.width(),size.height());
      if (!firstnode || !GUIProg->refresh)
        return;

      formnode = (CHEFormNode*)GUIProg->Root->successor;
      suffix = (CHEFormNode*)formnode->successor;
      if (suffix && !suffix->data.StringValue.Length())
        formnode->successor = 0;
      else
        suffix = 0;
      ((GUIScrollView*)GUIProg->scrView)->MaxBottomRight = QRect(0,0,0,0);
      pMaxBottomRight = &((GUIScrollView*)GUIProg->scrView)->MaxBottomRight;
      makeWidgets(firstnode,((GUIScrollView*)GUIProg->scrView)->qvbox,str0,empty);
      pMaxBottomRight->setRight(pMaxBottomRight->right()+1);
      pMaxBottomRight->setBottom(pMaxBottomRight->bottom()+1);
      ((GUIScrollView*)GUIProg->scrView)->qvbox->resize(pMaxBottomRight->size());
      if (suffix)
        formnode->successor = suffix;
    }

  }
  GUIProg->refresh = false;
  if (refreshWidgets  ) {
    Cursor(true,true);
    refreshWidgets = false;
  }
  GUIProg->Warning = 0;
  GUIProg->InCommandAgent = true;
  SetScrollSizes(((GUIScrollView*)GUIProg->scrView));

} // END OF DisplayScreen


void MakeGUICLASS::SetScrollSizes(QScrollArea* view)
{
  QSize size = ((GUIScrollView*)view)->MaxBottomRight.size(), size2;

  if (size.width() < view->viewport()->width() )
    size.setWidth(view->viewport()->width());
  if (size.height() < view->viewport()->height() )
    size.setHeight(view->viewport()->height());

  if (!((GUIScrollView*)view)->qvbox->FromPopup && GUIProg->ViewWin->inherits("LavaGUIDialog")) {
    size2 = ((GUIScrollView*)view)->MaxBottomRight.size();
    ((LavaGUIDialog*)GUIProg->ViewWin)->setpropSize(size2);
  }
  view->setWidget(((GUIScrollView*)view)->qvbox);
  view->horizontalScrollBar()->setValue(0);
  view->verticalScrollBar()->setValue(0);
}

void MakeGUICLASS::makeWidget (CHEFormNode* chFrmNd,
                                QWidget* parentWidget,
                                DString& parentWidgetName,
                                frameContext& context)
{
  QWidget* refFrame, *newWidget = 0, *refWidget, *parent;
  unsigned frameSpacing, widgetSpacing, selOrd, i;
  bool emptyWidget = true;
  CHEEnumSelId *enumSel;
  CHEFormNode* pred;
  DString str0;

  /* update current indent: */
  if (chFrmNd->data.Annotation.ptr
      && ((chFrmNd->data.Annotation.ptr->IndType == AbsIndent)
          || chFrmNd->data.Annotation.ptr->Indentation)) {
    if (chFrmNd->data.Annotation.ptr->IndType == AbsIndent)
      context.indent = chFrmNd->data.Annotation.ptr->Indentation;
    else context.indent += chFrmNd->data.Annotation.ptr->Indentation;
    if (context.indent < 0)
      context.indent = 0;
  }
//  chFrmNd->data.FIP.beg = context.indent;

  /* make new frame if necessary: */
  if (!context.currentFrame
      || (chFrmNd->data.Annotation.ptr
          && (chFrmNd->data.Annotation.ptr->FrameSpacing != -1))) {
    if (!context.currentFrame || !context.emptyFrame) { /* start a new frame */
      context.precedingFrame = context.currentFrame;
      CreateFormWidget( context.currentFrame, parentWidget, 0, parentWidgetName, chFrmNd);
      context.emptyFrame = true;
      /*if (context.currentFrame->parentWidget()->inherits("GUIVBox")) {
        QHBoxLayout* hb = new QHBoxLayout(context.currentFrame);
        hb->setAutoAdd(true);
      }*/
    }


    /* "SetNeighbour" for the new frame: */
    refFrame = 0;
    frameSpacing = 0;
    if (chFrmNd->data.Annotation.ptr
        && (chFrmNd->data.Annotation.ptr->FrameSpacing != -1))
      if (chFrmNd->data.Annotation.ptr->FrameSpacing > 0) {
        refFrame = context.precedingFrame;
        frameSpacing = chFrmNd->data.Annotation.ptr->FrameSpacing-1;
      }
      else  /* overlay of preceding frame */
        getPrecedingFramesNeighbourData(chFrmNd,refFrame, frameSpacing);

    SetTopLeft( context.currentFrame, NULL, context.indent, refFrame, frameSpacing);
    context.precedingWidgetInFrame = 0;
  }

  /* make new widget: */
  if (chFrmNd->data.Atomic
      || chFrmNd->data.BasicFlags.Contains(PopUp)
      || chFrmNd->data.IterFlags.Contains(Ellipsis)) {
    if (chFrmNd->data.BasicFlags.Contains(Groupbox)
      && (!chFrmNd->data.IterFlags.Contains(Optional)
          || !chFrmNd->data.IterFlags.Contains(Ellipsis))
      ) {
      pred = (CHEFormNode*)chFrmNd->predecessor;
      if (pred && pred->data.FormSyntax->DeclDescType == LiteralString)
        ((CFormWid*)context.currentFrame)->BGroup = new QGroupBox(pred->data.StringValue.c, context.currentFrame);
      else
        ((CFormWid*)context.currentFrame)->BGroup = new QGroupBox(context.currentFrame);
      QPalette palette;
      palette.setColor(QPalette::Active,QPalette::WindowText, context.currentFrame->palette().windowText().color());
      ((CFormWid*)context.currentFrame)->BGroup->setPalette(palette);
//      ((CFormWid*)context.currentFrame)->BGroup->setPaletteForegroundColor(context.currentFrame->foregroundColor());
      if (pred)
        GUIProg->SetLFont(((CFormWid*)context.currentFrame)->BGroup, pred);
      else
        GUIProg->SetLFont(((CFormWid*)context.currentFrame)->BGroup, chFrmNd);
      ((CFormWid*)context.currentFrame)->BGroup->show();
      parent = ((CFormWid*)context.currentFrame)->BGroup;
    }
    else
      parent = context.currentFrame;
    if (chFrmNd->data.WidgetName.l)
      AtomicWidget(chFrmNd,parent,newWidget, chFrmNd->data.WidgetName);
    else
      if (parentWidgetName.l)
        AtomicWidget(chFrmNd, parent, newWidget, parentWidgetName);
      else
        AtomicWidget(chFrmNd, parent, newWidget, str0);
    if (newWidget)
      emptyWidget = (newWidget == 0);
    chFrmNd->data.FIP.widget = newWidget;
    chFrmNd->data.FIP.frameWidget = context.currentFrame;
    if (!((CFormWid*)context.currentFrame)->usedInFormNode) {
      chFrmNd->data.IoSigFlags.INCL(firstUseOfFrame);
      ((CFormWid*)context.currentFrame)->usedInFormNode = true;
    }
    if (newWidget) {
//      if (chFrmNd->data.Annotation.ptr)
//        annotations(chFrmNd,newWidget);
      if (!chFrmNd->data.Annotation.ptr) {
        refWidget = context.precedingWidgetInFrame;
        widgetSpacing = 0;
      }
      else
        if (chFrmNd->data.Annotation.ptr->TabPosition == -1) {
          refWidget = context.precedingWidgetInFrame;
          widgetSpacing = chFrmNd->data.Annotation.ptr->Space;
        }
        else {
          refWidget = 0;
          widgetSpacing = unsigned(chFrmNd->data.Annotation.ptr->TabPosition)+chFrmNd->data.Annotation.ptr->Space;
        }
      /*
      if (vertical)
        SetTopLeft(newWidget, NULL, 0, refWidget, widgetSpacing);
      else*/
        SetTopLeft(newWidget, refWidget, widgetSpacing, NULL, 0);
    }

  }
  else {
    if (chFrmNd->data.SubTree.first) {
      int box;
      if ((chFrmNd->data.Annotation.ptr && (chFrmNd->data.Annotation.ptr->Box == FullBox)))
        box = 1;
      else
        box = -1;
      if (chFrmNd->data.WidgetName.l)
        CreateFormWidget(newWidget, context.currentFrame, box, chFrmNd->data.WidgetName, chFrmNd );
      else
        CreateFormWidget(newWidget, context.currentFrame, box, parentWidgetName, chFrmNd);
      if (!chFrmNd->data.Annotation.ptr ) {
        refWidget = context.precedingWidgetInFrame;
        widgetSpacing = 0;
      }
      else {
        if (chFrmNd->data.Annotation.ptr->TabPosition == -1) {
          refWidget = context.precedingWidgetInFrame;
          widgetSpacing = chFrmNd->data.Annotation.ptr->Space;
        }
        else {
          refWidget = 0;
          widgetSpacing = unsigned(chFrmNd->data.Annotation.ptr->TabPosition)+chFrmNd->data.Annotation.ptr->Space;
        }
      }
      /*
      if (vertical)
        SetTopLeft(newWidget, NULL, 0, refWidget, widgetSpacing);
      else
      */
      SetTopLeft(newWidget, refWidget, widgetSpacing, NULL, 0);
//      first = 0;
      if (chFrmNd->data.BasicFlags.Contains(Groupbox)) {
        pred = (CHEFormNode*)chFrmNd->predecessor;
        if (pred && pred->data.FormSyntax->DeclDescType == LiteralString)
          ((CFormWid*)newWidget)->BGroup = new QGroupBox(pred->data.StringValue.c, newWidget);
        else
          ((CFormWid*)newWidget)->BGroup = new QGroupBox(newWidget);
        QPalette palette;
        palette.setColor(QPalette::Active,QPalette::WindowText, newWidget->palette().windowText().color());
        ((CFormWid*)newWidget)->BGroup->setPalette(palette);
//        ((CFormWid*)newWidget)->BGroup->setPaletteForegroundColor(newWidget->foregroundColor());
        if (pred)
          GUIProg->SetLFont(((CFormWid*)newWidget)->BGroup, pred);
        else
          GUIProg->SetLFont(((CFormWid*)newWidget)->BGroup, chFrmNd);
        ((CFormWid*)newWidget)->BGroup->show();
        //QVBoxLayout* vb = new QVBoxLayout(((CFormWid*)newWidget)->BGroup);
        //vb->setAutoAdd(true);
      }
      /*else
        if (newWidget->parentWidget()->parentWidget()->inherits("GUIVBox")) {
          QVBoxLayout* vb = new QVBoxLayout(newWidget);
          vb->setAutoAdd(true);
        }*/
      if (chFrmNd->data.BasicFlags.Contains(ButtonMenu)
          || chFrmNd->data.BasicFlags.Contains(OptionMenu)
          || chFrmNd->data.BasicFlags.Contains(PopUpMenu)) {
        if (chFrmNd->data.BasicFlags.Contains(ButtonMenu)) {
          menuType = isButtonMenu;
          radioBox = newWidget;
        }
        else
          if (chFrmNd->data.BasicFlags.Contains(OptionMenu))
            menuType = isOptionMenu;
          else
            menuType = isPopupMenu;
        makingMenu = beforeField;
        if (!((CHEFormNode*)chFrmNd->data.SubTree.first)->data.StringValue.c)
          selCode.Destroy();
        else {
          selOrd  = ((CHEFormNode*)chFrmNd->data.SubTree.first)->data.D;
          enumSel = (CHEEnumSelId*)((CHEFormNode*)chFrmNd->data.SubTree.first)
                                               ->data.FormSyntax->Items.first;
          selCode.Reset(1); //=enumSel->data.SelectionCode;
          if (selOrd > 0)
            for (i = 1; enumSel && (i <= selOrd); i++)
              enumSel = (CHEEnumSelId*)enumSel->successor;
          if (enumSel)
            selCode = enumSel->data.SelectionCode;
//          else
//            selOrd = 0;
        }
      }
      if (((CFormWid*)newWidget)->BGroup)
        makeWidgets((CHEFormNode*)chFrmNd->data.SubTree.first,
                      ((CFormWid*)newWidget)->BGroup,chFrmNd->data.WidgetName,emptyWidget);
      else
        makeWidgets((CHEFormNode*)chFrmNd->data.SubTree.first,
                      newWidget,chFrmNd->data.WidgetName,emptyWidget);
      if (!emptyWidget) {
        chFrmNd->data.FIP.widget = newWidget;
        chFrmNd->data.FIP.frameWidget = context.currentFrame;
        if (!((CFormWid*)context.currentFrame)->usedInFormNode) {
          chFrmNd->data.IoSigFlags.INCL(firstUseOfFrame);
          ((CFormWid*)context.currentFrame)->usedInFormNode = true;
        }
      }
      else {
        ReallyDestroyWidget(newWidget);
        //newWidget->m_hWnd = 0;
        chFrmNd->data.FIP.widget = 0;
        chFrmNd->data.FIP.frameWidget = 0;
      }
      if (chFrmNd->data.BasicFlags.Contains(ButtonMenu)
          || chFrmNd->data.BasicFlags.Contains(OptionMenu)
          || chFrmNd->data.BasicFlags.Contains(PopUpMenu))
        makingMenu = none;
//      if (first)
//        permute(chFrmNd);
    }
    else
      emptyWidget = true;
  }//else
  if (!emptyWidget) {
    context.emptyFrame = false;
    if (newWidget) {
      GrowParent(newWidget);
      if (radioBox == newWidget)
        radioBox = 0;
    }
    context.precedingWidgetInFrame = newWidget;
  }
} // END OF makeWidget

/*--------------------------------------------------------------------------*/
/*------------------ makeWidgets
 */
/*--------------------------------------------------------------------------*/

void MakeGUICLASS::makeWidgets (CHEFormNode* chFrmNd,
                                 QWidget* parentWidget,
                                 DString& parentWidgetName,
                                 bool& empty)

{
  frameContext context;
  int oldTabPos;

  /*WITH context DO            */
  context.precedingFrame = 0;
  context.currentFrame = 0;
  context.precedingWidgetInFrame = 0;
  context.emptyFrame = true;
  context.indent = 0;

  switch (makingMenu) {
  case beforeMenu:
    makingMenu = beforeMenuLine;
    break;
  case beforeMenuLine:
    makingMenu = beforeSelCode;
    break;
  default: ;
  }

  for (; chFrmNd; chFrmNd = (CHEFormNode*)chFrmNd->successor) {
    if (chFrmNd->data.IterFlags.Contains(Ignore))
      continue;
    if (chFrmNd->data.IterFlags.Contains(Optional)
      && !chFrmNd->data.Atomic
      && !chFrmNd->data.SubTree.first)
        continue;
    if (chFrmNd->data.BasicFlags.Contains(Invisible))
        continue;
    switch (makingMenu) {
    case none:
      if (!chFrmNd->data.BasicFlags.Contains(BlankSelCode)) {
        makeWidget(chFrmNd,parentWidget,parentWidgetName,context);
        if (chFrmNd->data.BasicFlags.Contains(PopUp)
          && (chFrmNd->data.FIP.poppedUp || chFrmNd->data.FormSyntax->WorkFlags.Contains(poppedUp)))
          Popup(chFrmNd,true,false);
      }
      break;
    case beforeMenuLine:
      if (chFrmNd->data.BasicFlags.Contains(MenuText)) {
        makingMenu = none;
        makeWidget(chFrmNd,parentWidget,parentWidgetName,context);
        makingMenu = beforeMenuLine;
      }
      else if (!chFrmNd->data.BasicFlags.Contains(BlankSelCode))
        makeWidget(chFrmNd,parentWidget,parentWidgetName,context);
      break;
    case beforeMenu:
      if (menuType == isButtonMenu) {
        makeWidget(chFrmNd,parentWidget,parentWidgetName,context);
      }
      else if (menuType == isOptionMenu) {
        makeOptionMenu((CHEFormNode*)chFrmNd->data.SubTree.first);
        makingMenu = none;
      }
      else {
        makePopupMenu((CHEFormNode*)chFrmNd->data.SubTree.first);
        makingMenu = none;
      }
      break;

    case beforeExplanText:
      if (chFrmNd->data.Annotation.ptr) {
        oldTabPos = chFrmNd->data.Annotation.ptr->TabPosition;
        chFrmNd->data.Annotation.ptr->TabPosition = -1;
      }
      makeWidget(chFrmNd,parentWidget,parentWidgetName,context);
      if (chFrmNd->data.Annotation.ptr)
        chFrmNd->data.Annotation.ptr->TabPosition = oldTabPos;
      makingMenu = beforeMenuLine;
      goto exit;

    case beforeField:
      if (menuType != isButtonMenu)
        makeWidget(chFrmNd,parentWidget,parentWidgetName,context);
      if (makingMenu != none)
        makingMenu = beforeMenu;
      break;

    case beforeSelCode:
      makingMenu = beforeExplanText;
      menuSelCode = chFrmNd->data.StringValue;
      break;
    }

  } exit: ;

  if ((context.currentFrame) && context.emptyFrame)
    ReallyDestroyWidget(context.currentFrame);

  if (!context.precedingFrame && context.emptyFrame)
    empty = true;
  else
    empty = false;
} // END OF makeWidgets


void MakeGUICLASS::makeOptionMenu (CHEFormNode* chFrmNd)

{
  CHEFormNode* selCodeNode, *explanTextNode, *fieldNode,
          *firstExplanTextNode=0, *firstSelCodeNode;
  bool defaultDefined=false;

  while (chFrmNd) {
    if (!chFrmNd->data.BasicFlags.Contains(MenuText)
    && !chFrmNd->data.BasicFlags.Contains(BlankSelCode)) {
      selCodeNode = (CHEFormNode*)chFrmNd->data.SubTree.first;
      explanTextNode = (CHEFormNode*)selCodeNode->successor;
      if (!firstExplanTextNode) {
        firstSelCodeNode = selCodeNode;
        firstExplanTextNode = explanTextNode;
      }
      fieldNode = (CHEFormNode*)((CHEFormNode*)chFrmNd->data.FIP.up)->predecessor;
      if (fieldNode->data.IoSigFlags.Contains(UnprotectedUser))
        AddOptionMenuItem((QWidget*&)explanTextNode->data.FIP.widget,
                           menuWidget,
          ((CHEFormNode*)chFrmNd->data.SubTree.first->successor)->data.StringValue,
           (CHEFormNode*)chFrmNd->data.SubTree.first->successor);
      else
        AddOptionMenuItem((QWidget*&)explanTextNode->data.FIP.widget, menuWidget,
          ((CHEFormNode*)chFrmNd->data.SubTree.first->successor)->data.StringValue,
           NULL);
      if (selCode == selCodeNode->data.StringValue) {
        SetOptionDefault( menuWidget,
                         (QWidget*)explanTextNode->data.FIP.widget,
                         explanTextNode->data.StringValue);
        defaultDefined = true;
      }
    }
    chFrmNd = (CHEFormNode*)chFrmNd->successor;
  }
} // END OF makeOptionMenu


void MakeGUICLASS::makePopupMenu (CHEFormNode* chFrmNd)
{
  CHEFormNode* selCodeNode, *explanTextNode;

  while (chFrmNd) {
    if (!chFrmNd->data.BasicFlags.Contains(MenuText)
        && !chFrmNd->data.BasicFlags.Contains(BlankSelCode)) {
      selCodeNode = (CHEFormNode*)chFrmNd->data.SubTree.first;
      explanTextNode = (CHEFormNode*)selCodeNode->successor;
      if (((CHEFormNode*)chFrmNd->data.FIP.up->predecessor)->data.IoSigFlags.Contains(UnprotectedUser))
        AddPopupMenuItem( (QWidget*&)explanTextNode->data.FIP.widget, menuWidget,
             ((CHEFormNode*)chFrmNd->data.SubTree.first->successor)->data.StringValue ,
             ((CHEFormNode*)chFrmNd->data.SubTree.first->successor));
      else
        AddPopupMenuItem( (QWidget*&)explanTextNode->data.FIP.widget, menuWidget,
          ((CHEFormNode*)chFrmNd->data.SubTree.first->successor)->data.StringValue ,0);
    }
    chFrmNd = (CHEFormNode*)chFrmNd->successor;
  }
} // END OF makePopupMenu



void MakeGUICLASS::AtomicWidget (CHEFormNode* chFrmNd,
                                  QWidget* parent,
                                  QWidget*& newWidget,
                                  DString& widgetName)

{
  unsigned rows, cols, maxTextLength;
  bool  echo, protect, growable, enterFlag;
  unsigned length;
  QWidget *insertButton, *deleteButton;
  DString str0;
  CHEFormNode* enumNode, *pred;
  DString explanText;

  if (chFrmNd->data.IterFlags.Contains(Ellipsis)) {
    if (chFrmNd->data.Atomic) {
      TAnnotation* anno = (TAnnotation*)chFrmNd->data.FormSyntax->Annotation.ptr->FA.ptr;
      if (chFrmNd->data.IterFlags.Contains(Optional)) {
        pred = (CHEFormNode*)chFrmNd->predecessor;
        if (pred && pred->data.FormSyntax->DeclDescType == LiteralString)
          chFrmNd->data.StringValue = pred->data.StringValue ;
      }
      CreateEllipsisWidget( newWidget, parent,widgetName, chFrmNd->data.StringValue ,INSERT,  chFrmNd);
    }
    else {
      CreateFormWidget( newWidget, parent,0, str0, chFrmNd);
      CreateEllipsisWidget( insertButton, newWidget, widgetName ,
                           ((CHEFormNode*)chFrmNd->data.SubTree.first)->data.StringValue ,INSERT, chFrmNd);
      ((CHEFormNode*)chFrmNd->data.SubTree.first)->data.FIP.widget = insertButton;
      SetTopLeft( insertButton, NULL,0, NULL,0);
      GrowParent(insertButton);
      widgetName = ellipsisNameDEL;
      CreateEllipsisWidget( deleteButton, newWidget,widgetName ,
                           ((CHEFormNode*)chFrmNd->data.SubTree.first->successor)->data.StringValue ,DELETEWin, chFrmNd);
      ((CHEFormNode*)chFrmNd->data.SubTree.first->successor)->data.FIP.widget = deleteButton;
      SetTopLeft(deleteButton, insertButton,0,0,0);
      GrowParent(deleteButton);
      if (!chFrmNd->predecessor) {
        ShowDeleteButton(chFrmNd,false);
      }
      else VisibleDeleteButton(chFrmNd,true);
    }

  }
  else if (chFrmNd->data.Atomic)
    if (makingMenu == beforeField) {
      if (menuType == isOptionMenu) {
        length = chFrmNd->data.GetLengthField();
        CreateOptionMenuWidget( newWidget, menuWidget, parent, widgetName, chFrmNd->data.StringValue, length, chFrmNd );
      }
      else {
        if (menuButtonLabel.c)
          length = menuButtonLabel.Length();
        else
          length = chFrmNd->data.GetLengthField();
        if (menuButtonLabel.c)
          CreatePopupMenuWidget( newWidget, menuWidget, parent, widgetName ,menuButtonLabel ,length, chFrmNd);
        else
          if (chFrmNd->data.StringValue.Length())
            CreatePopupMenuWidget( newWidget, menuWidget, parent, widgetName ,chFrmNd->data.StringValue ,length, chFrmNd);

          else
            CreatePopupMenuWidget( newWidget, menuWidget, parent, widgetName, str0,length,  chFrmNd);
      }
      menuButtonWidget = newWidget;
      ((CHEFormNode*)chFrmNd->successor)->data.FIP.widget = menuWidget;
    }
    else if (chFrmNd->data.IoSigFlags.Contains(UnprotectedUser)
             || chFrmNd->data.IoSigFlags.Contains(UnprotectedProg)) {
      if ((chFrmNd->data.BType == B_Bool)
           && chFrmNd->data.BasicFlags.Contains(Toggle)) {
        if (!widgetName.l)
          widgetName = booleanName;
        pred = (CHEFormNode*)chFrmNd->predecessor;
        bool leftLabel;
        if (pred && (pred->data.FormSyntax->DeclDescType == LiteralString)
//            && pred->data.BasicFlags.Contains(Toggle)) {
              && pred->data.FormSyntax->Annotation.ptr->BasicFlags.Contains(Toggle)) {
          explanText = pred->data.StringValue ;
          leftLabel = true;
        }
        else {
          leftLabel = false;
          pred = (CHEFormNode*)chFrmNd->successor;
          if (pred && (pred->data.FormSyntax->DeclDescType == LiteralString)
//              && pred->data.BasicFlags.Contains(Toggle))
            && pred->data.FormSyntax->Annotation.ptr->BasicFlags.Contains(Toggle) )
          explanText = pred->data.StringValue;
        }
        CreateToggleWidget( newWidget, parent, widgetName, explanText, leftLabel,  chFrmNd);
        if (chFrmNd->data.B)
          SetToggleState( newWidget,true);
        if (!chFrmNd->data.IoSigFlags.Contains(UnprotectedUser))
          SetSensitive( newWidget,0);
      }
      else if (chFrmNd->data.Annotation.ptr->BasicFlags.Contains(DefaultMenu)) {
        //pure Enumeration
        length = chFrmNd->data.GetLengthField();
        CreateOptionMenuWidget(newWidget, menuWidget, parent, widgetName, chFrmNd->data.StringValue, length, chFrmNd );
        if (!chFrmNd->data.IoSigFlags.Contains(UnprotectedUser))
          SetSensitive(newWidget,0);
      }
      else {
        if (chFrmNd->data.BasicFlags.Contains(Text)) {
          rows = chFrmNd->data.GetLengthDecPoint();
          if (!rows)
            rows = 2;
        }
        else
          rows = 1;
        if ((chFrmNd->data.BType == Enumeration)
            && !chFrmNd->data.IoSigFlags.Contains(UnprotectedUser))
          cols = 1;
        else
          cols = chFrmNd->data.GetLengthField();
        maxTextLength = cols;
        chFrmNd->data.FIP.leng = cols;
        protect = !(chFrmNd->data.IoSigFlags.Contains(UnprotectedUser));
        echo = !((chFrmNd->data.Annotation.ptr) && (chFrmNd->data.Annotation.ptr->Emphasis == NoEcho));
        TAnnotation* anno = (TAnnotation*)chFrmNd->data.FormSyntax->Annotation.ptr->FA.ptr;
        growable = ((anno->Length.FieldBoundRel != EQ)
                    || ((chFrmNd->data.BType == Enumeration)
                    && !chFrmNd->data.IoSigFlags.Contains(UnprotectedUser)));
        switch (anno->Length.DataBoundRel) {
        case LE:
        case EQ:
          maxTextLength = anno->Length.Data;
          break;
        case GE:
//          if (growable)
            maxTextLength = 0;
//          else
//            maxTextLength = chFrmNd->data.GetLengthField();
        }

        CreateTextWidget( newWidget, parent, widgetName ,
                         rows,cols,
                         echo,  chFrmNd);

        SetText( newWidget,chFrmNd->data.StringValue );

//        if (chFrmNd->data.BasicFlags.Contains(Toggle))
//          SetCursor( newWidget,1);
      }
    }
    else if (makingMenu == beforeExplanText) {
      enumNode = (CHEFormNode*)((CHEFormNode*)chFrmNd->predecessor)->data.FIP.up->data.FIP.up->predecessor;
      enterFlag = (enumNode->data.IoSigFlags.Contains(Enter)
                    || enumNode->data.IoSigFlags.Contains(EnterAsIs))
                  /*&& enumNode->data.IoSigFlags.Contains(UnprotectedUser)*/;
      CreateButtonMenuButton( newWidget, parent, widgetName, chFrmNd->data.StringValue,
                              radioBox, enterFlag, chFrmNd);
      if ((selCode == menuSelCode) && !enterFlag)
        SetToggleState(newWidget,true);
      if (!enumNode->data.IoSigFlags.Contains(UnprotectedUser))
        SetSensitive(newWidget,0);
    }
    else if (!chFrmNd->data.StringValue.Length()) {
      newWidget = 0;
      return;
    }
    else
      if (!chFrmNd->data.FormSyntax->Annotation.ptr->BasicFlags.Contains(Toggle)  //Checkbox label
          && ( !chFrmNd->successor
              || ((CHEFormNode*)chFrmNd->successor)->data.IoSigFlags.Contains(Signature)
              || !((CHEFormNode*)chFrmNd->successor)->data.BasicFlags.Contains(Groupbox)
                    && !((CHEFormNode*)chFrmNd->successor)->data.BasicFlags.Contains(PopUp)
                    && (!((CHEFormNode*)chFrmNd->successor)->data.IterFlags.Contains(Optional)
                        || !((CHEFormNode*)chFrmNd->successor)->data.IterFlags.Contains(Ellipsis))))
        CreateLabelWidget( newWidget, parent,chFrmNd->data.StringValue, widgetName,  chFrmNd );
      else {
        newWidget = 0;
        return;
      }
  else if (chFrmNd->data.BasicFlags.Contains(PopUp)) {
    pred = (CHEFormNode*)chFrmNd->predecessor;
    if (pred && pred->data.FormSyntax->DeclDescType == LiteralString)
      explanText = pred->data.StringValue ;
    if (!((CHEFormNode*)chFrmNd->data.SubTree.first)->data.Atomic) {
      CreatePopupWidget( newWidget, parent, widgetName ,explanText, chFrmNd);
    }
    else if (explanText.l)
      CreatePopupWidget( newWidget, parent, widgetName ,explanText, chFrmNd);
    else if (((CHEFormNode*)chFrmNd->data.SubTree.first)->data.StringValue.l)
      CreatePopupWidget( newWidget, parent, widgetName ,
        ((CHEFormNode*)chFrmNd->data.SubTree.first)->data.StringValue , chFrmNd);
    else
      CreatePopupWidget( newWidget, parent, widgetName ,blank2, chFrmNd);
  }
} // END OF AtomicWidget


void MakeGUICLASS::ShowDeleteButton (CHEFormNode* ellipsisNode, bool yes)
{
  CHEFormNode* buttonNode;
  if (ellipsisNode->data.Atomic && ellipsisNode->data.IterFlags.Contains(Optional))
    buttonNode = ellipsisNode;
  else if (ellipsisNode->data.Atomic)
    return;
  else
    buttonNode = (CHEFormNode*)ellipsisNode->data.SubTree.first->successor;
  if (yes)
    SetSensitive((QWidget*)buttonNode->data.FIP.widget,1);
  else
    SetSensitive((QWidget*)buttonNode->data.FIP.widget,0);
} // END OF ShowDeleteButton


void MakeGUICLASS::RemakeParent (CHEFormNode* /*chFrmNd*/)

{
  ((CGUIProg*)GUIProg)->RedrawForm();
}

/*
void MakeGUICLASS::DeleteWidget (CHEFormNode* chFrmNd)
{
 // if (GUIProg->popUps > 0)
    deleteSiblings(chFrmNd);
} // END OF DeleteWidget


void MakeGUICLASS::deleteSiblings (CHEFormNode* chFrmNd)
{
  CHEFormNode* currentNode;
  QWidget* oldFrameWidget=0;

  if (!chFrmNd) return;
  ClearFocus();
  if (chFrmNd->data.FIP.up)
    currentNode = (CHEFormNode*)chFrmNd->data.FIP.up->data.SubTree.first;
  else
    currentNode = chFrmNd;
  while (currentNode) {
    if (currentNode->data.FIP.frameWidget
        && (currentNode->data.FIP.frameWidget != oldFrameWidget)) {
      oldFrameWidget = (QWidget*)currentNode->data.FIP.frameWidget;
      ReallyDestroyWidget((QWidget*)currentNode->data.FIP.frameWidget);
    }
    currentNode->data.FIP.frameWidget = 0;
    currentNode = (CHEFormNode*)currentNode->successor;
  }
} // END OF deleteSiblings
*/

void MakeGUICLASS::getLastRealizedNode (CHEFormNode* chFrmNd,
                                         CHEFormNode*& lastRealizedNode)

{
  lastRealizedNode = 0;
  while (chFrmNd->predecessor) {
    chFrmNd = (CHEFormNode*)chFrmNd->predecessor;
    if (chFrmNd->data.FIP.widget) {
      lastRealizedNode = chFrmNd;
      return;
    }
  }
} // END OF getLastRealizedNode

/*
void MakeGUICLASS::getNextRealizedNode (CHEFormNode* chFrmNd,
                                         CHEFormNode*& nextRealizedNode)

{
  nextRealizedNode = 0;
  while (chFrmNd->successor) {
    chFrmNd = (CHEFormNode*)chFrmNd->successor;
    if (chFrmNd->data.FIP.widget) {
      nextRealizedNode = chFrmNd;
      return;
    }
  }
} // END OF getNextRealizedNode
*/


void MakeGUICLASS::getPrecedingFramesNeighbourData (CHEFormNode* chFrmNd,
                                                     QWidget*& refFrame,
                                                     unsigned& frameSpacing)

{
  CHEFormNode* lastRealizedNode, *lastFrameNode, *precedingNode;

  getLastRealizedNode(chFrmNd,lastRealizedNode);
  if (!lastRealizedNode) {
    refFrame = 0;
    frameSpacing = 0;
    return;
  }

  precedingNode = (CHEFormNode*)lastRealizedNode->predecessor;
  lastFrameNode = lastRealizedNode;
  while (precedingNode
         && (precedingNode->data.FIP.frameWidget == lastRealizedNode->data.FIP.frameWidget)) {
    lastFrameNode = precedingNode;
    precedingNode = (CHEFormNode*)precedingNode->predecessor;
  }

  if (((CHEFormNode*)lastFrameNode->data.Annotation.ptr)
      && (lastFrameNode->data.Annotation.ptr->FrameSpacing != -1))
    if (!lastFrameNode->data.Annotation.ptr->FrameSpacing )
      getPrecedingFramesNeighbourData(lastFrameNode,
                                      refFrame,frameSpacing);
    else {
      getLastRealizedNode(lastFrameNode,lastRealizedNode);
      if (!lastRealizedNode)
        refFrame = 0;
      else refFrame = (QWidget*)lastRealizedNode->data.FIP.frameWidget;
      frameSpacing = lastFrameNode->data.Annotation.ptr->FrameSpacing-1;
    }
  else {
    refFrame = 0;
    frameSpacing = 0;
  }
} // END OF getPrecedingFramesNeighbourData


/*--------------------------------------------------------------------------*/
/*------- VisibleDeleteButton
 *  called from Append/Delete-IterItem to set/remove DeleteButton in
 *  EllipsisNode
 */
/*-----------------------------------------------------------------internal-*/
void MakeGUICLASS::VisibleDeleteButton (CHEFormNode* trp, bool maWi)
{
  CHEFormNode* itrp, *elltrp;
  bool setDeleteButton; /* optionaler Fall noch ausgeschlossen!! */
  if (((CGUIProg*)GUIProg)->LavaForm.IsIteration(trp)
    || trp->data.IterFlags.Contains(Optional))
    itrp = trp;
  else
    itrp = trp->data.FIP.up;
  if (!itrp || itrp->data.IterFlags.Contains(NoEllipsis))
    return;
  setDeleteButton = false;
  setDeleteButton = ((CGUIProg*)GUIProg)->LavaForm.IsAdmissibleIteration(itrp, setDeleteButton, 0);

  if (maWi) {
    if (!setDeleteButton) {
      ShowDeleteButton(trp,setDeleteButton);
    }
    return;
  }
  elltrp = 0; /* called from Append/Delete-IterItem */
  elltrp = CursorToEllipsis( itrp);
  if (elltrp && elltrp->data.IterFlags.Contains(Ellipsis))
    ShowDeleteButton(elltrp,setDeleteButton);
  return;
} // END OF VisibleDeleteButton


void MakeGUICLASS::Popup (CHEFormNode* popupNode,
                           bool popupWindow,
                           bool cursorOnField,
                           bool redraw)

{
  QWidget *pane, *popupTextWidget;
  CHEFormNode* pred;
  bool firstCall, empty, isTextPopup=false, isNew = !popupNode->data.FIP.popupShell;
  DString explanText, str0;
  QRect *mbr;

  firstCall = false;
  mbr = pMaxBottomRight;
  if (redraw && popupNode->data.FIP.popupShell)
    ((CGUIProg*)GUIProg)->LavaForm.DeleteWindows((CHEFormNode*)popupNode->data.SubTree.first, redraw);
  if (!popupNode->data.FIP.popupShell || redraw) {
    firstCall = true;
    pred = (CHEFormNode*)popupNode->predecessor;
    if (pred && pred->data.FormSyntax->DeclDescType == LiteralString)
      explanText = pred->data.StringValue;
    else
      if (popupNode->data.IoSigFlags.Contains(Signature))
        explanText = DString("Unterschrift");
    if (!popupNode->data.Atomic
    && (popupNode->data.SubTree.first == popupNode->data.SubTree.last)
    && (((CHEFormNode*)popupNode->data.SubTree.first)->data.BType == VLString)
    && (((CHEFormNode*)popupNode->data.SubTree.first)->data.IoSigFlags.Contains(UnprotectedUser)
        || ((CHEFormNode*)popupNode->data.SubTree.first)->data.IoSigFlags.Contains(UnprotectedProg)))
      isTextPopup = true;

    CreatePopupWindow(isTextPopup, (QWidget*&)popupNode->data.FIP.popupShell, pane, explanText, popupNode);
    if (GUIProg->isView)
      pMaxBottomRight = &((CLavaGUIPopup*)popupNode->data.FIP.popupShell)->view->MaxBottomRight;
    else
      pMaxBottomRight = &((CLavaGUIPopupD*)popupNode->data.FIP.popupShell)->view->MaxBottomRight;
    if (isTextPopup) {
      AtomicWidget((CHEFormNode*)popupNode->data.SubTree.first, pane,popupTextWidget,str0);
      ((CHEFormNode*)popupNode->data.SubTree.first)->data.FIP.widget = popupTextWidget;
      GrowParent(popupTextWidget);
    }
    else
      if (popupNode->data.Atomic) {
        popupNode->data.BasicFlags.EXCL(PopUp);
        makeWidgets(popupNode,
                    pane,popupNode->data.WidgetName,empty);
        popupNode->data.BasicFlags.INCL(PopUp);
      }
      else
        makeWidgets((CHEFormNode*)popupNode->data.SubTree.first,
                    pane,popupNode->data.WidgetName,empty);
    pMaxBottomRight->setRight(pMaxBottomRight->right()+1);
    pMaxBottomRight->setBottom(pMaxBottomRight->bottom()+1);
    if (isNew && !LBaseData->inRuntime && (popupNode->data.FormSyntax->SectionInfo1 >= 0)
      && popupNode->data.FormSyntax->WorkFlags.Contains(poppedUp)) {
      popupNode->data.FIP.popupShell->move(popupNode->data.FormSyntax->SectionInfo2,
        popupNode->data.FormSyntax->SectionInfo1 );
      popupNode->data.FormSyntax->SectionInfo1 = -1;
    }
    if (GUIProg->isView)
      SetScrollSizes(((CLavaGUIPopup*)popupNode->data.FIP.popupShell)->view);
    else
      SetScrollSizes(((CLavaGUIPopupD*)popupNode->data.FIP.popupShell)->view);
  }

  if (popupWindow) {
    PopupWindow((QWidget*)popupNode->data.FIP.popupShell);
    popupNode->data.FIP.poppedUp = true;
    popupNode->data.FormSyntax->WorkFlags.INCL(poppedUp);
  }

  if (firstCall)
    GUIProg->popUps++;
  if (cursorOnField)
    CursorOnField(popupNode);
  pMaxBottomRight = mbr;
} // END OF Popup


DString MakeGUICLASS::abbrevContents (DString& vls)

{
  unsigned len;

  len = vls.Length();
  switch (len) {

  case 0:
    return blank2;

  case 1:
  case 2:
    return vls;

  default:
    string = vls;
    string[2] = '>';
    string[3] = '\0';
    string.l = 3;
    return string;
  }
} // END OF abbrevContents


/*--------------------------------------------------------------------------
 * All poppedUp nodes beginning from fmno are popped-dowm
 * For popdownNode setFocus is called.
 *-----------------------------------------------------------------exported-*/
void MakeGUICLASS::PopDown (CHEFormNode* fmno,
                            CHEFormNode* popdownNode,
                            bool& ok)
{
  CHEFormNode* chFrmNd, *syo, *pred;
  /* All poppedUp nodes beginning from fmno are popped-dowm */

  chFrmNd = fmno;
  while (chFrmNd && !ok) {
    if (chFrmNd->data.FIP.popupShell) {
      PopdownWindow((QWidget*)chFrmNd->data.FIP.popupShell);
      chFrmNd->data.FIP.poppedUp = false;
      if (GUIProg->FocusPopup == chFrmNd)
        GUIProg->FocusPopup = 0;
    }

    ((CGUIProg*)GUIProg)->TreeSrch.NextNodeNotIGNORED(chFrmNd,syo);
    if (syo)
      PopDown(syo,popdownNode,ok);

    if (fmno == popdownNode) {
      SetFIPwidgetNULL(fmno);
      pred = (CHEFormNode*)fmno->predecessor;
      if (!fmno->data.IoSigFlags.Contains(Signature)
          && ((CHEFormNode*)fmno->data.SubTree.first)->data.Atomic
          && (!pred
              || pred->data.FormSyntax->DeclDescType != LiteralString)) {
        DString str = abbrevContents(((CHEFormNode*)fmno->data.SubTree.first) ->data.StringValue);
        SetLabel( (QWidget*)fmno->data.FIP.widget, str);
      }
      SetPointer( (QWidget*)fmno->data.FIP.widget,0);
      ok = true;
    }
  }
} // END OF PopDown


void MakeGUICLASS::SetFIPwidgetNULL (CHEFormNode* chFrmNd)

{
  if (!chFrmNd || chFrmNd->data.Atomic) return;

  for (chFrmNd = (CHEFormNode*)chFrmNd->data.SubTree.first;
       chFrmNd;
       chFrmNd = (CHEFormNode*)chFrmNd->successor) {
    chFrmNd->data.FIP.widget = 0;
    if (chFrmNd->data.Atomic) continue;
    else SetFIPwidgetNULL(chFrmNd);
  }
}


/*--------------------------------------------------------------------------*/
/*------------------ setFocus
 *  pos  controls the caret position in the field :
 * pos = 2 only for FOCUS the position curPos in field
 * pos = 1 fot TAB  and BACKTAB
 * pos = 0 else
 */
/*-------------------------------------------------------------- exported --*/

bool MakeGUICLASS::setFocus (unsigned pos, CHEFormNode* trpn)
{
  QWidget *widV, *widN;
  unsigned curPos;
  CHEFormNode* popup, *butt, *button, *butt0 = 0, *containingPopup=0;
  bool ready, ok=false;

  if (!trpn)
    return false;
  if (trpn->data.Atomic
    && (trpn->data.IoSigFlags.Contains(UnprotectedUser)
       //|| trpn->data.FIP.widget->inherits("QButton") && trpn->data.FIP.widget->isEnabled()
       )
     || trpn->data.IoSigFlags.Contains(Flag_INPUT) && trpn->data.IoSigFlags.Contains(Signature)
     || trpn->data.BasicFlags.Contains(PopUp)
     || trpn->data.IterFlags.Contains(Ellipsis)) {
    if (GUIProg->focNode)
      if (GUIProg->focNode->data.FIP.widget) {
        GUIProg->MPTR = GUIProg->focNode; /* alter Focus */
        widV = (QWidget*)GUIProg->focNode->data.FIP.widget;
      }
      else {
        GUIProg->CurPos = 0;
        GUIProg->setFocNode(0);
        widV = 0;
      }
    else
      widV = 0;

    popup = trpn;
    ready = false;
    while (!ready && popup && popup->data.FIP.up) {
      popup = popup->data.FIP.up;
      if (popup->data.BasicFlags.Contains(PopUp)) {
//          && !popup->data.FIP.poppedUp)
        ready = true;
        containingPopup = popup;
//        ImplicitPopup(popup,true);
      }
    }
    if (GUIProg->FocusPopup && (GUIProg->FocusPopup != containingPopup)
       && !GUIProg->InCommandAgent)  // ==> program request, no terminal request
      PopDown(GUIProg->FocusPopup,GUIProg->FocusPopup,ok);
    if (containingPopup)
      if (!containingPopup->data.FIP.poppedUp)
        ImplicitPopup(containingPopup,true);
      else
        containingPopup->data.FIP.popupShell->show();
    GUIProg->FocusPopup = containingPopup;

    if (trpn->data.FIP.up
        && trpn->data.FIP.up->data.BasicFlags.Contains(ButtonMenu)) {
      button = (CHEFormNode*)((CHEFormNode*)trpn->successor)->data.SubTree.first;
      if (!trpn->data.StringValue.c || (pos == 1)
          || !trpn->data.StringValue.l
          || !((CHEEnumSelId*)trpn->data.FormSyntax->Items.first)->data.SelectionCode.c ) {
        ok = false;
        while (button &&  !ok) {
          if (!button->data.BasicFlags.Contains(MenuText)
               && !button->data.BasicFlags.Contains(BlankSelCode) ) {
            butt = (CHEFormNode*)((CHEFormNode*)button->data.SubTree.first)->successor;
            if (!butt0)
              butt0 = butt;
            if (Compare(trpn->data.StringValue,butt->data.StringValue) == 0)
              ok = true;
            else
              button = (CHEFormNode*) button->successor;
          }
          else
            button = (CHEFormNode*) button->successor;
        }
        if (button) {
          butt0 = 0;
          button = butt;
        }
        else
          button = butt0;
      }
      else {
        ok = false;
        while (button &&  !ok) {
          butt = (CHEFormNode*)button->data.SubTree.first;
          if (Compare(trpn->data.StringValue,butt->data.StringValue) == 0)
            ok = true;
          else
            button = (CHEFormNode*)button->successor;
        }
        if (ok)
          button = (CHEFormNode*)button->data.SubTree.first->successor;
      }
      if (button)
        widN = (QWidget*)button->data.FIP.widget; /* neuer Focus */
      else
        widN = 0;
      if (widN && ((widV != widN)
                 //8.3.94       || (widV == widN) && (curPos != GUIProg->CurPos)
                                        )) {
        SetFocus( widV, widN);
        GUIProg->CurPos = 0;
        GUIProg->butNode = trpn;
        GUIProg->setFocNode(button);
        GUIProg->CurPTR = button;
        if (butt0)
          ((CRadioButton*)butt0->data.FIP.widget)->setChecked(false);
        return true;
      }
      return false;
    }
    else
      if (trpn->data.IterFlags.Contains(Ellipsis)) {
        if (trpn->data.Atomic)
          widN = (QWidget*)trpn->data.FIP.widget;
        else
          widN = (QWidget*)((CHEFormNode*)trpn->data.SubTree.first)->data.FIP.widget;
        if (widN && (widV != widN)) {
          SetFocus( widV, widN);
          GUIProg->setFocNode(trpn);
          GUIProg->CurPTR = trpn;
          GUIProg->editNode = 0;
          GUIProg->butNode = 0;
          GUIProg->CurPos = 0;
          return true;
        }
        return false;
      }
      else
        if (trpn->data.FIP.widget)
          widN = (QWidget*)trpn->data.FIP.widget; /* neuer Focus */
        else
          if (trpn->data.FIP.widget)
            widN = (QWidget*)trpn->data.FIP.widget; /* neuer Focus */
          else
            widN = 0;
    if (trpn->data.StringValue.c)
      curPos = trpn->data.StringValue.l;
    else
      curPos = 0;

    if (widN) {
      SetFocus( widV, widN);
      if (trpn
          && trpn->data.IoSigFlags.Contains(UnprotectedUser)
          && trpn->data.StringValue.c
          && (pos != 2))
        if (widV != widN)
          SetInsertPos( widN,curPos);
      if (trpn->data.IoSigFlags.Contains(UnprotectedUser)
          || (trpn->data.IoSigFlags.Contains(Signature)
               && trpn->data.IoSigFlags.Contains(Flag_INPUT))
          || trpn->data.BasicFlags.Contains(PopUp)) {
        GUIProg->setFocNode(trpn);
        GUIProg->CurPTR = trpn;
//        GUIProg->editNode = 0;
        GUIProg->butNode = 0;
        if (trpn->data.StringValue.c)
          GUIProg->CurPos = curPos;
        else
          GUIProg->CurPos = 0;
      }
      return true;
    }
  }
    return false;
} // END OF setFocus

void MakeGUICLASS::ImplicitPopup (CHEFormNode* popup, bool show)
{
  CHESigNodePtr *oldpopNode, *popNode;

  oldpopNode = 0;
  popNode = 0;
  while (popup) {
    if (popup->data.BasicFlags.Contains(PopUp)
        && !popup->data.FIP.poppedUp
        && !popup->data.IoSigFlags.Contains(Signature)) {
      oldpopNode = popNode;
      popNode = new CHESigNodePtr;
      popNode->successor = (CHESigNodePtr *)oldpopNode;
      popNode->data = (address)popup;
    }
    popup = popup->data.FIP.up;
  }
  if (!popNode)
    return;
  /* Popup top down ..........................*/
  oldpopNode = popNode;
  while (popNode) {
    popup = (CHEFormNode*)popNode->data;
    Popup(popup,show,false);
    popNode =(CHESigNodePtr *) popNode->successor;
  }

  /* Dealloc the stack .......................*/
  popNode = oldpopNode;
  while (popNode) {
    oldpopNode = (CHESigNodePtr *)popNode->successor;
    delete popNode;
    popNode = oldpopNode;
  }
} // END OF ImplicitPopup


void MakeGUICLASS::Cursor (bool CurInOri, bool sameOri)
{
  CHEFormNode* mousePTR, *trp;

  if (sameOri && CurInOri) {
    mousePTR = 0;
    if (GUIProg->CurPTR
        && GUIProg->CurPTR->data.IoSigFlags.Contains(UnprotectedUser))
      trp = GUIProg->CurPTR;
    else {
      trp = ((CGUIProg*)GUIProg)->TreeSrch.FirstNode();
      trp = ((CGUIProg*)GUIProg)->TreeSrch.NextUnprotected(trp);
    }
    if (trp) {
      setFocus(0,trp);
    }
  }
} // END OF Cursor

void MakeGUICLASS::CursorOnField ( CHEFormNode* trp)
{
  if (trp) {
    if (trp->data.BasicFlags.Contains(PopUp)) {
//      if (trp->data.FIP.poppedUp)
        trp = ((CGUIProg*)GUIProg)->TreeSrch.NUnpInSubtree(trp);
    }
    else
      if (!trp->data.Atomic
              || !trp->data.IoSigFlags.Contains(UnprotectedUser)
         //&& (!trp->data.FIP.widget->inherits("QButton") || !trp->data.FIP.widget->isEnabled())
                 )
        trp = ((CGUIProg*)GUIProg)->TreeSrch.NUnpInSubtree(trp);
    //Trace.TraceNode(trp,"CUoF");
    setFocus(0,trp);
  }
} // END OF CursorOnField

CHEFormNode* MakeGUICLASS::CursorToEllipsis ( CHEFormNode* trp)
{
  CHEFormNode* trpn, *trpx;

  trpn = trp;
  if (!trp) return trpn;

  if (trp->data.IterFlags.Contains(Ellipsis))
    return trpn;
  if (((CGUIProg*)GUIProg)->LavaForm.IsIteration(trp)) {
    trpn = (CHEFormNode*)trp->data.SubTree.first;
    if (trpn
        && trpn->data.IterFlags.Contains(Ellipsis))
      return trpn;
    trpx = trpn;
    while (trpn)
      if (trpn->data.IterFlags.Contains(Ellipsis)) {
        return trpn;
      }
      else {
        trpx = trpn;
        trpn = (CHEFormNode*)trpn->successor;
      }
    if (trpx && trpx->data.IterFlags.Contains(IteratedItem)) {
      trpn = ((CGUIProg*)GUIProg)->TreeSrch.NUnpInSubtree(trpx);
      return trpn;
    }
  }
  else if (trp->data.IterFlags.Contains(Optional)) {
    trpn = (CHEFormNode*)trp->data.SubTree.first;
    if (trpn
        && trpn->data.IterFlags.Contains(Ellipsis))
      return trpn;

    trpn = ((CGUIProg*)GUIProg)->TreeSrch.NUnpInSubtree(trpn);
    return trpn;
  }
  return trpn;
} // END OF CursorToEllipsis


void MakeGUICLASS::INIT (CGUIProgBase *guiPr)
{
  GUIProg = guiPr;
  makingMenu = none;
  ellipsisNameDEL = DString("ellipsisDEL");
  booleanName = DString("booleanField");
  blank2 = DString("  ");
  blank4 = DString("    ");
  questionMarks = DString("???");

}



