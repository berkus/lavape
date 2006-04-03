#ifndef __LavaMenu
#define __LavaMenu


#include "GUIProgBase.h"
#include "qlineedit.h"
//Added by qt3to4:
#include <QMenu>


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
  QMenu *myMenu;
  CHEFormNode* myFormNode;
  CHEFormNode* userItemData[100];
  bool isEnter;
  unsigned count;
private:
  Q_OBJECT



};
#endif
