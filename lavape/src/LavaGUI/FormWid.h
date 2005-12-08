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
           QWidget* pParentWnd, int border, const char* lpszWindowName); //Creates a 2x2 pane
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

};

/////////////////////////////////////////////////////////////////////////////

#endif
