// Bars.h : header file
//

#ifndef __Bars
#define __Bars

#include "SylTraversal.h"
#include "TreeView.h"
#include "Resource.h"
//#include "wxExport.h"
#include "qcombobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
#include "qtabdialog.h"
#include "qtextview.h"
#include "qtabwidget.h"


enum BarTabs { /*tabBuild,*/ tabComment, tabError, tabFind };

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
  //void ChangeTab(BarTabs actTab);

protected:
  QListView* FindPage;
  QTextEdit* CommentPage;
  QTextEdit* ErrorPage;
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
