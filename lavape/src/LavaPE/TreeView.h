/////////////////////////////////////////////////////////////////////////////
// TreeView.h
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUC__
// #pragma interface "TreeCtrl.h"
#endif

#ifndef __TREEVIEW__H_
#define __TREEVIEW__H_

#include "PEBaseDoc.h"
//#include "wxExport.h"
#include "docview.h" 
#include "qlistview.h" 
#include "qpixmap.h"

#define TVI_ROOT                ((CTreeItem*)0xFFFF0000)
#define TVI_FIRST               ((CTreeItem*)0xFFFF0001)
#define TVI_LAST                ((CTreeItem*)0xFFFF0002)

class CTreeView;
class TItemData
{
public:
  TItemData() {}
  ~TItemData() {}
};


class CTreeItem :public QListViewItem 
{
public:
    friend class CTreeView;

  CTreeItem(QString label, QPixmap* pix, CTreeItem* parent, CTreeItem* afterItem);
  CTreeItem(QString label, QListView* parent, CTreeItem* afterItem);
  CTreeItem(QString label, QPixmap* pix, QListView* parent);

  ~CTreeItem();

  void setItemData(TItemData* dat) {data = dat;}
  TItemData* getItemData() { return data;}
  void setPix(QPixmap* pix);
  void SetItemMask(bool hasErr, bool hasCom);
  void SetItemMask(QPixmap* pixMask);
  virtual const QPixmap *pixmap(int i) const { if (i) return 0; return nPix; }
  virtual void startRename(int col);
  virtual void okRename(int col);
  virtual void cancelRename(int col);
  QString completeText;
  DString actLab;
  bool inRename;
protected:
  TItemData *data;
  QPixmap *nPix;
  QPixmap *normalPix;
  bool delPix;

};

class CTreeView;

class MyListView: public QListView
{
public:
  MyListView(CTreeView* view);
  void setCurAndSel(QListViewItem* item, bool singleSel = true);
  void keyPressEvent(QKeyEvent* ev);
  void contentsMousePressEvent(QMouseEvent *ev);
  void focusInEvent ( QFocusEvent * e );
  QDragObject* dragObject();
  void contentsDragMoveEvent(QDragMoveEvent *ev);
  void contentsDragEnterEvent(QDragEnterEvent *ev);
  void contentsDragLeaveEvent(QDragLeaveEvent *ev);
  void contentsDropEvent(QDropEvent* ev);
  CTreeView* lavaView;
  bool withShift;
  bool withControl;
private:
  Q_OBJECT

};


class CTreeView: public CLavaBaseView {
public:
  CTreeView(QWidget *parent,wxDocument *doc, const char* name);
  ~CTreeView() {}
  MyListView* GetListView() {return m_tree;}

//  VIEWFACTORY(CTreeView);

  MyListView *m_tree;

 // bool Expand(CTreeItem* item, bool exp);
  //CTreeItem* GetNextVisibleItem(CTreeItem* item);
  //TItemData* GetItemData(CTreeItem* item);
  //CTreeItem* GetNextItem(CTreeItem* item, bool firstChild);
  CTreeItem* GetPrevSiblingItem(CTreeItem* item);
  CTreeItem* HitTest(QPoint qp);
  CTreeItem* InsertItem(QString label, QPixmap* nPix, CTreeItem* parent, CTreeItem* afterItem=TVI_LAST);
  //void SetItemData(CTreeItem* item, TItemData* itd);
  QLineEdit* GetEditControl() {return labelEditWid;}
  QLineEdit *labelEditWid;
  virtual QDragObject* OnBegindrag() {return 0;}
  virtual void OnDelete() {}
  virtual void startRename(CTreeItem* item) {}
  virtual void okRename(CTreeItem* item) {}
  virtual void cancelRename(CTreeItem* item) {}
  virtual void OnDragOver(QDragMoveEvent* ev) {}
  virtual void OnDragEnter(QDragEnterEvent* ev) {}
  virtual void OnDragLeave(QDragLeaveEvent* ev) {}
  virtual void OnDrop(QDropEvent* ev) {}
  virtual void OnVkreturn() {}
private:
  Q_OBJECT

};

class TreeWhatsThis : public WhatsThis
{
public:
  TreeWhatsThis(CTreeView *tv);

  QString text(const QPoint&);
private:
  CTreeView *treeView;
};

#endif
