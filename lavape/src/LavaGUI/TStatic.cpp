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


#include "TStatic.h"
#include "GUIProgBase.h"
#include "qwidget.h"
//Added by qt3to4:
#include <QLabel>
#include <QFrame>
#include "FormWid.h"

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
// CTStatic


CTStatic::~CTStatic()
{
}


CTStatic::CTStatic(CGUIProgBase *guiPr, CHEFormNode* data,
                   QWidget* parentWidget, const QString& label, const char* widgetName )
:QLabel(label, parentWidget, "TStatic")
{
  myFormNode = data;
  GUIProg = guiPr;
  int bord = GUIProg->GetLineWidth(parentWidget);
  myFormNode->data.ownLFont = GUIProg->SetLFont(this, myFormNode);
  QSize size = GUIProg->CalcStringRect(label, font());
  setGeometry(bord,bord, size.width(), size.height());
  GUIProg->SetColor(this, myFormNode, QPalette::Window, QPalette::WindowText);
  setAutoFillBackground(true);
  if (!myFormNode->data.ColorFValid) {
    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::WindowText, parentWidget->palette().color(QPalette::Active, QPalette::WindowText));
    p.setColor(QPalette::Inactive, QPalette::WindowText, parentWidget->palette().color(QPalette::Active, QPalette::WindowText));
    setPalette(p);
    //setPaletteForegroundColor(parentWidget->parentWidget()->colorGroup().foreground());
  }
  show();
}

