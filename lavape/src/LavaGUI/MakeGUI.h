#ifndef __MakeGUI
#define __MakeGUI


#include "SYSTEM.h"
#include "DString.h"
#include "Syntax.h"
#include "GUIProgBase.h"
#include "GUIwid.h"
//#include "q3scrollview.h"


enum TMenu {
  isButtonMenu,
  isOptionMenu,
  isPopupMenu};


class LAVAGUI_DLL MakeGUICLASS : public GUIwidCLASS {

public:
  MakeGUICLASS() {radioBox = 0;}
  ~MakeGUICLASS() {}
  bool refreshWidgets;
  virtual void INIT (CGUIProgBase *guiPr);

  void DisplayScreen (bool suppressed);
  void SetScrollSizes(QScrollArea* view);
  void RemakeParent (CHEFormNode* chFrmNd);
  void AppendIterWidget (CHEFormNode* firstnode);
  void Popup (CHEFormNode* popupNode,
              bool popupWindow,
              bool cursorOnField,
              bool redraw = false);

  /*------- PopDown ----------------------------------------------------------*/
  // All poppedUp nodes beginning from fmno are popped-dowm
  // For popdownNode setFocus is called.
  void PopDown (CHEFormNode* fmno, CHEFormNode* popdownNode, bool& ok);
  void SetFIPwidgetNULL (CHEFormNode* chFrmNd);
  void ShowDeleteButton (CHEFormNode* ellipsisNode,bool yes);
  void VisibleDeleteButton (CHEFormNode* trp, bool maWi);
  bool setFocus (unsigned pos, CHEFormNode* trpn);

  void Cursor (bool CurInOri, bool sameOri);
  /*------- Cursor
   *
   * moves the cursor to row/col of current screen section.
   * If sameOri is valid the screen origin is changed too.
   * If row/col is part of an unprotected field in dependence pressed PF-Key
   * the node containing this  position as input field is determined.
   */



  void CursorOnField (CHEFormNode* trp);
  /*------- CursorOnField
   *
   * used by AppenIterItem/DeleteIterItem/SelectCase/Scroll..
   * Provides/moves the cursor to  node trp  of screen section sp preparing
   * the screen by a DisplaySection call  in suppressed phase.
   * If not the whole node has place on the real  screen section in dependence
   * of scroll mode the form is displayed such that node trp and section
   * coincide in the
   * - left  upper corner  (logical   scrolling)
   * - right lower corner  (phys/curs scrolling)
   * such that the field trp is still visible.
   */


  CHEFormNode* CursorToEllipsis (CHEFormNode* trp);
  /*--------CursorToEllipsis
   *
   * Recursive procedure which provides the last iterated Item (successor)
   * in the current form starting in trp
   */

/**********************************************************************/

private:

  struct frameContext {
    QWidget* precedingFrame, *currentFrame, *precedingWidgetInFrame;
    int indent;
    bool emptyFrame;
  };

  enum {
    none,
    beforeField,
    beforeMenu,
    beforeMenuLine,
    beforeSelCode,
    beforeExplanText} makingMenu;
    
  DString selCode, menuSelCode, menuButtonLabel;
  TMenu menuType;
  bool verticalMenu;
  QWidget *radioBox, *menuWidget, *menuButtonWidget;
  DString ellipsisNameDEL, booleanName, questionMarks;
  DString string, blank2;
  DString blank4;

  void makeOptionMenu (CHEFormNode* chFrmNd);
  void makePopupMenu (CHEFormNode* chFrmNd);
  void makeWidgets (CHEFormNode* chFrmNd,
                    QWidget* parentWidget,
                    DString& parentWidgetName,
                    bool& empty);

  void makeWidget (CHEFormNode* chFrmNd,
                   QWidget* parentWidget,
                   DString& parentWidgetName,
                   frameContext& context);

  DString abbrevContents (DString& vls);
//  void annotations (CHEFormNode* chFrmNd, address widget);
  void getLastRealizedNode (CHEFormNode* chFrmNd, CHEFormNode*& lastRealizedNode);
  //void getNextRealizedNode (CHEFormNode* chFrmNd, CHEFormNode*& nextRealizedNode);
  void getPrecedingFramesNeighbourData (CHEFormNode* chFrmNd, QWidget*& refFrame,
                                        unsigned& frameSpacing);
  //void deleteSiblings (CHEFormNode* chFrmNd);
  //void permute (CHEFormNode* chFrmNd);
  void AtomicWidget (CHEFormNode* chFrmNd,
                     QWidget* parent,
                     QWidget*& newWidget,
                     DString& widgetName);
  void ImplicitPopup (CHEFormNode* popup, bool show);
};


#endif
