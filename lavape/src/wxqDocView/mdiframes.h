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
#include <QWindowsStyle>
#include <QWindowsXPStyle>
#include <QWindowsVistaStyle>
#include <QMotifStyle>
#include <QCDEStyle>
#include <QPlastiqueStyle>
#include <QCleanlooksStyle>
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>

#ifdef __Darwin
#include <QMacStyle>
#endif
#include "qlist.h"
#include "qworkspace.h"


class WXDLLEXPORT HistWindow : public DString
{
public:
	HistWindow(DString caption, wxChildFrame *win) : DString(caption) { window = win; }
	wxChildFrame *window;
};

class WXDLLEXPORT wxTabChangeData
{
public:
  wxTabWidget* source;
  int sIndex;
  wxTabWidget* dest;
  int dIndex;
  QAction *action;
  wxTabChangeData(wxTabWidget* w, int i) {source=w; sIndex=i; dest=0; dIndex=0; action=0;}
  wxTabChangeData(wxTabWidget* s, int i, QAction *act) {source = s; sIndex = i; action = act; dest=0;dIndex=0;}
  wxTabChangeData(wxTabWidget* s, int si, wxTabWidget* d, int di) {source = s; sIndex = si; dest = d; dIndex = di; action=0;}
};

class WXDLLEXPORT wxMainFrame: public QMainWindow
{
    friend class wxChildFrame;
    friend class wxApp;
public:
  wxMainFrame();
  virtual ~wxMainFrame();

  virtual bool OnCreate();
  virtual void UpdateUI() {}

//  wxChildFrame *GetActiveChild() const;

  // Get the client window
  wxTabWidget *GetCurrentTabWindow() const { return m_currentTabWidget; }
  void SetCurrentTabWindow(wxTabWidget *tw) { m_currentTabWidget = tw; }

//  bool eventFilter(QObject *obj, QEvent *ev);
  void resizeEvent(QResizeEvent& event);
  void closeEvent (QCloseEvent*);
  bool eventFilter(QObject *o, QEvent *e);

  void OnMRUFile(int histFileIndex);
  //void OnMRUWindow(int histWindowIndex);
  void LoadFileHistory();
	//wxHistory *GetWindowHistory () { return m_childFrameHistory; }
  virtual void helpContents(){}
  virtual wxTabWidget* Workspace() {
    return m_currentTabWidget;
  }
  void MoveToNewTabbedWindow(wxTabWidget *tw,int index);
  void MoveToNextTabbedWindow(wxTabWidget *tw,int index);
  void MoveToPrecedingTabbedWindow(wxTabWidget *tw,int index);
  void DropPage(wxTabWidget* s_wt, int s_index, wxTabWidget* d_wt, int d_index);
  void equalize();
  QSplitter *m_ClientArea;
  
protected:
  virtual QSplitter *CreateWorkspace(QWidget* parent);
  QWidget *m_CentralWidget;
  QVBoxLayout *m_layout;
	//wxHistory *m_childFrameHistory;
  QWidget *theActiveFrame;
	bool completelyCreated;
  wxTabWidget *m_currentTabWidget;

public slots:
  virtual void OnFileExit();
  void histFile(int);
  //void histWindow(int);

private:
	virtual void customEvent(QEvent *ev){}

  Q_OBJECT
};

class WXDLLEXPORT wxTabBar : public QTabBar {
public:
  wxTabBar(QWidget* parent);
  void mouseMoveEvent(QMouseEvent *evt);
  void mousePressEvent (QMouseEvent *evt);
  void dragEnterEvent(QDragEnterEvent *ev);
  void dropEvent(QDropEvent* ev);

  QPoint dragStartPosition;
  const char* wxDragFormat;
  QAction *closePageAction;
  QAction *closeFileAction;
  QAction *newTabWidAction;
  QAction *movePageRightAction;
  QAction *movePageLeftAction;
};

class WXDLLEXPORT wxTabWidget : public QTabWidget {
public:
  wxTabWidget(QWidget *parent) : QTabWidget(parent) {
    m_tabBar = new wxTabBar(this);
    setTabBar(m_tabBar);
    connect(this ,SIGNAL(currentChanged(int)), SLOT(windowActivated(int)));
  }

  void postTabChange(int index, QAction* triggeredAction);
  void removePage(wxChildFrame *page);
  void setTabTextColor (int index, const QColor &color) {
    tabBar()->setTabTextColor(index,color);
  }

  wxTabBar *m_tabBar;

public slots:
  void closePage();
  void windowActivated(int);

private:
  Q_OBJECT
};


class WXDLLEXPORT wxChildFrame : public QFrame
{
public:
  wxChildFrame(QWidget *parent);
  virtual bool OnCreate(wxDocTemplate *temp, wxDocument *doc);
  virtual void InitialUpdate();

  virtual ~wxChildFrame();

  bool deleting;
  void correctMyTabWidget(wxTabWidget *tw);
  void closeMyPage();
  virtual void UpdateUI() {}
  QWidget *GetClientWindow() const { return m_clientWindow; }
  virtual void SetTitle(QString &title);
  void AddView(wxView *v);
  void RemoveView(wxView *v);
  void NotifyActive(wxView* view) {lastActive = view;}
  wxView* GetLastActive() {return lastActive;}
  virtual void Activate(bool activate=true);
  //virtual void focusIn();
  //virtual void Activate(bool activate=true,bool windowMenuAction=false);
  wxTabWidget *m_tabWidget;

//  FRAMEFACTORY(wxChildFrame)
	//Qt::WindowState oldWindowState;

protected:
  void resizeEvent(QResizeEvent *ev);
  bool event (QEvent * e);
  QList<wxView*> m_viewList;
  QWidget *m_clientWindow;
  wxView* lastActive;
  wxDocument *m_document;
  QVBoxLayout *layout;

private:
  Q_OBJECT
};


#define MYSTYLE(sty) \
class WXDLLEXPORT My##sty##Style : public Q##sty##Style {\
public:\
  int pixelMetric(PixelMetric pm, const QStyleOption *option, const QWidget *widget) const;\
};

#ifdef WIN32
MYSTYLE(WindowsXP)
MYSTYLE(WindowsVista)
#endif
#ifdef __Darwin
MYSTYLE(Mac)
#endif
MYSTYLE(Windows)
MYSTYLE(Plastique)
MYSTYLE(Motif)
MYSTYLE(Cleanlooks)
MYSTYLE(CDE)

#endif
