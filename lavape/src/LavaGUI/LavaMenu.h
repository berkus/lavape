#ifndef __LavaMenu
#define __LavaMenu


#include "GUIProgBase.h"
#include "qlineedit.h"


/////////////////////////////////////////////////////////////////////////////
// CLavaMenu window

//the DISCO Popup-menu is realized as CEdit-window wich has a popup CMenu-window
class CLavaMenu : public QLineEdit
{
public:
  CLavaMenu(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* pParentWnd, 
    const char* WindowName, QString label, int cx);
  virtual ~CLavaMenu();

  //bool CreateM(LPCTSTR label, QWidget* pParentWnd, LPCTSTR WindowName, int cx);
  void SetItemData(unsigned pos, CHEFormNode* data);
    
  CGUIProgBase *GUIProg;
  QPopupMenu *myMenu;
  CHEFormNode* myFormNode;
  CHEFormNode* userItemData[100];
  bool isEnter;
  unsigned count;
private:
  Q_OBJECT

/*
// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLavaMenu)
  protected:
  virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

protected:
  //{{AFX_MSG(CLavaMenu)
  afx_msg void OnLButtonDown(UINT nFlags, QPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, QPoint point);
  afx_msg void OnSetFocus(QWidget* pOldWnd);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
  */
};

/////////////////////////////////////////////////////////////////////////////

#endif
