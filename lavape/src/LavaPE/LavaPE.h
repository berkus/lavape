#ifndef __LAVAPE
#define __LAVAPE


//#include "Resource.h"       // main symbols
#include "DString.h"
//#include "LavaPEFrames.h"
#include "ChString.h"
#include "LavaAppBase.h"
#include "DebuggerPE.h"
#include "ExecUpdate.h"
//#include "prelude.h"
#include <QTcpSocket>
#include "ASN1File.h"

#include "qcombobox.h"
//Added by qt3to4:
#include <QPixmap>
#include <QEvent>
#include <QIcon>
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
#include "qprocess.h"


class CLavaPEBrowse : public CPEBaseBrowse
{
public:
  CLavaPEBrowse() {}
  int HasDefaultForm(LavaDECL* decl, LavaDECL* patDECL, SynDef *lavaCode);
  int findAnyForm(LavaDECL * decl, TID& refID, SynDef *lavaCode);
  void makeDefaultMenu(LavaDECL* decl);
  virtual bool OnFindRefs(wxDocument* fromDoc, LavaDECL* decl, CFindData& fw);

  virtual bool GotoImpl(wxDocument* fromDoc, LavaDECL* decl);
  bool FindImpl(wxDocument* fromDoc, LavaDECL* decl, wxDocument*& implDoc, LavaDECL**& implDECL);
  bool GotoDECL(wxDocument* fromDoc, LavaDECL* decl, TID id, bool sendMess, DString* enumID=0,bool openExec=false, TDeclType execType=ExecDef);
};


/////////////////////////////////////////////////////////////////////////////
// CLavaPEApp:
// See LavaPE.cpp for the implementation of this class
//

class CLavaPEApp : public wxApp
{
public:
  CLavaPEApp(int &argc, char ** argv);
  ~CLavaPEApp();
  wxDocTemplate *pLavaTemplate, *pFormTemplate, *pExecTemplate, *pLComTemplate;
  CLavaPEBrowse Browser;
  CExecUpdate ExecUpdate;
  CLavaBaseData LBaseData;
//  QString CMDLine;
  QProcess interpreter;
  CLavaPEDebugger debugger;
  //CLavaPEPMdumpThread pmDumpThread;

  bool DoSaveAll();
  bool inTotalCheck;
  ChainOfString FindList;
  QIcon* LavaIcons[50];
  QPixmap* LavaPixmaps[50];

  public:
  virtual bool event(QEvent *e);
  virtual int OnAppExit();
  virtual void OpenDocumentFile(const QString& lpszFileName);
  virtual void HtmlHelp();
  virtual void EditingLavaProgs();
  virtual void LearningLava();
  virtual void UpdateUI();
  void OnAppAbout();
  void OnEditUndo();
  void OnFileNew();
  void OnFileOpen();
  void OnPopcontext();
  void OnEditRedo();
  void OnNewComponent();
  void OnChooseTreeFont();
  void OnChooseExecFont();
  void OnChooseFormFont(int font_case);
  void OnChooseGlobalFont();
  void OnSaveAll();
  void OnImport();
  void OnFindByName();
  void OnUpdateEditUndo(QAction* action);
  void OnUpdatePopcontext(QAction* action);
  void OnUpdateEditRedo(QAction* action);
  void OnUpdateSaveAll(QAction* action);
  void saveSettings();
  QString InitWebBrowser ();
  void OnCloseLastExecView();

  Q_OBJECT;
};

extern QString lavaFileDialog(const QString& startFileName, QWidget* parent, const QString& caption, bool existing);

extern  int enumBM;
extern  int enumselBM;
extern  int lavafileBM;
extern  int includeBM;
extern  int xerrBM;
extern  int xcomBM;
extern  int xerrcomBM;
extern  int overRidesBM;

//???extern bool Q_EXPORT qt_use_native_dialogs;

#endif
