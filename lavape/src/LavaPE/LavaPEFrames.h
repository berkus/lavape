#ifndef __LavaPEFrames
#define __LavaPEFrames


#include "LavaGUIFrame.h"
#include "PEBaseDoc.h"
#include "Bars.h"
#include "LavaGUIView.h"
#include "docview.h"
#include "mdiframes.h"
#include "cmainframe.h"
#include "qapplication.h"
#include "qframe.h"
#include "qtoolbar.h"
#include "qcombobox.h"
#include <QMenu>
#include "qsplitter.h"
//#include "q3vbox.h"
#include "qevent.h"
#include "qstring.h"
#include "qfont.h"
//Added by qt3to4:
#include <QCloseEvent>
#include <QTreeWidget>

/*
typedef QMap<QString,QString> HelpTextMap;

class ToolbarWhatsThis : public WhatsThis
{
public:
  ToolbarWhatsThis(QToolBar *tb):WhatsThis(0,tb) { toolbar = tb; }

//  bool clicked(const QString &whatsThisHref);
  QString text(const QPoint&);
  HelpTextMap helpTextMap;

private:
  QToolBar *toolbar;
};
*/

class CLavaMainFrame : public CMainFrame
{
public:
  CLavaMainFrame();
  virtual ~CLavaMainFrame();

  bool OnCreate();
	virtual void UpdateUI();
  CUtilityView  * m_UtilityView;
  bool UtilitiesHidden;
  int LastUtilitiesState; //-1:hidden, else: ActTab of m_UtilityView
//  void ShowUtilitiesTab(UtilityTabs tab);
  void helpContents() { on_helpContentsAction_triggered(); }
  void fillKwdToolbar(QToolBar *tb);
  void fillHelpToolbar(QToolBar *tb);
  void newKwdToolbutton(QToolBar *tb,QPushButton *&pb,char *text,char *slotParm,QString tooltip=QString::null,QString whatsThis=QString::null);
  void newHelpToolbutton(QToolBar *tb,QPushButton *&pb,char *text,char *slotParm,char *tooltip=0,char *whatsThis=0);
	void customEvent(QEvent *ev);
  bool eventFilter(QObject *obj,QEvent *ev);
/*
  void fillHelpMap1(ToolbarWhatsThis *tbw);
  void fillHelpMap2(ToolbarWhatsThis *tbw);
  void fillHelpMap3(ToolbarWhatsThis *tbw);
  void fillHelpMap4(ToolbarWhatsThis *tbw);
  void fillHelpMap5(ToolbarWhatsThis *tbw);
  void fillHelpMap6(ToolbarWhatsThis *tbw);
*/
private:
	int	lastTile;

  void OnUpdateshowUtil(QAction* action);
//  void OnUpdateViewToolbar1(QAction* action);
//  void OnUpdateViewToolbar2(QAction* action);
//  void OnUpdateViewToolbar4(QAction* action);
//  void OnUpdateViewToolbar3(QAction* action);
//	bool event(QEvent *ev);

public slots:
    void makeStyle(const QString &style);

    virtual void on_fileNewAction_triggered();
    virtual void on_fileOpenAction_triggered();
    virtual void on_fileSaveAction_triggered();
    virtual void on_fileSaveAsAction_triggered();
    virtual void on_fileSaveAllAction_triggered();
    virtual void on_fileCloseAction_triggered();
    virtual void filePrint();
    virtual void on_fileExitAction_triggered() { OnFileExit(); }
    virtual void on_editUndoAction_triggered();
    virtual void on_editRedoAction_triggered();
    virtual void on_editCutAction_triggered();
    virtual void on_editCopyAction_triggered();
    virtual void on_editPasteAction_triggered();
    virtual void on_helpContentsAction_triggered();
    virtual void on_editingLavaProgsAction_triggered();
    virtual void on_learningLavaAction_triggered();
    virtual void on_helpAboutAction_triggered();
    virtual void on_tileHorizAction_triggered();
    virtual void on_tileVerticAction_triggered();
    virtual void on_saveEveryChangeAction_triggered(bool);
    virtual void on_viewTB1Action_triggered();
    virtual void on_viewTB2Action_triggered();
    virtual void on_viewTB3Action_triggered();
    virtual void on_viewTB4Action_triggered();
    virtual void on_viewHelpTBAction_triggered();
    virtual void on_viewDebugTBAction_triggered();
    virtual void on_whatNextAction_triggered();
//    virtual void on_howTo_clicked();

    virtual void on_findRefsAction_triggered();

    virtual void on_newCompoAction_triggered();
    virtual void on_importComponentAction_triggered();
    virtual void on_expandAction_triggered();
    virtual void on_collapseAction_triggered();
    virtual void on_showOptsAction_triggered();
    virtual void on_showAllOptsAction_triggered();
    virtual void on_hideEmptyOptsAction_triggered();
    virtual void on_nextCommentAction_triggered();
    virtual void on_prevCommentAction_triggered();
    virtual void on_nextErrorAction_triggered();
    virtual void on_prevErrorAction_triggered();
    virtual void on_checkAction_triggered();
    virtual void on_checkAllAction_triggered();
    virtual void on_runAction_triggered();
    virtual void on_newIncludeAction_triggered();
    virtual void on_newPackageAction_triggered();
    virtual void on_newInitiatorAction_triggered();
    virtual void on_newInterfaceAction_triggered();
    virtual void on_newImplAction_triggered();
    virtual void on_newCOspecAction_triggered();
    virtual void on_newCOimplAction_triggered();
    virtual void on_newSetAction_triggered();
    virtual void on_newEnumAction_triggered();
    virtual void on_newVTAction_triggered();
    virtual void on_newFuncAction_triggered();
    virtual void on_newMemVarAction_triggered();
    virtual void on_newEnumItemAction_triggered();
    virtual void on_editSelItemAction_triggered();
    virtual void on_editCommentAction_triggered();
    virtual void on_gotoDeclAction_triggered();
    virtual void on_gotoImplAction_triggered();
    virtual void on_returnToRefAction_triggered();
    virtual void on_findByNameAction_triggered();
    virtual void on_showOverridablesAction_triggered();
    virtual void on_overrideAction_triggered();
    virtual void on_openFormViewAction_triggered();
    virtual void on_insertTextAction_triggered();
    virtual void on_showUtilWindowAction_triggered();
    virtual void on_makeGUIAction_triggered();
/*
    virtual void newCompo();
    virtual void showUtilWindow();
    virtual void expandSubtree();
    virtual void collapseSubtree();
    virtual void showOpts();
    virtual void showAllOpts();
    virtual void hideEmptyOpts();
    virtual void nextComment();
    virtual void prevComment();
    virtual void nextError();
    virtual void prevError();
    virtual void check();
    virtual void checkAll();
    virtual void run();
    virtual void newInclude();
    virtual void newPackage();
    virtual void newInitiator();
    virtual void new_Interface();
    virtual void newImpl();
    virtual void newCOspec();
    virtual void newCOimpl();
    virtual void newSet();
    virtual void newEnum();
    virtual void newVT();
    virtual void newFunc();
    virtual void newMemVar();
    virtual void newEnumItem();
    virtual void editSelItem();
    virtual void editComment();
    virtual void gotoDec();
    virtual void gotoImpl();
    virtual void returnToRef();
    virtual void findByName();
    virtual void showOverridables();
    virtual void override();
    virtual void openFormView();
    virtual void makeGUI();
    virtual void insertText();
    virtual void importComponent();
*/
    virtual void on_insAction_triggered();
    virtual void on_delAction_triggered();
    virtual void on_genLinkedHtmlAction_triggered();
    virtual void on_genSingleHtmlAction_triggered();
    virtual void deleteOp();

// Exec handlers (tool buttons):
    virtual void on_DbgAction_triggered();
    virtual void on_DbgStepNextAct_triggered();
    virtual void on_DbgStepNextFunctionAct_triggered();
    virtual void on_DbgStepintoAct_triggered();
    virtual void on_DbgStepoutAct_triggered();
    virtual void on_DbgRunToSelAct_triggered();
    virtual void on_DbgBreakpointAct_triggered();
    virtual void on_DbgClearBreakpointsAct_triggered();
    virtual void on_DbgStopAction_triggered();
    virtual void on_toggleCategoryAction_triggered();
    virtual void on_optLocalVarAction_triggered();
    virtual void on_toggleSubstTypeAction_triggered();
    virtual void on_toggleClosedAction_triggered();
    virtual void on_insertBeforeAction_triggered();
    virtual void on_execInsAction_triggered();
    virtual void on_toggleArrowsAction_triggered();
    virtual void on_toggleInputArrowsAction_triggered();
    virtual void on_conflictingAssigAction_triggered();
    virtual void on_newLineAction_triggered();
    virtual void on_commentOutAction_triggered();
    virtual void on_toggleCommentsAction_triggered();
    virtual void on_lessThanAction_triggered();
    virtual void on_viewTB5Action_triggered();
    virtual void on_lessEqualAction_triggered();
    virtual void on_equalAction_triggered();
    virtual void on_notEqualAction_triggered();
    virtual void on_greaterEqualAction_triggered();
    virtual void on_greaterThanAction_triggered();
    virtual void on_functionCallAction_triggered();
    virtual void on_staticCallAction_triggered();
    virtual void on_trueAction_triggered();
    virtual void on_falseAction_triggered();
    virtual void on_evaluateAction_triggered();
    virtual void on_nullAction_triggered();
    virtual void on_intervalAction_triggered();
    virtual void on_toggleSignAction_triggered();
    virtual void on_additionAction_triggered();
    virtual void on_multiplicationAction_triggered();
    virtual void on_divideAction_triggered();
    virtual void on_modulusAction_triggered();
    virtual void on_leftShiftAction_triggered();
    virtual void on_rightShiftAction_triggered();
    virtual void on_bitAndAction_triggered();
    virtual void on_bitOrAction_triggered();
    virtual void on_bitXorAction_triggered();
    virtual void on_invertAction_triggered();
    virtual void on_handleAction_triggered();
    virtual void on_ordAction_triggered();

// Exec handlers (keyword push buttons):
    virtual void declare();
    virtual void exists();
    virtual void foreachStm();
    virtual void select();
    virtual void elInSet();
    virtual void ifStm();
    virtual void ifdefStm();
    virtual void ifExpr();
    virtual void elseExpr();
    virtual void switchStm();
    virtual void typeStm();
    virtual void and_stm();
    virtual void or_stm();
    virtual void xor_stm();
    virtual void not_stm();
    virtual void assert_stm();
    virtual void try_stm();
    virtual void succeed();
    virtual void throwEx();
    virtual void call();
    virtual void set();
    virtual void newExpr();
    virtual void old();
    virtual void clone();
    virtual void copy();
    virtual void attach();
    virtual void qryItf();
    virtual void scale();
    virtual void getEnumItem();
    virtual void connectSignal();
    virtual void disconnect();
    virtual void emitSignal();
// end of exec handlers

    virtual void on_viewTB6Action_triggered();
    virtual void on_viewTB7Action_triggered();
    virtual void on_setTreeFontAction_triggered();
    virtual void on_setFormTextFontAction_triggered();
    virtual void on_setFormLabelFontAction_triggered();
//    virtual void setFormButtonFont();
    virtual void on_setExecFontAction_triggered();
    virtual void on_setGlobalFontAction_triggered();

private:
    Q_OBJECT
};


class CTreeFrame : public wxMDIChildFrame
{
public:
  CTreeFrame(QWidget *parent);
  FRAMEFACTORY(CTreeFrame)
  bool OnCreate(wxDocTemplate *temp, wxDocument *doc);
  virtual void InitialUpdate();
  virtual ~CTreeFrame();
	void closeEvent(QCloseEvent*);

  virtual void Activate(bool activate=true, bool windowMenuAction=false);
  void CalcSplitters(bool showVT = false, bool showINCL=false);

  QSplitter* splitter;
//  QVBox *vb;
  CLavaBaseView* viewL;
  CLavaBaseView* viewM;
  CLavaBaseView* viewR;
  bool showIt;
  int widthPrev;

  void SetModified(bool changed);
private:
    Q_OBJECT;
};


/////////////////////////////////////////////////////////////////////////////
// CFormFrame frame
//
// This is the Childframe class of the Splitter - View containing
// a CLavaGUIView - View and a CLavaPEView - View
//
/////////////////////////////////////////////////////////////////////////////


class CFormFrame : public CLavaGUIFrame
{
public:
  CFormFrame(QWidget* parent);
  FRAMEFACTORY(CFormFrame)

  QSplitter *splitter;
  CLavaGUIView* viewL;
  CLavaBaseView* viewR;
  CLavaBaseView* wizardView;
  int widthPrev;
  bool OnCreate(wxDocTemplate *temp, wxDocument *doc);
  virtual void InitialUpdate();
  virtual ~CFormFrame();
  void CalcSplitters();
private:
    Q_OBJECT;
};


#define DOCVIEW_CUT     1
#define DOCVIEW_ABOUT   2
#endif
