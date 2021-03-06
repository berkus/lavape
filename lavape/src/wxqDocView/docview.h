/////////////////////////////////////////////////////////////////////////////
// Name:        docview.h
// Purpose:     Doc/View classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCH__
#define _WX_DOCH__


#include "DString.h"
#include "wxExport.h"
#include "qapplication.h"
#include "qobject.h"
#include "qstring.h"
#include "qicon.h"
#include <QHBoxLayout>
#include "qlist.h"
#include "qaction.h"
#include "qsettings.h"
#include "qthread.h"
#include "qfileinfo.h"
//Added by qt3to4:
#include <QFocusEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QAbstractEventDispatcher>
#include <QTimer>
#include <QProcess>

#if wxUSE_PRINTING_ARCHITECTURE
    #include "print.h"
#endif


// Document manager flags
enum
{
    wxDOC_SDI = 1,
    wxDOC_MDI,
    wxDOC_NEW,
    wxDOC_SILENT,
    wxDEFAULT_DOCMAN_FLAGS = wxDOC_NEW
};

// Document template flags
enum
{
    wxTEMPLATE_VISIBLE = 1,
    wxTEMPLATE_INVISIBLE,
    wxDEFAULT_TEMPLATE_FLAGS = wxTEMPLATE_VISIBLE
};

//#define wxMAX_FILE_HISTORY 9


class WXDLLEXPORT wxDocManager;
class WXDLLEXPORT wxMainFrame;
class WXDLLEXPORT wxChildFrame;

class WXDLLEXPORT Assistant
{
public:
    Assistant();
    ~Assistant();
    void ShowPage(const QString &file);

private:
    bool startAssistant();
    QProcess *proc;
};

class WXDLLEXPORT wxApp : public QApplication
{
public:
    wxApp(int &argc, char ** argv);
    virtual ~wxApp();

    QString appDir;
    QThread *mainThread;
    bool deletingMainFrame;
    bool isChMaximized;
    bool appExit;
    bool cmdLineEvaluated;
    bool selectionChanged;

    int argc;
    char **argv;
    QAbstractEventDispatcher *m_eventLoop;

    virtual void saveSettings() {}
    virtual void UpdateUI() {}
    virtual void onUpdateUI();
    virtual void customEvent(QEvent *e);
    // set/get the application name
    QString GetAppName() const
    {
      if ( !m_appName.length() )
        return m_className;
      else
        return m_appName;
    }
    void SetAppName(const QString& name);
    QString GetSettingsPath() { return m_settingsPath; }
    QString GetLastFileOpen();
    void SetLastFileOpen(QString &lfo);
        // set/get the app class name
    QString wxGetOpenFileName(  QWidget *parent, const QString& startFileName,
                              const QString& caption, const QString& filter,  const QString& filter2=QString::null, bool save=false);
    QStringList wxGetOpenFileNames(QWidget *parent, const QString& startFileName,
                                  const QString& caption, const QString& filter, const QString& filter2=QString::null);
    QString GetClassName() const { return m_className; }
    void SetClassName(const QString& name) { m_className = name; }

        // set/get the vendor name
    const QString& GetVendorName() const { return m_vendorName; }
    void SetVendorName(const QString& name);
    virtual void OpenDocumentFile (const QString& name) { return;}
    virtual wxView *activeView();
    virtual void OnCloseLastExecView() {}
    virtual bool DoSaveAll();
    //QTimer *idleTimer;
    wxDocManager* m_docManager;
    wxMainFrame *m_appWindow;
    //wxChildFrame *m_oldActFrame;
//    QSettings *settings;
    QString lastFileOpen; //last file open from history or file dialog
    //QTimer *m_timer;
    Assistant *assistant;

public slots:
    void about();
    void updateButtonsMenus();

private:
    QString m_vendorName, m_appName, m_className, m_settingsPath;
    //wxView *m_activeView;

    Q_OBJECT
};

class wxChildFrame;

class WXDLLEXPORT wxDocument : public QObject
{
  friend class wxTabBar;

public:
    wxDocument(wxDocument *parent = (wxDocument *) NULL);
    virtual ~wxDocument();

    bool deleting;
    QList<wxView*> m_documentViews;

    // accessors
    //virtual wxDocument* CreateFailed();
    void SetFilename(const QString& filename, bool notifyViews = FALSE);
    QString GetFilename() const { return m_documentFile; }
    void SetUserFilename(const QString& filename)  {m_userFilename = filename; }
    QString GetUserFilename() const { return m_userFilename; }

    void SetTitle(const QString& title);
    QString GetTitle() const { return m_documentTitle; }

    void SetDocTypeName(const QString& name) { m_documentTypeName = name; };
    QString GetDocTypeName() const { return m_documentTypeName; }

    bool GetDocumentSaved() const { return m_savedYet; }
    void SetDocumentSaved(bool saved = true) { m_savedYet = saved; }

    virtual void customEvent(QEvent *ev);

    virtual bool Close();
    virtual bool Save();
    virtual bool SaveAs();
    virtual bool Revert();

    virtual QDataStream& SaveObject(QDataStream& stream);
    virtual QDataStream& LoadObject(QDataStream& stream);

    // Called by wxWidgets
    virtual bool OnSaveDocument(const QString& filename); //all links resolved
    virtual bool OnOpenDocument(const QString& filename); //all links resolved
    virtual bool OnNewDocument();
    virtual bool OnCloseDocument();
    virtual bool DeleteContents() {
      return true;
    };

    // Prompts for saving if about to close a modified document. Returns true
    // if ok to close the document (may have saved in the meantime, or set
    // modified to FALSE)
    virtual bool OnSaveModified();

    // Called by framework if created automatically by the default document
    // manager: gives document a chance to initialise and (usually) create a
    // view
    virtual bool OnCreate(const QString& path) { return true; } //all links resolved

    // Called after a view is added or removed. The default implementation
    // deletes the document if this is there are no more views.
    virtual void OnChangedViewList();

    virtual bool IsModified() //const
        { return m_documentModified; }
    virtual void Modify(bool mod) { m_documentModified = mod; }

    virtual void AddChildFrame(wxChildFrame *chf);
    virtual int RemoveChildFrame(wxChildFrame *chf);

    virtual bool AddView(wxView *view);
    virtual bool RemoveView(wxView *view);
    QList<wxView*>& GetViews() const { return (QList<wxView*>&) m_documentViews; }
    wxView *GetFirstView() const;
    //POSITION GetFirstViewPos();
    //wxView* GetNextView(POSITION& pos);
    //void ViewPosRelease(POSITION& pos);

    virtual void UpdateAllViews(wxView *sender = (wxView *) NULL, unsigned param = 0, QObject *hint = (QObject *) NULL);

    // Remove all MDI child frames (because we're closing the document)
    virtual bool DeleteAllChildFrames();

    // Other stuff
    virtual wxDocTemplate *GetDocumentTemplate() const { return m_documentTemplate; }
    virtual void SetDocumentTemplate(wxDocTemplate *temp) { m_documentTemplate = temp; }

    // Get title, or filename if no title, else [unnamed]
    virtual bool GetPrintableName(QString& buf) const;

    virtual  int GetChildFrameCount() {
      return m_docChildFrames.count();
    }

protected:
    QList<wxChildFrame*> m_docChildFrames;
    QString              m_documentFile; //all links are resolved
    QString              m_userFilename; //no link resolved
    QString              m_documentTitle, m_oldTitle; //the used name
    QString              m_documentTypeName;
    wxDocTemplate*       m_documentTemplate;
    bool                 m_documentModified;
    wxDocument*          m_documentParent;
    bool                 m_savedYet;

private:
    Q_OBJECT
};

class WXDLLEXPORT wxChildFrame;
class WXDLLEXPORT wxTabWidget;

class WXDLLEXPORT wxView : public QWidget
{
public:
    wxView(QWidget *parent, wxDocument *doc, const char* name);
    virtual bool OnCreate() { return true; };
    virtual void OnInitialUpdate() {}
    virtual ~wxView();
    bool active, deleting;
    QHBoxLayout *layout;
    wxTabWidget *myTabWidget;
    wxChildFrame *m_viewFrame;

    virtual void UpdateUI() {}
    virtual void DisableActions() {}
    wxDocument *GetDocument() const { return m_viewDocument; }
    void SetDocument(wxDocument *doc);

    QString GetViewName() const { return m_viewTypeName; }
    void SetViewName(const QString& name) { m_viewTypeName = name; };

    wxChildFrame *GetParentFrame() const {
      return m_viewFrame ; }
    void focusInEvent (QFocusEvent *e) {
      Activate(false); }

    virtual void Activate(bool topDown);
    virtual void OnUpdate(wxView *sender, unsigned param, QObject *hint = (QObject *) NULL);
    virtual void OnChangeFilename();


    // Checks if the view is the last one for the document; if so, asks user
    // to confirm save data (if modified). If ok, deletes itself and returns
    // true.
    virtual bool Close(/*bool deleteWindow = true*/);

    // Override to do cleanup/veto close
    virtual bool on_cancelButton_clicked(/*bool deleteWindow*/);

protected:
    wxDocument*               m_viewDocument;
    QString                   m_viewTypeName;

private:
    wxChildFrame *CalcParentFrame();

    Q_OBJECT
};


// Represents user interface (and other) properties of documents and views
class WXDLLEXPORT wxDocTemplate : public QObject
{
friend class WXDLLEXPORT wxDocManager;
friend class WXDLLEXPORT wxChildFrame;

public:
    // Associate document and view types. They're for identifying what view is
    // associated with what template/document type
    wxDocTemplate(wxDocManager *manager,
                  const QString& descr,
                  const QString& filter,
                  const QString& dir,
                  const QString& ext,
                  const QString& docTypeName,
                  const QString& frameTypeName,
                  const QString& viewTypeName,
                  Factory *docFactory=0,
                  FrameFactory *frameFactory=0,
                  ViewFactory *viewFactory=0,
                  long flags = wxDEFAULT_TEMPLATE_FLAGS);

    virtual ~wxDocTemplate();

    // By default, these two member functions dynamically creates document and
    // view using dynamic instance construction. Override these if you need a
    // different method of construction.
    virtual wxDocument *CreateDocument(const QString& path, long flags=0); //path is the used path name (no links resolved)
    virtual wxChildFrame *CreateChildFrame(wxDocument *doc);

    QString GetDefaultExtension() const { return m_defaultExt; };
    QString GetDescription() const { return m_description; }
    QString GetDirectory() const { return m_directory; };
    QString GetFileFilter() const { return m_fileFilter; };
    long GetFlags() const { return m_flags; };
    virtual QString GetViewName() const { return m_viewTypeName; }
    virtual QString GetDocTypeName() const { return m_docTypeName; }

    void SetFileFilter(const QString& filter) { m_fileFilter = filter; };
    void SetDirectory(const QString& dir) { m_directory = dir; };
    void SetDescription(const QString& descr) { m_description = descr; };
    void SetDefaultExtension(const QString& ext) { m_defaultExt = ext; };
    void SetFlags(long flags) { m_flags = flags; };

    bool IsVisible() const { return ((m_flags & wxTEMPLATE_VISIBLE) == wxTEMPLATE_VISIBLE); }

    virtual bool FileMatchesTemplate(const QString& path); //all links resolved

//protected:
    long              m_flags;
    QString          m_fileFilter;
    QString          m_directory;
    QString          m_description;
    QString          m_defaultExt;
    QString          m_docTypeName;
    QString          m_frameTypeName;
    QString          m_viewTypeName;

    // For dynamic creation of appropriate instances.
    Factory*         m_docClassInfo;
    FrameFactory*    m_frameClassInfo;
    ViewFactory*     m_viewClassInfo;

private:
    Q_OBJECT
};

// One object of this class may be created in an application, to manage all
// the templates and documents.
class WXDLLEXPORT wxDocManager : public QObject
{
public:
    wxDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS);
    virtual ~wxDocManager();
    QList<wxDocument*> m_docs;

    int m_currFrameInd, m_currTabWidInd;

    // Handlers for UI update commands
    void OnUpdateFileOpen(QAction *action);
    void OnUpdateFileClose(QAction *action);
    void OnUpdateFileRevert(QAction *action);
    void OnUpdateFileNew(QAction *action);
    void OnUpdateFileSave(QAction *action);
    void OnUpdateFileSaveAs(QAction *action);
    void OnUpdateUndo(QAction *action);
    void OnUpdateRedo(QAction *action);

    virtual void OnOpenFileFailure() { } //!!!

    virtual wxDocument *CreateDocument(const QString& path, long flags=0); //path is the used path name (no links resolved)
    virtual bool CreateView(wxDocument *doc);
    virtual void DeleteTemplate(wxDocTemplate *temp, long flags = 0);
    virtual wxDocTemplate *MatchTemplate(const QString& path); //all links resolved
    virtual wxDocTemplate *SelectDocumentPath(wxDocTemplate **templates,
            int noTemplates, QString& path, long flags, bool save = FALSE); //all links resolved
    virtual wxDocTemplate *SelectDocumentType(wxDocTemplate **templates,
            int noTemplates);
    virtual wxDocTemplate *SelectViewType(wxDocTemplate **templates,
            int noTemplates);
    virtual wxDocTemplate *FindTemplateForPath(const QString& path);//all links resolved

    void AssociateTemplate(wxDocTemplate *temp);
    void DisassociateTemplate(wxDocTemplate *temp);

    wxDocument *GetActiveDocument();// const;

    void SetMaxDocsOpen(int n) {
      m_maxDocsOpen = n; }
    int GetMaxDocsOpen() const {
      return m_maxDocsOpen; }

    // Add and remove a document from the manager's list
    void AddDocument(wxDocument *doc);
    void RemoveDocument(wxDocument *doc);

    // Clear remaining documents and templates
    bool Clear(bool force = true);

    // Views or windows should inform the document manager
    // when a view is going in or out of focus
    virtual void RememberActiveView(wxView *view, bool forget=false);
    virtual wxView *GetActiveView();// const;
    virtual wxChildFrame *GetActiveFrame() {
      return m_activeFrame; }
    virtual wxChildFrame *GetOldActiveFrame() {
      return m_oldActiveFrame; }
    virtual void RememberActiveFrame(wxChildFrame *af);
    virtual void RememberActiveIndexes(int pageIndex, int tabWidIndex) {
      m_currFrameInd = pageIndex;
      m_currTabWidInd = tabWidIndex;
    }
    virtual int GetDeletedFrame() {
      return m_currFrameInd; }
    virtual void ResetOldActiveFrame() {
      m_oldActiveFrame = 0; };
    virtual void RememberTotalCheckFrame() {
      m_totalCheckFrame = m_activeFrame; }
    virtual void ResetTotalCheckFrame() {
      SetNewCurrentFrame();
      m_totalCheckFrame = 0; }
    virtual wxChildFrame *GetTotalCheckFrame() {
      return m_totalCheckFrame; }
    void SetNewCurrentFrame();

    wxTabWidget *GetCurrentTabWidget() const {
      return m_currentTabWidget; }
    void SetCurrentTabWidget(wxTabWidget *tw) {
      m_currentTabWidget = tw; }

    virtual QList<wxDocument*>& GetDocuments() const {
      return (QList<wxDocument*>&) m_docs; }

    //POSITION GetFirstDocPos();
    //wxDocument* GetNextDoc(POSITION& pos);
    //void DocPosRelease(POSITION pos);
    wxDocument* FindOpenDocument(const QString& path);//all links resolved
    wxDocument* FindOpenDocumentN(const QString& path, bool& isNew);

    // Make a default document name
    virtual bool MakeDefaultName(QString& buf);

    // Make a frame title (override this to do something different)
    virtual QString MakeFrameTitle(wxDocument* doc);

    virtual wxHistory *GetFileHistory() const { return m_fileHistory; }

    // File history management
    virtual void AddFileToHistory(QString& file);
    virtual void SetFirstInHistory(int histFileIndex);
    virtual void RemoveFileFromHistory(int histFileIndex);
    virtual int GetNoHistoryFiles() const;
    virtual QString *GetHistoryFile(int histFileIndex) const;
//    virtual void FileHistoryUseMenu(QPopupMenu *menu);
//    virtual void FileHistoryRemoveMenu(QPopupMenu *menu);
#if wxUSE_CONFIG
    virtual void FileHistoryLoad(QSettings& config);
    virtual void FileHistorySave(QSettings& config);
#endif // wxUSE_CONFIG

    virtual void FileHistoryAddFilesToMenu();

    inline QString GetLastDirectory() const { return m_lastDirectory; }
    inline void SetLastDirectory(const QString& dir) { m_lastDirectory = dir; }

    // Get the current document manager
    static wxDocManager* GetDocumentManager();
    static int GetOpenDocCount();

    wxHistory*        m_fileHistory;

protected:
    long              m_flags;
    int               m_defaultDocumentNameCounter;
    int               m_maxDocsOpen;
    QList<wxDocTemplate*>  m_templates;
    QString           m_lastDirectory;
//    static wxDocManager* sm_docManager;

public slots:
    // Handlers for common user commands
    void OnFileNew();
    void OnFileOpen();
    void OnFileRevert();
    void OnFileSave();
    void OnFileSaveAs();
    void OnFileClose();
    void OnUndo();
    void OnRedo();

private:
    wxTabWidget *m_currentTabWidget;
    wxChildFrame      *m_activeFrame, *m_oldActiveFrame, *m_totalCheckFrame;
    wxView            *m_activeView;
    Q_OBJECT
};


class WXDLLEXPORT wxHistory : public QObject
{
public:
    wxHistory(QObject *receiver, int maxItems = 9);
    virtual ~wxHistory();

    // Operations
    virtual void AddToHistory(DString *item, QObject *receiver);
    virtual void SetFirstInHistory(int histFileIndex);
    virtual void SetFirstInHistory(const QString& file);
    virtual void RemoveItemFromHistory(int histFileIndex);
    virtual void RemoveItemFromHistory(QString name);

#if wxUSE_CONFIG
    virtual void Load(QSettings& config);
    virtual void Save(QSettings& config);
#endif // wxUSE_CONFIG

    virtual void AddFilesToMenu();
//    virtual void AddFilesToMenu(QMenu* menu);

    // Accessors
    virtual DString *GetHistoryItem(int i) const;

    virtual int GetCount() const { return m_historyN; }
    int GetMaxNoHistItems() const { return m_maxHistItems; }

    QMenu    *m_menu;

    // Max files to maintain
    int      m_maxHistItems;

    DString  **m_history;
    QAction **m_actions;
    QSignalMapper *m_signalMapper;

    // Number of files saved
    int      m_historyN;
    bool suppressHistAction;

private:
    Q_OBJECT
};

extern WXDLLEXPORT QString ResolveLinks(QFileInfo &qf);

extern WXDLLEXPORT_DATA(wxApp*) wxTheApp;

#endif // _WX_DOCH__
