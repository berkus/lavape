#if !defined(AFX_FINDREFSBOX1_H__E10CBE4D_44E9_40E1_8933_7B9B82AA1F76__INCLUDED_)
#define AFX_FINDREFSBOX1_H__E10CBE4D_44E9_40E1_8933_7B9B82AA1F76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindRefsBox1.h : header file
//


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

class CGotoBox : public IDD_GotoDeclSel
{
public:
  CGotoBox(QWidget* pParent = 0); 
  CGotoBox(LavaDECL * fromDECL, CLavaPEDoc* doc, QWidget* pParent);
  TID SelID;
  CLavaPEDoc* myDoc;
  LavaDECL *myDECL;

public slots:
  virtual void OnOK() {QDialog::accept();}
  virtual void OnCancel() {QDialog::reject();}
  virtual void m_GotoCombo_activated(int);
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CFindRefsBox dialog

class CFindRefsBox : public IDD_FindRefsBox
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
	virtual void OnOK();
  virtual void OnCancel() {QDialog::reject();}
	virtual void m_FindDerivs_toggled(bool);
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CFindByNameBox dialog

class CFindByNameBox : public IDD_FindByNameBox
{
public:
	CFindByNameBox(QWidget* pParent = 0);
  CFindByNameBox(CFindData* fw, QWidget* pParent = 0);
  CFindData *findWhat;

public slots:
	virtual void OnOK();
  virtual void OnCancel() {QDialog::reject();}
private:
  Q_OBJECT
};


#endif 
