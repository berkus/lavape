#ifndef __InclView
#define __InclView



#include "LavaPEDoc.h"
#include "TreeView.h"
//#include "wxExport.h"
#include "qcombobox.h"
//Added by qt3to4:
#include "docview.h"
#include "qobject.h"
#include "qstring.h"


class CInclView : public CTreeView
{
public:
  CInclView(QWidget* parent, wxDocument* doc); 
  CLavaPEDoc* GetDocument(); // non-debug version is inline
//  int AddIncl(SynDef *LavaCode, CTreeItem* parent = NULL);
  virtual void OnInitialUpdate();
  virtual void OnUpdate(wxView* pSender, unsigned lHint, QObject* pHint);
  virtual void OnActivateView(bool bActivate=true, wxView *deactiveView=0);
  virtual ~CInclView();
  virtual void UpdateUI();
  void DisableActions();

  CTreeView *myTree;

  //Toolbutton/menu item  handler:
  void customEvent(QEvent *ev);
  void OnNewInclude();
  void OnDelete();
  void OnEditSel();
  bool Expanded;
  bool InitComplete;
  virtual void on_whatNextAction_triggered();

protected:
  void OnUpdateDelete(QAction* action);
  void OnUpdateEditSel(QAction* action);

public slots:
  void OnDblclk(QTreeWidgetItem* item, int col);
  //void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
    Q_OBJECT;
};
/*
class InclWhatsThis// : public WhatsThis
{
public:
  InclWhatsThis(MyListView *lv);

  QString text(const QPoint&);

private:
  MyListView *listView;
};
*/
#endif
