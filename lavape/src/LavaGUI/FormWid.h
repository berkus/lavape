#ifndef __FormWid
#define __FormWid

#include "TButton.h"
#include "GUIProgBase.h"
#include "qwidget.h"
#include "qpainter.h"
#include "q3groupbox.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QFrame>
#include <QEvent>
#include <QMenu>
#include <Q3Button>


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
  Q3Button *Radio [100];
  Q3GroupBox *BGroup;

  void AddRadio(Q3Button* radio);

  //the FormWidget may be a chain element widget which has a popup menu
  QMenu* myMenu;
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
