// Bars.h : header file
//

#ifndef __Bars
#define __Bars

#include "SylTraversal.h"
#include "TreeView.h"
#include "DumpView.h"
#include "Resource.h"
#include "Syntax.h"
//#include "wxExport.h"
#include "qcombobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
#include "qtabdialog.h"
#include "qtextview.h"
#include "qtabwidget.h"
#include "qsplitter.h"


enum BarTabs { /*tabBuild,*/ tabComment, tabError, tabFind, tabDebug };

class COutputBar;

class StackListView: public QListView
{
public:
  StackListView(QWidget *parent, COutputBar* bar);
  ~StackListView() {}
  void makeItems(DebugStopData* data, CLavaBaseDoc* doc);
  COutputBar* myBar;
  CTreeItem* lastSelected;
  bool allDrawn;

public slots:
  void selChanged();
private:
  Q_OBJECT

};

class VarItem;

class VarListView: public QListView
{
public:
  VarListView(QWidget *parent, COutputBar* bar);
  ~VarListView() {}
  void makeItems(DebugStopData* data);
  COutputBar* myBar;
  VarItem *itemToOpen;
private:
  Q_OBJECT

};


class VarItem  :public QListViewItem {
public:
  VarItem(VarItem* parent, VarItem* afterItem, ObjItemData* data);
  VarItem(VarItem* parent, ObjItemData* data);
  VarItem(VarListView* parent, ObjItemData* data);
  VarItem(VarListView* parent, VarItem* afterItem, ObjItemData* data);
  void makeChildren(const CHAINX& children);
  virtual void paintCell( QPainter * p, const QColorGroup & cg,
			       int column, int width, int align );
  virtual void setOpen(bool O);
  void makeNrs(ChObjRq *rqs);
  VarListView* myView;
  bool hasChildren;
  bool childrenDrawn;
  bool isPriv;
  bool isSelected;
  int itemCount;
};


class COutputBar : public QTabWidget //CSizeDlgBar
{
public:
  COutputBar(QWidget *parent);
  ~COutputBar();

  BarTabs ActTab;
  QString IdlMsg;
  bool ErrorEmpty;
  bool CommentEmpty;
  void DeleteAllFindItems();
  void ResetError();
  void SetErrorOnBar(LavaDECL* decl);
  void SetErrorOnBar(const CHAINX& ErrChain);
  void setError(const CHAINX& ErrChain, QString * strA);
  void SetComment(const DString& str, bool toStatebar = false);
  void SetFindText(const DString& text, CFindData* data);
  void SetTab(BarTabs tab);
  void setDebugData(DebugMessage* message, CLavaBaseDoc* doc);
  void showExecStackPos(DebugStopData* data, CLavaBaseDoc* doc);
  //void ChangeTab(BarTabs actTab);
  VarListView* VarView;
  StackListView* StackView;

protected:
  QListView* FindPage;
  QTextEdit* CommentPage;
  QTextEdit* ErrorPage;
  QSplitter *DebugPage;
  //void OnSize(UINT nType, int cx, int cy);
  //HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public slots:
  void OnDblclk(QListViewItem*);
  void OnTabChange(QWidget* curPage);
//  void OnSelChange();
private:
    Q_OBJECT;
};


#endif
