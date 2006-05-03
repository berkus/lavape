#ifndef __Bars
#define __Bars

#include "SylTraversal.h"
#include "TreeView.h"
#include "DumpView.h"
#include "DbgThreadPE.h"
#include "Resource.h"
#include "Syntax.h"
#include "wxExport.h"
//#include "q3combobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
//#include "q3tabdialog.h"
//#include "q3textview.h"
#include "qtabwidget.h"
#include "qsplitter.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>


enum UtilityTabs { /*tabBuild,*/ tabComment, tabError, tabFind, tabDebug };

class CUtilityView;

class StackListView: public QTreeWidget
{
public:
  StackListView(QWidget *parent, CUtilityView* util);
  ~StackListView() {}
  void makeItems(DbgStopData* data, CLavaBaseDoc* doc);
  CUtilityView* myUtilityView;
  CTreeItem* lastSelected;
  bool allDrawn;
  //CTreeItem* firstChild() {return (CTreeItem*)itemFromIndex(rootIndex().child(0,0)); }

public slots:
  void selChanged();
  void itemClicked(QTreeWidgetItem *item, int );

private:
  Q_OBJECT

};

class VarItem;

class VarListView: public QTreeWidget
{
public:
  VarListView(QWidget *parent, CUtilityView* util, bool forParams);
  ~VarListView() {}
  void makeItems(const CHAINX& objChain);
  CUtilityView* myUtilityView;
  int width0, width1, width2;

public slots:
  virtual void expandedItem(QTreeWidgetItem* item);
 // VarItem* firstChild() {return (VarItem*)itemFromIndex(rootIndex().child(0,0)); }

private:
  Q_OBJECT

};


class VarItem  :public QTreeWidgetItem {
public:
  VarItem(VarItem* parent, VarItem* afterItem, DDItemData* data, VarListView* view);
  VarItem(VarItem* parent, DDItemData* data, VarListView* view);
  VarItem(VarListView* parent, DDItemData* data);
  VarItem(VarListView* parent, VarItem* afterItem, DDItemData* data);
  void makeChildren(const CHAINX& children);
  void setExpandable(bool withChildren);
  //virtual void paintCell( QPainter * p, const QColorGroup & cg,
	//		       int column, int width, int align );
  virtual void setOpen(bool O);
  void makeNrs(ChObjRq *rqs);
  VarListView* myView;
  bool hasChildren;
  bool childrenDrawn;
  bool isPriv;
  bool isSelected;
  int itemCount;
};


class CUtilityView : public QTabWidget //CSizeDlgBar
{
public:
  CUtilityView(QWidget *parent);
  ~CUtilityView();

  UtilityTabs ActTab;
  QString IdlMsg;
  bool ErrorEmpty;
  bool CommentEmpty;
  void DeleteAllFindItems();
  void ResetError();
  void SetErrorOnUtil(LavaDECL* decl);
  void SetErrorOnUtil(const CHAINX& ErrChain);
  void setError(const CHAINX& ErrChain, QString * strA);
  void SetComment(const DString& str, bool toStatebar = false);
  void SetFindText(const DString& text, CFindData* data);
  void SetTab(UtilityTabs tab);
  void setDebugData(DbgMessages* message, CLavaBaseDoc* doc);
  void showExecStackPos(DbgStopData* data, CLavaBaseDoc* doc);
  void removeExecStackPos(DbgStopData* data, CLavaBaseDoc* doc);
  //void ChangeTab(UtilityTabs actTab);
  QSplitter *splitter;
  bool firstTime;
  VarListView* VarView;
  VarListView* ParamView;
  StackListView* StackView;
  CLavaBaseDoc* stopDoc;
  VarItem *itemToOpen;

protected:
  bool eventFilter(QObject *watched,QEvent *ev);
  QTreeWidget* FindPage;
  QTextEdit* CommentPage;
  QTextEdit* ErrorPage;
  QFrame *DebugPage;
  //QSplitter *DebugPage;
  LavaDECL *stopExecDECL;
  bool firstDebug;
  //void OnSize(UINT nType, int cx, int cy);
  //HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public slots:
  void OnDblclk(QTreeWidgetItem* item, int col);
  void OnTabChange(int);
//  void OnSelChange();
private:
    Q_OBJECT;
};


#endif
