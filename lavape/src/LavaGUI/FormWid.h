#ifndef __FormWid
#define __FormWid

#include "TButton.h"
#include "GUIProgBase.h"
#include "qwidget.h"
#include "qpainter.h"
#include "qgroupbox.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QFrame>
#include <QEvent>
#include <QMenu>
#include <QPushButton>


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
  //CHEFormNode* myHandlerNode;
  SynFlags IterFlags;
  QColor ForegroundColor;

 //the FormWidget may be used as a (radio) button menu box
  int nRadio;
  QPushButton *Radio [100];
  QGroupBox *BGroup;

  void AddRadio(QPushButton* radio);

  //the FormWidget may be a chain element widget which has a popup menu
  QMenu* myMenu;
  bool origMenu;
  bool usedInFormNode;
/*public slots:
  void InsActivated();
  void DelActivated();*/
private:
  Q_OBJECT

};

/////////////////////////////////////////////////////////////////////////////

#endif
