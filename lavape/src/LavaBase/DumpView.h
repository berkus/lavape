/////////////////////////////////////////////////////////////////////////////
// TreeView.h
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUC__
// #pragma interface "TreeCtrl.h"
#endif

#ifndef __DUMPVIEW__H_
#define __DUMPVIEW__H_

#include "LavaBaseDoc.h"
#include "Syntax.h"
//#include "wxExport.h"
#include "qmainwindow.h" 
#include "qlistview.h" 
#include "qdialog.h" 


class DumpListView;

class LAVABASE_DLL LavaDumpFrame : public QDialog //QMainWindow 
{
public:
  LavaDumpFrame( QWidget* parent, DumpEventData* data);
  virtual ~LavaDumpFrame();
  CLavaThread* fromThread;
  DumpListView * view;
  bool returned;
  void closeEvent(QCloseEvent *e);
public slots:
  void OnOK();
private:
  Q_OBJECT

};
class DumpItem;
class ObjDebugItem;

class LAVABASE_DLL ObjItem
{
public:
  bool hasChildren();
  void makeChildren(DumpItem* item, ObjDebugItem* data);
  void makeSetChildren(DumpItem* item, ObjDebugItem* data);
  void makeArrayChildren(DumpItem* item, ObjDebugItem* data);
  void setValues(DumpItem* item, ObjDebugItem* data);
  LavaObjectPtr myObject;
  bool withChildren;
  bool childrenDrawn;
  bool isSection;
  bool isTop;
  bool isPriv;
  CLavaBaseDoc *myDoc;
};

class LAVABASE_DLL DumpItem :public QListViewItem, public ObjItem 
{
public:
  DumpItem(DumpItem* parent, DumpItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec=true, bool isPriv=false);
  DumpItem(DumpItem* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec=true, bool isPriv=false);
  DumpItem(DumpListView* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName);
  void setOpen(bool o);
  ~DumpItem();

  virtual void paintCell( QPainter * p, const QColorGroup & cg,
			       int column, int width, int align );
};

class LAVABASE_DLL DumpListView: public QListView
{
public:
  DumpListView(QWidget *parent,CLavaBaseDoc* doc, LavaObjectPtr object, QString varName);
  ~DumpListView() {}
  DumpItem *rootItem;
  CLavaBaseDoc *myDoc;

private:
  Q_OBJECT

};


class LAVABASE_DLL ObjDebugItem :public ObjItem, public ObjItemData {
public:
  ObjDebugItem() {}
  ~ObjDebugItem() {}
  ObjDebugItem(ObjDebugItem* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec=true, bool isPriv=false, bool drawChildren=false);
  
  static ObjDebugItem* openObj (CHE* cheObj, CHEint* number);
};





#endif
