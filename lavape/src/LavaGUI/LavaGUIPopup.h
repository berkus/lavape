#ifndef _LavaGUIPopup
#define _LavaGUIPopup

#ifdef _GNUG__
#pragma interface
#endif


/////////////////////////////////////////////////////////////////////////////
// CLavaGUIPopup window

#include "LavaGUIView.h"
#include "GUIProgBase.h"
#include "mdiframes.h"

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
  void moveEvent(QMoveEvent* ev);
private:
  Q_OBJECT

};

#endif

