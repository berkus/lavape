#ifdef _GNUG__
#pragma interface
#endif

// TComboBox.h : header file
//

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

  //bool CreateOptM(QWidget* pParentWnd, QString* WindowName, QString* defaultSel, unsigned width);
  void AddItem(const DString& label, CHEFormNode* data);
  bool isEnter;
  CHEFormNode* myFormNode;
  DString DefaultSel;
  virtual void focusInEvent(QFocusEvent *ev);
  void mousePressEvent(QMouseEvent* ev);

public slots:
  void OnSelendok(int pos);
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
