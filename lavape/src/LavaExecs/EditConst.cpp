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


#include "Constructs.h"
#include "ExecView.h"
#include "Scanner.h"
#include "LavaBaseStringInit.h"
#include "qstring.h"
#include "qmessagebox.h"
#include "qapplication.h"
#include "qlayout.h"
//Added by qt3to4:
#include <QFocusEvent>
#include <QKeyEvent>

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
// MiniEdit

extern void *focusWindow;
extern bool isExecView;


MiniEdit::MiniEdit(ExecContents *execCont) : QLineEdit("MiniEdit",execCont) {  
  execView = execCont->execView;
  returnPressed = false;
}

int MiniEdit::frameWidth() {
  QSize frSize=frameSize(), totalSize=size();

  return totalSize.width() - frSize.width();
}


VarConstCheck MiniEdit::InputIsCorrect(TToken &token, CComboBar *comboBar) 
{
  // TODO: Add your control notification handler code here

  QString txt;
  VarConstCheck rc=correct;
  int pos;
  QString *msgID;

  if (!isModified())
    return noChange;

  txt = text();
  if (txt.isEmpty())
    return noChange;

  if (!IsOK(txt,token,pos,msgID,comboBar)) {
    setCursorPosition(pos);
    QMessageBox::critical(this,qApp->applicationName(),*msgID);
    rc = wrong;
  }

  setFocus();
  return rc;
}

void MiniEdit::OnUpdate() 
{
  // TODO: Add your control notification handler code here
  QString txt;

  txt = text();
  if ((int)txt.length() >= maxLength())
    setMaxLength(maxLength()+5);
}

void MiniEdit::keyPressEvent (QKeyEvent *ev)
{
  // TODO: Add your message handler code here and/or call default

  switch (ev->key()) {
  case Qt::Key_Escape:
    execView->escapePressed = true;
    execView->EditOK();
    execView->escapePressed = false;
    execView->redCtl->miniEditRightEdge = 0;
    break;
  case Qt::Key_Up:
  case Qt::Key_Return:
    returnPressed = true;
    execView->EditOK();
    returnPressed = false;
    execView->redCtl->miniEditRightEdge = 0;
    break;
  case Qt::Key_Left:
  case Qt::Key_Right:
    QLineEdit::keyPressEvent(ev);
    break;
  case Qt::Key_Backspace:
  case Qt::Key_Delete:
  default:
    QLineEdit::keyPressEvent(ev);
    int w = fontMetrics().width(text()+"w");
    setFixedWidth(w);
    execView->redCtl->miniEditRightEdge = geometry().right();
    execView->redCtl->update();
  }
}

void MiniEdit::focusInEvent(QFocusEvent *ev) 
{
  QLineEdit::focusInEvent(ev);
  
  // TODO: Add your message handler code here
  focusWindow = (void*)this;
  isExecView = false; 
}

void MiniEdit::focusOutEvent(QFocusEvent *ev) 
{
  QLineEdit::focusOutEvent(ev);

  // TODO: Add your message handler code here
  focusWindow = 0;
}
