#ifndef __LavaPEFrames
#define __LavaPEFrames


#include "LavaGUIFrame.h"
#include "PEBaseDoc.h"
#include "Bars.h"
#include "LavaGUIView.h"
//#include "wxExport.h"
#include "docview.h"
#include "mdiframes.h"
#include "cmainframe.h"
#include "qapplication.h"
#include "q3frame.h"
#include "qtoolbar.h"
#include "q3combobox.h"
#include "q3popupmenu.h"
#include "q3listview.h"
#include "qsplitter.h"
#include "q3vbox.h"
#include "qevent.h"
#include "qstring.h"
#include "qfont.h"
//Added by qt3to4:
#include <QCustomEvent>
#include <QCloseEvent>


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


class CLavaMainFrame : public wxMainFrame, public Ui_CMainFrame
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
  void fillKwdToolbar(QToolBar *tb);
  void fillHelpToolbar(QToolBar *tb);
  void newKwdToolbutton(QToolBar *tb,QPushButton *&pb,char *text,char *slotParm,QString tooltip=QString::null,QString whatsThis=QString::null);
  void newHelpToolbutton(QToolBar *tb,QPushButton *&pb,char *text,char *slotParm,char *tooltip=0,char *whatsThis=0);
	void customEvent(QCustomEvent *ev);
  void fillHelpMap1(ToolbarWhatsThis *tbw);
  void fillHelpMap2(ToolbarWhatsThis *tbw);
  void fillHelpMap3(ToolbarWhatsThis *tbw);
  void fillHelpMap4(ToolbarWhatsThis *tbw);
  void fillHelpMap5(ToolbarWhatsThis *tbw);
  void fillHelpMap6(ToolbarWhatsThis *tbw);

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

    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
//    virtual void fileExit();
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editFind();
    virtual void helpContents();
    virtual void editingLavaProgs();
    virtual void learningLava();
    virtual void helpAbout();
    virtual void tileHoriz();
    virtual void tileVertic();
    virtual void cascade();
    virtual void fileSaveAll();
    virtual void fileClose();
    virtual void saveEveryChange(bool);
    virtual void viewTB1();
    virtual void viewTB2();
    virtual void viewTB3();
    virtual void viewTB4();
    virtual void viewHelpTB();
    virtual void whatNext_clicked();
    virtual void howTo_clicked();
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
    virtual void insAction_activated();
    virtual void delAction_activated();
    virtual void importComponent();
    virtual void genLinkedHtml();
    virtual void genSingleHtml();
    virtual void deleteOp();

// Exec handlers (tool buttons):
    virtual void DbgStart();
    virtual void DbgStepNext();
    virtual void DbgStepNextFunction();
    virtual void DbgStepinto();
    virtual void DbgStepout();
    virtual void DbgRunToSel();
    virtual void DbgBreakpoint();
    virtual void DbgClearBreakpoints();
    virtual void DbgStop();
    virtual void toggleCategory();
    virtual void optLocalVar();
    virtual void toggleSubstType();
    virtual void insertBefore();
    virtual void insert();
    virtual void toggleArrows();
    virtual void toggleInputArrows();
    virtual void conflictingAssig();
    virtual void newLine();
    virtual void commentOut();
    virtual void toggleComments();
    virtual void lessThan();
    virtual void viewTB5();
    virtual void lessEqua();
    virtual void equal();
    virtual void notEqual();
    virtual void greaterEqual();
    virtual void greaterThan();
    virtual void functionCall();
    virtual void staticCall();
    virtual void trueHandler();
    virtual void falseHandler();
    virtual void evaluate();
    virtual void null();
    virtual void interval();
    virtual void toggleSign();
    virtual void addition();
    virtual void multiplication();
    virtual void divide();
    virtual void modulus();
    virtual void leftShift();
    virtual void rightShift();
    virtual void bitAnd();
    virtual void bitOr();
    virtual void bitXor();
    virtual void invert();
    virtual void handle();
    virtual void ord();

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
    virtual void fail();
    virtual void call();
    virtual void set();
    virtual void newExpr();
    virtual void old();
    virtual void clone();
    virtual void copy();
    virtual void attach();
    virtual void qryItf();
    virtual void scale();
    virtual void item();
    virtual void connectSignal();
    virtual void disconnect();
    virtual void emitSignal();
// end of exec handlers

    virtual void viewTB6();
    virtual void viewTB7();
    virtual void adjustToolbar_7 ();
    virtual void setTreeFont();
    virtual void setFormTextFont();
    virtual void setFormLabelFont();
//    virtual void setFormButtonFont();
    virtual void setExecFont();
    virtual void setGlobalFont();
    virtual void newCompo();

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
  Q3VBox *vb;
  CLavaBaseView* viewL;
  CLavaBaseView* viewM;
  CLavaBaseView* viewR;
  int widthPrev;
  bool showIt;
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
