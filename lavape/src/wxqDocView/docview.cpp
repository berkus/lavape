/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
         (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This file has been taken over from www.wxwidgets.org
   and adapted to the needs of LavaPE */



/////////////////////////////////////////////////////////////////////////////
// Name:        docview.cpp
// Purpose:     Document/view classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------


#include "SYSTEM.h"
#include "setup.h"

#include "defs.h"
#include "docview.h"
#include "mdiframes.h"

#include "qmetaobject.h"
#include "qlist.h"
#include "qstring.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qfiledialog.h"
#include "qapplication.h"
#include "qmessagebox.h"
#include "qinputdialog.h"
#include "qmenu.h"
#include "qstatusbar.h"
#include "qmenudata.h"
#include "qdatastream.h"
#include "qtimer.h"
#include "qsettings.h"
//Added by qt3to4:
#include <QFocusEvent>
#include <QMouseEvent>
#include <QAbstractEventDispatcher>
#include <QtDebug>


//#if wxUSE_PRINTING_ARCHITECTURE
//  #include "prntbase.h"
//  #include "wx/printdlg.h"
//#endif

#include <stdio.h>
#include <string.h>
#include <signal.h>

#pragma hdrstop

#ifdef MINGW32
#include <shlobj.h>
#include <windows.h>
#include <winbase.h>
#endif

QString wxEmptyString;

static inline QString FindExtension(const char *path);
static const QString s_MRUEntryFormat("%1 %2");


// break into the debugger
void Trap()
{
//#ifdef __WXMSW__
//    DebugBreak();
#if defined(__WXMAC__)
#if __powerc
    Debugger();
#else
    SysBreak();
#endif
#elif defined(__UNIX__)
    raise(SIGTRAP);
#else
    // TODO
#endif // Win/Unix
}


WXDLLEXPORT_DATA(wxApp*) wxTheApp;

wxApp::wxApp(int &argc, char **argv) : QApplication(argc,argv)
{
  wxTheApp = this;
  this->argc = argc;
  this->argv = argv;
  m_appName = argv[0];
  mainThread = QThread::currentThread();

  //// Create a document manager
  m_docManager = new wxDocManager;
  deletingMainFrame = false;
  isChMaximized = true;

  SetClassName(argv[0]);

  //QApplication::connect((const QObject*)QAbstractEventDispatcher::instance(),SIGNAL(aboutToBlock()),this,SLOT(updateButtonsMenus()));
  appExit = false;
}

wxApp::~wxApp() {
  QSettings settings(QSettings::NativeFormat,QSettings::UserScope,wxTheApp->GetVendorName(),wxTheApp->GetAppName());

  appExit = true;
  m_docManager->FileHistorySave(settings);
  settings.sync();
  delete m_appWindow;
  delete m_docManager;
}

void wxApp::SetVendorName(const QString& name) {
  m_vendorName = name;
  m_settingsPath = "/" + name;
}

void wxApp::SetAppName(const QString& name) {
  m_appName = name;
  m_settingsPath += "/" + name;
}

static bool cmdLineEvaluated=false;
static int cnt=0;

void wxApp::updateButtonsMenus()
{
  if (!m_appWindow->m_workspace)
    return;
  QWidget *actMDIChild=m_appWindow->m_workspace->activeWindow();
  onUpdateUI();

  QWidget *newActMDIChild=m_appWindow->m_workspace->activeWindow();
  if (newActMDIChild != actMDIChild)
    m_appWindow->m_workspace->setActiveWindow(actMDIChild);
}

void wxApp::customEvent(QEvent *e)
{
}

void wxApp::onUpdateUI()
{
  QWidget *focView;
  char **argv=qApp->argv();

  if (appExit /*deletingMainFrame*/)
    return;

  UpdateUI();
  if (m_appWindow)
    m_appWindow->UpdateUI();

  focView = m_docManager->GetActiveView();
  if (focView) {
    ((wxView*)focView)->GetParentFrame()->UpdateUI();
    ((wxView*)focView)->UpdateUI();
  }

  if (!cmdLineEvaluated && wxTheApp->argc > 1) {
    cmdLineEvaluated = true;
    wxTheApp->OpenDocumentFile(argv[1]);
  }
}

wxView *wxApp::activeView() {
  return wxDocManager::GetDocumentManager()->GetActiveView();
}

void wxApp::SetLastFileOpen(QString &lfo)
{
  lastFileOpen = lfo;
  saveSettings();
}

QString wxApp::GetLastFileOpen()
{
  return lastFileOpen;
}

void wxMainFrame::histFile(int i) {
  OnMRUFile(i);
}

void wxMainFrame::histWindow(int i) {
  OnMRUWindow(i);
}

void wxApp::about()
{
    QMessageBox::about(m_appWindow,"Qt Application Example","Qt Application Example");
}
/*
QAction::QAction (QObject *parent, const char *name)
    : QAction(name,parent) {
    enable = false;
}*/

/*
QAction::QAction (const QString &text, const QIcon &icon, const QString &menuText, int accel, QObject *parent, const char *name, bool toggle)
    : QAction(text,icon,menuText,accel,parent,name,toggle) {
    enable = false;
}

QAction::QAction (const QString &text, const QString &menuText, int accel, QObject *parent, const char *name, bool toggle)
    : QAction(text,menuText,accel,parent,name,toggle) {
    enable = false;
}
*/
static QString FindExtension(const char *path)
{
    QFileInfo fi(path);
    return fi.suffix();
}

// ----------------------------------------------------------------------------
// Definition of wxDocument
// ----------------------------------------------------------------------------

wxDocument::wxDocument(wxDocument *parent)
{
    m_documentModified = false;
    m_documentParent = parent;
    m_documentTemplate = (wxDocTemplate *) NULL;
//    m_commandProcessor = (wxCommandProcessor*) NULL;
    m_savedYet = false;
    deleting = false;
//    m_documentViews.setAutoDelete(true);
}

wxDocument::~wxDocument()
{
  deleting = true;
  DeleteAllChildFrames();
  wxDocManager::GetDocumentManager()->RemoveDocument(this);
}

void wxDocument::customEvent(QEvent *ev) {
  switch (ev->type()) {
  case UEV_Close:
    Close();
    break;
  default: ;
  }
}

bool wxDocument::Close()
{
  if (OnSaveModified())
    return OnCloseDocument();
  else
    return false;
}

bool wxDocument::OnCloseDocument()
{
  DeleteContents();
  deleteLater();
  return true;
}

// Note that this implicitly deletes the document when the last view is
// deleted.
bool wxDocument::DeleteAllChildFrames()
{
  while (m_docChildFrames.size())
    delete m_docChildFrames.takeAt(0);
  return true;
}

wxView *wxDocument::GetFirstView() const
{
  if (m_documentViews.isEmpty())
    return 0;
  return m_documentViews.first();
}

/*
POSITION wxDocument::GetFirstViewPos()
{
  if (m_documentViews.isEmpty())
    return 0;
  QList<wxView*>::iterator* iter = new QList<wxView*>::iterator(m_documentViews.begin());
  return (POSITION)iter;
}


wxView* wxDocument::GetNextView(POSITION& pos)
{
  if (!pos)
    return 0;
  QList<wxView*>::iterator* iter = (QList<wxView*>::iterator*)pos;
  wxView *view = **iter;
  ++(*iter);
  if (*iter == m_documentViews.end()) {
    delete iter;
    pos = 0;
  }
  return view;
}

void wxDocument::ViewPosRelease(POSITION& pos)
{
  if (pos) {
    QList<wxView*>::iterator* iter = (QList<wxView*>::iterator*)pos;
    delete iter;
//   m_documentViews.erase(iter);
  }
}
*/

bool wxDocument::OnNewDocument()
{
  Modify(false);
  SetDocumentSaved(false);

  QString name;
  wxDocManager::GetDocumentManager()->MakeDefaultName(name);
  name += "." + m_documentTemplate->GetDefaultExtension();
  SetTitle(name);
  SetFilename(name, true);
  SetUserFilename(name);

  return true;
}

bool wxDocument::Save()
{
  QFileInfo fileInfo(m_documentFile);
  if (fileInfo.exists() && !fileInfo.isWritable()) {
    QString str = QString("File '") + m_documentFile + QString("' couldn't be opened for writing");
    QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),str ,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return false;
  }
    bool ret = false;

//    if (!IsModified() && m_savedYet) {
        wxDocManager::GetDocumentManager()->GetFileHistory()->SetFirstInHistory(m_userFilename);
//        return true;
//    }
    if (m_documentFile == QString("") || !m_savedYet)
        ret = SaveAs();
    else
        ret = OnSaveDocument(m_documentFile);
    if ( ret )
        SetDocumentSaved(true);
    return ret;
}

bool wxDocument::SaveAs()
{
    wxDocTemplate *docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return false;

    QString fn = QFileDialog::getSaveFileName(wxTheApp->m_appWindow,QString(),GetFilename(), docTemplate->GetFileFilter());

    if (fn.isEmpty())
        return false;
#ifdef WIN32
    QString driveLetter = QString(fn[0].toUpper());
    fn.replace(0,1,driveLetter);
#endif

    QFileInfo fileInfo(fn);
    if (fileInfo.exists() && !fileInfo.isWritable()) {
      QString str = QString("File '") + fn + QString("' couldn't be opened for writing");
      QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),str ,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
      return false;
  }
    QString ext = fileInfo.suffix();

    if (ext.isEmpty())
    {
        fn += ".";
        fn += docTemplate->GetDefaultExtension();
    }

    SetUserFilename(fn);
    wxDocManager::GetDocumentManager()->AddFileToHistory(fn);
    SetTitle(fileInfo.filePath());
    fn = ResolveLinks(fileInfo);
    SetFilename(fn);


    // Notify the views that the filename has changed
    wxView *view = m_documentViews.first();
    for (int i = 0; i < m_documentViews.size(); ++i) {
        m_documentViews.at(i)->OnChangeFilename();
    }

    SetDocumentSaved(true);
    return OnSaveDocument(m_documentFile);
}

bool wxDocument::OnSaveDocument(const QString& file)
{
  if ( !file.length() )
        return false;

    QString msgTitle;
    if (!QString(qApp->applicationName()).isEmpty())
        msgTitle = qApp->applicationName();
    else
        msgTitle = tr("File error");

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),tr("Sorry, couldn't open this file for saving."),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return false;
    }

    QDataStream store(&f);
    SaveObject(store);
    f.close();

    Modify(false);
    //SetFilename(file); //??
    wxDocManager::GetDocumentManager()->GetFileHistory()->SetFirstInHistory(m_userFilename);
    return true;
}

bool wxDocument::OnOpenDocument(const QString& file)
{
    if (!OnSaveModified())
        return false;

    QString msgTitle;
    if (!QString(qApp->applicationName()).isEmpty())
        msgTitle = qApp->applicationName();
    else
        msgTitle = tr("File error");

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),tr("Sorry, couldn't open this file."),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
        return false;
    }

    QDataStream load(&f);
    LoadObject(load);
    f.close();

    SetFilename(file, true); //??
    Modify(false);
    m_savedYet = true;

    UpdateAllViews();

    return true;
}

QDataStream& wxDocument::LoadObject(QDataStream& stream)
{
    return stream;
}

QDataStream& wxDocument::SaveObject(QDataStream& stream)
{
    return stream;
}

bool wxDocument::Revert()
{
    return false;
}


// Get title, or filename if no title, else unnamed
bool wxDocument::GetPrintableName(QString& buf) const
{
    if (!m_documentTitle.isEmpty())
    {
        buf = m_documentTitle;
        return true;
    }
    else if (!m_documentFile.isEmpty())
    {
        buf = m_documentFile;
        return true;
    }
    else
    {
        buf = tr("unnamed");
        return true;
    }
}

// true if safe to close
bool wxDocument::OnSaveModified()
{
    if (IsModified())
    {
        QString title;
        GetPrintableName(title);

        QString msgTitle;
        if (!QString(qApp->applicationName()).isEmpty())
            msgTitle = qApp->applicationName();
        else
            msgTitle = tr("Warning");

        QString prompt;
        prompt.sprintf("Do you want to save changes to document %s?",
                qPrintable(title));
        switch (QMessageBox::question(
          wxTheApp->m_appWindow,qApp->applicationName(),prompt,
          QMessageBox::Yes | QMessageBox::Default,
          QMessageBox::No,
          QMessageBox::Cancel)) {
        case QMessageBox::No:
            Modify(false);
            return true;
        case QMessageBox::Yes:
            return Save();
        case QMessageBox::Cancel:
            return false;
        }
    }
    return true;
}

bool wxDocument::AddView(wxView *view)
{
    if (m_documentViews.indexOf(view) == -1)
    {
        m_documentViews.append(view);
        OnChangedViewList();
    }
    return true;
}

bool wxDocument::RemoveView(wxView *view)
// Called from ~wxView
{
    m_documentViews.removeAt(m_documentViews.indexOf(view));
    OnChangedViewList();
    return true;
}

void wxDocument::AddChildFrame(wxMDIChildFrame *chf) {
  m_docChildFrames.append(chf);
}

void wxDocument::RemoveChildFrame(wxMDIChildFrame *chf)
// Called from ~wxMDIChildFrame
{
  m_docChildFrames.removeAt(m_docChildFrames.indexOf(chf));
}

// Called after a view is added or removed.
// The default implementation closes the document if
// there are no more views.
void wxDocument::OnChangedViewList()
{
  if (m_documentViews.isEmpty())
  {
    OnSaveModified();
    if (!deleting)
      OnCloseDocument();
  }
}

void wxDocument::UpdateAllViews(wxView *sender, unsigned param, QObject *hint)
{
  int pos;
  for (pos = 0; pos < m_documentViews.size(); pos++)
    m_documentViews[pos]->OnUpdate(sender, param, hint);
}

void wxDocument::SetTitle(const QString& title)
{
  if (m_documentTitle == title)
    return;
  m_oldTitle = m_documentTitle;
  m_documentTitle = title;
  if (!m_oldTitle.isEmpty())
    wxTheApp->m_appWindow->GetWindowHistory()->OnChangeOfWindowTitle(m_oldTitle,m_documentTitle);
}

void wxDocument::SetFilename(const QString& filename, bool notifyViews)
{
  m_documentFile = filename;
  if ( notifyViews )
  {
    // Notify the views that the filename has changed
    for (int i=0; i<m_documentViews.size(); i++)
      m_documentViews.at(i)->OnChangeFilename();
  }
}

// ----------------------------------------------------------------------------
// Document view
// ----------------------------------------------------------------------------

wxView::wxView(QWidget *parent, wxDocument *doc, const char* name) : QWidget(parent)
{
  setObjectName(QString(name));
  active = false;
  deleting = false;
  layout = new QHBoxLayout(this);
  layout->setMargin(0);
  setLayout(layout);

  m_viewDocument = doc;
  m_viewFrame = CalcParentFrame();
  m_viewFrame->AddView(this);
  if (parent->inherits("QMainWindow"))
    ((QMainWindow*)parent)->setCentralWidget(this);
}

void wxView::mousePressEvent ( QMouseEvent * e )
{
  wxDocManager::GetDocumentManager()->SetActiveView(this);
}

void wxView::focusInEvent ( QFocusEvent * e )
{
  wxDocManager::GetDocumentManager()->SetActiveView(this, true);
}

wxMDIChildFrame *wxView::CalcParentFrame()
{
  QWidget *parent;

  for (parent = parentWidget();
       parent && !parent->inherits("wxMDIChildFrame");
       parent = parent->parentWidget());
  return (wxMDIChildFrame*)parent;
}

wxView::~wxView()
{
  deleting = true;
  wxDocManager::GetDocumentManager()->SetActiveView(this, false);
  //m_viewFrame->RemoveView(this); // at this time the frame and its m_viewList has already been destroyed
  m_viewDocument->RemoveView(this);
}


void wxView::OnUpdate(wxView *WXUNUSED(sender), unsigned param, QObject *WXUNUSED(hint))
{
}

void wxView::OnChangeFilename()
{
  QString newName, oldName;

  if (GetParentFrame() && GetDocument()) {
    GetDocument()->GetPrintableName(newName);
    GetParentFrame()->SetTitle(newName);
  }
}

void wxView::SetDocument(wxDocument *doc)
{
    m_viewDocument = doc;
    if (doc)
        doc->AddView(this);
}

bool wxView::Close()
{
  if (on_cancelButton_clicked()) {
      //deleteLater();
      return true;
  }
  else
      return false;
}

void wxView::ActivateView(bool activate)
{
  if (activate)
    active = true;
  GetParentFrame()->NotifyActive(this);
  GetParentFrame()->Activate(activate);
}

void wxView::OnActivateView(bool activate, wxView *deactiveView)
{
  if (activate) {
    active = true;
    setFocus();
    wxTheApp->updateButtonsMenus();
  }
  else
    active = false;
}

bool wxView::on_cancelButton_clicked()
{
  return true;
}

#if wxUSE_PRINTING_ARCHITECTURE
wxPrintout *wxView::OnCreatePrintout()
{
    return new wxDocPrintout(this);
}
#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// wxDocTemplate
// ----------------------------------------------------------------------------

wxDocTemplate::wxDocTemplate(wxDocManager *manager,
                             const QString& descr,
                             const QString& filter,
                             const QString& dir,
                             const QString& ext,
                             const QString& docTypeName,
                             const QString& frameTypeName,
                             const QString& viewTypeName,
                             Factory *docFactory,
                             FrameFactory *frameFactory,
                             ViewFactory *viewFactory,
                             long flags)
{
  setObjectName(QString("wxDocTemplate"));
  m_description = descr;
  m_directory = dir;
  m_defaultExt = ext;
  m_fileFilter = filter;
  m_flags = flags;
  m_docTypeName = docTypeName;
  m_frameTypeName = frameTypeName;
  m_viewTypeName = viewTypeName;
  wxDocManager::GetDocumentManager()->AssociateTemplate(this);

  m_docClassInfo = docFactory;
  m_frameClassInfo = frameFactory;
  m_viewClassInfo = viewFactory;
}

wxDocTemplate::~wxDocTemplate()
{
    wxDocManager::GetDocumentManager()->DisassociateTemplate(this);
}

// Tries to dynamically construct an object of the right class.
wxDocument *wxDocTemplate::CreateDocument(const QString& path, long flags)
{
  bool ok;
  QString fn;

  if (!m_docClassInfo)
      return (wxDocument *) NULL;
  wxDocument *doc = (wxDocument *)m_docClassInfo();
  fn = path;
#ifdef WIN32
  if (!fn.isEmpty()) {
    QString driveLetter = QString(fn[0].toUpper());
    fn.replace(0,1,driveLetter);
  }
#endif
  QFileInfo info(fn);
  if (flags == wxDOC_NEW) {
    doc->SetUserFilename(info.fileName());
    doc->SetTitle(info.fileName());
  }
  else {
    doc->SetUserFilename(info.filePath());
    doc->SetTitle(info.filePath());
    fn = ResolveLinks(info);
  }
  doc->SetFilename(fn);
  doc->SetDocumentTemplate(this);
  wxDocManager::GetDocumentManager()->AddDocument(doc);
//  doc->SetCommandProcessor(doc->OnCreateCommandProcessor());
  doc->SetDocTypeName(m_docTypeName);
  if (doc->OnCreate(fn)) {
    if (flags == wxDOC_NEW)
      ok = doc->OnNewDocument();
    else
      ok = doc->OnOpenDocument(fn);
    if (ok) {
      wxMDIChildFrame* newFrame = CreateChildFrame(doc);
      newFrame->InitialUpdate();
      //newFrame->show();
      return doc;
    }
    else {
      doc->deleting = true;
      doc->DeleteAllChildFrames();
      delete doc;
      return (wxDocument *) NULL;
    }
  }
  else {
    if (wxDocManager::GetDocumentManager()->GetDocuments().indexOf(doc) != -1) {
      doc->deleting = true;
      doc->DeleteAllChildFrames();
      delete doc;
    }
    return (wxDocument *) NULL;
  }
}

wxMDIChildFrame *wxDocTemplate::CreateChildFrame(wxDocument *doc)
{
  QWidget *mw = wxTheApp->m_appWindow;
  QWidget * clw = ((wxMainFrame*)mw)->GetClientWindow();
  wxMDIChildFrame *frame = (wxMDIChildFrame *)m_frameClassInfo(clw);
  frame->parentWidget()->resize(600, 400);
  if (frame->OnCreate(this,doc)) {
    return frame;
  }
  else {
    delete frame;
    return 0;
  }
}

// The default (very primitive) format detection: check is the extension is
// that of the template
bool wxDocTemplate::FileMatchesTemplate(const QString& path)
{
    return GetDefaultExtension().compare(FindExtension(qPrintable(path))) == 0;
}

// ----------------------------------------------------------------------------
// wxDocManager
// ----------------------------------------------------------------------------

wxDocManager* wxDocManager::sm_docManager = 0;

wxDocManager::wxDocManager(long flags)
{
  setObjectName(QString("wxDocManager"));
  m_defaultDocumentNameCounter = 1;
  m_flags = flags;
  m_activeView = (wxView *) NULL;
  m_maxDocsOpen = 10000;
  m_fileHistory = (wxHistory *) NULL;
  sm_docManager = this;
}

wxDocManager::~wxDocManager()
{
    Clear();
    if (m_fileHistory)
        delete m_fileHistory;
    sm_docManager = (wxDocManager*) NULL;
}

bool wxDocManager::Clear(bool force)
{
  wxDocument *doc;

  while (m_docs.size()) {
    doc = m_docs.takeAt(0);
    if (!doc->Close() && !force)
        return false;
    delete doc;

    // Implicitly deletes the document when the last
    // view is removed (deleted)
    //doc->DeleteAllChildFrames();

    // Check document is deleted
    //if (m_docs.findRef(doc) != -1)
     // delete doc;

    // This assumes that documents are not connected in
    // any way, i.e. deleting one document does NOT
    // delete another.
  }
  while (!m_templates.isEmpty()) {
    wxDocTemplate *templ = m_templates.takeFirst();
    delete templ;
  }
  return true;
}

void wxDocManager::OnFileNew()
{
    CreateDocument(QString(""), wxDOC_NEW);
}

void wxDocManager::OnFileOpen()
{
    if ( !CreateDocument(QString(""), 0) )
        OnOpenFileFailure();
}

void wxDocManager::OnFileRevert()
{
    wxDocument *doc = GetActiveDocument();
    if (!doc)
        return;
    doc->Revert();
}

void wxDocManager::OnFileSave()
{
    wxDocument *doc = GetActiveDocument();
    if (!doc)
        return;
    doc->Save();
}

void wxDocManager::OnFileSaveAs()
{
  wxDocument *doc = GetActiveDocument();
  if (!doc)
      return;
  doc->SaveAs();
}

void wxDocManager::OnFileClose()
{
  wxDocument *doc = GetActiveDocument();
  if (!doc)
    return;
  doc->Close();
  //RemoveDocument(doc);
}

void wxDocManager::OnUndo()
{
    wxDocument *doc = GetActiveDocument();
    if (!doc)
        return;
/*    if (doc->GetCommandProcessor())
        doc->GetCommandProcessor()->Undo();*/
}

void wxDocManager::OnRedo()
{
    wxDocument *doc = GetActiveDocument();
    if (!doc)
        return;
/*    if (doc->GetCommandProcessor())
        doc->GetCommandProcessor()->Redo();*/
}

// Handlers for UI update commands

void wxDocManager::OnUpdateFileOpen(QAction *action)
{
    action->setEnabled( true );
}

void wxDocManager::OnUpdateFileClose(QAction *action)
{
    wxDocument *doc = GetActiveDocument();
    action->setEnabled( (doc != (wxDocument*) NULL) );
}

void wxDocManager::OnUpdateFileRevert(QAction *action)
{
    wxDocument *doc = GetActiveDocument();
    action->setEnabled( (doc != (wxDocument*) NULL) );
}

void wxDocManager::OnUpdateFileNew(QAction *action)
{
    action->setEnabled( true );
}

void wxDocManager::OnUpdateFileSave(QAction *action)
{
    wxDocument *doc = GetActiveDocument();
    action->setEnabled( doc && doc->IsModified() );
}

void wxDocManager::OnUpdateFileSaveAs(QAction *action)
{
    wxDocument *doc = GetActiveDocument();
    action->setEnabled( (doc != (wxDocument*) NULL) );
}

void wxDocManager::OnUpdateUndo(QAction *action)
{
    wxDocument *doc = GetActiveDocument();
//    action->setEnabled( (doc && doc->GetCommandProcessor() && doc->GetCommandProcessor()->CanUndo()) );
}

void wxDocManager::OnUpdateRedo(QAction *action)
{
//    wxDocument *doc = GetActiveDocument();
//    action->setEnabled( (doc && doc->GetCommandProcessor() && doc->GetCommandProcessor()->CanRedo()) );
}

/*
void wxDocManager::OnUpdatePrint(QAction *action)
{
    wxDocument *doc = GetActiveDocument();
    action->setEnabled( (doc != (wxDocument*) NULL) );
}


void wxDocManager::OnUpdatePrintSetup(wxUpdateUIEvent& event)
{
    event.Enable( true );
}

void wxDocManager::OnUpdatePreview(wxUpdateUIEvent& event)
{
    wxDocument *doc = GetActiveDocument();
    event.Enable( (doc != (wxDocument*) NULL) );
}
*/

wxView *wxDocManager::GetActiveView()
{
  if (m_docs.isEmpty())
    return 0;
  if (m_activeView)
    return m_activeView;
  if (m_docs.count() == 1) {
    wxDocument* doc = (wxDocument*) m_docs.first();
    return doc->GetFirstView();
  }
  return 0;
}

/*
POSITION wxDocManager::GetFirstDocPos()
{
  if (m_docs.isEmpty())
    return 0;
  QList<wxDocument*>::iterator* iter = new QList<wxDocument*>::iterator(m_docs.begin());
  return (POSITION)iter;
}

wxDocument* wxDocManager::GetNextDoc(POSITION& pos)
{
  if (!pos)
    return 0;
  QList<wxDocument*>::iterator* iter = (QList<wxDocument*>::iterator*)pos;
  wxDocument *doc = **iter;
  ++(*iter);
  if (*iter == m_docs.end()) {
    delete iter;
    pos = 0;
  }
  return doc;
}

void wxDocManager::DocPosRelease(POSITION pos)
{
  if (pos) {
    QList<wxDocument*>::iterator iter = *(QList<wxDocument*>::iterator*)pos;
    m_docs.erase(iter);
  }
}*/

wxDocument *wxDocManager::CreateDocument(const QString& path, long flags)
{
  int i, n = 0;
  wxDocument *doc, *newDoc;
  int pos;
  wxDocTemplate *temp, **templates = new wxDocTemplate *[m_templates.count()];

  for (i = 0; i < m_templates.count(); i++) {
    temp = (wxDocTemplate *)(m_templates.at(i));
    if (temp->IsVisible()) {
            templates[n] = temp;
            n ++;
        }
    }
  if (n == 0) {
        delete[] templates;
        return (wxDocument *) NULL;
    }

  // If we've reached the max number of docs, close the first one.
  if (GetDocuments().count() >= m_maxDocsOpen) {
    doc = (wxDocument *)GetDocuments().first();
    if (doc->Close()) {
      doc->DeleteAllChildFrames();
        // Implicitly deletes the document when the last childFrame is deleted
      if (m_docs.indexOf(doc) != -1) //Check we're really deleted
        delete doc;
    }
    else
      return (wxDocument *) NULL;
  }

  // New document: user chooses a template, unless there's only one.
  if (flags == wxDOC_NEW) {
    if (n == 1) {
      temp = templates[0];
      delete[] templates;
      newDoc = temp->CreateDocument(path, flags);
      return newDoc;
    }
    temp = SelectDocumentType(templates, n);
      delete[] templates;
    if (temp) {
      newDoc = temp->CreateDocument(path, flags);
      return newDoc;
    }
    else
      return (wxDocument *) NULL;
  }

  // Existing document
  temp = (wxDocTemplate *) NULL;

  QString fn, path2;
  QFileInfo qfi;
  if (path != QString("")) {
    path2 = path;
#ifdef WIN32
    QString driveLetter = QString(path2[0].toUpper());
    path2.replace(0,1,driveLetter);
#endif
  }
  qfi.setFile(path2);
  if (!path2.isEmpty())
    fn = ResolveLinks(qfi);

  if (flags == wxDOC_SILENT)
    temp = FindTemplateForPath(fn);
  else {
    temp = SelectDocumentPath(templates, n, path2, flags);
    fn = ResolveLinks(qfi);
  }
  delete[] templates;
  if (temp) {
    for (pos = 0; pos < m_docs.size(); pos++) {
      doc = m_docs[pos];
      if (doc && (fn == doc->GetFilename())) { //filename comp
        doc->GetFirstView()->GetParentFrame()->Activate();
        m_fileHistory->SetFirstInHistory(path2);
        return doc;
      }
    }
    newDoc = temp->CreateDocument(path2);
    if (newDoc) {
      newDoc->SetDocumentSaved();
      return newDoc;
    }
  }
  return (wxDocument *) NULL;
}

wxDocument *wxDocManager::FindOpenDocument(const QString& path)
{
  wxDocument *doc;
  int pos;
  for (pos = 0; pos < m_docs.size(); pos++) {
    doc = m_docs[pos];
    if (doc && (path == doc->GetFilename())) { //filename comp
      m_fileHistory->SetFirstInHistory(doc->GetUserFilename());
      return doc;
    }
  }
  return CreateDocument(path, wxDOC_SILENT);
}

wxDocument *wxDocManager::FindOpenDocumentN(const QString& path, bool& isNew)
{
  wxDocument *doc;
  int pos;
  for (pos = 0; pos < m_docs.size(); pos++) {
    doc = m_docs[pos];
    if (doc && (path == doc->GetFilename())) { //filename comp
      m_fileHistory->SetFirstInHistory(doc->GetUserFilename());
      isNew = false;
      return doc;
    }
  }
  isNew = true;
  return CreateDocument(path, wxDOC_SILENT);
}

bool wxDocManager::CreateView(wxDocument *doc)
{
  int i, n =0;
  wxDocTemplate *temp, **templates = new wxDocTemplate *[m_templates.count()];

  for (i = 0; i < m_templates.count(); i++) {
    temp = (wxDocTemplate *)(m_templates.at(i));
    if (temp->IsVisible()) {
      if (temp->GetDocTypeName() == doc->GetDocTypeName()) {
                templates[n] = temp;
                n ++;
            }
        }
    }
  if (n == 0) {
        delete[] templates;
        return false;
    }
  if (n == 1) {
    temp = templates[0];
        delete[] templates;
        temp->CreateChildFrame(doc);
        return true;
    }

  temp = SelectViewType(templates, n);
    delete[] templates;
  if (temp) {
        temp->CreateChildFrame(doc);
        return true;
    }
    else
        return false;
}

// Not yet implemented
void wxDocManager::DeleteTemplate(wxDocTemplate *WXUNUSED(temp), long WXUNUSED(flags))
{
}

// Not yet implemented
bool wxDocManager::FlushDoc(wxDocument *WXUNUSED(doc))
{
    return false;
}

wxDocument *wxDocManager::GetActiveDocument()
{
  if (m_docs.isEmpty())
    return 0;
  wxView *view = GetActiveView();
  if (view)
    return view->GetDocument();
  else
    return 0;
}

// Make a default document name
bool wxDocManager::MakeDefaultName(QString& name)
{
    name.sprintf("unnamed%d", m_defaultDocumentNameCounter);
    m_defaultDocumentNameCounter++;

    return true;
}

// Make a frame title (override this to do something different)
// If docName is empty, a document is not currently active.
QString wxDocManager::MakeFrameTitle(wxDocument* doc)
{
  QString appName = qApp->applicationName();
  QString title;
  if (!doc)
      title = appName;
  else {
    QString docName;
    doc->GetPrintableName(docName);
    title = docName + QString(tr(" - ")) + appName;
  }
  return title;
}


// Not yet implemented
wxDocTemplate *wxDocManager::MatchTemplate(const QString& WXUNUSED(path))
{
    return (wxDocTemplate *) NULL;
}

// File history management
void wxDocManager::AddFileToHistory(QString& file)
{
  QSettings settings(QSettings::NativeFormat,QSettings::UserScope,wxTheApp->GetVendorName(),wxTheApp->GetAppName());

  if (m_fileHistory) {
    m_fileHistory->AddToHistory(new DString(qPrintable(file)),wxTheApp);
    m_fileHistory->Save(settings);
  }
}

void wxDocManager::SetFirstInHistory(int i)
{
    if (m_fileHistory)
        m_fileHistory->SetFirstInHistory(i);
}

void wxDocManager::RemoveFileFromHistory(int i)
{
    if (m_fileHistory)
        m_fileHistory->RemoveItemFromHistory(i);
}

QString *wxDocManager::GetHistoryFile(int i) const
{
    QString *histFile;

    if (m_fileHistory)
        histFile = new QString(m_fileHistory->GetHistoryItem(i)->c);

    return histFile;
}
/*
void wxDocManager::FileHistoryUseMenu(QPopupMenu *menu)
{
    if (m_fileHistory)
        m_fileHistory->UseMenu(menu);
}

void wxDocManager::FileHistoryRemoveMenu(QPopupMenu *menu)
{
    if (m_fileHistory)
        m_fileHistory->RemoveMenu(menu);
}
*/
#if wxUSE_CONFIG
void wxDocManager::FileHistoryLoad(QSettings& config)
{
    if (m_fileHistory)
        m_fileHistory->Load(config);
}

void wxDocManager::FileHistorySave(QSettings& config)
{
    if (m_fileHistory)
        m_fileHistory->Save(config);
}
#endif

void wxDocManager::FileHistoryAddFilesToMenu()
{
    if (m_fileHistory)
        m_fileHistory->AddFilesToMenu();
}

int wxDocManager::GetNoHistoryFiles() const
{
    if (m_fileHistory)
        return m_fileHistory->GetCount();
    else
        return 0;
}


// Find out the document template via matching in the document file format
// against that of the template
wxDocTemplate *wxDocManager::FindTemplateForPath(const QString& path)
{
    wxDocTemplate *theTemplate = (wxDocTemplate *) NULL;

    // Find the template which this extension corresponds to
    int i;
    for (i = 0; i < m_templates.count(); i++)
    {
        wxDocTemplate *temp = (wxDocTemplate *)m_templates.at(i);
        if ( temp->FileMatchesTemplate(path) )
        {
            theTemplate = temp;
            break;
        }
    }
    return theTemplate;
}

// Try to get a more suitable parent frame than the top window,
// for selection dialogs. Otherwise you may get an unexpected
// window being activated when a dialog is shown.
static QWidget* wxFindSuitableParent()
{
    QWidget* parent = wxTheApp->m_appWindow;

    QWidget* focusWindow = qApp->focusWidget();
  if (focusWindow) {
        while (focusWindow &&
                !focusWindow->inherits("QDialog") &&
                !focusWindow->inherits("QMainWindow"))

            focusWindow = focusWindow->parentWidget();

        if (focusWindow)
            parent = focusWindow;
    }
    return parent;
}

// Prompts user to open a file, using file specs in templates.
// How to implement in wxWidgets? Must extend the file selector
// dialog or implement own; OR match the extension to the
// template extension.

wxDocTemplate *wxDocManager::SelectDocumentPath(wxDocTemplate **templates,
                                                int noTemplates,
                                                QString& path,
                                                long WXUNUSED(flags),
                                                bool save)
{
    // We can only have multiple filters in Windows and GTK
  QString descrBuf, pathTmp, msgTitle;
  int i,  FilterIndex = -1;
  QWidget* parent;
  wxDocTemplate *theTemplate;

  for (i = 0; i < noTemplates; i++) {
    if (templates[i]->IsVisible()) {
            // add a '|' to separate this filter from the previous one
            if ( !descrBuf.isEmpty() )
                descrBuf += QString("|");

            descrBuf += templates[i]->GetDescription()
                += QString(" (") += templates[i]->GetFileFilter() += QString(") |")
                += templates[i]->GetFileFilter();
        }
    }
  parent = wxFindSuitableParent();
    if (save)
      pathTmp = QFileDialog::getSaveFileName(wxTheApp->m_appWindow,QString("Save as"),m_lastDirectory);
    else
      pathTmp = QFileDialog::getOpenFileName(wxTheApp->m_appWindow,QString("Open"),m_lastDirectory);

  theTemplate = (wxDocTemplate *)NULL;
  if (!pathTmp.isEmpty()) {
        QFileInfo fi(pathTmp);
    if (!fi.exists()) {
            if (!QString(qApp->applicationName()).isEmpty())
                msgTitle = qApp->applicationName();
            else
                msgTitle = QString(tr("File error"));

            QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),tr("Sorry, couldn't open this file."),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);

            path = QString("");
            return (wxDocTemplate *) NULL;
        }
        m_lastDirectory = fi.absolutePath();
        path = pathTmp;

        // first choose the template using the extension, if this fails (i.e.
        if ( FilterIndex != -1 )
            theTemplate = templates[FilterIndex];
        if ( !theTemplate )
            theTemplate = FindTemplateForPath(path);
    }
  else {
        path = QString("");
    }
    QDir qdir;
    qdir.convertSeparators(path);
    return theTemplate;

#if 0
    // In all other windowing systems, until we have more advanced
    // file selectors, we must select the document type (template) first, and
    // _then_ pop up the file selector.
  temp = SelectDocumentType(templates, noTemplates);
    if (!temp)
        return (wxDocTemplate *) NULL;

    char *pathTmp = wxFileSelector(tr("Select a file"), QString(""), QString(""),
            temp->GetDefaultExtension(),
            temp->GetFileFilter(),
            0, qApp->GetTopWindow());
  if (pathTmp) {
        path = pathTmp;
        return temp;
    }
    else
        return (wxDocTemplate *) NULL;
#endif // 0
}

wxDocTemplate *wxDocManager::SelectDocumentType(wxDocTemplate **templates,
                                                int noTemplates)
{
    QStringList strings;
    wxDocTemplate **data = new wxDocTemplate*[noTemplates], *temp;
    int i;
    int n = 0;
    bool ok;

  for (i = 0; i < noTemplates; i++) {
    if (templates[i]->IsVisible()) {
            strings << templates[i]->m_description;
            data[n] = templates[i];
            n ++;
        }
    }
  if (n == 0) {
        delete[] data;
        return (wxDocTemplate *) NULL;
    }
  else if (n == 1) {
        temp = data[0];
        delete[] data;
        return temp;
    }

    QWidget* parent = wxFindSuitableParent();

    QString selection = QInputDialog::getItem(
      wxTheApp->m_appWindow,
      tr("Templates"),
      tr("Please select a document template:"),
      strings,
      0,
      false,
      &ok);
    if (!ok)
      return (wxDocTemplate*)0;

  for (i = 0; i < noTemplates; i++) {
      if (selection == strings[i]) {
        temp = data[i];
        delete[] data;
        return  temp;
      }
    }
    return 0;
}

wxDocTemplate *wxDocManager::SelectViewType(wxDocTemplate **templates,
        int noTemplates)
{
    QStringList strings;
    char **data = new char*[noTemplates];
    int i;
    int n = 0;
    bool ok;

  for (i = 0; i < noTemplates; i++) {
    if (templates[i]->IsVisible() && (templates[i]->GetViewName() != QString(""))) {
            strings << templates[i]->m_viewTypeName;
            data[n] = (char*)templates[i];
            n ++;
        }
    }
    QWidget* parent = wxFindSuitableParent();

    QString selection = QInputDialog::getItem(
      wxTheApp->m_appWindow,
      tr("Document views"),
      tr("Please Select a document view:"),
      strings,
      0,
      false,
      &ok);
    delete[] data;
    if (!ok)
      return (wxDocTemplate*)0;

    for (i = 0; i < noTemplates; i++)
    {
      if (selection == strings[i])
        return  (wxDocTemplate*)data[i];
    }
    return (wxDocTemplate*)0;
}

void wxDocManager::AssociateTemplate(wxDocTemplate *temp)
{
    if (m_templates.indexOf(temp) == -1)
        m_templates.append(temp);
}

void wxDocManager::DisassociateTemplate(wxDocTemplate *temp)
{
  m_templates.removeAt(m_templates.indexOf(temp));
}

// Add and remove a document from the manager's list
void wxDocManager::AddDocument(wxDocument *doc)
{
    if (m_docs.indexOf(doc) == -1)
        m_docs.append(doc);
}

void wxDocManager::RemoveDocument(wxDocument *doc)
// Called from ~wxDocument
{
  m_docs.removeAt(m_docs.indexOf(doc));
}

// Views or windows should inform the document manager
// when a view is going in or out of focus
void wxDocManager::SetActiveView(wxView *view, bool activate)
{
  if (activate && view) {
    if (view != m_activeView) {
      if (m_activeView)
        m_activeView->OnActivateView(false);
      m_activeView = view;
      view->OnActivateView();
      view->GetParentFrame()->NotifyActive(view);
    }
  }
  else
    if (m_activeView == view)
      m_activeView = 0;
}


// ----------------------------------------------------------------------------
// File history processor
// ----------------------------------------------------------------------------


wxHistory::wxHistory(QObject *receiver, int maxItems)
{
  m_maxHistItems = maxItems;
  m_historyN = 0;
  m_history = new DString*[m_maxHistItems];
  m_actions = new QAction*[m_maxHistItems];
  for (int i = 0; i < m_maxHistItems; i++)
    m_actions[i] = 0;
  m_signalMapper=new QSignalMapper(this);
}

wxHistory::~wxHistory()
{
  int i;
  for (i = 0; i < m_historyN; i++)
      delete m_history[i];
  delete [] m_history;
  delete [] m_actions;
  delete m_signalMapper;
}

// File history management
void wxHistory::SetFirstInHistory(const QString& file)
{
    int i;
    for (i = 0; i < m_historyN; i++)
      if (*m_history[i] == DString(qPrintable(file))) {
          SetFirstInHistory(i);
          return;
      }
}

void wxHistory::SetFirstInHistory(int histFileIndex)
{
  DString *s;
  int i;

  if (histFileIndex == 0)
    return;

  s = m_history[histFileIndex];

  for (i = histFileIndex-1; i >= 0; i--)
    m_history[i+1] = m_history[i];

  m_history[0] = s;

  for (i = 0; i <= histFileIndex; i++) {
    QString buf;
    buf = s_MRUEntryFormat.arg(i+1).arg(m_history[i]->c);
    m_actions[i]->setText(buf);
  }
}

void wxHistory::AddToHistory(DString *item, QObject *receiver)
{
    int i;
    // Check we don't already have this item
    for (i = 0; i < m_historyN; i++)
    {
      if (m_history[i]->l && (*m_history[i] == *item)) {
        SetFirstInHistory(i);
        delete item;
        return;
      }
    }

    // Add to the item history:
    // Move existing files (if any) down so we can insert item at beginning.

    // First delete item that has popped off the end of the array (if any)
    if (m_historyN == m_maxHistItems)
    {
        delete m_history[m_maxHistItems-1];
        m_history[m_maxHistItems-1] = 0;
    }
    if (m_historyN < m_maxHistItems)
    {
        if (m_historyN == 0)
            m_menu->addSeparator();
        m_actions[m_historyN] = m_menu->addAction("xxx");
        connect(m_actions[m_historyN],SIGNAL(triggered()),m_signalMapper,SLOT(map()));
        m_signalMapper->setMapping(m_actions[m_historyN],m_historyN);
        m_actions[m_historyN]->setStatusTip("Open this file");
        m_historyN++;
    }
    // Shuffle filenames down
    for (i = (m_historyN-1); i > 0; i--)
        m_history[i] = m_history[i-1];

    m_history[0] = item;

    for (i = 0; i < m_historyN; i++) {
        QString buf;
        buf = s_MRUEntryFormat.arg(i+1).arg(m_history[i]->c);
        m_actions[i]->setText(buf);
    }
}

static DString ret;
DString *wxHistory::GetHistoryItem(int i) const
{
//    DString *s=new DString();

    if ( i < m_historyN )
      return m_history[i];

		QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),tr("bad index in wxHistory::GetHistoryItem"),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    return &ret;
}

void wxHistory::RemoveItemFromHistory(QString name)
{
  int i;

  for (i=0; i<m_historyN; i++)
    if (name == m_history[i]->c) {
      RemoveItemFromHistory(i);
      return;
    }
  return;
}

void wxHistory::RemoveItemFromHistory(int i)
{
    if (i >= m_historyN) {
       QMessageBox::critical(wxTheApp->m_appWindow,qApp->applicationName(),tr("invalid index in wxHistory::RemoveFileFromHistory"),QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
       return;
    }
        // delete the element from the array (could use memmove() too...)
        delete m_history[i];

        int j;
        for ( j = i; j < m_historyN - 1; j++ )
        {
            m_history[j] = m_history[j + 1];
        }

        // shuffle filenames up
        QString buf;
        for ( j = i; j < m_historyN - 1; j++ )
        {
            buf = s_MRUEntryFormat.arg(j+1).arg(m_history[j]->c);
            m_actions[j]->setText(buf);
        }

        // delete the last menu item which is unused now
        m_menu->removeAction(m_actions[m_historyN - 1]);
				delete m_actions[m_historyN - 1];
        m_actions[m_historyN - 1] = 0;

        // delete the last separator too if no more files are left
        if ( m_historyN == 1 )
        {
            QList<QAction*> myActions = m_menu->actions();
            QAction *menuItem = myActions.last();
						if ( menuItem && menuItem->isSeparator()) {
              m_menu->removeAction(menuItem);
							delete menuItem;
						}
        }
    m_historyN--;
}

void wxHistory::OnChangeOfWindowTitle(QString &oldName, QString &newName)
{
  int i;
  DString str(qPrintable(newName));

  for (i=0; i<m_historyN; i++)
    if (oldName == m_history[i]->c) {
      *m_history[i] = str;
      m_actions[i]->setText(newName);
      return;
    }
  return;
}
/*
void wxHistory::UseMenu(QPopupMenu *menu)
{
    if (m_fileMenus.findRef(menu) == -1)
        m_fileMenus.append(menu);
}

void wxHistory::RemoveMenu(QPopupMenu *menu)
{
    m_fileMenus.remove(menu);
}
*/

#if wxUSE_CONFIG
void wxHistory::Load(QSettings& config)
{
    QString buf, historyFile, def;

    config.beginGroup("fileHistory");
    m_historyN = 0;
    buf.sprintf("file%d", m_historyN+1);

    while (m_historyN <= m_maxHistItems) {
      historyFile = config.value(buf).toString();
      if (historyFile.isEmpty()) break;
      m_history[m_historyN++] = new DString(qPrintable(historyFile));
      buf.sprintf("file%d", m_historyN+1);
    }
    config.endGroup();
    AddFilesToMenu();
}

void wxHistory::Save(QSettings& config)
{
    int i;
    QString buf;

    config.beginGroup("fileHistory");
    config.remove("");

    for (i = 0; i < m_historyN; i++)
    {
        buf.sprintf("file%d", i+1);
        config.setValue(buf,QString(m_history[i]->c));
    }
    config.endGroup();
}
#endif // wxUSE_CONFIG

void wxHistory::AddFilesToMenu()
{
  int i;

  if (m_historyN > 0)
  {
    m_menu->addSeparator();
    for (i = 0; i < m_historyN; i++)
    {
      if (m_history[i]->l)
      {
        QString buf;
        buf = s_MRUEntryFormat.arg(i+1).arg(m_history[i]->c);
        m_actions[i] = m_menu->addAction(buf);
        connect(m_actions[i],SIGNAL(triggered()),m_signalMapper,SLOT(map()));
        m_signalMapper->setMapping(m_actions[i],i);
        m_actions[i]->setStatusTip("Open this file");
      }
    }
;
  }
}
/*
void wxHistory::AddFilesToMenu(QMenu* menu)
{
    m_menu = menu;
    if (m_historyN > 0)
    {
        m_menu->addSeparator();
        int i;
        for (i = 0; i < m_historyN; i++)
        {
            if (m_history[i])
            {
                QString buf;
                buf = s_MRUEntryFormat.arg(i+1).arg(m_history[i]->c);
//                m_menu->insertItem(buf,wxTheApp,SLOT(histFile(int)),0, wxID_FILE1+i);
//                m_menu->setItemParameter(wxID_FILE1+i,i);
            }
        }
    }
}
*/

#ifdef WIN32_

static bool ResolveShortCut (HWND hwnd, QString pszLink, QString &resolved)
{
  HRESULT hres;
  IShellLink *psl;
  char szGotPath [MAX_PATH];
  WIN32_FIND_DATA wfd;
  QString mm;

  // Get a pointer to the IShellLink interface.
  hres = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
  IID_IShellLink, (void **)&psl);
  if (SUCCEEDED (hres))
  {
    IPersistFile *ppf;

    // Get a pointer to the IPersistFile interface.
    hres = psl->QueryInterface (IID_IPersistFile, (void **)&ppf);

    if (SUCCEEDED (hres))
    {
      WCHAR wsz [MAX_PATH]; // buffer for Unicode string

      // Ensure that the string consists of Unicode characters.
      MultiByteToWideChar (CP_ACP, 0, pszLink, -1, wsz, MAX_PATH);

      // Load the shortcut.
      hres = ppf->Load (wsz, STGM_READ);

      if (SUCCEEDED (hres))
      {
        // Resolve the shortcut.
        hres = psl->Resolve (hwnd, SLR_ANY_MATCH);
        if (SUCCEEDED (hres))
        {
          // strcpy (szGotPath, pszLink);
          // Get the path to the shortcut target.
          hres = psl->GetPath (szGotPath, MAX_PATH,(WIN32_FIND_DATA *)&wfd, SLGP_UNCPRIORITY /*SLGP_SHORTPATH*/);
          if (! SUCCEEDED (hres)) {
            mm = QString("Resolving shortcut ") + pszLink + " failed!";
            QMessageBox::critical(wxTheApp->m_appWindow, qApp->name(), mm /*ERR_ResolvingShortcutFailed.arg(pszLink)*/,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
          }
          resolved = szGotPath;
        }
        else {
          mm = QString("Resolving shortcut ") + pszLink + " failed!";
          QMessageBox::critical(wxTheApp->m_appWindow, qApp->name(), mm /*ERR_ResolvingShortcutFailed.arg(pszLink)*/,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        }
      }
      else {
        mm = QString("Loading shortcut ") + pszLink + " failed!";
        QMessageBox::critical(wxTheApp->m_appWindow, qApp->name(), mm /*ERR_ResolvingShortcutFailed.arg(pszLink)*/,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
      }
      // Release the pointer to IPersistFile.
      ppf->Release ();
      }
      // Release the pointer to IShellLink.
      psl->Release ();
    }
  if (SUCCEEDED(hres))
                return true;
        else
    return false;
}
#endif

QString ResolveLinks(QFileInfo &qf)
{
  QString abslnkName=qf.absoluteFilePath();
#ifdef WIN32_
  QString synName;

  if (qf.isSymLink()) {
    while (QFileInfo(abslnkName).isSymLink()) {
      if (!ResolveShortCut(0, abslnkName, synName))
         return QString::null;
      abslnkName = synName;
    }
  }
  else
    synName = qf.absFilePath();
  synName.replace('\\',"/");
  return synName;
  //QFileInfo qf1(synName);
  //return qf1.dir().canonicalPath() + "/" + qf1.fileName();
#else
  QFileInfo qf1(abslnkName), qflink;
  QString link;
  while (qf1.isSymLink()) {
    link = qf1.readLink();
    if (link.isEmpty())
      return link;
    qflink = QFileInfo(link);
    if (qflink.isRelative())
      abslnkName = qf1.path() + "/" + link;
    else
      abslnkName = link;
    qf1 = QFileInfo(abslnkName);
    }
  return qf1.dir().canonicalPath() + "/" + qf1.fileName();
#endif
}

// ----------------------------------------------------------------------------
// Permits compatibility with existing file formats and functions that
// manipulate files directly
// ----------------------------------------------------------------------------
/*
#if wxUSE_STD_IOSTREAM
bool wxTransferFileToStream(const QString& filename, ostream& stream)
{
    FILE *fd1;
    int ch;

    if ((fd1 = wxFopen (filename.fn_str(), _T("rb"))) == NULL)
        return false;

    while ((ch = getc (fd1)) != EOF)
        stream << (unsigned char)ch;

    fclose (fd1);
    return true;
}

bool wxTransferStreamToFile(istream& stream, const QString& filename)
{
    FILE *fd1;
    int ch;

    if ((fd1 = wxFopen (filename.fn_str(), _T("wb"))) == NULL)
    {
        return false;
    }

    while (!stream.eof())
    {
        ch = stream.get();
        if (!stream.eof())
            putc (ch, fd1);
    }
    fclose (fd1);
    return true;
}
#else
bool wxTransferFileToStream(const QString& filename, wxOutputStream& stream)
{
    FILE *fd1;
    int ch;

    if ((fd1 = wxFopen (filename, QString("rb"))) == NULL)
        return false;

    while ((ch = getc (fd1)) != EOF)
        stream.PutC((char) ch);

    fclose (fd1);
    return true;
}

bool wxTransferStreamToFile(wxInputStream& stream, const QString& filename)
{
    FILE *fd1;
    char ch;

    if ((fd1 = wxFopen (filename, QString("wb"))) == NULL)
    {
        return false;
    }

    int len = stream.StreamSize();
    // TODO: is this the correct test for EOF?
    while (stream.TellI() < (len - 1))
    {
        ch = stream.GetC();
        putc (ch, fd1);
    }
    fclose (fd1);
    return true;
}
#endif
*/

