#ifdef _GNUG__
#pragma interface
#endif

// FormWid.h : header file
//

#ifndef __FormWid
#define __FormWid

#include "TButton.h"
#include "GUIProgBase.h"
#include "qwidget.h"
#include "qpainter.h"
#include "qgroupbox.h"

/////////////////////////////////////////////////////////////////////////////
// CFormWid window

class CFormWid : public QFrame//QWidget
{
public:
  CFormWid(CGUIProgBase *guiPr, CHEFormNode* data,
           QWidget* pParentWnd, int border, SynFlags iterFlags,
           const char* lpszWindowName); //Creates a 2x2 pane
  void mousePressEvent(QMouseEvent* ev);
  virtual bool event(QEvent* ev);

  virtual ~CFormWid();
  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  CHEFormNode* iterData;
  SynFlags IterFlags;
  QColor ForegroundColor;

 //the FormWidget may be used as a (radio) button menu box
  int nRadio;
  QButton *Radio [100];
  QGroupBox *BGroup;

  void AddRadio(QButton* radio);

  //the FormWidget may be a chain element widget which has a popup menu
  QPopupMenu* myMenu;
  bool origMenu;
  bool usedInFormNode;
public slots:
  void InsActivated();
  void DelActivated();
private:
  Q_OBJECT

/*

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFormWid)
  protected:
  virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

  //{{AFX_MSG(CFormWid)
//  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnRButtonDown(UINT nFlags, QPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, QPoint point);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  */
};

/////////////////////////////////////////////////////////////////////////////

#endif
