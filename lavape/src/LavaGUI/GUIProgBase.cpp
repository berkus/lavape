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


#include "GUIProgBase.h"
#include "qfont.h"
#include "qstyle.h"
#include "qfontmetrics.h"
#include "LavaGUIView.h"
#include "FormWid.h"
#include "mdiframes.h"



void CGUIMet::Create()
{
  Font = 0;
  oldFocus = 0;
  newFocus = 0;
  globalIndent = 8;
  xMargin = 4;
  yMargin = 4;
}

void  CGUIMet::SetFont(QFont *font)
{
  Font = font;
 // SetDC(ViewWin);
}

TFontCase CGUIMet::SetLFont(QWidget* win, CHEFormNode* myFormNode)
{
  CHEFormNode *pp;
  switch (myFormNode->data.ownLFont) {
  case ownFont:
    if (win)
      win->setFont(myFormNode->data.LFont);
    return ownFont;
  case parentFont:
  case mainFont:
    for (pp = myFormNode->data.FIP.up; pp; pp = pp->data.FIP.up) {
      if (pp->data.ownLFont == ownFont) {
        if (win)
          win->setFont(pp->data.LFont);
        return parentFont;
      }
      else if (pp->data.ownLFont == mainFont) {
        if (Font && win)
          win->setFont(LBaseData->m_FormLabelFont);
        return mainFont;
      }
      else
        if (!win)
          return pp->data.ownLFont;
    }
    break;
  }
  if (Font && win)
    win->setFont(LBaseData->m_FormLabelFont);
  return mainFont;
} 

TFontCase CGUIMet::SetTFont(QWidget* win, CHEFormNode* myFormNode)
{
  CHEFormNode *pp;
  switch (myFormNode->data.ownTFont) {
  case ownFont:
    if (win)
      win->setFont(myFormNode->data.TFont);
    return ownFont;
  case parentFont:
  case mainFont:
    for (pp = myFormNode->data.FIP.up; pp; pp = pp->data.FIP.up) {
      if (pp->data.ownTFont == ownFont) {
        if (win)
          win->setFont(pp->data.TFont);
        return parentFont;
      }
      else if (pp->data.ownTFont == mainFont) {
        if (Font && win)
          win->setFont(*Font);
        return mainFont;
      }
      else
        if (!win)
          return pp->data.ownTFont;
    }
    break;
  }
  if (Font && win)
    win->setFont(*Font);
  return mainFont;
} 

void CGUIMet::SetColor(QWidget* w, CHEFormNode * node)
{
  if (node->data.ColorBValid)
    w->setPaletteBackgroundColor(node->data.ColorB);
  if (node->data.ColorFValid)
    w->setPaletteForegroundColor(node->data.ColorF);
  CHETAnnoEx *exT, *exPB;
  exT = node->data.FormSyntax->Annotation.ptr->GetAnnoEx(anno_TextColor);
  exPB = node->data.FormSyntax->Annotation.ptr->GetAnnoEx(anno_PBColor);
  if (exT && (exT->data.RgbBackValid || exT->data.RgbForeValid)
    || exPB && (exPB->data.RgbBackValid || exPB->data.RgbForeValid)) {
    QPalette p = w->palette();
    if (exT) {
      if (exT->data.RgbBackValid) 
        p.setColor( QPalette::Active, QColorGroup::Base, QColor(exT->data.RgbBackColor));
      if (exT->data.RgbForeValid) 
        p.setColor( QPalette::Active, QColorGroup::Text, QColor(exT->data.RgbForeColor));
    }
    exPB = node->data.FormSyntax->Annotation.ptr->GetAnnoEx(anno_PBColor);
    if (exPB) {
      if (exPB->data.RgbBackValid) 
        p.setColor( QPalette::Active, QColorGroup::Button, QColor(exPB->data.RgbBackColor));
      if (exPB->data.RgbForeValid) 
        p.setColor( QPalette::Active, QColorGroup::ButtonText, QColor(exPB->data.RgbForeColor));
    }
    w->setPalette(p);
  }
}


unsigned CGUIMet::aveCharWidth(const QFont& font)
{
  QFontMetrics metric(font);
  return metric.width('n');
}

unsigned CGUIMet::maxCharWidth(const QFont& font)
{
  QFontMetrics metric(font);
  return metric.width('W');
}

QSize CGUIMet::ArrowSize(const QFont& font)
{
  QFontMetrics metric(font);
  return CalcTextRect(3, 1, font);
}

QSize CGUIMet::MinSizeComboBox(const QFont& font)
{
  QFontMetrics metric(font);
  return CalcTextRect(6,1,font);
}

unsigned CGUIMet::itemHeight(const QFont& font)
{
  QFontMetrics metric(font);
  return metric.height();
}

/*
unsigned CGUIMet::itemDist(const QFont& font)
{
  QFontMetrics metric(font);
  return metric.lineSpacing();
}
*/

QSize CGUIMet::CalcStringRect(const QString& label, const QFont& font)
{
  QFontMetrics fm(font);
  QSize size(fm.width(label, -1)+2, fm.height()+2);
  return size;
}

QSize CGUIMet::CalcTextRect(int xcols, int yrows, const QFont& font)
{
  QFontMetrics fm(font);
  QSize size(xcols * maxCharWidth(font), yrows * fm.lineSpacing());
  return size;
}
int CGUIMet::GetLineWidth(QWidget *win)
{
  if (win->inherits("QFrame"))
    return ((QFrame*)win)->lineWidth();
  if (win->inherits("QGroupBox"))
    return 1;
  return 0;
}

QString CGUIMet::winCaption()
{
  if (ViewWin->inherits("CLavaGUIView"))
    return ((wxView*)ViewWin)->GetParentFrame()->caption();
  else
    return ((LavaGUIDialog*)ViewWin)->caption();
}

 /*
QSize CGUIMet::CalcToolStringRect(const QString& label, QWidget* win)
{
  CClientDC* ToolDC = new CClientDC(win);
  //if (ToolFont) 
    //ToolDC->SelectObject (ToolFont);
  QSize size = QSize(0,0);
  if (label)
    size = ToolDC->GetTextExtent(label, strlen(label));
  delete ToolDC;
  return size;
}
  */


CGUIMet::~CGUIMet()
{
  //delete TIPDC;
}

void CGUIMet::ScrollIntoFrame(QWidget *win)
{
  QWidget *parentWin;
  QRect pRect, rect = win->geometry();
  for (parentWin = win->parentWidget();
      !parentWin->inherits("GUIVBox");
      parentWin = parentWin->parentWidget()) {
    pRect = parentWin->geometry();
    rect.setLeft(rect.left() + pRect.left());
    rect.setTop(rect.top() + pRect.top());
  }
  if (parentWin)
    ((GUIVBox*)parentWin)->View->ensureVisible(rect.left(), rect.top());
}

