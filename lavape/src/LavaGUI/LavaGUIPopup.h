#ifndef _LavaGUIPopup
#define _LavaGUIPopup


/////////////////////////////////////////////////////////////////////////////
// CLavaGUIPopup window

#include "LavaGUIView.h"
#include "GUIProgBase.h"
#include "mdiframes.h"
//Added by qt3to4:
#include <QCloseEvent>

#undef QMainWindow

class CLavaGUIPopup : public QMainWindow 
{
public:
  CLavaGUIPopup(QWidget *parent, CGUIProgBase *guiPr, CHEFormNode* data);
  virtual ~CLavaGUIPopup();
  bool hasFocNode(CHEFormNode *actFNode);
  GUIScrollView * view;
  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  void closeEvent(QCloseEvent *e);
private:
  Q_OBJECT

};

class CLavaGUIPopupD : public QDialog
{
public:
  CLavaGUIPopupD(QWidget *parent, CGUIProgBase *guiPr, CHEFormNode* data);
  virtual ~CLavaGUIPopupD();
  bool hasFocNode(CHEFormNode *actFNode);
  GUIScrollView * view;
  CGUIProgBase *GUIProg;
  CHEFormNode* myFormNode;
  void closeEvent(QCloseEvent *e);
  LavaVariablePtr StartDataPtr;
  LavaVariablePtr ResultDPtr;

public slots:
  void on_cancelButton_clicked();
//  void OnCancel();
private:
  Q_OBJECT

};

#endif

