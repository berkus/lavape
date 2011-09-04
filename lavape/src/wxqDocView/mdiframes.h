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
#include <QGtkStyle>
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

  void resizeEvent(QResizeEvent& event);
  void closeEvent (QCloseEvent*);
  bool eventFilter(QObject *o, QEvent *e);

  void OnMRUFile(int histFileIndex);
  //void OnMRUWindow(int histWindowIndex);
  void LoadFileHistory();
	//wxHistory *GetWindowHistory () { return m_childFrameHistory; }
  virtual void helpContents(){}
  void MoveToNewTabbedWindow(wxTabWidget *tw,int index);
  void MoveToNextTabbedWindow(wxTabWidget *tw,int index);
  void MoveToPrecedingTabbedWindow(wxTabWidget *tw,int index);
  void DropPage(wxTabWidget* s_wt, int s_index, wxTabWidget* d_wt, int d_index);
  void equalize();
  virtual void ClearMainFrame(){}
  QSplitter *m_ClientArea;
  
protected:
  virtual QSplitter *CreateWorkspace(QWidget* parent);
  QWidget *m_CentralWidget;
  QVBoxLayout *m_layout;
	//wxHistory *m_childFrameHistory;
  QWidget *theActiveFrame;
	bool completelyCreated;

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
  QMenu tabMenu;
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
      deleting = false;
  }

  ~wxTabWidget() {
    wxDocManager *docMan=wxDocManager::GetDocumentManager();

    if (docMan && (this == docMan->GetCurrentTabWidget())) {
      docMan->SetCurrentTabWidget(0);
      docMan->RememberActiveIndexes(0,0);
    }
  }

  void postTabChange(int index, QAction* triggeredAction);
  void setTabTextColor (int index, const QColor &color) {
    tabBar()->setTabTextColor(index,color);
  }
  void closePage(wxChildFrame *page=0);
  void customEvent(QEvent*);

  wxTabBar *m_tabBar;
  int m_closedPageIndex;
  bool deleting;

public slots:
  void postClosePage();

private:
  Q_OBJECT
};


class WXDLLEXPORT wxChildFrame : public QFrame
{
  friend class wxTabBar;

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
  void RememberLastActive(wxView* view) {lastActive = view;}
  wxView* GetLastActive() {return lastActive;}
  virtual void Activate(bool topDown);
  wxTabWidget *m_tabWidget;
  int m_pageIndex;
  int m_tabWidIndex;
  wxDocument *m_document;

//  FRAMEFACTORY(wxChildFrame)
	//Qt::WindowState oldWindowState;


protected:
  void resizeEvent(QResizeEvent *ev);
  bool event (QEvent * e);
  QList<wxView*> m_viewList;
  QWidget *m_clientWindow;
  wxView* lastActive;
  QVBoxLayout *layout;

private:
  Q_OBJECT
};


#define MYSTYLE(sty) \
class WXDLLEXPORT My##sty##Style : public Q##sty##Style {\
public:\
  int pixelMetric(PixelMetric pm, const QStyleOption *option, const QWidget *widget) const;\
};

#ifdef Q_WS_WIN
MYSTYLE(WindowsXP)
#endif
#ifdef Q_WS_MAC
MYSTYLE(Mac)
#endif
//#ifdef __Linux
//MYSTYLE(Gtk)
//#endif
MYSTYLE(Windows)
MYSTYLE(Plastique)
MYSTYLE(Motif)
MYSTYLE(Cleanlooks)
MYSTYLE(CDE)

#endif
