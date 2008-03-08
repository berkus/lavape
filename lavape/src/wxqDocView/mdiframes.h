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
#ifdef Darwin
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

class WXDLLEXPORT wxPostTabData
{
public:
  wxTabWidget* source;
  int tabIndex;
  QAction *action;
  wxPostTabData(wxTabWidget* s, int i, QAction *act) {source = s; tabIndex = i; action = act;}
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
  void OnMRUWindow(int histWindowIndex);
  void LoadFileHistory();
	wxHistory *GetWindowHistory () { return m_childFrameHistory; }
  virtual void helpContents(){}
  virtual wxTabWidget* Workspace() {
    return m_currentTabWidget;
  }
  void MoveToNewTabbedWindow(wxTabWidget *tw,int index);
  void MoveToNextTabbedWindow(wxTabWidget *tw,int index);
  void MoveToPrecedingTabbedWindow(wxTabWidget *tw,int index);
  void equalize();
  QSplitter *m_ClientArea;
  
protected:
  virtual QSplitter *CreateWorkspace(QWidget* parent);
  QWidget *m_CentralWidget;
  QVBoxLayout *m_layout;
	wxHistory *m_childFrameHistory;
  QWidget *theActiveFrame;
	bool completelyCreated;
  wxTabWidget *m_currentTabWidget;

public slots:
  virtual void OnFileExit();
  void windowActivated(int);
  void histFile(int);
  void histWindow(int);

private:
	virtual void customEvent(QEvent *ev){}

  Q_OBJECT
};

class WXDLLEXPORT wxTabWidget : public QTabWidget {
public:
  wxTabWidget(QWidget *parent) : QTabWidget(parent) {}

  void mousePressEvent (QMouseEvent *evt);
  void postTabChange(int index, QAction* triggeredAction);
  void closePage2(wxChildFrame *page, int index);
  public slots:
  void closePage();

private:
  QAction *closePageAction;
  QAction *closeFileAction;
  QAction *newTabWidAction;
  QAction *movePageRightAction;
  QAction *movePageLeftAction;
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
  virtual void UpdateUI() {}
  QWidget *GetClientWindow() const { return m_clientWindow; }
  virtual void SetTitle(QString &title);
  void AddView(wxView *v);
  void RemoveView(wxView *v);
  void NotifyActive(wxView* view) {lastActive = view;}
  wxView* GetLastActive() {return lastActive;}
  virtual void Activate(bool activate=true,bool windowMenuAction=false);
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
#ifdef Darwin
MYSTYLE(Mac)
#endif
MYSTYLE(Windows)
MYSTYLE(Plastique)
MYSTYLE(Motif)
MYSTYLE(Cleanlooks)
MYSTYLE(CDE)

#endif
