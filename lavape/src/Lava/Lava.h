// Lava.h : Haupt-Header-Datei für die Anwendung LAVA
//

#ifndef _LAVA
#define _LAVA_


#include "resource.h"       // Hauptsymbole
#include "LavaAppBase.h"       // Hauptsymbole
#include "LavaBaseDoc.h"       // Hauptsymbole
#include "docview.h"
#include "qstring.h"

/////////////////////////////////////////////////////////////////////////////
// CLavaApp:
// Siehe Lava.cpp für die Implementierung dieser Klasse
//

class CLavaApp : public wxApp
{
public:
  CLavaApp(int argc, char ** argv);
  ~CLavaApp() { ExitInstance(); }

  wxDocTemplate *pLavaTaskTemplate, *pLavaLdocTemplate, *pLavaLcomTemplate;
  CLavaBaseData LBaseData;
//  CLavaThread *mainThread;
  DString SyntaxToOpen;
  DString NameToStore;

  QString CMDLine;
  public:
  bool event(QEvent *e);
  virtual wxDocument* OpenDocumentFile(const QString& lpszFileName);
  virtual void OnSaveAll();
  virtual bool DoSaveAll();
  virtual void HtmlHelp();
  virtual void EditingLavaProgs();
  virtual void LearningLava();

public:
  void OnFileOpen();
  void OnFileNew(); 
  void OnAppAbout();
  virtual int ExitInstance();
  void OnChooseFormFont(int font_case);
  void OnChooseGlobalFont();
  void saveSettings();
  QString InitWebBrowser ();

private:
    Q_OBJECT;
};


extern bool Q_EXPORT qt_use_native_dialogs;

extern QString lavaFileDialog(const QString& startFileName, QWidget* parent, const QString& caption, bool existing);

/////////////////////////////////////////////////////////////////////////////


#endif
