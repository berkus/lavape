// ExecView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExecView view

#ifdef __GNUC__
#pragma interface "ExecView.h"
#endif


#ifndef __CExecView
#define __CExecView

#include "qevent.h"
#include "qscrollview.h"
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
#include "Constructs.h"
#include "docview.h"
#include "mdiframes.h"
#include "LavaAppBase.h"
#include "PEBaseDoc.h"
#include "ComboBar.h"
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

  bool editCtlVisible, insertBefore, editCut, doubleClick,
       clicked, active, forcePrimTokenSelect, deletePending,
       inIgnore, inExecHeader, inFormParms, inBaseInits,
       inParameter, inForeach, externalHint, execReplaced, nextError,
       destroying, autoScroll, errMsgUpdated, initialUpdateDone;
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
  bool Taboo ();
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
  void OnIfExpr();
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
  void OnCallback();



//  LRESULT OnCalledView(WPARAM, LPARAM lparam);
//  void OnSetFocus(QWidget* pOldWnd);
//  void OnKillFocus(QWidget* pNewWnd);
  void OnChar(QKeyEvent *e);

	void OnUpdateOrd(wxAction* action);
	void OnUpdateOptLocalVar(wxAction* action);
  void OnUpdateHandle(wxAction* action);
  void OnUpdateInputArrow(wxAction* action);
  void OnUpdateEditSel(wxAction* action);
  void OnUpdateEvaluate(wxAction* action);
  void OnUpdateToggleSubstitutable(wxAction* action);
  void OnUpdateConflict(wxAction* action);
  void OnUpdateToggleCategory(wxAction* action);
  void OnUpdateNextComment(wxAction* action);
  void OnUpdatePrevComment(wxAction* action);
  void OnUpdateNextError(wxAction* action);
  void OnUpdatePrevError(wxAction* action);
  void OnUpdateInterval(wxAction* action);
  void OnUpdateStaticCall(wxAction* action);
  void OnUpdateIgnore(wxAction* action);
  void OnUpdateModulus(wxAction* action);
  void OnUpdateNewLine(wxAction* action);
  void OnUpdateShowOptionals(wxAction* action);
  void OnUpdateGotoDecl(wxAction* action);
  void OnUpdateGotoImpl(wxAction* action);
  void OnUpdateFindReferences(wxAction* action);
  void OnUpdateToggleArrows(wxAction* action);
  void OnUpdateComment(wxAction* action);
  void OnUpdateButtonEnum();
  void OnUpdateNewFunc();
  void OnUpdateNewPFunc();
  void OnUpdateEditCut(wxAction* action);
  void OnUpdateEditCopy(wxAction* action);
  void OnUpdateEditPaste(wxAction* action);
  void OnUpdateEditUndo(wxAction* action);
  void OnUpdateEditRedo(wxAction* action);
  void OnUpdateBitOr(wxAction* action);
  void OnUpdateBitAnd(wxAction* action);
  void OnUpdateBitXor(wxAction* action);
  void OnUpdateDelete(wxAction* action);
  void OnUpdateDivide(wxAction* action);
  void OnUpdateEq(wxAction* action);
  void OnUpdateFalse(wxAction* action);
  void OnUpdateFunctionCall(wxAction* action);
  void OnUpdateGe(wxAction* action);
  void OnUpdateGt(wxAction* action);
  void OnUpdateInsert(wxAction* action);
  void OnUpdateInsertBefore(wxAction* action);
  void OnUpdateInvert(wxAction* action);
  void OnUpdateLe(wxAction* action);
  void OnUpdateLshift(wxAction* action);
  void OnUpdateLt(wxAction* action);
  void OnUpdatePlusMinus(wxAction* action);
  void OnUpdateMult(wxAction* action);
  void OnUpdateNe(wxAction* action);
  void OnUpdateShowComments(wxAction* action);
  void OnUpdateNull(wxAction* action);
  void OnUpdatePlus(wxAction* action);
  void OnUpdateRshift(wxAction* action);
  void OnUpdateTrue(wxAction* action);

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
  void OnUpdateCallback(QPushButton *pb);
  void OnUpdateAssign(QPushButton *pb);
  void OnUpdateAttach(QPushButton *pb);
  void OnUpdateQueryItf(QPushButton *pb);
  void OnUpdateAnd(QPushButton *pb);
  void OnUpdateClone(QPushButton *pb);
  void OnUpdateSelect(QPushButton *pb);
  void OnUpdateDeclare(QPushButton *pb);
  void OnUpdateForeach(QPushButton *pb);
  void OnUpdateIf(QPushButton *pb);
  void OnUpdateIfExpr(QPushButton *pb);
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


class MyScrollView : public QScrollView {
public:
  MyScrollView::MyScrollView (QWidget *parent);

	int calcIndent(CHETokenNode *currentToken);

  CProgText *text;
  CExecView *execView;
  Format fmt;
	int currentX, currentY, widthOfIndent, widthOfBlank, contentsWidth, contentsHeight;
  QFontMetrics *fm;
	QPainter *p;

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
  ExecWhatsThis(CExecView *ev);

  bool clicked(const QString &whatsThisHref);
  QString text(const QPoint&);
private:
  CExecView *execView;
};


extern LAVAEXECS_DLL QToolBar* makeToolbar(wxMainFrame* pMainFrame);
extern LAVAEXECS_DLL QToolBar* makeExecSwitches(wxMainFrame* pMainFrame);
extern LAVAEXECS_DLL QToolBar* makeConstraintbar(wxMainFrame* pMainFrame);

#endif
/////////////////////////////////////////////////////////////////////////////
