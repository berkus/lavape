#ifndef _LAVA
#define _LAVA

#include "LavaAppBase.h"       // Hauptsymbole
#include "LavaDoc.h"       // Hauptsymbole
#include "DumpView.h"       // Hauptsymbole
#include "docview.h"
#include "docview.h"
#include "qstring.h"
#include "Debugger.h"


/////////////////////////////////////////////////////////////////////////////
// CLavaApp:
// Siehe Lava.cpp fr die Implementierung dieser Klasse
//

class CLavaApp : public wxApp
{
public:
  CLavaApp(int &argc, char ** argv);
  ~CLavaApp() { OnAppExit(); }

  wxDocTemplate *pLavaTaskTemplate, *pLavaLdocTemplate, *pLavaLcomTemplate;
  CLavaBaseData LBaseData;
  DString SyntaxToOpen;
  DString NameToStore;

  CLavaDebugger debugger;

  public:
  bool event(QEvent *e);
  virtual void OpenDocumentFile(const QString& lpszFileName);
  virtual void OnSaveAll();
  virtual bool DoSaveAll();
  virtual void HtmlHelp();
  virtual void EditingLavaProgs();
  virtual void LearningLava();

public:
  void OnFileOpen();
  void OnFileNew();
  void OnAppAbout();
  virtual int OnAppExit();
  void OnChooseFormFont(int font_case);
  void OnChooseGlobalFont();
  void saveSettings();
  QString InitWebBrowser ();

  Q_OBJECT;
};


//extern bool Q_EXPORT qt_use_native_dialogs;

extern QString lavaFileDialog(const QString& startFileName, QWidget* parent, const QString& caption, bool existing);

/////////////////////////////////////////////////////////////////////////////


#endif
