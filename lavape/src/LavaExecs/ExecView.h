#ifndef __CExecView
#define __CExecView


#include "qevent.h"
#include <QScrollArea>
#include "qtoolbar.h"
#include "qpainter.h"
#include "qwidget.h"
#include "qdialog.h"
#include "qstring.h"
#include "qlineedit.h"
#include "qcolor.h"
#include "qfont.h"
#include "qfontmetrics.h"
#include "qrect.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QScrollArea>
#include "Constructs.h"
#include "docview.h"
#include "mdiframes.h"
#include "LavaAppBase.h"
#include "PEBaseDoc.h"
#include "ComboBar.h"
#include "ui_Comment.h"
#include "Check.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// MiniEdit window
class MiniEdit;
class MyScrollView;
class ExecContents;


class LAVAEXECS_DLL CExecView : public CLavaBaseView
{
public:
  CExecView(QWidget *parent,wxDocument *doc);
  ~CExecView();

// Attributes
  //bool OnCreate();
  void OnCloseExec();
  bool event(QEvent *ev);
  void focusInEvent(QFocusEvent *ev);
  void focusOutEvent(QFocusEvent *ev);
  void UpdateUI();

  CPEBaseDoc* GetDocument()
     { return (CPEBaseDoc*)m_viewDocument; }

  VIEWFACTORY(CExecView);
  MyScrollView *sv;
  ExecContents *redCtl;
  CPEBaseDoc *myDoc;
  wxView *myMainView;
  TID myID;
  LavaDECL *myDECL;
  TDeclType myExecCategory;
  QStatusBar *statusBar;
  CLavaBaseData *Base;
  CProgText *text;
  CLavaPEHint *multHint, *nextHint;
  MiniEdit *editCtl;
        CComboBar *m_ComboBar;

  bool notYetPainted, isDirty, editCtlVisible, insertBefore, on_editCutAction_triggered, doubleClick,
       clicked, escapePressed, forcePrimTokenSelect, deletePending,
       inIgnore, inExecHeader, inFormParms, inBaseInits,
       inParameter, inForeach, externalHint, nextError,
       destroying, autoScroll, makeSelectionVisible, errMsgUpdated, initialUpdateDone;
  TToken editToken;
  SelfVar *selfVar;
  CHAINX plhChain;
  unsigned nErrors, nPlaceholders;
  unsigned long tempNo;
  CHETokenNode *selStartPos, *selEndPos;

public:
  virtual void OnInitialUpdate();
  virtual void OnUpdate(wxView* pSender, unsigned lHint, QObject* pHint);

  void DisableActions();
  static void DisableKwdButtons();
  bool EditOK();
  void Select (SynObject *selObj=0);
  void SetSelectAtHint (CLavaPEHint *hint);
  void SetSelectAt (SynObject *obj);
  void Check ();
  void Redraw (SynObject *newObject);
  void RedrawExec ();
  virtual void on_whatNextAction_triggered();

  void OnInsertEnum (QString &refName, TID &refTID);
  void OnInsertObjRef (QString &refName, TDODC &refTIDs, bool append);
  void OnInsertRef (QString &refName, TID &refTID, bool isStatic=false, TID *vtypeID=0, bool fromNew=false);
  void SetRefTypeFlags (ObjReference *objRef);
  void UpdateErrMsg (QString &helpMsg);
  bool EnableGotoDecl();
  bool EnableInsert();
  bool IsTopLevelToken ();
  bool IsDeletablePrimary ();
  bool ConflictSelected();
  bool ToggleCatEnabled();
  bool ToggleSubstitutableEnabled();
  bool Ignorable ();
  bool Taboo (bool isEnableBreakpoints=false);
  void OnConst();
  void PutInsFlagHint(SET insFlags, SET firstLastHint=SET(firstHint,lastHint,-1));
  void PutDelFlagHint(SET insFlags, SET firstLastHint=SET(firstHint,lastHint,-1));
  void PutInsHint(SynObject *insObj, SET firstLastHint=SET(firstHint,lastHint,-1));
  void PutInsChainHint(CHE *newChe,CHAINX *chain,CHE *pred,SET firstLastHint=SET(firstHint,lastHint,-1));
  void PutInsMultOpHint(SynObject *multOp, SET firstLastHint=SET(firstHint,-1));
  void PutIniCall(SynObject *varItem, bool after=true, bool onlyIniCall=false, bool replace=false);
  void PutChgCommentHint(TComment *pCmt);
  void PutChgOpHint(TToken token);
  void PutDelHint(SynObject *delObj, SET firstLastHint=SET(firstHint,lastHint,-1));
  void PutDelNestedHint(SET firstLastHint);
  void PutPlusMinusHint();
  void InsertOrReplace (SynObject *insObj);
  void InsMultChain (SynObject *insObj);
  void InsMultOp (TToken primaryToken, MultipleOp *multOp);
  void InsertAfter();
  void InsertBefore();
  void RestoreIniCalls();
  bool EnableCopy();
  bool EnableCut();
  bool EnablePaste();
  bool EnableUndo();
  void SetHelpText ();
  SynObject *FirstChild ();
  SynObject *LeftSibling ();
  SynObject *RightSibling ();
  SynObject * AdjacentSynObj (SynObject *currentSynObj, CHETokenNode *nextTokenNode);

  void on_DbgBreakpointAct_triggered();
  void on_DbgRunToSelAct_triggered();

// tool buttons:
  void OnBitAnd();
  void OnBitOr();
  void OnBitXor();
  void OnDelete();
  void OnDivide();
  void OnEditCut();
  void OnEditCopy();
  void OnEditPaste();
  void OnEditUndo();
  void OnEditRedo();
  void OnEq();
  void OnFunctionCall();
  void OnGe();
  void OnGt();
  void OnInsert();
  void OnInsertBefore();
  void OnInvert();
  void OnLButtonDown(QMouseEvent *e);
  void OnLButtonDblClk(QMouseEvent *e);
  void OnLe();
  void OnLshift();
  void OnLt();
  void OnPlusMinus();
  void OnMult();
  void OnNe();
  void OnShowComments();
  void OnNull();
  void OnPlus();
  void OnRshift();
  void OnEditSel();
  void OnComment();
  void OnToggleArrows();
  void OnNewLine();
  void OnShowOptionals();
  void OnGotoDecl();
  void OnGotoImpl();
  void OnFindReferences();
  void OnModulus();
  void OnIgnore();
  void OnStaticCall();
  void OnInterval();
//  void OnUuid();
  void OnNextError();
  void OnPrevError();
  void OnNextComment();
  void OnPrevComment();
  void OnConflict();
  void OnToggleCategory();
  void OnToggleSubstitutable();
  void OnToggleClosed();
  void OnToggleParmNames();
  void OnEvaluate();
  void OnInputArrow();
  void OnHandle();
  void OnOptLocalVar();
  void OnTrue();
  void OnFalse();
  void OnOrd();

// keyword bar:
  void OnDeclare();
  void OnExists();
  void OnForeach();
  void OnSelect();
  void OnIn();
  void OnIf();
  void OnIfdef();
  void OnIfExpr();
  void OnElseExpr();
  void OnSwitch();
  void OnTypeSwitch();
  void OnAnd();
  void OnOr();
  void OnTryStatement();
  void OnXor();
  void OnNot();
  void OnAssert();
  void OnIgnoreStm();
  void OnSucceed();
  void OnFail();
  void OnOld();
  void OnCall();
  void OnAssign();
  void OnCreateObject();
  void OnClone();
  void OnCopy();
  void OnAttach();
  void OnQueryItf();
  void OnQua();
  void OnItem();
  void OnConnect();
  void OnDisconnect();
  void OnEmitSignal();

//  LRESULT OnCalledView(WPARAM, LPARAM lparam);
//  void OnSetFocus(QWidget* pOldWnd);
//  void OnKillFocus(QWidget* pNewWnd);
  void OnChar(QKeyEvent *e);

  void OnUpdateOrd(QAction* action);
  void OnUpdateOptLocalVar(QAction* action);
  void OnUpdateHandle(QAction* action);
  void OnUpdateInputArrow(QAction* action);
  void OnUpdateEditSel(QAction* action);
  void OnUpdateEvaluate(QAction* action);
  void OnUpdateToggleSubstitutable(QAction* action);
  void OnUpdateToggleClosed(QAction* action);
  void OnUpdateConflict(QAction* action);
  void OnUpdateToggleCategory(QAction* action);
  void OnUpdateNextComment(QAction* action);
  void OnUpdatePrevComment(QAction* action);
  void OnUpdateNextError(QAction* action);
  void OnUpdatePrevError(QAction* action);
  void OnUpdateInterval(QAction* action);
  void OnUpdateStaticCall(QAction* action);
  void OnUpdateIgnore(QAction* action);
  void OnUpdateModulus(QAction* action);
  void OnUpdateNewLine(QAction* action);
  void OnUpdateShowOptionals(QAction* action);
  void OnUpdateGotoDecl(QAction* action);
  void OnUpdateGotoImpl(QAction* action);
  void OnUpdateFindReferences(QAction* action);
  void OnUpdateToggleArrows(QAction* action);
  void OnUpdateToggleParmNames(QAction* action);
  void OnUpdateComment(QAction* action);
  void OnUpdateButtonEnum();
  void OnUpdateNewFunc();
  void OnUpdateNewPFunc();
  void OnUpdateEditCut(QAction* action);
  void OnUpdateEditCopy(QAction* action);
  void OnUpdateEditPaste(QAction* action);
  void OnUpdateEditUndo(QAction* action);
  void OnUpdateEditRedo(QAction* action);
  void OnUpdateBitOr(QAction* action);
  void OnUpdateBitAnd(QAction* action);
  void OnUpdateBitXor(QAction* action);
  void OnUpdateDelete(QAction* action);
  void OnUpdateDivide(QAction* action);
  void OnUpdateEq(QAction* action);
  void OnUpdateFalse(QAction* action);
  void OnUpdateFunctionCall(QAction* action);
  void OnUpdateGe(QAction* action);
  void OnUpdateGt(QAction* action);
  void OnUpdateInsert(QAction* action);
  void OnUpdateInsertBefore(QAction* action);
  void OnUpdateInvert(QAction* action);
  void OnUpdateLe(QAction* action);
  void OnUpdateLshift(QAction* action);
  void OnUpdateLt(QAction* action);
  void OnUpdatePlusMinus(QAction* action);
  void OnUpdateMult(QAction* action);
  void OnUpdateNe(QAction* action);
  void OnUpdateShowComments(QAction* action);
  void OnUpdateNull(QAction* action);
  void OnUpdatePlus(QAction* action);
  void OnUpdateRshift(QAction* action);
  void OnUpdateTrue(QAction* action);

//  void OnUpdateDbgStart(QAction* action);
  void OnUpdateDbgStepNext(QAction* action);
  void OnUpdateDbgStepNextFunction(QAction* action);
  void OnUpdateDbgStepinto(QAction* action);
  void OnUpdateDbgStepout(QAction* action);
  void OnUpdateDbgRunToSel(QAction* action);
  void OnUpdateDbgBreakpoint(QAction* action);
  void OnUpdateDbgClearBreakpoints(QAction* action);
  void OnUpdateDbgStop(QAction* action);

  void OnUpdateTry(QToolButton *pb);
  void OnUpdateFail(QToolButton *pb);
  void OnUpdateSucceed(QToolButton *pb);
  void OnUpdateQua(QToolButton *pb);
  void OnUpdateItem(QToolButton *pb);
  void OnUpdateCopy(QToolButton *pb);
  void OnUpdateAssert(QToolButton *pb);
  void OnUpdateIgnoreButton(QToolButton *pb);
  void OnUpdateCreate(QToolButton *pb);
  void OnUpdateOld(QToolButton *pb);
  void OnUpdateExists(QToolButton *pb);
  void OnUpdateCall(QToolButton *pb);
//  void OnUpdateUuid(QToolButton *pb);
  void OnUpdateTypeSwitch(QToolButton *pb);
  void OnUpdateConnect(QToolButton *pb);
  void OnUpdateDisconnect(QToolButton *pb);
  void OnUpdateEmitSignal(QToolButton *pb);
  void OnUpdateAssign(QToolButton *pb);
  void OnUpdateAttach(QToolButton *pb);
  void OnUpdateQueryItf(QToolButton *pb);
  void OnUpdateAnd(QToolButton *pb);
  void OnUpdateClone(QToolButton *pb);
  void OnUpdateSelect(QToolButton *pb);
  void OnUpdateDeclare(QToolButton *pb);
  void OnUpdateForeach(QToolButton *pb);
  void OnUpdateIf(QToolButton *pb);
  void OnUpdateIfdef(QToolButton *pb);
  void OnUpdateIfExpr(QToolButton *pb);
  void OnUpdateElseExpr(QToolButton *pb);
  void OnUpdateIn(QToolButton *pb);
  void OnUpdateNot(QToolButton *pb);
  void OnUpdateOr(QToolButton *pb);
  void OnUpdateSwitch(QToolButton *pb);
  void OnUpdateXor(QToolButton *pb);

private:
  Q_OBJECT
};

struct Format {
  bool bold, italic;
  QColor color;
  QFont font;
  bool symbolFont;
  QString fontFamily, symbolFamily;
};

class LAVAEXECS_DLL ExecContents : public QWidget {
public:
  ExecContents (MyScrollView *sv);
  ~ExecContents () {
    delete debugStop;
    delete debugStopGreen;
    delete breakPoint;
  }

  CExecView *execView;
	MyScrollView *sv;

  Format fmt;
  int currentX, currentY, debugStopY, callerStopY, breakPointY, widthOfIndent,
  widthOfBlank, contentsWidth, contentsHeight, miniEditRightEdge;
  QPixmap *debugStop, *debugStopGreen, *breakPoint;
  bool inDebugStop, inBreakPoint, innermostStop;
  CHETokenNode *debugStopToken;
  CHETokenNode *callerStopToken;
  StopReason stopReason;

  bool event(QEvent *ev);
  void keyPressEvent (QKeyEvent *e);
  //void focusInEvent (QFocusEvent *e) { execView->wxView::focusInEvent(e); }
  QString text(const QPoint &point);
  void paintEvent (QPaintEvent *ev);
  void DrawToken (QPainter &p,CProgText *text,CHETokenNode *tNode,bool inSelection);
  void SetTokenFormat (CHETokenNode *currToken);
  int calcIndent(CHETokenNode *currentToken);
};

class MyScrollView : public QScrollArea {
public:
  MyScrollView (QWidget *parent);

  CProgText *text;
  CExecView *execView;
  ExecContents *execCont;

  void mousePressEvent (QMouseEvent *e);
  void mouseDoubleClickEvent (QMouseEvent *e);
};

enum VarConstCheck {
  correct,
  wrong,
  noChange
};

class ObjComboUpdate : public VarAction {
public:
  CExecView *execView;

  ObjComboUpdate (CExecView *ev);
  virtual bool Action (CheckData &ckd, VarName *vn, TID &tid);
};

class  MiniEdit : public QLineEdit
{
public:
  MiniEdit(ExecContents *execCnt);
  void keyPressEvent (QKeyEvent *ev);
  VarConstCheck InputIsCorrect(TToken &token, CComboBar *comboBar) ;
  int frameWidth();

  CExecView *execView;
  bool returnPressed;

protected:
  void OnUpdate();
  void focusInEvent(QFocusEvent *ev);
  void focusOutEvent(QFocusEvent *ev);

};
/*
class ExecWhatsThis// : public WhatsThis
{
public:
  ExecWhatsThis(CExecView *ev) : WhatsThis(0,ev) { execView = ev; }

//  bool clicked(const QString &whatsThisHref);
  QString text(const QPoint&);

private:
  CExecView *execView;
};
*/
class LAVAEXECS_DLL CComment : public QDialog, public Ui_CComment {

public:
  CComment(QWidget* parent = 0) : QDialog(parent) {
    setupUi(this);
  }

public slots:
  virtual void on_okButton_clicked();
  virtual void on_cancelButton_clicked();

private:
  Q_OBJECT
};

#endif
/////////////////////////////////////////////////////////////////////////////
