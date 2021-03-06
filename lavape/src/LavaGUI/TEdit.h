#ifndef __TEdit
#define __TEdit

#include "GUIProgBase.h"
#include "qlineedit.h"
#include <QTextEdit>
//Added by qt3to4:
#include <QFocusEvent>
#include <QEvent>
#include <QMenu>
#include <QContextMenuEvent>

/////////////////////////////////////////////////////////////////////////////
// CTEdit window

class CTEdit : public QLineEdit
{
public:
  CTEdit(CGUIProgBase *guiPr, CHEFormNode* data,
    QWidget* parentWidget, const char* widgetName, QSize size, bool withEcho);
  virtual ~CTEdit();

  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  QPoint CaretP;
  bool enableInsert;
  bool enableDelete;
  bool enableFunc;
  bool hasMenu;
  bool hasFuncMenu;
  bool inError;
  virtual void focusInEvent(QFocusEvent *ev);
  virtual void focusOutEvent(QFocusEvent *ev);
  virtual void contextMenuEvent(QContextMenuEvent * e);

private:
  Q_OBJECT

};

/////////////////////////////////////////////////////////////////////////////
// CTEdit window

class CMultiLineEdit : public QTextEdit
{
public:
  CMultiLineEdit(CGUIProgBase *guiPr, CHEFormNode* data,
    QWidget* parentWidget, const char* widgetName, QSize size, bool withEcho);
  virtual ~CMultiLineEdit() {}

  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  QPoint CaretP;
  bool enableInsert;
  bool enableDelete;
  bool enableFunc;
  bool hasMenu;
  bool hasFuncMenu;
  bool inError;
  virtual void focusInEvent(QFocusEvent *ev);
  virtual void focusOutEvent(QFocusEvent *ev);
  virtual void contextMenuEvent(QContextMenuEvent * e);
  //virtual QMenu* createStandardContextMenu();

private:
  Q_OBJECT

};

#endif
