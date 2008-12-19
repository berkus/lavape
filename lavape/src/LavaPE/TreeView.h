#ifndef __TREEVIEW__H_
#define __TREEVIEW__H_


#include "PEBaseDoc.h"
//#include "wxExport.h"
#include "docview.h" 
//#include "q3listview.h" 
#include "qpixmap.h"
//Added by qt3to4:
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QItemDelegate>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QFocusEvent>
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <QDropEvent>

#define MY_TVI_ROOT                ((CTreeItem*)0xFFFF0000)
#define MY_TVI_FIRST               ((CTreeItem*)0xFFFF0001)
#define MY_TVI_LAST                ((CTreeItem*)0xFFFF0002)

class CTreeView;
class MyListView;

class TItemData
{
public:
  TItemData() {}
  ~TItemData() {}
};


class CTreeItem :public QTreeWidgetItem 
{
public:
    friend class CTreeView;

  CTreeItem(QString label, int ipix, CTreeItem* parent, CTreeItem* afterItem);
  CTreeItem(QString label, QTreeWidget* parent, CTreeItem* afterItem);
  CTreeItem(QString label, QTreeWidget* parent);
  CTreeItem(QString label, int ipix, MyListView* parent);

  ~CTreeItem();
  void setItemData(TItemData* dat) {data = dat;}
  TItemData* getItemData() { return data;}
  void setPixmapIndex(int index);
  void SetIcon(QIcon* icon);
  void SetItemMask(bool hasErr, bool hasCom);
  void SetItemMask(int maskIndex);
  void setRenameEnabled(int col, bool enable);
  bool renameEnabled(int col);
  //virtual const QIcon* icon(int i) const { if (i) return 0; return nPix; }
  CTreeItem* nextSibling();
  void setDropEnabled(bool enable);
  
  virtual void startRename(QLineEdit* editor, int col);
  virtual void okRename(QLineEdit* editor, int col);
  virtual void cancelRename(int col);
  QString completeText;
  DString actLab;
  bool inRename;
  int itemCount; //0 - based
protected:
  TItemData *data;
  int pixmapIndex;
  QIcon *nPix;
  QIcon *normalPix;
  bool delPix;
};

class CTreeView;

class MyListView: public QTreeWidget //Q3ListView
{
public:
  MyListView(CTreeView* view);
  CTreeItem* itemAbove(CTreeItem* item);
  CTreeItem* itemAtIndex(const QModelIndex& index);
  void setCurAndSel(QTreeWidgetItem* item, bool singleSel = true);
  void keyPressEvent(QKeyEvent* ev);
  void mousePressEvent(QMouseEvent *ev);
  void mouseReleaseEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  //void focusInEvent ( QFocusEvent * e );
  void ResetSelections();
  //QDrag* dragObject();
  virtual void startDrag(Qt::DropActions supportedActions);
  void editItem ( QTreeWidgetItem * item, int column = 0 );
  QString text(const QPoint &point);
  bool event(QEvent *ev);

  void dragMoveEvent(QDragMoveEvent *ev);
  void dragEnterEvent(QDragEnterEvent *ev);
  void dragLeaveEvent(QDragLeaveEvent *ev);
  void dropEvent(QDropEvent* ev);

//  CTreeItem* firstChild() {return topLevelItem(0)); }
  
  CTreeItem* RootItem;
  CTreeView* lavaView;
  bool withShift;
  bool withControl;
  bool committed;

public slots:
   virtual void closeEditor(QWidget* , QAbstractItemDelegate::EndEditHint );
   virtual void commitData(QWidget* );
   void onItemSelectionChanged();

private:
  Q_OBJECT

};


class CTreeView: public CLavaBaseView {
public:
  CTreeView(QWidget *parent,wxDocument *doc, const char* name);
  ~CTreeView() {}
  MyListView* GetListView() {return Tree;}

//  VIEWFACTORY(CTreeView);

  MyListView *Tree;
  CTreeItem* GetPrevSiblingItem(CTreeItem* item);
//  CTreeItem* HitTest(QPoint qp);
  CTreeItem* InsertItem(QString label, int nPix, CTreeItem* parent, CTreeItem* afterItem=MY_TVI_LAST);
  virtual QDrag* OnDragBegin() {return 0;}
  virtual void OnDelete() {}
  virtual void RenameStart(QLineEdit* editor, CTreeItem* item) {}
  virtual void RenameOk(QLineEdit* editor, CTreeItem* item) {}
  virtual void RenameCancel(CTreeItem* item) {}
  virtual void OnDragOver(QDragMoveEvent* ev) {ev->ignore();}
  virtual void OnDragEnter(QDragEnterEvent* ev) {ev->ignore();}
  virtual void OnDragLeave(QDragLeaveEvent* ev) {}
  virtual void OnDrop(QDropEvent* ev) {ev->ignore();}
  virtual void OnVkreturn() {}
  LavaDECL* CollectDECL;
  bool multiSelectCanceled;
private:
  Q_OBJECT

};


class CTreeItemDelg: public QItemDelegate
{
public:
  CTreeItemDelg(MyListView* tree);
  virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const;
  virtual QWidget* createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const ;
  MyListView* Tree;

private:
  Q_OBJECT

};

#endif
