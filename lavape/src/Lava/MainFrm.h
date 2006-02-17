#ifndef __MainFrame
#define __MainFrame

#include "cmainframe.h"
#include "mdiframes.h"


class CLavaMainFrame : public wxMainFrame, public Ui_CMainFrame
{
public:
  CLavaMainFrame();
  virtual ~CLavaMainFrame();
  virtual void UpdateUI();
  int lastTile;
  virtual void customEvent(QCustomEvent *ev);

public slots:
  virtual void on_helpContentsAction_triggered();
  virtual void on_editingLavaProgsAction_triggered();
  virtual void on_learningLavaAction_triggered();
  virtual void on_preconditionsAction_triggered(bool on);
  virtual void on_postconditionsAction_triggered(bool on);
  virtual void on_invariantsAction_triggered(bool on);
  virtual void on_pmDumpAction_triggered(bool on);
  virtual void on_fileNewAction_triggered();
  virtual void on_fileOpenAction_triggered();

  virtual void on_fileSaveAction_triggered();
  virtual void on_fileSaveAsAction_triggered();
  virtual void on_fileSaveAllAction_triggered();
  virtual void on_fileCloseAction_triggered();
  virtual void on_fileExitAction_triggered();
  virtual void makeStyle(const QString &style);
  virtual void on_helpAboutAction_triggered();
  virtual void on_toggleCategoryAction_triggered();
  virtual void on_editUndoAction_triggered();
  virtual void on_insAction_triggered();
  virtual void on_delAction_triggered();
  virtual void on_editCutAction_triggered();
  virtual void on_editCutCopy_triggered();
  virtual void on_editPasteAction_triggered();
  virtual void on_setFormTextFontAction_triggered();
  virtual void on_setFormLabelFontAction_triggered();
//  virtual void setFormButtonFont();
  virtual void on_setGlobalFontAction_triggered();
  virtual void on_tileHorizAction_triggered();
  virtual void on_tileVerticAction_triggered();

private:
  Q_OBJECT
};

#endif
