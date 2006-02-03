#ifndef __TREEVIEW__H_
#define __TREEVIEW__H_


#include "PEBaseDoc.h"
//#include "wxExport.h"
#include "docview.h" 
#include "q3listview.h" 
#include "qpixmap.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QFocusEvent>
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <QDropEvent>

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


class CTreeItem :public Q3ListViewItem 
{
public:
    friend class CTreeView;

  CTreeItem(QString label, QPixmap* pix, CTreeItem* parent, CTreeItem* afterItem);
  CTreeItem(QString label, Q3ListView* parent, CTreeItem* afterItem);
  CTreeItem(QString label, QPixmap* pix, Q3ListView* parent);

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
  int itemCount; //0 - based
protected:
  TItemData *data;
  QPixmap *nPix;
  QPixmap *normalPix;
  bool delPix;

};

class CTreeView;

class MyListView: public Q3ListView
{
public:
  MyListView(CTreeView* view);
  void setCurAndSel(Q3ListViewItem* item, bool singleSel = true);
  void keyPressEvent(QKeyEvent* ev);
  void contentsMousePressEvent(QMouseEvent *ev);
  void contentsMouseReleaseEvent(QMouseEvent *ev);
  void contentsMouseMoveEvent(QMouseEvent *ev);
  void focusInEvent ( QFocusEvent * e );
  Q3DragObject* dragObject();
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

  CTreeItem* GetPrevSiblingItem(CTreeItem* item);
//  CTreeItem* HitTest(QPoint qp);
  CTreeItem* InsertItem(QString label, QPixmap* nPix, CTreeItem* parent, CTreeItem* afterItem=TVI_LAST);
  QLineEdit* GetEditControl() {return labelEditWid;}
  QLineEdit *labelEditWid;
  virtual Q3DragObject* OnDragBegin() {return 0;}
  virtual void OnDelete() {}
  virtual void RenameStart(CTreeItem* item) {}
  virtual void RenameOk(CTreeItem* item) {}
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

#endif
