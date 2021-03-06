#ifndef __GUIProgBase
#define __GUIProgBase

#include "SynIO.h"
#include "Syntax.h"
#include "DString.h"
#include "LavaBaseDoc.h"
#include "SynIDTable.h"
#include "qsize.h"
#include "qwidget.h"
#include "qscrollarea.h"
#include "qlist.h"
#include <QAction>

#ifdef WIN32
#ifdef LAVAGUI_EXPORT
#define LAVAGUI_DLL __declspec( dllexport )
#else
#define LAVAGUI_DLL __declspec( dllimport )
#endif
#else
#define LAVAGUI_DLL
#endif


class  LAVAGUI_DLL CGUIMet : public QObject
{ public:

  QWidget* ViewWin;  //CLavaGUIView or LavaGUIDialog
  bool isView;
  QScrollArea* scrView;
  CHEFormNode* DelNode;
  CHEFormNode* InsertNode;
  CHEFormNode* ActNode;
  CLavaBaseDoc *myDoc;
  LavaDECL *myDECL;
  LavaVariablePtr ServicePtr;
  CheckData ckd;
  int FrozenObject;
  int fromFillIn;
  QFont *Font; //this Object is allocated by the application and may not be destroyed

  unsigned nID;
  unsigned globalIndent;
  unsigned xMargin;
  unsigned yMargin;

  QWidget* oldFocus;
  QWidget* newFocus;
  QAction* delActionPtr;
  QAction* insActionPtr;
  QAction* newHandlerActionPtr;
  QAction* attachHandlerActionPtr;
  QList<LavaObjectPtr> allocatedObjects; //GUI-service objects

  CGUIMet() {}
  ~CGUIMet();
  QString winCaption();
  void SetFont(QFont *font);
  TFontCase SetTFont(QWidget* win, CHEFormNode* myFormNode);
  TFontCase SetLFont(QWidget* win, CHEFormNode* myFormNode);
  void SetColor(QWidget* w, CHEFormNode * node, QPalette::ColorRole brol, QPalette::ColorRole frol);
  virtual void Create();
//  void SetDC(QWidget* win);
  unsigned itemHeight(const QFont& font);
//  unsigned itemDist(const QFont& font); //the y-distance
  unsigned aveCharWidth(const QFont& font);
  unsigned maxCharWidth(const QFont& font);
  QSize ArrowSize(const QFont& font);       //the arrow size add on to combobox size
  QSize MinSizeComboBox(const QFont& font); //the CEdit and a minimum Listbox size
  QSize CalcStringRect(const QString& label, const QFont& font);
  QSize CalcTextRect(int xcols, int yrows, const QFont& font);
  void ScrollIntoFrame(QWidget *win);
  int GetLineWidth(QWidget *win);
};


enum ErrorCodes {
  NONE,
  WrongOptionalLabel,
  WrongIterationLabel,
  WrongEllipsisLabel,
  WrongIteratedLabel,  //10
  WrongSubTreeLabel,
  WrongCopyLabel,
  WrongNumber,
  WrongInteger,
  WrongBoolean,
  TruncationCard,
  TruncationInt,
  NoNumbers,
  NoIdentifier,
  NoSmallLetter,
  NotNegative,
  WrongFieldLength,
  ShortFieldLength,
  LongFieldLength,
  ShortDataLength,
  LongDataLength,
  PressButton,         //30
  IOError,

};

class LAVAGUI_DLL CSyncData {
public:
  LavaDECL* FormSyntax;
  TIDs* HandlerIDs;
  CSyncData() {}
  CSyncData(LavaDECL* formSyntax, TIDs* handlerIDs) 
  {FormSyntax = formSyntax; HandlerIDs = handlerIDs;}
};

class LAVAGUI_DLL CAttachData {
public:
  LavaDECL* GUIService;
  TIDs ClientIDs;
  CAttachData() {}
  CAttachData(LavaDECL* guiService, TIDs clientIDs) 
  {GUIService = guiService; ClientIDs = clientIDs;}
};

class   LAVAGUI_DLL CGUIProgBase : public CGUIMet {
public:
  CGUIProgBase() {inSyncForm = false; inSynchTree = false; selDECL = 0;}
  SynDef *mySynDef;
  DString FormName;
  CHEFormNode  *Root;
  CHEFormNode* MPTR;
  bool InCommandAgent;
  LavaDECL* selDECL;

  unsigned  popUps, CurPos;
  CHEFormNode *butNode, *focNode, *CurPTR, *FocusPopup, *editNode;
  bool refresh;
  unsigned Warning;
  bool allowIncompleteForms;
  unsigned ErrPos;
  ErrorCodes ErrorCode;
  bool inSyncForm;
  bool inSynchTree;
  virtual void SyncTree(CHEFormNode *node, bool noSync = false) {}
  virtual void NoteLastModified() {}
  virtual void setFocNode(CHEFormNode*);
  void setHandler(CHEFormNode* formNode);

};


enum CMDsym {
  FORWARDTAB,
  BACKTAB,
  PRINT, /* print current window contents */
  FOCUS, /* set input focus to this widget; possibly toggle */

  /* normal function keys: */

  INSERT,
  DELETEWin,
  WINPOPUP,
};


enum MSGCodes {
  ItItemFix,
  ItFix,
  NoDelete,
  NoItFound,
  NoFurIns,
  NoInsert,
  NoExpand,
  ProgToLong

};

/*
enum MSGCodes {
  NoMsg,
  NoSyntax,
  NoType,
  NoService,
  Bottom,
  Top,
  NoGOTO,
  LeftMarg,
  RightMarg,
  CheckCase,        //10
  TraceOFF,
  TraceInit,
  ItItemFix,
  ItFix,
  NoItFound,
  NoInsert,
  NoFurIns,
  NoItItem,
  NoDelete,
  ReadyToAp,        //20
  CheckInput,
  EnterCom,
  ChangeCom,
  ScrollLog,
  ReturnCor,
  NoLERI,
  NoGoOn,
  NoExpand,
  NoScroll,
  ProgToLong,       //30
  FormFails,
  NoField,
  NoCHKEYS,
  AnnotAend,
  ExpandNow,
  CheckProgram,
  BlankSel,
  InsSmart,        //<-------- hier neue Codes einfuegen
  EnterPIN,
  ApplErr
};                 //39
*/

#endif
