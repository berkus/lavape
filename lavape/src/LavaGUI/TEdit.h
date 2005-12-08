#ifndef __TEdit
#define __TEdit

#include "GUIProgBase.h"
#include "qlineedit.h"
#include "qtextedit.h"


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
  QPopupMenu* myMenu;
  bool inError;
  virtual void focusInEvent(QFocusEvent *ev);
  virtual void focusOutEvent(QFocusEvent *ev);
  virtual QPopupMenu* createPopupMenu();
  virtual bool event(QEvent* ev);

public slots:
  void DelActivated();

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
  QPopupMenu* myMenu;
  bool inError;
  virtual void focusInEvent(QFocusEvent *ev);
  virtual void focusOutEvent(QFocusEvent *ev);
  virtual QPopupMenu* createPopupMenu();
  virtual bool event(QEvent* ev);
public slots:
  void DelActivated();

private:
  Q_OBJECT

};

/////////////////////////////////////////////////////////////////////////////


#endif
