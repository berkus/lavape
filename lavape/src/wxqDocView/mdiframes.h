/////////////////////////////////////////////////////////////////////////////
// Name:        mdiframes.h
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCMDI_H_
#define _WX_DOCMDI_H_

#include "DString.h"
#include "qglobal.h"
//Added by qt3to4:
#include <QCustomEvent>
#include <QResizeEvent>
#include <QCloseEvent>
#include "setup.h"
#include "defs.h"
#include "qobject.h"
#include "qevent.h"
#include "docview.h"
#include "mdiframes.h"
#include "defs.h"
#include "qmainwindow.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include "qlist.h"
#include "qworkspace.h"


class WXDLLEXPORT HistWindow : public DString
{
public:
	HistWindow(DString caption, wxMDIChildFrame *win) : DString(caption) { window = win; }
	wxMDIChildFrame *window;
};

class WXDLLEXPORT wxMainFrame: public QMainWindow
{
    //friend class WXDLLEXPORT wxMDIChildFrame;
public:
  wxMainFrame() {}
  wxMainFrame( QWidget* parent, const char* name, Qt::WFlags fl=0);
  virtual ~wxMainFrame();

  virtual bool OnCreate();
  virtual void UpdateUI() {}

//  wxMDIChildFrame *GetActiveChild() const;

  // Get the client window
  QWorkspace *GetClientWindow() const { return m_workspace; }

  void resizeEvent(QResizeEvent& event);
  void closeEvent (QCloseEvent*);
  void OnMRUFile(int histFileIndex);
  void OnMRUWindow(int histWindowIndex);
  void OnCloseWindow();
  void LoadFileHistory();
	wxHistory *GetWindowHistory () { return m_childFrameHistory; }
  void TileVertic(QMenuBar *menubar, int& lastTile);
  void TileHoriz(QMenuBar *menubar, int& lastTile);

protected:
  virtual QWorkspace* CreateWorkspace(QWidget* parent);
  QWidget *m_CentralWidget;
  QVBoxLayout *m_layout;
  QWorkspace *m_workspace;
	wxHistory *m_childFrameHistory;
  QMainWindow *theActiveFrame;
	bool completelyCreated;

public slots:
  virtual void on_fileExitAction_triggered();
  void on_cascadeAction_triggered();
  virtual void on_helpContentsAction_triggered() {};
  void windowActivated(QWidget*);
  void histFile(int histFileIndex);
  void hfStatusText(int itemId);

private:
	virtual void customEvent(QCustomEvent *ev){}

  Q_OBJECT
};

/*
 * Use this instead of wxMDIChildFrame
 */

class WXDLLEXPORT wxMDIChildFrame  : public QWidget
{
public:
  wxMDIChildFrame(QWidget *parent, const char* name);
  virtual bool OnCreate(wxDocTemplate *temp, wxDocument *doc);
  virtual void InitialUpdate();

  virtual ~wxMDIChildFrame();

  bool deleting;
  virtual void UpdateUI() {}
  unsigned GetViewCount() const { return m_viewCount; }
  QWidget *GetClientWindow() const { return m_clientWindow; }
  virtual void SetTitle(QString &title);
  void AddView(wxView *v);
  void RemoveView(wxView *v);
  void NotifyActive(wxView* view) {lastActive = view;}
  wxView* GetLastActive() {return lastActive;}
  virtual void Activate(bool activate=true,bool windowMenuAction=false);
  //virtual void Activate(bool activate=true,bool windowMenuAction=false);

//  FRAMEFACTORY(wxMDIChildFrame)
	Qt::WindowState oldWindowState;

protected:
  virtual bool event ( QEvent * e );
  unsigned m_viewCount;
  QList<wxView*> m_viewList;
  QWidget *m_clientWindow;
  wxView* lastActive;
  QHBoxLayout layout;

private:
  Q_OBJECT
};

#endif
    // _WX_DOCMDI_H_
