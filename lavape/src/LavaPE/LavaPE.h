// LavaPE.h : main header file for the LavaPE application
//

#if !defined(_LAVAPE_H__)
#define _LAVAPE_H__


//#include "Resource.h"       // main symbols
#include "DString.h"
//#include "LavaPEFrames.h"
#include "LavaAppBase.h"
#include "ConstrUpdate.h"
//#include "wxExport.h"
#include "qcombobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
#include "ChString.h"


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
  CConstrUpdate ConstrUpdate;
  CLavaBaseData LBaseData;
  QString CMDLine;
  bool DoSaveAll();
  bool inTotalCheck;
  ChainOfString FindList;
  QPixmap* LavaPixmaps[50];



  public:
  virtual int ExitInstance();
  virtual wxDocument* OpenDocumentFile(const QString& lpszFileName);
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


/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_LAVAPE_H__BC8733D8_2301_11D3_B7AF_000000000000__INCLUDED_)
