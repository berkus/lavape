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


/////////////////////////////////////////////////////////////////////////////
// MiniEdit

extern void *focusWindow;
extern bool isExecView;


MiniEdit::MiniEdit(CExecView *execView) : QLineEdit(execView->sv,"MiniEdit") {  
  conView = execView;
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

  if (!edited())
    return noChange;

  txt = text();
  if (txt.isEmpty())
    return noChange;

  if (!IsOK(txt,token,pos,msgID,comboBar)) {
    setCursorPosition(pos);
    QMessageBox::critical(this,qApp->name(),*msgID,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
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
    conView->escapePressed = true;
    conView->EditOK();
    conView->escapePressed = false;
    break;
  case Qt::Key_Up:
  case Qt::Key_Return:
    returnPressed = true;
    conView->EditOK();
    returnPressed = false;
    break;
  case Qt::Key_Backspace:
  case Qt::Key_Delete:
    QLineEdit::keyPressEvent(ev);
    break;
  case Qt::Key_X:
    QLineEdit::keyPressEvent(ev);
    break;
  default:
    QLineEdit::keyPressEvent(ev);
    int w = fontMetrics().width(text()+" ");
    if (w > width()) {
      setFixedWidth(w+2*1);//frameWidth());
      QRect mw_rect = geometry();
      int c_x, c_y;
      conView->sv->viewportToContents(mw_rect.right(),mw_rect.top(),c_x,c_y);
      conView->sv->contentsWidth = QMAX(conView->sv->contentsWidth,c_x);
      conView->sv->resizeContents(conView->sv->contentsWidth,conView->sv->contentsHeight);
      conView->sv->viewport()->update();
      conView->sv->update();
    }
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
