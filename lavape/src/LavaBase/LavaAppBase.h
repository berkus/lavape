#ifndef __LavaAppBase
#define __LavaAppBase


#include "SYSTEM.h"

#include "wx_obj.h"
#include "DString.h"
#include "SynIO.h"
#include "Syntax.h"
#include "CHAINANY.h"
#include "SynIDTable.h"
#include "defs.h"
#include "docview.h"
#include "LavaThread.h"
#include "wxExport.h"
#include "qcombobox.h"
#include "qfont.h"
#include "qsize.h"
#include "qobject.h"
#include "qwidget.h"
#include "qstring.h"
#include "qfileinfo.h"
#include "qtoolbutton.h"
#include "QtAssistant/qassistantclient.h"
#include "qwhatsthis.h"


#ifdef WIN32
#include "qt_windows.h"
#include "commdlg.h"
//#define QASSISTANT "assistant_adp.exe"
#endif


/*
  LavaPE Commands for operation  | Commanddata and meaning in the direction
                               |  Views to Document               |  Document to View| Undo queue
  ---------------------------------------------------------------------------------------------
  1. Open view operations (no undo facility):

  CPECommand_OpenFormView,  |  CommandData1: always the related LavaDECL    | not in Queue
  CPECommand_OpenWizardView,|  CommandData2: always the CDEView* mainView
  CPECommand_OpenSelView,   |

  --------------------------------------------------------------------------
  CPECommand_OpenExecView,  |  CommandData1: always the related CHE*
                            |  CommandData2: always the CDEView* mainView


-----------------------------------------------------------------------------------------------
                               |
  2. Include lava file operations (with undo facility):

  CPECommand_Include,        |  CommandData1: CHESimpleSyntax*,  the included simple syntax
                               |  CommandData2: DString* full path syntax name
                               |  CommandData3: CHESimpleSyntax*, position in the SynDefTree
                                | CommandData4: DString* full path usersName
  CPECommand_Exclude,        |  CommandData1: CHESimpleSyntax* , the deleted simple syntax
                               |  CommandData2: DString* full path syntax name
                               |  CommandData3: CHESimpleSyntax*, position in the SynDefTree
  CPECommand_ChangeInclude,  |  CommandData1: CHESimpleSyntax* , the new/old simple syntax
                               |  CommandData2: unused
                               |  CommandData3: CHESimpleSyntax*, position in the SynDefTree
-----------------------------------------------------------------------------------------------
                               |
  3. Operations on syntax via tree view (with undo facility):

    One complete operation is composed of one or more operation commands, the first
    one has the 'First' flag.

    Doing of a complete operation will start with the first operation command.
    Undoing of a complete operation will start with the last operation command.

    You only can copy, paste, cut, drag, drop, delete or insert a pattern, an exec,
    a member (or CaseConstruct), an input member or output member.

                               |
  CPECommand_Insert,         |   always CommandData1: new LavaDECL*,
  and                          |
  CPECommand_Move            |          CommandData2: name of (drop-)parent DECL if exists
                               |          CommandData3: position in the chain of (drop-)parent DECL
                               |          CommandData4: LavaDECL** of (drop-)parentDECL
                               |          CommandData5: dragParent, if drop from drag and drop
                               |          CommandData6: ClipSyntaxDefinition from clipboard operation or drag and drop
                               |          CommandData7: Full path name of Clip-document from clipboard operation or drag and drop
                               |          CommandData8: drag and drop operations refac case
                               |          CommandData9: the CHE* from which CommandData1 is the data, set only in UpdateDoc

  CPECommand_Delete,         |   always CommandData1: old LavaDECL*
                               |          CommandData2: name of parent DECL if exists
                               |          CommandData3: position in the chain of parent DECL
                               |          CommandData4: LavaDECL** of parentDECL
                               |          CommandData5: dropParent, if drag from drag and drop in the same document
                               |          CommandData8: drag and drop operations refac case
                               |          CommandData9: the CHE* from which CommandData1 is the data, set only in UpdateDoc

  CPECommand_Change,         | CommandData1: new LavaDECL*  |  CommandData1.old LavaDECL*
                               | CommandData2: old name of the DECL |  CommandData2.new name of the DECL
                               | CommandData4: the LavaDECL** of the changed element
                               | CommandData5: new enum item Id, in case of enum items labeledit
                               | CommandData6: old enum item Id, in case of enum items labeledit
                               | CommandData7: if != 0: draw the tree, because the change has new tree-nodes

  CPECommand_Exec,     |
                               |
                               |
                               |

  CPECommand_Comment         | CommandData1: LavaDECL* or CHEEnumSelId*
                               | CommandData2: new TDECLComment*    | old TDECLComment*
                               | CommandData3: = EnumDef-DECL if CommandData1 is CHEEnumSelId*, else =0
                               |


------------------------------------------------------------------------------------------
*
*/


enum CPECommand {
  CPECommand_OpenFormView,
  CPECommand_OpenExecView,
  CPECommand_OpenSelView,
  CPECommand_OpenWizardView,
  CPECommand_Include,
  CPECommand_Exclude,
  CPECommand_ChangeInclude,
  CPECommand_Insert,
  CPECommand_Move, //not used in UpdateAllViews
  CPECommand_Delete,
  CPECommand_Change,
  CPECommand_Exec,
  CPECommand_Comment,
  CPECommand_FromOtherDoc,
  CPECommand_LavaEnd //used only at runtime
};

enum TFirstLast { firstHint, lastHint, impliedExecHint, multiDocHint, blockedHint, noDrawHint};
//the multiDocHint must be set at least in the first and in the last hint of a multi
//document update (move or refactoring)

class LAVABASE_DLL  CLavaPEHint : public QObject
{
public:
  CPECommand com;
  wxDocument* fromDoc;
  SynFlags FirstLast; //first-last indication of a range of commands which are part of one update
  void *CommandData1;
  void *CommandData2;
  void *CommandData3;
  void *CommandData4;
  void *CommandData5;
  void *CommandData6;
  void *CommandData7;
  void *CommandData8;
  void *CommandData9;

  CLavaPEHint(CPECommand cc,
             wxDocument* fromdoc,
             SynFlags first,
             void *commandData1=0,
             void *commandData2=0,
             void *commandData3=0,
             void *commandData4=0,
             void *commandData5=0,
             void *commandData6=0,
             void *commandData7=0,
             void *commandData8=0);

  ~CLavaPEHint();
  void Destroy(bool inRedo);
private:
    Q_OBJECT
};


enum TRefacMove {
  noRefac,
  privToPublic,
  publicToPriv,
  baseToEx,
  exToBase
};

enum TCompoProt { PROT_LAVA, PROT_NATIVE, PROT_DOTNET, PROT_CORBA, PROT_EJB,
                  PROT_HTTP, PROT_FTP, PROT_MAIL};


class LAVABASE_DLL  CBrowseContext : public QObject
{
public:
  wxView *fromView;
  SynObjectBase* synObjSel;
  CBrowseContext* prev;
  CHEFormNode* formNode;
  TID id;
  TID parentID;
  int type; //only used in VTView

  CBrowseContext(wxView* view, SynObjectBase* synObj);
  CBrowseContext(wxView* view, LavaDECL* decl, int t=0);
  CBrowseContext(wxView* view, CHEFormNode* node);

  void RemoveView(wxView* view);
  void RemoveSynObj(SynObjectBase* synObj);
  void RemoveFormNode();
  ~CBrowseContext() {}
private:
    Q_OBJECT
};


enum TFindRefFlag {
  readRefs,
  writeRefs,
  derivRefs,
  wholeWord,
  matchCase
};

enum TFindWhere { findInThisView, findInThisDoc, findInDocs, findInIncl};

class LAVABASE_DLL CFindData {
public:
  int index; //=0: reference in declaration tree,
             //=1: reference in exec,
             //=2: name in declaration tree,
             //=3: name in exec
  SynFlags FindRefFlags;
  TFindWhere FWhere;
  DString fname; //file
  DString searchName;
  int nID;
  int refCase; //and SynObjectID
  TID refTid;
  DString enumID;
  CFindData() {index=0; nID=-1;}
};

class LAVABASE_DLL  CPEBaseBrowse //: public CSylBrowse
{
public:
  CPEBaseBrowse() {}
  CBrowseContext *LastBrowseContext;
  void DestroyLastBrsContext(); //use this function only in case of failing browse result

  virtual bool GotoDECL(wxDocument* , LavaDECL* , TID , bool /*popUp*/, DString* enumID=0, bool openExec=false, TDeclType execType=ExecDef) {return false;}
  LavaDECL* BrowseDECL(wxDocument* doc, TID& id, DString* enumID=0, bool openExec=false);
  bool CanBrowse(LavaDECL* DECL);

  SynDef *mySynDef;

  TBasicType GetBasicType(SynDef *lavaCode, LavaDECL *elDef);
  TDeclDescType GetExprType(SynDef *lavaCode, LavaDECL *elDef);
  TDeclType GetCategory(SynDef *lavaCode, LavaDECL *elDef, SynFlags& flags);
  virtual bool GotoImpl(wxDocument* fromDoc, LavaDECL* decl) {return false;}
  virtual bool GotoImpl(wxDocument* fromDoc, TID id); //id in fromDoc
  virtual bool OnFindRefs(wxDocument* fromDoc, LavaDECL* decl, CFindData& fw ){return false;}
};



class LAVABASE_DLL  CUndoMem {

  bool wasFirst;
  int max_undo;
  CLavaPEHint** UndoMemory;
  int undoMemPosition;
  int saveMemPosition;
  CLavaPEHint** RedoMemory;
  int redoMemPosition;
public:
  void SetLastHint();
  CLavaPEHint* GetLastHint();
  CLavaPEHint* GetLastRedo();
  void AddToMem(CLavaPEHint* hint);
  CLavaPEHint* DoFromMem(int& pos);   //called by LavaPEDoc
  CLavaPEHint* UndoFromMem(int& pos); //called by LavaPEDoc
  CLavaPEHint* RedoFromMem(int& pos); //called by LavaPEDoc
  void SetCom8();
  void OnEditUndo();
  void OnUpdateEditUndo(QAction *action);
  bool EnableUndo();
  void OnEditRedo();
  void OnUpdateEditRedo(QAction *action);
  CUndoMem();
  ~CUndoMem();
  bool DocModified();
  void SetSavePos();
  void CleanUndoMem();
  bool DrawTree;
};


class LAVABASE_DLL  CMultiDocEntry : public DObject
{
public:
  CLavaPEHint* FirstHint; //of one doc
  CLavaPEHint* LastHint;  //of the same doc
  DString FileName;
  bool StartUpdate;
  CMultiDocEntry() {FirstHint=0; LastHint=0; StartUpdate=false;}
  virtual void CDP(PutGetFlag pg,ASN1* cid, bool baseCDP) {}
};

class LAVABASE_DLL  CMultiUndoMem : public QObject
{
public:
  CHAINX MultiUndo; //with CHE(CMultiDocEntry)
  bool StartUpdate;
  CMultiUndoMem() { setObjectName("MultiUndoMem"); StartUpdate = false;}
  void StartMultiDocUpdate();
  void AddToMem(CLavaPEHint *hint);
  void SetLastHint(CLavaPEHint *hint);
  bool Undo(CLavaPEHint *withLastHint);
  bool Redo(CLavaPEHint *withFirstHint);
  void RemoveHint(CLavaPEHint *hint);
private:
    Q_OBJECT
};

class LAVABASE_DLL CBaseExecUpdate
{
public:
  virtual void MakeExec(LavaDECL *myDECL){};
  virtual bool ChangeExec(CLavaPEHint* , wxDocument* , bool /*undo*/) {return FALSE;}
  virtual void DeleteHint(CLavaPEHint* ) {}
};


class LAVABASE_DLL  CLavaBaseData : public QObject
{
public:
  CLavaBaseData() {
    setObjectName("LavaBaseData");
    declareButton = 0;
    ContData = 0;
    debugger = 0;
    tempBrkPoint = 0;
    openForDebugging = false;
  }

  void Init(CPEBaseBrowse *browser, CBaseExecUpdate *execUpdate);
  ~CLavaBaseData();

  wxApp *theApp;
  CLavaDbgBase* debugger;
  wxDocument* docModal;
  //bool debugOn;
  DbgContData* ContData;  //used in LavaPE
  unsigned long /*HCURSOR*/ ArrowCurser;
  bool inMultiDocUpdate; //open document in multi document update
  bool inRuntime;
  bool openForDebugging;

  CLavaPEHint* actHint;
  CMultiUndoMem MultiUndoMem;
  CPEBaseBrowse *Browser;
  CBaseExecUpdate *ExecUpdate;
  SynObjectBase *tempBrkPoint;
  QSize ptMaxSize;
  QString ldocFileExt;
  QString lcomFileExt;
  int stdUpdate;

  bool m_saveEveryChange;
  QString m_strSaveEveryChange;
  bool m_checkPreconditions;
  QString m_strCheckPreconditions;
  bool m_checkPostconditions;
  QString m_strCheckPostconditions;
  bool m_checkInvariants;
  QString m_strCheckInvariants;
  bool m_pmDumps;
  QString m_strPmDumps;

  QFont m_ExecFont;
  QFont m_FormFont;
  QFont m_FormLabelFont;
  bool useLabelFont;
//  QFont m_FormButtonFont;
//  bool useButtonFont;
  QFont m_TreeFont;
  QFont m_GlobalFont;
  QString m_lfDefExecFont;
  QString m_lfDefFormFont;
  QString m_lfDefFormLabelFont;
//  QString m_lfDefFormButtonFont;
  QString m_lfDefTreeFont;
  QString m_lfDefGlobalFont;
  QString m_myWebBrowser;
  QString m_style;

  QAction* additionActionPtr;
  QAction* bitAndActionPtr;
  QAction* bitOrActionPtr;
  QAction* bitXorActionPtr;
  QAction* commentOutActionPtr;
  QAction* conflictingAssigActionPtr;
  QAction* delActionPtr;
  QAction* deleteActionPtr;
  QAction* divideActionPtr;
  QAction* editCommentActionPtr;
  QAction* editCopyActionPtr;
  QAction* editCutActionPtr;
  QAction* editPasteActionPtr;
  QAction* editSelItemActionPtr;
  QAction* editRedoActionPtr;
  QAction* editUndoActionPtr;
  QAction* equalActionPtr;
  QAction* evaluateActionPtr;
  QAction* falseActionPtr;
  QAction* findRefsActionPtr;
  QAction* functionCallActionPtr;
  QAction* gotoDeclActionPtr;
  QAction* gotoImplActionPtr;
  QAction* greaterEqualActionPtr;
  QAction* greaterThanActionPtr;
  QAction* handleActionPtr;
  QAction* insActionPtr;
  QAction* insertActionPtr;
  QAction* insertBeforeActionPtr;
  QAction* intervalActionPtr;
  QAction* invertActionPtr;
  QAction* leftShiftActionPtr;
  QAction* lessEqualActionPtr;
  QAction* lessThanActionPtr;
  QAction* modulusActionPtr;
  QAction* multiplicationActionPtr;
  QAction* newFuncActionPtr;
  QAction* newLineActionPtr;
  QAction* nextCommentActionPtr;
  QAction* nextErrorActionPtr;
  QAction* notEqualActionPtr;
  QAction* nullActionPtr;
//  QAction* okActionPtr;
	QAction* optLocalVarActionPtr;
	QAction* ordActionPtr;
  QAction* prevCommentActionPtr;
  QAction* prevErrorActionPtr;
  QAction* rightShiftActionPtr;
  QAction* showOptsActionPtr;
  QAction* staticCallActionPtr;
  QAction* toggleArrowsActionPtr;
  QAction* toggleCatActionPtr;      //Lava.exe
  QAction* toggleCategoryActionPtr; //LavaPE.exe
  QAction* toggleCommentsActionPtr;
  QAction* toggleInputArrowsActionPtr;
  QAction* toggleSignActionPtr;
  QAction* toggleSubstTypeActionPtr;
  QAction* toggleClosedActionPtr;
  QAction* parmNameActionPtr;
  QAction* trueActionPtr;
  QAction* updateResetActionPtr;

  QAction* DbgActionPtr;
  QAction* DbgStepNextActPtr;
  QAction* DbgStepNextFunctionActPtr;
  QAction* DbgStepintoActPtr;
  QAction* DbgStepoutActPtr;
  QAction* DbgRunToSelActPtr;
  QAction* DbgClearBreakpointsActPtr;
  QAction* DbgBreakpointActPtr;
  QAction* DbgStopActionPtr;

  QToolButton
    *declareButton, *existsButton, *foreachButton,
    *selectButton, *elInSetButton, *ifButton, *ifdefButton,
    *ifxButton, *elsexButton, *switchButton, *typeSwitchButton,
    *andButton, *orButton,
    *xorButton, *notButton, *assertButton, *ignoreButton,
    *tryButton, *succeedButton, *failButton, *runButton,
    *setButton, *newButton, *oldButton, *cloneButton,
    *copyButton, *attachButton, *qryItfButton,
    *scaleButton, *itemButton, *connectButton, *disconnectButton, *emitButton;

  QAction *myWhatsThisAction;

  const char* BasicNames[30];
  DString OperatorNames [OP_high];
  DString OpFuncNames [OP_high];
  int maxb;

  bool isIdentifier(const QString& ident);  //no dots
  DString calcRelName(const DString& qname, const DString& scopeName);
private:
    Q_OBJECT
};


class LAVABASE_DLL CSearchData {
public:
  CSearchData () {
    nextFreeID = 0;
    synObj = 0;
    debugStop = false;
    //finished = false;
  }

  //DString fileName;
  LavaDECL *execDECL;
  wxView *execView;
  //TID refID;
  //DString enumID;
  unsigned synObjectID, nextFreeID;
  SynObjectBase *synObj;
  DString constructNesting;
  wxDocument *doc;
  CFindData findRefs;
  bool debugStop,innermostStop/*, finished*/;
  StopReason stopReason;
};


extern LAVABASE_DLL CLavaBaseData *LBaseData;

//extern  LAVABASE_DLL DString dach;
extern LAVABASE_DLL void LavaEnd(wxDocument* fromDoc, bool doClose);
extern LAVABASE_DLL QString L_GetOpenFileName(const QString& startFileName,
				                                      QWidget *parent,
				                                      const QString& caption,
				                                      const QString& filter,
                                              const QString& filter2=QString::null
                                              );
extern LAVABASE_DLL QStringList L_GetOpenFileNames(const QString& startFileName,
				                                      QWidget *parent,
				                                      const QString& caption,
				                                      const QString& filter,
				                                      const QString& filter2=QString::null
                                              );
#ifdef WIN32
extern unsigned int CALLBACK myOFNHookProc(HWND hdlg, unsigned int uiMsg,
                                           WPARAM wParam, LPARAM lParam);
#endif

/*
extern LAVABASE_DLL HRESULT ResolveShortCut (HWND hwnd, LPCSTR pszLink, LPSTR szGotPath);
extern LAVABASE_DLL int FindStringCaseExact(QComboBox* box, const DString& str, int start);
*/


class LAVABASE_DLL CMsgBoxParams {
public:
  CMsgBoxParams (CLavaThread *thr, int funcSpec, QWidget *parent, const QString &caption,
			 const QString &text, int button0, int button1, int button2) {
		this->thr = thr;
		this->funcSpec = funcSpec;
		this->parent = parent;
		this->caption = &caption;
		this->text = &text;
		this->button0 = button0;
		this->button1 = button1;
		this->button2 = button2;
		result = 0;
  }

	CLavaThread *thr;
  int funcSpec;
	QWidget *parent;
	const QString *caption, *text;
	int button0, button1, button2, result;
};

class LAVABASE_DLL WhatNextEvent : public QEvent {
public:
  WhatNextEvent(QEvent::Type t,const QPoint &pos, const QPoint &globalPos)
    : QEvent(t) {
    this->pos = pos;
    this->globalPos = globalPos;
  }

  QPoint pos, globalPos;
};

class LAVABASE_DLL Assistant
{
public:
    Assistant();
    ~Assistant();
    void ShowPage(const QString &file);

private:
    bool startAssistant();
    QProcess *proc;
};


extern LAVABASE_DLL int critical(QWidget *parent, const QString &caption,
			 const QString& text,
			 int button0, int button1=0, int button2=0);

extern LAVABASE_DLL int information(QWidget *parent, const QString &caption,
			 const QString& text,
			 int button0, int button1, int button2=0);

extern LAVABASE_DLL int question(QWidget *parent, const QString &caption,
			 const QString& text,
			 int button0, int button1, int button2=0);

extern LAVABASE_DLL QAssistantClient *qacl;

extern LAVABASE_DLL void ShowPage(const QString &file);

#endif
