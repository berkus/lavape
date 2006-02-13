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
  virtual void helpContents();
  virtual void editingLavaProgs();
  virtual void learningLava();
  void PreconditionsToggled(bool on);
  void PostconditionsToggled(bool on);
  void InvariantsToggled(bool on);
  void PmDumpsToggled(bool on);

public slots:
  virtual void on_fileNewAction_activated();
  virtual void on_fileOpenAction_activated();

  virtual void on_fileSaveAction_activated();
  virtual void on_fileSaveAsAction_activated();
  virtual void on_fileSaveAll_activated();
  virtual void on_fileClose_activated();
  virtual void on_fileExitAction_activated();
  void makeStyle(const QString &style);
  void helpAbout();
  void okAction_activated();
  void toggleCategoryAction_activated();
  void editUndoAction_activated();
  void insAction_activated();
  void delAction_activated();
  void editCut();
  void editCopy();
  void editPaste();
  virtual void setFormTextFont();
  virtual void setFormLabelFont();
//  virtual void setFormButtonFont();
  virtual void setGlobalFont();
  virtual void tileHoriz();
  virtual void tileVertic();
  virtual void cascade();

private:
  Q_OBJECT
};

#endif
