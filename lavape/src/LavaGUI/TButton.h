#ifndef __TButton
#define __TButton


#include "GUIProgBase.h"
#include "qradiobutton.h"
#include "qpushbutton.h"
#include "qcheckbox.h"
#include "qevent.h"


/////////////////////////////////////////////////////////////////////////////
// 

enum TDISCOButtonType {isToggle, isPush, isRadio, isElli, isPop}; 

class CRadioButton : public QRadioButton
{
public:
  CRadioButton(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, const char* lpszWindowName, const QString& label, QWidget* radioBox, unsigned num);
  virtual ~CRadioButton() {}
  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  CHEFormNode* EnumNode;
  QWidget * Radio;  //radioBox
  TDISCOButtonType DISCOButtonType;
  void focusInEvent(QFocusEvent *ev);
public slots:
  void OnClicked();
private:
  Q_OBJECT

};

class CPushButton : public QPushButton
{
public:
  CPushButton(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, const char* lpszWindowName, const QString& label, unsigned cmnd);
  CPushButton(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, const char* lpszWindowName, const QString& label, QWidget* radioBox, unsigned num);
  CPushButton(bool withPix, CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, const char* lpszWindowName, const QString& label, QWidget* radioBox, unsigned num);
  virtual ~CPushButton() {}

  //bool CreateElli(LPCTSTR lpszWindowName, QWidget* pParentWnd, const QString& label, unsigned cmnd);
  //bool CreatePop(LPCTSTR lpszWindowName, QWidget* pParentWnd, const QString& label, unsigned cmnd);
  //bool CreatePush(LPCTSTR lpszWindowName, QWidget* pParentWnd, const QString& label, QWidget* radioBox, unsigned num);

  void focusInEvent(QFocusEvent *ev);
  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  CHEFormNode* EnumNode;
  CHEFormNode* IterNode;
  unsigned Cmnd;
  QWidget * Radio;  //radioBox
  TDISCOButtonType DISCOButtonType;
public slots:
  void OnClicked();
private:
  Q_OBJECT


};

class CToggleButton : public QCheckBox
{
public:
  CToggleButton(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parent, const char* lpszWindowName,
    const QString& label, bool leftText=false);
  virtual ~CToggleButton() {}

 // bool CreateToggle(QWidget* parent, LPCTSTR label, bool leftText=FALSE);

  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  CHEFormNode* EnumNode;
  QWidget * Radio;  //radioBox
  TDISCOButtonType DISCOButtonType;
  virtual void focusInEvent(QFocusEvent *ev);
public slots:
  void OnClicked();
private:
  Q_OBJECT


/*
  // Generated message map functions
protected:
  //{{AFX_MSG(CTButton)
  afx_msg void OnClicked();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKillFocus(QWidget* pNewWnd);
  afx_msg void OnSetFocus(QWidget* pOldWnd);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
  */
};

/////////////////////////////////////////////////////////////////////////////

#endif
