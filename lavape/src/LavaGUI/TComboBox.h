#ifndef __TComboBox
#define __TComboBox

#include "GUIProgBase.h"
#include "qcombobox.h"
#include "qlistbox.h"
#include "qevent.h"


/////////////////////////////////////////////////////////////////////////////
// CTComboBox window

class CComboItem: public QListBoxText
{
public:
  CComboItem(const DString& text, CHEFormNode* data) {setText(QString(text.c)); Data = data;}
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
  QPopupMenu* myMenu;

  void AddItem(const DString& label, CHEFormNode* data);
  virtual void focusInEvent(QFocusEvent *ev);
  void mousePressEvent(QMouseEvent* ev);
  virtual bool event(QEvent* ev);
public slots:

public slots:
  void OnSelendok(int pos);
  void DelActivated();
private:
  Q_OBJECT

/*
  // Generated message map functions
protected:
  //{{AFX_MSG(CTComboBox)
  afx_msg void OnSelendok();
  afx_msg void OnSetFocus(QWidget* pOldWnd);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
*/
};

/////////////////////////////////////////////////////////////////////////////

#endif
