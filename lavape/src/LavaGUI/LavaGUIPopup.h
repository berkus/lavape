#ifdef _GNUG__
#pragma interface
#endif


/////////////////////////////////////////////////////////////////////////////
// CLavaGUIPopup window

#include "LavaGUIView.h"
#include "GUIProgBase.h"
#include "mdiframes.h"


//extern word PopupShellID;
//extern word ChainToolFrameID;

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

