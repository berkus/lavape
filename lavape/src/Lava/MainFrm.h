// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__EF6A882E_2475_11D3_B7B1_000000000000__INCLUDED_)
#define AFX_MAINFRM_H__EF6A882E_2475_11D3_B7B1_000000000000__INCLUDED_

#include "cmainframe.h"


class CLavaMainFrame : public CMainFrame
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
  virtual void fileNew();
  virtual void fileOpen();

  virtual void fileSave();
  virtual void fileSaveAs();
  virtual void fileSaveAll();
  virtual void fileClose();
  virtual void fileExit();
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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_MAINFRM_H__EF6A882E_2475_11D3_B7B1_000000000000__INCLUDED_)
