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
#include "qgroupbox.h"

/////////////////////////////////////////////////////////////////////////////
// CFormWid window
class CLavaGroupBox;

class CFormWid : public QFrame//QWidget
{
public:
  CFormWid(CGUIProgBase *guiPr, CHEFormNode* data,
           QWidget* pParentWnd, int border, const char* lpszWindowName); //Creates a 2x2 pane
  void contextMenuEvent(QContextMenuEvent* ev);
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
  CLavaGroupBox *BGroup;

  void AddRadio(QPushButton* radio);

  //the FormWidget may be a chain element widget which has a popup menu
  //QMenu* myMenu;
  bool hasMenu;
  bool hasFuncMenu;
  bool usedInFormNode;

private:
  Q_OBJECT

};

class CLavaGroupBox : public QGroupBox
{
public:
  CLavaGroupBox(CGUIProgBase *guiPr, CHEFormNode* data, QString label, QWidget* pParentWnd); 
  CLavaGroupBox(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* pParentWnd); 
  void contextMenuEvent(QContextMenuEvent* ev);
  virtual bool event(QEvent* ev);
  void InitGroupBox(CGUIProgBase *guiPr, CHEFormNode* data);
  virtual ~CLavaGroupBox() {}

  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  SynFlags IterFlags;
  bool hasMenu;
  bool hasFuncMenu;

private:
  Q_OBJECT

};

/////////////////////////////////////////////////////////////////////////////

#endif
