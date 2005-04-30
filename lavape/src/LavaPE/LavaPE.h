#ifndef __LAVAPE
#define __LAVAPE


//#include "Resource.h"       // main symbols
#include "DString.h"
//#include "LavaPEFrames.h"
#include "ChString.h"
#include "LavaAppBase.h"
#include "DbgThreadPE.h"
#include "ExecUpdate.h"
#include "prelude.h"
#include "sflsock.h"
#include "ASN1File.h"

#include "qcombobox.h"
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
  bool GotoDECL(wxDocument* fromDoc, LavaDECL* decl, TID id, bool sendMess, DString* enumID=0,bool openExec=false);
};


/////////////////////////////////////////////////////////////////////////////
// CLavaPEApp:
// See LavaPE.cpp for the implementation of this class
//

class CLavaPEApp : public wxApp
{
public:
  CLavaPEApp(int argc, char ** argv);
  ~CLavaPEApp();
  wxDocTemplate *pLavaTemplate, *pFormTemplate, *pExecTemplate, *pLComTemplate;
  CLavaPEBrowse Browser;
  CExecUpdate ExecUpdate;
  CLavaBaseData LBaseData;
  QString CMDLine;
  QProcess interpreter;
  CLavaPEDebugThread debugThread;
  //CLavaPEPMdumpThread pmDumpThread;

  bool DoSaveAll();
  bool inTotalCheck;
  ChainOfString FindList;
  QPixmap* LavaPixmaps[50];

  public:
  virtual bool event(QEvent *e);
  virtual int ExitInstance();
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
  void OnUpdateEditUndo(wxAction* action);
  void OnUpdatePopcontext(wxAction* action);
  void OnUpdateEditRedo(wxAction* action);
  void OnUpdateSaveAll(wxAction* action);
  void saveSettings();
  QString InitWebBrowser ();
private:
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

extern bool Q_EXPORT qt_use_native_dialogs;

#endif
