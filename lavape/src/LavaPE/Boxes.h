#ifndef __Boxes
#define __Boxes


#include "SylTraversal.h"
#include "LavaPEDoc.h"
//#include "Resource.h"
//#include "wxExport.h"
#include "q3combobox.h"
#include "q3listbox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"

#include "idd_attrbox.h"
#include "idd_compspecbox.h"
#include "idd_corroverbox.h"
#include "idd_enumbox.h"
#include "idd_enumitem.h"
#include "idd_interfacebox.h"
#include "idd_initbox.h"
#include "idd_funcbox.h"
#include "idd_implbox.h"
#include "idd_includebox.h"
#include "idd_iobox.h"
#include "idd_packagebox.h"
#include "idd_setbox.h"
#include "idd_vtypebox.h"

enum ValOnInit {BoxContinue, BoxOK, BoxCancel};


class CListBoxItem: public QListWidgetItem
{
public:
  CListBoxItem(const QString& txt, const TID& id) { setText(txt); ID = id;}
  CListBoxItem(const DString& text, const TID& id) {setText(QString(text.c)); ID = id;}
  CListBoxItem(const QString& text, unsigned* flags) {setText(text); Flags = flags;}
  CListBoxItem() {}
  TID itemData() {return ID;}
  unsigned* flags() {return Flags;}
  void setText(const QString &txt) { myText = txt; }
  QString text() { return myText; }
protected:
  TID ID;
  unsigned* Flags;
  QString myText;
};

/////////////////////////////////////////////////////////////////////////////
// CAttrBox dialog

class CAttrBox : public QDialog, public Ui_IDD_AttrBox
{
// Construction
public:
  CAttrBox(QWidget* pParent = NULL);   // standard constructor
  CAttrBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);

  LavaDECL* myDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  bool second;
  LavaDECL * OrigDECL;
  SynFlags TypeFlags;
 // bool SetSelections();

  QString valNewName;
  QString valNewTypeType;
  int valkindOfField;
  int valkindOfRef;

public slots:
    virtual void on_BasicTypes_triggered( int );
    virtual void on_NamedTypes_triggered( int );
    virtual void on_EnableName_clicked();
    virtual void on_DownC_clicked();
    virtual void on_DownInd_clicked();
    virtual void on_SetGet_clicked();
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
    virtual void on_RMOverrides_clicked();
    virtual void on_Substitutable_clicked();
    virtual void on_ValueObject_clicked() {}

 
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCompSpecBox 

class CCompSpecBox : public QDialog, public Ui_IDD_CompSpecBox
{
// Konstruktion
public:
  CCompSpecBox(QWidget* pParent = NULL);   // Standardkonstruktor
  CCompSpecBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  void UpdateData(bool getData);
  ValOnInit OnInitDialog();

  LavaDECL* myDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  LavaDECL * OrigDECL;

  QString valNewName;
  bool valPersistent;

   void SetButtons(int sel);

public slots:
    virtual void on_DelSupport_clicked();
    virtual void on_ExtTypes_triggered( int );
    virtual void on_CompoProt_triggered( int );
    virtual void on_EnumAdd_clicked();
    virtual void on_EnumEdit_clicked();
    virtual void on_EnumDel_clicked();
    virtual void on_EnumItems_selectionChanged( Q3ListBoxItem * );
    virtual void on_ID_OK_clicked();
    virtual void on_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();

private:
  Q_OBJECT
};

/////////////////////////////////////////////////////////////////////////////
// CCorrOverBox dialog

class CCorrOverBox : public QDialog, public Ui_IDD_CorrOverBox
{
// Construction
public:
	CCorrOverBox(QWidget* pParent = NULL);   // standard constructor
  CCorrOverBox(LavaDECL* decl, CLavaPEDoc* doc, QWidget* pParent = NULL);
  bool PossibleOver(LavaDECL* vtDecl);
  ValOnInit OnInitDialog();

  LavaDECL* myDECL;
  CLavaPEDoc* myDoc;
  CHETID* CheTID;

public slots:
    virtual void on_NoOver_clicked();
    virtual void on_NewOver_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_OK_clicked();
private:
  Q_OBJECT
};



class CEnumBox : public QDialog, public Ui_IDD_EnumBox
{

// Construction
public:
  CEnumBox(QWidget* pParent);
  CEnumBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ~CEnumBox();
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);
  LavaDECL* myDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  LavaDECL * OrigDECL;

  int   valItemNr;
  QString valNewName;
  bool  valBuildSet;

 void SetButtons(int sel);
  
public slots:
    virtual void on_EnumAdd_clicked();
    virtual void on_EnumEdit_clicked();
    virtual void on_EnumDel_clicked();
    virtual void on_EnumItems_selectionChanged( Q3ListBoxItem * );

    virtual void on_ID_OK_clicked();
    virtual void on_ID_HELP_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CEnumItem dialog

class CEnumItem : public QDialog, public Ui_IDD_EnumItem
{
public:
  CEnumItem(QWidget* parent = NULL);   // standard constructor
  CEnumItem(QString * enumItem, QListWidget* itemsBox, ChainAny0* items, bool isId, QWidget* parent =NULL);

  QString *m_ItemAdr;
  QListWidget* ItemsBox;
  QString StartText;
  ChainAny0* Items;
  bool isID;
  
public slots:
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFuncBox 

class CFuncBox : public QDialog, public Ui_IDD_FuncBox
{
public:
  CFuncBox(QWidget* pParent = NULL);   // Standardkonstruktor
  CFuncBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);
  LavaDECL* myDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  LavaDECL * OrigDECL;
  TID exID;
  bool hasOutput;
  bool hasParams;
  bool second;
  void CalcOpBox();

  QString valNewName;
  int valSynch;

public slots:
    virtual void on_EnableName_clicked();
    virtual void on_DelInherits_clicked();
    virtual void on_NamedTypes_triggered( int );
    virtual void on_CMBOperator_triggered( int );
    virtual void on_CHECKOp_clicked();
    virtual void on_Abstract_clicked();
    virtual void on_Native_clicked();
    virtual void on_Signal_clicked();
    virtual void on_StaticFunc_clicked();
    virtual void on_RMOverrides_clicked();
    virtual void on_Initializer_clicked();
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();

private:
  Q_OBJECT
};




/////////////////////////////////////////////////////////////////////////////
// CImplBox dialog

class CImplBox : public QDialog, public Ui_IDD_ImplBox
{
public:
  CImplBox(QWidget* pParent = NULL);   // standard constructor
  CImplBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);
  LavaDECL* myDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  LavaDECL * OrigDECL;

  //QComboBox m_ImplTypesCtrl;
  QString valImplSel;

public slots:
    virtual void on_ImplTypes_triggered( int );
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
private:
  Q_OBJECT

 };



/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CIncludeBox 

class CIncludeBox : public QDialog, public Ui_IDD_IncludeBox
{
public:
  CIncludeBox(QWidget* pParent = NULL);   // Standardkonstruktor
  CIncludeBox(CLavaPEDoc* myDoc, CHESimpleSyntax* newChe, CHESimpleSyntax* oldChe, QWidget* pParent = NULL);
  //ValOnInit OnInitDialog();
  void UpdateData(bool getData);
  CHESimpleSyntax* NewChe;
  CHESimpleSyntax* OldChe;
  CLavaPEDoc* MyDoc;

  QString valUseAs;
  QString valFullPathName;


  
public slots:
    virtual void on_OtherPath_clicked();
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
private:
  Q_OBJECT

};


/////////////////////////////////////////////////////////////////////////////
// CInitBox dialog

class CInitBox : public QDialog, public Ui_IDD_InitBox
{
public:
  CInitBox(QWidget* pParent = NULL);   // standard constructor
  CInitBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);

  LavaDECL* myDECL;
  LavaDECL * OrigDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  bool isTop;
  QString valNewName;
  int valSynch;


public slots:
//  virtual void on_m_Transaction1_clicked() {}
  virtual void on_ID_OK_clicked();
  virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
  virtual void on_ID_HELP_clicked();
private:
  Q_OBJECT

};


/////////////////////////////////////////////////////////////////////////////
// CInterfaceBox dialog

class CInterfaceBox : public QDialog, public Ui_IDD_InterfaceBox
{
public:
  CInterfaceBox(QWidget* pParent = NULL);   // standard constructor
  CInterfaceBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);
  void SupportsToList();

  LavaDECL *myDECL;
  LavaDECL *OrigDECL;
  LavaDECL *ContextDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  QString m_Struct;
  TID exID;
  QString valNewName;
  QString valIfaceID;
  QString valGUIStruct;
  bool  valIsGUI;
  bool  valBuildSet;
  int   valKindOfInterface;

public slots:
    virtual void on_DelSupport_clicked();
    virtual void on_BasicTypes_triggered( int );
    virtual void on_ExtTypes_triggered( int );
//    virtual void on_m_DelInherits_clicked();
//    virtual void on_m_InheritTypes_triggered( int );
    virtual void on_IsComponent_clicked();
    virtual void on_NonCreatable_clicked();
    virtual void on_Creatable_clicked();
    virtual void on_IsGUI_clicked();
    virtual void on_GUIStructs_triggered( int );
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
private:
  Q_OBJECT

};


/////////////////////////////////////////////////////////////////////////////
// CIOBox dialog

class CIOBox : public QDialog, public Ui_IDD_IOBox
{
public:
  CIOBox(QWidget* pParent = NULL);   // standard constructor
  CIOBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);
  void BaseClassesToCombo(LavaDECL *decl);

  LavaDECL* myDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  LavaDECL * OrigDECL;
  SynFlags TypeFlags;
 // bool SetSelections();

  QString valNewName;
  QString valNewTypeType;
  int valkindOfField;


public slots:
    virtual void on_BasicTypes_triggered( int );
    virtual void on_NamedTypes_triggered( int );
    virtual void on_Substitutable_clicked();
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
private:
  Q_OBJECT

};

/////////////////////////////////////////////////////////////////////////////
// CPackageBox dialog

class CPackageBox : public QDialog, public Ui_IDD_PackageBox
{
public:
  CPackageBox(QWidget* parent = NULL);   // standard constructor
  CPackageBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* parent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);

  LavaDECL* myDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  bool isTop;
  LavaDECL * OrigDECL;

  QString valNewName;


public slots:
    virtual void on_DelSupport_clicked();
    virtual void on_ExtTypes_triggered( int );
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CSetBox dialog

class CSetBox : public QDialog, public Ui_IDD_SetBox
{
// Construction
public:
  CSetBox(QWidget* pParent = NULL);   // standard constructor
  CSetBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);
 
  LavaDECL* myDECL;
  LavaDECL* InEl;
  CLavaPEDoc *myDoc;
  bool onNew;
  LavaDECL * OrigDECL;
  TID exID;

  QString valNewName;
  QString valExtend;


public slots:
    virtual void on_BasicTypes_triggered( int );
    virtual void on_ExTypes_triggered( int );
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked() {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CVTypeBox dialog

class CVTypeBox : public QDialog, public Ui_IDD_VTypeBox
{
public:
  CVTypeBox(QWidget* pParent = NULL);   // standard constructor
  CVTypeBox(LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew = false, QWidget* pParent = NULL); 
  ValOnInit OnInitDialog();
  void UpdateData(bool getData);

  LavaDECL* myDECL;
  CLavaPEDoc *myDoc;
  bool onNew;
  bool second;
  bool baseAbstract;
  SynFlags inheritedFlag;
  LavaDECL * OrigDECL;
 // bool SetSelections();
  void SetCategoryChecks();

  QString valNewName;
  QString valNewTypeType;
	int	valkindOfLink;

public slots:
    virtual void on_BasicTypes_triggered( int );
    virtual void on_NamedTypes_triggered( int );
    virtual void on_RMOverrides_clicked();
    virtual void on_StateObject_clicked();
    virtual void on_ValueObject_clicked();
    virtual void on_ID_OK_clicked();
    virtual void on_ID_CANCEL_clicked()  {QDialog::reject();}
    virtual void on_ID_HELP_clicked();
    virtual void on_EnableName_clicked();
    virtual void on_VTAbstract_clicked();
    virtual void on_DefCat_clicked();
    virtual void on_Substitutable_clicked();


private:
  Q_OBJECT

};
//////////////////////////////////////////////////////////

extern bool SetSelections(QComboBox* basics, QComboBox* types, const QString& name);

extern QString* CheckNewName(const QString& m_NewName, LavaDECL* myDECL, CLavaPEDoc* myDoc/*, LavaDECL* OrigDECL*/);


extern int SelEndOKToList(QComboBox* cbox, QListWidget* list, int chpos = -1);
extern int SelEndOKToStr(QComboBox* cbox, QString* editStr, TID* exID);


extern int CallBox( LavaDECL* decl, LavaDECL * origDECL, CLavaPEDoc* doc, bool isNew,
            bool& buildSet, QWidget* parent, bool asFirst=false); 

class CExecExtensions: public CExec
{
public:
  CExecExtensions(CLavaPEDoc* doc, QComboBox* combo, QComboBox* basicCombo, LavaDECL* decl, LavaDECL* origDECL);
  void ExecDefs (LavaDECL ** pelDef, int level);
  CLavaPEDoc* myDoc;
  TIDs BasicIDs;
  LavaDECL* myDECL;
  LavaDECL* OrigDECL;
  QComboBox *Combo;
  QComboBox *BasicCombo;
  //CDC* dc;
  //QSize sz;
  //int maxWidth;
};


extern void ListToChain(QListWidget *list, TSupports* supports);

class CExecDefs: public CExec
{ //Package with virtual types
public:
  CExecDefs(CLavaPEDoc* doc, QComboBox* combo, LavaDECL *decl);
  void ExecDefs (LavaDECL ** pelDef, int level);
  CLavaPEDoc* myDoc;
  LavaDECL *myDECL;
  QComboBox *Combo;
  //CDC* dc;
  //QSize sz;
  //int maxWidth;
};


class CExecBase: public CExec
{
public:
  CExecBase(CInterfaceBox* box);
  void ExecDefs(LavaDECL ** pelDef, int level);
  CInterfaceBox* IBox;
  //CDC* dc;
  QSize sz;
  //int maxWidth;
};

class CExecAllDefs: public CExec
{
public:
  CExecAllDefs(CLavaPEDoc* doc, QComboBox* combo, QComboBox* combo2, //QComboBox* list3,
      LavaDECL *parentDECL, LavaDECL *callingDECL, TDeclType deftype, SynFlags typeFlag);
  void ExecDefs (LavaDECL ** pelDef, int level);
  void FitBox(QComboBox* combo, int maxWidth);
  CLavaPEDoc* myDoc;
  QComboBox *Combo;
  QComboBox *Combo2;
  //QComboBox *List3;
  LavaDECL *ParentDECL;
  LavaDECL *CallingDECL;
  LavaDECL *IntfDECL;
  TDeclType DeclType;
  TID ClassID;
  SynFlags TypeFlags;
  ~CExecAllDefs() {}
};

#endif
