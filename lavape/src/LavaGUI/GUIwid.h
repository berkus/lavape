#ifndef __Widgets
#define __Widgets


#include "SYSTEM.h"
#include "GUIProgBase.h"

#include "UNIX.h"
#include "qwidget.h"

class GUIwidCLASS {

public:
CGUIProgBase *GUIProg;
QRect *pMaxBottomRight;

void ReallyDestroyWidget(QWidget* widget);
//void Ancestors(QWidget* widget, QWidget* & window, QWidget* & paneTopForm);
//void GetSize (QWidget* widget, unsigned &height, unsigned &width, unsigned &borderWidth);
//void getSize(address widget, int& h, int& w, int& bw);
//void getPos (address widget, int& x,int& y);
//void setSize(address widget, int lines, int cols);
//void count (DString& text, int *lines, int *cols);
//void ResizePane (address paneTopForm, bool isTextPopup);
void AddOptionMenuItem (QWidget* & pos, QWidget* menu, DString& text, CHEFormNode*  data);
void AddPopupMenuItem (QWidget* & pos, QWidget* menu, DString& text, CHEFormNode*  data);
void CreatePopupWindow (bool isTextPopup,
                                   QWidget*& popupShell,
                                   QWidget*& pane, /* viewport widget returned */
                                   DString& label, CHEFormNode* data);

void CreateFormWidget (QWidget*& formWidget, QWidget* parentWidget, int box, SynFlags iterFlags, DString& widgetName, CHEFormNode* data);
void CreateTextWidget (QWidget*& textWidget, QWidget* parentWidget, DString& widgetName,
                   int rows, int cols, 
                   bool echo, CHEFormNode* data);
void CreateLabelWidget (QWidget* & labelWidget, QWidget*  parentWidget, DString& text, DString& widgetName, CHEFormNode* data);
void CreateToggleWidget (QWidget*& toggleWidget, QWidget*  parentWidget, DString& widgetName, DString& label, bool leftLabel, CHEFormNode* data);
void CreateButtonMenuButton (QWidget* & toggleWidget, QWidget*  parentWidget, DString& widgetName,
                             DString& text, QWidget* & radioBox, bool enterFlag, CHEFormNode*  node);
void CreateEllipsisWidget (QWidget* & ellipsisWidget, QWidget* parentWidget,
                           DString& widgetName, DString& label, unsigned cmnd, CHEFormNode* data);
void CreateExternalEllipsis (QWidget* & ellipsisWidget,
                           DString& widgetName, DString& label, unsigned cmnd, CHEFormNode* data);
void CreatePopupWidget (QWidget*& popupWidget, QWidget*  parentWidget, DString& widgetName,
                        DString& label, CHEFormNode* data);
void CreateOptionMenuWidget (QWidget*& widget, QWidget* & menu, QWidget*  parentWidget, DString& widgetName, 
                             DString& defaultSel, unsigned width, CHEFormNode* data);
void CreatePopupMenuWidget (QWidget*& widget,QWidget* & popupMenu,QWidget*  parentWidget,
                       DString& widgetName, DString& label, unsigned width, CHEFormNode* data);

void PopupWindow (QWidget* widget);
void PopdownWindow (QWidget* widget);
//void DeleteWindow (QWidget* window);
void SetText (QWidget* widget, DString& text);
void SetInsertPos (QWidget* widget, unsigned pos);
void SetTopLeft (QWidget*  widget,QWidget*  leftNeighbour,int horizDist,QWidget* upperNeighbour,int vertDist);
void GrowParent (QWidget* widget);
void SetLabel (QWidget* widget, DString& label);
//void SetCursor (address  widget, unsigned ind);
void SetOptionDefault (QWidget*  menuWidget, const QWidget* entry, DString& labelText);
void SetToggleState (QWidget*  widget, bool state);
//void SetSize (QWidget*  widget, unsigned width, unsigned height);
//int Modified (QWidget*  widget);
//void GetText (QWidget*  widget,DString& text);
void ClearFocus ();
//void SetFocusNow ();
void SetFocus (QWidget* & oldWidget,QWidget*  newWidget);
void SetSensitive (QWidget* widget, int on);
void SetPointer (QWidget*  newWidget, unsigned isWindow);


};

#endif
