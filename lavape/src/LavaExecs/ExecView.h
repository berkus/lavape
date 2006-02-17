#ifndef __CExecView
#define __CExecView


#include "qevent.h"
#include "q3scrollview.h"
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
#include "Constructs.h"
#include "docview.h"
#include "mdiframes.h"
#include "LavaAppBase.h"
#include "PEBaseDoc.h"
#include "ComboBar.h"
#include "Comment.h"
#include "Check.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// MiniEdit window
class MiniEdit;
class MyScrollView;


class LAVAEXECS_DLL CExecView : public CLavaBaseView
{
protected:
  CExecView(QWidget *parent,wxDocument *doc);
  ~CExecView();

// Attributes
public:
  bool OnCreate();
  void OnCloseExec();
  void focusInEvent(QFocusEvent *ev);
  void focusOutEvent(QFocusEvent *ev);
  void UpdateUI();

  CPEBaseDoc* CExecView::GetDocument()
     { return (CPEBaseDoc*)m_viewDocument; }

  VIEWFACTORY(CExecView);
  MyScrollView *sv;
  QWidget *redCtl;
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

  bool editCtlVisible, insertBefore, on_editCutAction_triggered, doubleClick,
       clicked, escapePressed, active, forcePrimTokenSelect, deletePending,
       inIgnore, inExecHeader, inFormParms, inBaseInits,
       inParameter, inForeach, externalHint, execReplaced, nextError,
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

  virtual void OnActivateView(bool bActivate=true, wxView *deactiveView=0);
  void DisableActions();
  static void DisableKwdButtons();
  bool EditOK();
  void Select (SynObject *selObj=0);
  void SetSelectAt (CLavaPEHint *hint);
  void Check ();
  void Redraw (SynObject *newObject);
  void RedrawExec (SynObject *selectAt=0);
  virtual void whatNext();

  void OnInsertEnum (QString &refName, TID &refTID, unsigned pos);
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
  void PutInsFlagHint(SET insFlags, SET firstLasthint=SET(firstHint,lastHint,-1));
  void PutDelFlagHint(SET insFlags, SET firstLastHint=SET(firstHint,lastHint,-1));
  void PutInsHint(SynObject *insObj, SET firstLastHint=SET(firstHint,lastHint,-1), bool putHint=false);
  void PutInsChainHint(CHE *newChe,CHAINX *chain,CHE *pred,SET firstLastHint=SET(firstHint,lastHint,-1));
  void PutInsMultOpHint(SynObject *multOp);
  void PutChgCommentHint(TComment *pCmt);
  void PutChgOpHint(TToken token);
  void PutDelHint(SynObject *delObj, SET firstLastHint=SET(firstHint,lastHint,-1));
  void PutDelNestedHint(SET firstLastHint);
//  void PutArrowHint();
  void PutPlusMinusHint();
  void InsertOrReplace (SynObject *insObj);
  void InsMultChain (SynObject *insObj);
  void InsMultOp (TToken primaryToken, MultipleOp *multOp);
  void InsertAfter();
  void InsertBefore();
//  void PrintPageHeader(CDC* pDC, CPrintInfo* pInfo,const QString& strHeader);
//  void OnKeyPressed(WPARAM wParam, LPARAM lParam);
  bool EnableCopy();
  bool EnableCut();
  bool EnablePaste();
  bool EnableUndo();
  void SetHelpText ();
  SynObject *FirstChild ();
  SynObject *LeftSibling ();
  SynObject *RightSibling ();
  SynObject * AdjacentSynObj (SynObject *currentSynObj, CHETokenNode *nextTokenNode);

  void DbgBreakpoint();
  void DbgRunToSel();

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

  void OnUpdateDbgStart(QAction* action);
  void OnUpdateDbgStepNext(QAction* action);
  void OnUpdateDbgStepNextFunction(QAction* action);
  void OnUpdateDbgStepinto(QAction* action);
  void OnUpdateDbgStepout(QAction* action);
  void OnUpdateDbgRunToSel(QAction* action);
  void OnUpdateDbgBreakpoint(QAction* action);
  void OnUpdateDbgClearBreakpoints(QAction* action);
  void OnUpdateDbgStop(QAction* action);

	void OnUpdateTry(QPushButton *pb);
	void OnUpdateFail(QPushButton *pb);
	void OnUpdateSucceed(QPushButton *pb);
	void OnUpdateQua(QPushButton *pb);
	void OnUpdateItem(QPushButton *pb);
  void OnUpdateCopy(QPushButton *pb);
  void OnUpdateAssert(QPushButton *pb);
  void OnUpdateCreate(QPushButton *pb);
  void OnUpdateOld(QPushButton *pb);
  void OnUpdateExists(QPushButton *pb);
  void OnUpdateCall(QPushButton *pb);
//  void OnUpdateUuid(QPushButton *pb);
  void OnUpdateTypeSwitch(QPushButton *pb);
  void OnUpdateConnect(QPushButton *pb);
  void OnUpdateDisconnect(QPushButton *pb);
  void OnUpdateEmitSignal(QPushButton *pb);
  void OnUpdateAssign(QPushButton *pb);
  void OnUpdateAttach(QPushButton *pb);
  void OnUpdateQueryItf(QPushButton *pb);
  void OnUpdateAnd(QPushButton *pb);
  void OnUpdateClone(QPushButton *pb);
  void OnUpdateSelect(QPushButton *pb);
  void OnUpdateDeclare(QPushButton *pb);
  void OnUpdateForeach(QPushButton *pb);
  void OnUpdateIf(QPushButton *pb);
  void OnUpdateIfdef(QPushButton *pb);
  void OnUpdateIfExpr(QPushButton *pb);
  void OnUpdateElseExpr(QPushButton *pb);
  void OnUpdateIn(QPushButton *pb);
  void OnUpdateNot(QPushButton *pb);
  void OnUpdateOr(QPushButton *pb);
  void OnUpdateSwitch(QPushButton *pb);
  void OnUpdateXor(QPushButton *pb);

private:
  Q_OBJECT
};

struct Format {
  bool bold, italic;
  QColor color;
  QFont font;
};


class MyScrollView : public Q3ScrollView {
public:
  MyScrollView (QWidget *parent);
  ~MyScrollView () { 
	  delete debugStop; 
	  delete debugStopGreen; 
		delete breakPoint; 
//		delete fm;
	}

	int calcIndent(CHETokenNode *currentToken);

  CProgText *text;
  CExecView *execView;
  Format fmt;
	int currentX, currentY, debugStopY, callerStopY, breakPointY, widthOfIndent, widthOfBlank, contentsWidth, contentsHeight;
//  QFontMetrics *fm;
	QPainter *p;
  QPixmap *debugStop, *debugStopGreen, *breakPoint;
  bool inDebugStop, inBreakPoint, innermostStop;
  CHETokenNode *debugStopToken;
  CHETokenNode *callerStopToken;
  StopReason stopReason;

  void keyPressEvent (QKeyEvent *e);
  void focusInEvent(QFocusEvent *ev);
  void contentsMousePressEvent (QMouseEvent *e);
  void contentsMouseDoubleClickEvent (QMouseEvent *e);
  void drawContents (QPainter *p, int clipx, int clipy, int clipw, int cliph);
  void SetTokenFormat (CHETokenNode *currToken);
  void DrawToken (CProgText *text, CHETokenNode *tNode, bool inSelection);
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
  MiniEdit(CExecView *execView);
  void keyPressEvent (QKeyEvent *ev);
  VarConstCheck InputIsCorrect(TToken &token, CComboBar *comboBar) ;
  int frameWidth();

  CExecView *conView;
  bool returnPressed;

protected:
  void OnUpdate();
  void focusInEvent(QFocusEvent *ev);
  void focusOutEvent(QFocusEvent *ev);

};

class ExecWhatsThis : public WhatsThis
{
public:
  ExecWhatsThis(CExecView *ev) : WhatsThis(0,ev) { execView = ev; }

//  bool clicked(const QString &whatsThisHref);
  QString text(const QPoint&);

private:
  CExecView *execView;
};

class CComment : public QDialog, public Ui_CComment {

public:
  CComment(QWidget* parent = 0) : QDialog(parent) {
    setupUi(this);
  }

public slots:
};

#endif
/////////////////////////////////////////////////////////////////////////////
