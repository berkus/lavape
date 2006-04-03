#ifndef __TButton
#define __TButton


#include "GUIProgBase.h"
#include "qradiobutton.h"
#include "qpushbutton.h"
#include "qcheckbox.h"
#include "qevent.h"
//Added by qt3to4:
#include <QFocusEvent>


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

};

/////////////////////////////////////////////////////////////////////////////

#endif
