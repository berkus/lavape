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

class DDItem;

class LAVABASE_DLL DDMakeClass
{
public:
  DDMakeClass() {myItem = 0;}
  DDItem *myItem;
  virtual bool hasChildren();
  virtual void makeChildren();
  virtual QString getValue0(const QString& stdLabel);
  virtual QString getValue1();
  virtual QString getValue2();
  LavaObjectPtr myObject;
  CLavaBaseDoc *myDoc;
  bool isCasted;
  bool isSection;
};

class  LAVABASE_DLL DDArrayClass : public DDMakeClass
{
public:
  DDArrayClass() {myItem = 0;}
  virtual bool hasChildren();
  virtual void makeChildren();
};

class  LAVABASE_DLL DDSetClass : public DDMakeClass
{
public:
  DDSetClass() {myItem = 0;}
  virtual bool hasChildren();
  virtual void makeChildren();
  virtual QString getValue0(const QString& stdLabel);
};

class LAVABASE_DLL DDItem
{
public:
  virtual DDItem* createChild(DDMakeClass* dd, DDItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object,
    QString varName, bool isSec=true, bool isPriv=false) {return 0;}
  DDMakeClass *DD;
  bool withChildren;
  bool childrenDrawn;
  bool isTop;
  bool isPriv;
  ~DDItem() {delete DD;}
};

class LAVABASE_DLL DumpItem :public QListViewItem, public DDItem 
{
public:
  DumpItem(DDMakeClass* dd, DumpItem* parent, DumpItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec=true, bool isPriv=false);
  DumpItem(DDMakeClass* dd, DumpItem* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec=true, bool isPriv=false);
  DumpItem(DDMakeClass* dd, DumpListView* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName);
  virtual DDItem* createChild(DDMakeClass* dd, DDItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec=true, bool isPriv=false);
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


class LAVABASE_DLL DebugItem :public DDItem, public DDItemData {
public:
  DebugItem() {}
  ~DebugItem() {}
  DebugItem(DDMakeClass* dd, DebugItem* parent, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec=true, bool isPriv=false, bool drawChildren=false);
  virtual DDItem* createChild(DDMakeClass* dd, DDItem* afterItem, CLavaBaseDoc* doc, LavaObjectPtr object, QString varName, bool isSec=true, bool isPriv=false);
  
  static DebugItem* openObj (CHE* cheObj, CHEint* number);
};





#endif
