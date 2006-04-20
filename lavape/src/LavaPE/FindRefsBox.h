#ifndef __FindRefsBox
#define __FindRefsBox


#include "LavaAppBase.h"
#include "LavaPEDoc.h"
//#include "wxExport.h"
#include "qcombobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
#include "idd_gotodeclsel.h"
#include "idd_findbynamebox.h"
#include "idd_findrefsbox.h"



/////////////////////////////////////////////////////////////////////////////
// CGotoBox dialog

class CGotoBox : public QDialog, public Ui_IDD_GotoDeclSel
{
public:
  CGotoBox(QWidget* pParent = 0); 
  CGotoBox(LavaDECL * fromDECL, CLavaPEDoc* doc, QWidget* pParent);
  TID SelID;
  CLavaPEDoc* myDoc;
  LavaDECL *myDECL;

public slots:
  virtual void on_ID_OK_clicked() {QDialog::accept();}
  virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
  virtual void on_GotoCombo_triggered(int);
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CFindRefsBox dialog

class CFindRefsBox : public QDialog, public Ui_IDD_FindRefsBox
{
public:
  CFindRefsBox(QWidget* pParent =0);   // standard constructor
  CFindRefsBox(LavaDECL* decl, bool inExec, CFindData* fw, QWidget* pParent);
  void UpdateData(bool getData);
  CFindData *findWhat;
  LavaDECL* myDECL;
  bool setExec;

	int		valFindWhere;
	int		valFindWhat;
  QString valRefName;


public slots:
	virtual void on_ID_OK_clicked();
  virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
	virtual void on_FindDerivs_toggled(bool);
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CFindByNameBox dialog

class CFindByNameBox : public QDialog, public Ui_IDD_FindByNameBox
{
public:
	CFindByNameBox(QWidget* pParent = 0);
  CFindByNameBox(CFindData* fw, QWidget* pParent = 0);
  CFindData *findWhat;

public slots:
	virtual void on_ID_OK_clicked();
  virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
private:
  Q_OBJECT
};


#endif 
