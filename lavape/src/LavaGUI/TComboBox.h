#ifndef __TComboBox
#define __TComboBox

#include "GUIProgBase.h"
#include "qcombobox.h"
//#include "q3listbox.h"
#include "qevent.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QFocusEvent>
#include <QMenu>
#include <QComboBox>


/////////////////////////////////////////////////////////////////////////////
// CTComboBox window

class CComboItem//: public Q3ListBoxText
{
public:
  CComboItem(/*const DString& text,*/ CHEFormNode* data) {/*setText(QString(text.c));*/ Data = data;}
  CHEFormNode* itemData() {return Data;}
protected:
  CHEFormNode* Data;
};

class CTComboBox : public QComboBox
{
public:
  CGUIProgBase *GUIProg;
  CTComboBox();
  CTComboBox(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* pParentWnd, char* WindowName, DString& defaultSel, unsigned width);
  virtual ~CTComboBox();

  bool isEnter;
  CHEFormNode* myFormNode;
  DString DefaultSel;
  QMenu* myMenu;

  void AddItem(const DString& label, CHEFormNode* data);
  virtual void focusInEvent(QFocusEvent *ev);
  void mousePressEvent(QMouseEvent* ev);
  void focusOutEvent(QFocusEvent *ev);
  //virtual bool event(QEvent* ev);

public slots:
  void OnSelendok(int pos);
  //void DelActivated();
private:
  Q_OBJECT

};

/////////////////////////////////////////////////////////////////////////////

#endif
