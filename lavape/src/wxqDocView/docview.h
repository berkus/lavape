/////////////////////////////////////////////////////////////////////////////
// Name:        docview.h
// Purpose:     Doc/View classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCH__
#define _WX_DOCH__


#include "DString.h"
#include "wxExport.h"
#include "qapplication.h"
#include "qobject.h"
#include "qstring.h"
#include "qicon.h"
#include "q3hbox.h"
#include "q3ptrlist.h"
#include "qaction.h"
#include "qsettings.h"
#include "qthread.h"
#include "qfileinfo.h"
//Added by qt3to4:
#include <QFocusEvent>
#include <Q3PopupMenu>
#include <QMouseEvent>
#include <QCustomEvent>

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
//class WXDLLEXPORT QAction;
/*
class WXDLLEXPORT QAction : public QAction {
public:
    QAction (QObject *parent = 0, const char *name = 0);
    virtual ~QAction() {}
    bool enable, deletingMainFrame;

private:
    Q_OBJECT
};
*/
class WXDLLEXPORT wxApp : public QApplication
{
public:
    wxApp(int & argc, char ** argv);
    virtual ~wxApp();

		QThread *mainThread;
		bool deletingMainFrame;
    bool isChMaximized;
    bool appExit;

    virtual void saveSettings() {}
    virtual void UpdateUI() {}
		virtual void onUpdateUI();
    virtual void customEvent(QCustomEvent *e);
//    void addAction(QAction* act) {actionList.append(act);}
    // application info: name, description, vendor
    // -------------------------------------------

    // NB: all these should be set by the application itself, there are no
    //     reasonable default except for the application name which is taken to
    //     be argv[0]

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

        // set/get the app class name
    QString GetClassName() const { return m_className; }
    void SetClassName(const QString& name) { m_className = name; }

        // set/get the vendor name
    const QString& GetVendorName() const { return m_vendorName; }
    void SetVendorName(const QString& name);
    virtual void OpenDocumentFile (const QString& name) { return;}
    virtual wxView *activeView();

    QTimer *idleTimer;
    wxDocManager* m_docManager;
    wxMainFrame *m_appWindow;
//    QSettings *settings;

public slots:
    void about();
    void onGuiThreadAwake();
    void onIdle();
    void histFile(int histFileIndex);
    void hfStatusText(int itemId);

private:
    QString m_vendorName, m_appName, m_className, m_settingsPath;
//    Q3PtrList<QAction> actionList;
    bool inUpdateUI;

    Q_OBJECT
};

class WXDLLEXPORT wxDocument : public QObject
{
public:
    wxDocument(wxDocument *parent = (wxDocument *) NULL);
    virtual ~wxDocument();

    bool deleting;

    // accessors
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

    virtual bool Close();
    virtual bool Save();
    virtual bool SaveAs();
    virtual bool Revert();

    virtual QDataStream& SaveObject(QDataStream& stream);
    virtual QDataStream& LoadObject(QDataStream& stream);

    // Called by wxWindows
    virtual bool OnSaveDocument(const QString& filename); //all links resolved
    virtual bool OnOpenDocument(const QString& filename); //all links resolved
    virtual bool OnNewDocument();
    virtual bool OnCloseDocument();

    // Prompts for saving if about to close a modified document. Returns true
    // if ok to close the document (may have saved in the meantime, or set
    // modified to FALSE)
    virtual bool OnSaveModified();

    // Called by framework if created automatically by the default document
    // manager: gives document a chance to initialise and (usually) create a
    // view
    virtual bool OnCreate(const QString& path) { return true; } //all links resolved

    // By default, creates a base wxCommandProcessor.
    virtual wxCommandProcessor *OnCreateCommandProcessor();
    virtual wxCommandProcessor *GetCommandProcessor() const { return m_commandProcessor; }
    virtual void SetCommandProcessor(wxCommandProcessor *proc) { m_commandProcessor = proc; }

    // Called after a view is added or removed. The default implementation
    // deletes the document if this is there are no more views.
    virtual void OnChangedViewList();

    virtual bool DeleteContents();

//!!!    virtual bool Draw(wxDC&);
    virtual bool IsModified() const
        { return m_documentModified; }
    virtual void Modify(bool mod) { m_documentModified = mod; }

    virtual bool AddView(wxView *view);
    virtual bool RemoveView(wxView *view);
    Q3PtrList<wxView>& GetViews() const { return (Q3PtrList<wxView>&) m_documentViews; }
    wxView *GetFirstView() const;
    POSITION GetFirstViewPos();
    wxView* GetNextView(POSITION& pos);
    void ViewPosRelease(POSITION pos);

    virtual void UpdateAllViews(wxView *sender = (wxView *) NULL, unsigned param = 0, QObject *hint = (QObject *) NULL);

    // Remove all views (because we're closing the document)
    virtual bool DeleteAllViews();

    // Other stuff
    virtual wxDocTemplate *GetDocumentTemplate() const { return m_documentTemplate; }
    virtual void SetDocumentTemplate(wxDocTemplate *temp) { m_documentTemplate = temp; }

    // Get title, or filename if no title, else [unnamed]
    virtual bool GetPrintableName(QString& buf) const;

    // Returns a window that can be used as a parent for document-related
    // dialogs. Override if necessary.
    virtual QWidget *GetDocumentWindow();// const;
    virtual  int GetViewCount() { 
			return m_documentViews.count(); }

protected:
    Q3PtrList<wxView>        m_documentViews;
    QString              m_documentFile; //all links are resolved
    QString              m_userFilename; //no link resolved
    QString              m_documentTitle, m_oldTitle; //the used name
    QString              m_documentTypeName;
    wxDocTemplate*        m_documentTemplate;
    bool                  m_documentModified;
    wxDocument*           m_documentParent;
    wxCommandProcessor*   m_commandProcessor;
    bool                  m_savedYet;

private:
    Q_OBJECT
};

class WXDLLEXPORT wxMDIChildFrame;

class WXDLLEXPORT wxView : public Q3HBox
{
public:
    wxView(QWidget *parent, wxDocument *doc, const char* name);
    virtual bool OnCreate() { return true; };
    virtual void OnInitialUpdate() {}
    virtual ~wxView();
    bool deleting;

    virtual void UpdateUI() {}
    virtual void focusInEvent ( QFocusEvent * e );
    virtual void mousePressEvent ( QMouseEvent * e );
    wxDocument *GetDocument() const { return m_viewDocument; }
    void SetDocument(wxDocument *doc);

    QString GetViewName() const { return m_viewTypeName; }
    void SetViewName(const QString& name) { m_viewTypeName = name; };

    wxMDIChildFrame *GetParentFrame() const { return m_viewFrame ; }
//    void SetFrame(wxMDIChildFrame *frame) { m_viewFrame = frame; }
    virtual void ActivateView(bool activate);
    virtual void OnActivateView(bool activate=true, wxView *deactiveView=0);
//    virtual void OnDraw(QPainter *dc) = 0;
//    virtual void OnPrint(wxDC *dc, QObject *info);
    virtual void OnUpdate(wxView *sender, unsigned param, QObject *hint = (QObject *) NULL);
    virtual void OnChangeFilename();


    // Checks if the view is the last one for the document; if so, asks user
    // to confirm save data (if modified). If ok, deletes itself and returns
    // true.
    virtual bool Close(/*bool deleteWindow = true*/);

    // Override to do cleanup/veto close
    virtual bool OnClose(/*bool deleteWindow*/);

    // Extend event processing to search the document's event table
//    virtual bool ProcessEvent(wxEvent& event);

    // A view's window can call this to notify the view it is (in)active.
    // The function then notifies the document manager.

#if wxUSE_PRINTING_ARCHITECTURE
    virtual wxPrintout *OnCreatePrintout();
#endif

protected:
    wxDocument*               m_viewDocument;
    QString                   m_viewTypeName;
    wxMDIChildFrame*       m_viewFrame;

private:
    wxMDIChildFrame *CalcParentFrame();

    Q_OBJECT
};


// Represents user interface (and other) properties of documents and views
class WXDLLEXPORT wxDocTemplate : public QObject
{
friend class WXDLLEXPORT wxDocManager;
friend class WXDLLEXPORT wxMDIChildFrame;

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
    virtual wxMDIChildFrame *CreateChildFrame(wxDocument *doc);

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

protected:
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

class WXDLLEXPORT wxDocManager : public QObject //wxEvtHandler
{
public:
    wxDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = true);
    virtual ~wxDocManager();

    virtual bool Initialize();

public:
    // Handlers for UI update commands
    void OnUpdateFileOpen(QAction *action);
    void OnUpdateFileClose(QAction *action);
    void OnUpdateFileRevert(QAction *action);
    void OnUpdateFileNew(QAction *action);
    void OnUpdateFileSave(QAction *action);
    void OnUpdateFileSaveAs(QAction *action);
    void OnUpdateUndo(QAction *action);
    void OnUpdateRedo(QAction *action);

//    void OnUpdatePrint(QAction *action);
//    void OnUpdatePrintSetup(QAction *action);
//    void OnUpdatePreview(QAction *action);

    // called when file format detection didn't work, can be overridden to do
    // something in this case
    // This is of course completely stupid, because if the file dialog is
    // cancelled you get an assert. Brilliant. -- JACS
//    virtual void OnOpenFileFailure() { wxFAIL_MSG(_T("file format mismatch")); }
    virtual void OnOpenFileFailure() { } //!!!

    virtual wxDocument *CreateDocument(const QString& path, long flags=0); //path is the used path name (no links resolved)
    virtual bool CreateView(wxDocument *doc);
    virtual void DeleteTemplate(wxDocTemplate *temp, long flags = 0);
    virtual bool FlushDoc(wxDocument *doc);
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

    void SetMaxDocsOpen(int n) { m_maxDocsOpen = n; }
    int GetMaxDocsOpen() const { return m_maxDocsOpen; }

    // Add and remove a document from the manager's list
    void AddDocument(wxDocument *doc);
    void RemoveDocument(wxDocument *doc);

    // Clear remaining documents and templates
    bool Clear(bool force = true);

    // Views or windows should inform the document manager
    // when a view is going in or out of focus
    virtual void SetActiveView(wxView *view, bool activate = true);
    virtual wxView *GetActiveView();// const;

    virtual Q3PtrList<wxDocument>& GetDocuments() const { return (Q3PtrList<wxDocument>&) m_docs; }

    POSITION GetFirstDocPos();
    wxDocument* GetNextDoc(POSITION& pos);
    void DocPosRelease(POSITION pos);
    wxDocument* FindOpenDocument(const QString& path);//all links resolved

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
    static wxDocManager* GetDocumentManager() { return sm_docManager; }
    static int GetOpenDocCount() { return wxDocManager::sm_docManager->m_docs.count(); };
    wxHistory*    m_fileHistory;

protected:
    long              m_flags;
    int               m_defaultDocumentNameCounter;
    unsigned               m_maxDocsOpen;
    Q3PtrList<wxDocument>            m_docs;
    Q3PtrList<wxDocTemplate>            m_templates;
    wxView*           m_activeView;
    QString          m_lastDirectory;
    static wxDocManager* sm_docManager;

public slots:
    // Handlers for common user commands
    void OnFileNew();
    void OnFileOpen();
    void OnFileRevert();
    void OnFileSave();
    void OnFileSaveAs();
    void OnFileClose();
//    void OnPrint();
//    void OnPrintSetup();
//    void OnPreview();
    void OnUndo();
    void OnRedo();
private:
    Q_OBJECT
};


// ----------------------------------------------------------------------------
// Provide simple default printing facilities
// ----------------------------------------------------------------------------
/*
#if wxUSE_PRINTING_ARCHITECTURE
class WXDLLEXPORT wxDocPrintout : public wxPrintout
{
public:
    wxDocPrintout(wxView *view = (wxView *) NULL, const QString& title = "Printout");
    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

    virtual wxView *GetView() { return m_printoutView; }

protected:
    wxView*       m_printoutView;

private:
    Q_OBJECT;
};
#endif // wxUSE_PRINTING_ARCHITECTURE
*/

// ----------------------------------------------------------------------------
// Command processing framework
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCommand : public QObject
{
public:
    wxCommand(bool canUndoIt = FALSE, const QString& name = "");
    virtual ~wxCommand();

    // Override this to perform a command
    virtual bool Do() = 0;

    // Override this to undo a command
    virtual bool Undo() = 0;

    virtual bool CanUndo() const { return m_canUndo; }
    virtual QString GetName() const { return m_commandName; }

protected:
    bool     m_canUndo;
    QString m_commandName;

private:
    Q_OBJECT
};

class WXDLLEXPORT wxCommandProcessor : public QObject
{
public:
    wxCommandProcessor(int maxCommands = 100);
    virtual ~wxCommandProcessor();

    // Pass a command to the processor. The processor calls Do(); if
    // successful, is appended to the command history unless storeIt is FALSE.
    virtual bool Submit(wxCommand *command, bool storeIt = true);
    virtual bool Undo();
    virtual bool Redo();
    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    // Call this to manage an edit menu.
    void SetEditMenu(Q3PopupMenu *menu) { m_commandEditMenu = menu; }
    Q3PopupMenu *GetEditMenu() const { return m_commandEditMenu; }
    virtual void SetMenuStrings();
    virtual void Initialize();

    Q3PtrList<wxCommand>& GetCommands() const { return (Q3PtrList<wxCommand>&) m_commands; }
    int GetMaxCommands() const { return m_maxNoCommands; }
    virtual void ClearCommands();

protected:
    unsigned           m_maxNoCommands;
    Q3PtrList<wxCommand>        m_commands;
    /*QLNode*/wxCommand*       m_currentCommand;
    Q3PopupMenu*       m_commandEditMenu;

private:
    Q_OBJECT
};

// ----------------------------------------------------------------------------
// File history management
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxHistory : public QObject
{
public:
    wxHistory(int maxFiles = 9, Q3PopupMenu *m=0);
    virtual ~wxHistory();

    // Operations
    virtual void AddToHistory(DString *item, QObject *receiver);
    virtual void SetFirstInHistory(int histFileIndex);
    virtual void SetFirstInHistory(const QString& file);
    virtual void RemoveItemFromHistory(int histFileIndex);
    virtual void RemoveItemFromHistory(QString name);
    virtual void OnChangeOfWindowTitle(QString &oldName, QString &newName);
//    virtual void UseMenu(QPopupMenu *menu);

    // Remove menu from the list (MDI child may be closing)
//    virtual void RemoveMenu(QPopupMenu *menu);

#if wxUSE_CONFIG
    virtual void Load(QSettings& config);
    virtual void Save(QSettings& config);
#endif // wxUSE_CONFIG

    virtual void AddFilesToMenu();
    virtual void AddFilesToMenu(Q3PopupMenu* menu); // Single menu

    // Accessors
    virtual DString *GetHistoryItem(int i) const;

    virtual int GetCount() const { return m_historyN; }
    int GetMaxNoHistItems() const { return m_maxHistItems; }

//    Q3PtrList<QPopupMenu>& GetMenus() const { return (Q3PtrList<QPopupMenu>&) m_fileMenus; }

    QMenu/*Q3PopupMenu*/        *m_menu;

protected:
    // Max files to maintain
    int               m_maxHistItems;

    DString           **m_history;

    // Number of files saved
    int               m_historyN;

private:
    Q_OBJECT
};

extern WXDLLEXPORT QString ResolveLinks(QFileInfo &qf);

extern WXDLLEXPORT_DATA(wxApp*) wxTheApp;


/*
#if wxUSE_STD_IOSTREAM
// For compatibility with existing file formats:
// converts from/to a stream to/from a temporary file.
bool WXDLLEXPORT wxTransferFileToStream(const QString& filename, ostream& stream);
bool WXDLLEXPORT wxTransferStreamToFile(istream& stream, const QString& filename);
#else
// For compatibility with existing file formats:
// converts from/to a stream to/from a temporary file.
bool WXDLLEXPORT wxTransferFileToStream(const QString& filename, wxOutputStream& stream);
bool WXDLLEXPORT wxTransferStreamToFile(wxInputStream& stream, const QString& filename);
#endif
*/

#endif // _WX_DOCH__
