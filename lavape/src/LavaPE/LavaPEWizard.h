#ifndef __CLavaPEWizard
#define __CLavaPEWizard


#include "LavaPEView.h"
#include "SylTraversal.h"
#include "Syntax.h"
#include "q3combobox.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QCustomEvent>
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
#include "q3tabdialog.h"
#include "q3listbox.h"
#include "qlineedit.h"
#include "q3button.h"
#include "qspinbox.h"
#include "qtabwidget.h"
#include "LavaGUIView.h"

#include "idd_generalpage.h"
#include "idd_supportpage.h"
#include "idd_menupage.h"
#include "idd_fontcolorpage.h"
#include "idd_iopage.h"
#include "idd_literalspage.h"
#include "idd_chainformpage.h"
#include "idd_literalitem.h"
#include "idd_menuitem.h"
#include "idd_ftextpage.h"

/////////////////////////////////////////////////////////////////////////////
// CLavaPEWizard

class  CGeneralPage;
class  CSupportPage ;
class  CMenuPage;
class  CIOPage ;
class  CLiteralsPage;
class  CChainFormPage ;
class  CFormTextPage ;
class  CFontColorPage ;
class CLavaPEWizard;

class CWizardView: public CLavaBaseView
{
public:
  CWizardView(QWidget* parent, wxDocument* doc);
  ~CWizardView() {}
  void OnUpdate(wxView* , unsigned undoRedo, QObject* pHint); 
  void PostApplyHint();
  void customEvent(QCustomEvent *ev);
  void Resize();
  GUIScrollView* myScrv;
  CLavaPEWizard* wizard;
  GUIVBox* guibox;
  CLavaPEView* myTree;
  int postedPage;
  int delayedID;
  QPushButton* resetButton;
  QPushButton* applyButton;
  QPushButton* helpButton;
  virtual void setFont(const QFont& font);
public slots:
  void Apply();
  void Reset();
  void Help();
private:
  Q_OBJECT
};

class CLavaPEWizard : public QTabWidget
{
public:
  CLavaPEWizard(LavaDECL ** p_origDECL, LavaDECL* formDECL, CWizardView* view, QWidget* parent);
  ~CLavaPEWizard();

  void AddPages();
  void AddChainPages(LavaDECL *chainEl);
  void UpdateData();
  CLavaPEHint* ApplyHint();
  //void SetFont(const QFont& font);

  void SetSpace(TAnnotation *anno, unsigned& m_tab, unsigned& m_space, unsigned& m_frmSpace);
  void GetSpace(TAnnotation **p_anno, unsigned m_tab, unsigned m_space, unsigned m_frmSpace);
  CLavaPEDoc *myDoc;
  LavaDECL *FormDECL;
  LavaDECL *VFormDECL;
  LavaDECL *OrigDECL;
  LavaDECL **synEl;
  CWizardView* myView;
  bool isActive;
  bool modified;
  void setModified(bool mod);
  CGeneralPage   * GenPage;
  CSupportPage   * SupportPage;
  CMenuPage      * MenuPage;
  CFontColorPage * FontColorPage;
  CIOPage        * IOPage;
  CLiteralsPage  * LitPage;
  CChainFormPage * ChFormPage ;
  CIOPage        * ChainIOPage; 
  CLiteralsPage  * ChainLitPage;
  CFormTextPage  * FormTextPage;
protected:
public slots:
  void OnSetActive(QWidget *);
  void OnApply();
  virtual void OnCancel() {}//  {QDialog::reject();}
private:
  Q_OBJECT
};


class ColorSetting
{
public:
  ColorSetting() {}
  void Init(AnnoExType role, LavaDECL* decl, QCheckBox* defaultB,
                               QCheckBox* defaultF,
                               QPushButton* ButtonB,
                               QPushButton* ButtonF,
                               QFrame* Fore,
                               QFrame* Back);
  ~ColorSetting() {}
  LavaDECL * FormDECL;
  QColor BColor;
  QColor FColor;
  AnnoExType Role;
  QCheckBox* defaultBackground;
  QCheckBox* defaultForeground;
  QPushButton* colorButtonB;
  QPushButton* colorButtonF;
  QFrame* ForeColor;
  QFrame* BackColor;

  bool on_defaultForeground_clicked();
  bool on_defaultBackground_clicked();
  bool on_colorButtonF_clicked();
  bool on_colorButtonB_clicked();

};



class CChainFormPage : public QDialog, public Ui_IDD_ChainFormPage
{
public:
  CChainFormPage(CLavaPEWizard *wizard);
  ~CChainFormPage();
  void UpdateData(bool getData);
  bool OnApply();

  CLavaPEWizard *myWizard;
  LavaDECL * CHEEl;

	/*
    QCheckBox* m_HasButtons;
    QCheckBox* m_ConstChain;
    QLineEdit* m_EditInsertButton;
    QLineEdit* m_EDITDelButton;
    QLineEdit* m_ArrayLen;
    QSpinBox* SpinBox_IDC_SPINAR;
    QComboBox* m_ChElemFormTypes;
    QLineEdit* m_ChFormElem;
    QLineEdit* m_NewName2;

  QLineEdit	m_ArrayLen;
	QSpinBox	m_SpinAr;
  QLineEdit m_InsTextCtrl;
  QLineEdit m_DelTextCtrl;
  QButton m_ConstChain;
  QButton m_HasButtons;
  QComboBox m_ElemselCtrl;

  */
  QString m_ElemSel;
  QString m_ChFormElemStr;
  QString m_DelText;
  QString m_InsText;
  //QString m_Name;
  int m_Len;
  bool modify;


  public slots:
  virtual void OnSelendokChElemFormTypes(int);
  virtual void OnConstChain();
  virtual void OnHasButtons();
  virtual void on_m_EditInsertButton_textChanged( const QString & );
  virtual void on_m_EDITDelButton_textChanged( const QString & );
  virtual void on_m_DefaultLength_valueChanged( int );

private:
  Q_OBJECT
};


class CFormTextPage : public QDialog, public Ui_FormtextPage
{
public:
  CFormTextPage(LavaDECL * litEl,  CLavaPEWizard *wizard);  
  void UpdateData(bool getData);
  void OnApply();
  ~CFormTextPage() {}
  LavaDECL * myDecl;
  CLavaPEWizard *myWizard;
  bool modify;

  /*
  QSpinBox m_spin3;
  QSpinBox m_spin2;
  QSpinBox m_spin1;
  */
  QString m_lit;
  unsigned  m_LiFrmSpace;
  unsigned  m_LiSpace;
  unsigned  m_LiTab;
  ColorSetting colorSetting;

public slots:
  virtual void on_m_EditLiteral_textChanged( const QString & );
  virtual void on_m_HorizTab_valueChanged( int );
  virtual void on_m_HorizSpace_valueChanged( int );
  virtual void on_m_VertSpace_valueChanged( int );
  virtual void on_defaultFont_clicked();
  virtual void on_fontButton_clicked();
  virtual void on_defaultForeground_clicked();
  virtual void on_defaultBackground_clicked();
  virtual void on_colorButtonF_clicked();
  virtual void on_colorButtonB_clicked();

private:
  Q_OBJECT
};

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage dialog

class CGeneralPage : public QDialog, public Ui_IDD_GeneralPage
{
public:
  CGeneralPage(CLavaPEWizard *wizard);
  void UpdateData(bool getData);
  ~CGeneralPage();
  void SetSelections();
  CLavaPEWizard *myWizard;

  //QComboBox m_NamedTypesCtrl;
  //QString m_NewName;
  QString m_NewType;

  public slots:
  virtual void OnSelendokNamedTypes(int);

private:
  Q_OBJECT

};

class CFontColorPage : public QDialog, public Ui_idd_fontcolorpage
{
public:
  CFontColorPage(CLavaPEWizard *wizard, LavaDECL *formDECL);
  ~CFontColorPage();
  CLavaPEWizard *myWizard;
  LavaDECL *FormDECL;
  bool OnApply();
  ColorSetting colorSetting;
  ColorSetting TextColorSetting;
  ColorSetting PBColorSetting;

public slots:
  virtual void on_defaultFontT_clicked();
  virtual void on_defaultFontL_clicked();
  virtual void on_fontButtonT_clicked();
  virtual void on_fontButtonL_clicked();
  virtual void on_defaultForeground_clicked();
  virtual void on_defaultBackground_clicked();
  virtual void on_colorButtonF_clicked();
  virtual void on_colorButtonB_clicked();
  virtual void on_defaultTForeground_clicked();
  virtual void on_TColorButtonF_clicked();
  virtual void on_TColorButtonB_clicked();
  virtual void on_defaultTBackground_clicked();
  virtual void on_defaultPBForeground_clicked();
  virtual void on_PBColorButtonF_clicked();
  virtual void on_defaultPBBackground_clicked();
  virtual void on_PBColorButtonB_clicked();

private:
  Q_OBJECT

};


class CIOPage : public QDialog, public Ui_IDD_IOPage
{

public:
  CIOPage(CLavaPEWizard *wizard, LavaDECL *formDECL, bool forChElem);
  void UpdateData(bool getData);
  ~CIOPage();
  void SetProps();
  void GetProps();

  CLavaPEWizard *myWizard;
  LavaDECL *FormDECL;
  bool ForChainElem;

  bool OnApply();
  
 /* 
	QButton	m_PopupwCtrl;
  QSpinBox m_spin8;  //Rows
  QSpinBox m_spin7;  //FieldLength
  QSpinBox m_spin6;   //FrSpaceItem
  QSpinBox m_spin5;  //TabItem
  QSpinBox m_spin4;  //SpaceItem
  QSpinBox m_spin3;  //FrSpace
  QSpinBox m_spin2; //Tab
  QSpinBox m_spin1;  //Space
  QButton m_MultiLineCtrl;
  QLineEdit m_RowsCtrl;
  QLineEdit m_FLengthCtrl;
  QLineEdit m_TabItemCtrl;
  QLineEdit m_SpaceItemCtrl;
  QLineEdit m_FramespaceItemCtrl;
  QLineEdit m_TabCtrl;
  QLineEdit m_SpaceCtrl;
  QLineEdit m_FrmspaceCtrl;
  QButton m_noEchoCtrl;
  QButton m_InputCtrl;
  QButton m_OutputCtrl;
  QButton m_NoFIOCtrl;
  QButton m_DefaultIOCtrl;
*/
  bool  v_Input;
  bool  v_Output;
  bool  v_NoFIO;
  bool  v_DefaultIO;
  bool  v_noEcho;
  bool  v_isPopupW;
  bool  v_MultiLine;
  bool modify;
  unsigned  m_Frmspace;
  unsigned  m_Space;
  unsigned  m_Tab;
  unsigned  m_SpaceItem;
  unsigned  m_TabItem;
  unsigned  m_FramespaceItem;
  unsigned  m_FLength;
  unsigned  m_Rows;

public slots:
  virtual void OnisInput();
  virtual void OnisOutput();
  virtual void OnNoFIO();
  virtual void OnUseDefault();
  virtual void OnPopupw();
  virtual void OnMultiLine();
  virtual void on_m_Echo_clicked();
  virtual void horizTabChanged( int );
  virtual void horizSpaceChanged( int );
  virtual void vertSpaceChanged( int );
  virtual void colsChanged( int );
  virtual void rowsChanged( int );
  virtual void on_m_groupbox_clicked();

private:
  Q_OBJECT

};


/////////////////////////////////////////////////////////////////////////////
// CLiteralsPage dialog

class CLiteralsPage : public QDialog, public Ui_LiteralsPage
{
public:
  CLiteralsPage(CLavaPEWizard *wizard, LavaDECL *formDECL, bool forChElem);
  void UpdateData(bool getData);
  ~CLiteralsPage();
  void SetProps();
  void GetProps();
  bool OnAdd(ChainAny0* chain, QListWidget* m_list/*, int transpos=0*/);
  bool OnEdit(ChainAny0* chain, QListWidget* m_list/*, int transpos=0*/); 

  CLavaPEWizard *myWizard;
  LavaDECL *FormDECL;
  bool isEnumera;
  bool enabled;
  bool ForChainElem;

  bool OnApply();
  bool OnSetActive();
  bool modify;

  /*
  QComboBox m_BoolDefaultCtrl;
  QComboBox m_EnumDefaultCtrl; m_EnumDefault
  QLineEdit m_DefaultCtrl; m_Default
  QButton m_AddSuffix;  m_ADDSuf
  QButton m_AddPrefix; m_ADDPre
  QButton m_EditSuffix; m_EDITSuf
  QButton m_EditPrefix;m_EDITPre
  QButton m_DelSuffix; m_DELETESuf
  QButton m_DelPrefix; m_DELETEPre
  QListBox  m_Suffixe;
  QListBox  m_Prefixe;
  */

  QString v_Default;
  QString v_EnumDefault;
  QString v_BoolDefault;


  public slots:
  virtual void OnADDPre();
  virtual void OnADDSuf();
  virtual void OnDELETEPre();
  virtual void OnDELETESuf();
  virtual void OnEDITPre();
  virtual void OnEditSuf();
  virtual void on_m_Prefixe_doubleClicked( Q3ListBoxItem * );
  virtual void on_m_Suffixe_doubleClicked( Q3ListBoxItem * );
  virtual void OnSelchangePrefixe();
  virtual void OnSelchangeSuffixe();
  virtual void OnLButtonDown(unsigned nFlags, QPoint point);
  virtual void on_m_Default_textChanged( const QString & );
  virtual void on_m_EnumDefault_triggered( int );
  virtual void on_m_BoolDefault_triggered( int );


private:
  Q_OBJECT

};



enum EMenuType {isOMenu, isRMenu, isBMenu, isPMenu, isNoMenu};


/////////////////////////////////////////////////////////////////////////////
// CMenuPage dialog

class CMenuPage : public QDialog, public Ui_IDD_MenuPage
{
public:
  CMenuPage(CLavaPEWizard *wizard);
  void UpdateData(bool getData);
  ~CMenuPage();

  void SetEProps();
  void GetEProps();
  void SetBProps();
  void GetBProps();
  void SetDefaults(EMenuType newMenuT, EMenuType oldMenuT);
  void SetButtons(int sel);

  CLavaPEWizard *myWizard;
  bool isBool;
  bool isEnumera;
  bool modify;

  bool OnApply();
  bool OnSetActive();

  /*
  QButton m_LeftLabCtrl; m_LeftLabel
  QLineEdit m_ToggleLabCtrl; m_ToggleLabel
  QButton m_EditButton;
  QButton m_DeleteButton;
  QButton m_AddButton;
  QComboBox m_MenutypeCtrl; m_Menutype
  QListBox  m_LButtonText;
  QListBox  m_ItemList; m_Menuitems
  */

  int   v_Menutype;
  int   m_ItemNr;
  bool  v_LeftLabel;
  QString v_ToggleLabel;

public slots:
  virtual void OnSelchangeMenuitems();
  virtual void OnSelchangeLButtonText();
  virtual void m_Pixmap_selectionChanged();
  virtual void OnAddButton();
  virtual void OnDeleteButton();
  virtual void OnEditButton();
  virtual void OnSelendokMenutype(int current);
  virtual void OnLButtonDown(unsigned nFlags, QPoint point);
  virtual void on_m_ToggleLabel_textChanged( const QString & );
  virtual void on_m_LeftLabel_clicked();

private:
  Q_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CSupportPage dialog

class CSupportPage : public QDialog, public Ui_IDD_SupportPage
{
public:
  CSupportPage(CLavaPEWizard* wizard);
  //void UpdateData(bool getData);
  ~CSupportPage();

  CLavaPEWizard* myWizard;
  int LPos;

/*
  QListBox  m_ExtendsCtrl; m_Supports
*/
  //QString v_Name;
  //QString v_FormClass;
  QSize mySize;

  //bool OnApply();
  virtual QSize sizeHint() {return mySize;}

private:
  Q_OBJECT

};


/////////////////////////////////////////////////////////////////////////////
// CLiteralItem dialog

class CLiteralItem : public QDialog, public Ui_IDD_LiteralItem
{
public:
  CLiteralItem(CLiteralsPage *page, bool isNew, int ipos, QListWidget *litList,
               LavaDECL * litEl);  
  void UpdateData(bool getData);
  CLiteralsPage *LitPage;
  QListWidget *LitList;
  LavaDECL * myDecl;
  int litPos;
  bool insert;

  /*
  QSpinBox m_spin3;
  QSpinBox m_spin2;
  QSpinBox m_spin1;
*/
  QString m_lit;
  unsigned  m_LiFrmSpace; //3
  unsigned  m_LiSpace;    //1
  unsigned  m_LiTab;      //2
  //QColor BColor;
  //QColor FColor;
  ColorSetting colorSetting;

public slots:
  virtual void OnOK();
  virtual void OnCancel()  {QDialog::reject();}
  virtual void on_defaultFont_clicked();
  virtual void on_fontButton_clicked();
  virtual void on_defaultForeground_clicked();
  virtual void on_defaultBackground_clicked();
  virtual void on_colorButtonF_clicked();
  virtual void on_colorButtonB_clicked();

private:
  Q_OBJECT
};


class CFormTextBox : public QDialog, public Ui_IDD_LiteralItem
{
public:
  CFormTextBox(LavaDECL * litEl, QWidget* pParent = NULL, bool asFirst=false);  
  void UpdateData(bool getData);
  LavaDECL * myDecl;

  /*
  QSpinBox m_spin3;
  QSpinBox m_spin2;
  QSpinBox m_spin1;
  */
  QString m_lit;
  unsigned  m_LiFrmSpace;
  unsigned  m_LiSpace;
  unsigned  m_LiTab;


public slots:
  virtual void OnOK();
  virtual void OnCancel()  {QDialog::reject();}


private:
  Q_OBJECT
};

class CMenuItem : public QDialog, public Ui_IDD_Menuitem
{
public:
  CMenuItem(CMenuPage* mpage, bool isNew, int ipos, QString enumsel,
    QString btext, QString stext, unsigned *itemData);
  void UpdateData(bool getData);

  CMenuPage *menuPage;
  bool insert;
  bool onType;
  int iPos;
  unsigned *mflags;
  void EnableTextWindows();

  /*
  QSpinBox m_spin3; m_MenuSPIN3  -frSpace
  QSpinBox m_spin2; m_MenuSPIN2  -tab
  QSpinBox m_spin1; m_MenuSPIN1  -space
  QButton m_isButtonCtrl; BGroup_Style
  QLineEdit m_MenuTextCtrl; m_MenuText
  QLineEdit m_SelcodeCtrl; m_EDITSelcode
  QLineEdit m_ButtontextCtrl; m_EDITButtontext
  QLineEdit m_EnumselCtrl; m_Enumsel
  */

  int   m_flags;
  unsigned  m_MenuFrmspace;
  unsigned  m_Menuspace;
  unsigned  m_Menutab;


  public slots:
  virtual void OnnoButton(bool);
  virtual void OnNoFIO(bool);
  virtual void OnisButton(bool);
  virtual void OnisMenuText(bool);
  virtual void on_button_browse_clicked();
  virtual void OnOK();
  virtual void OnCancel()  {QDialog::reject();}


private:
  Q_OBJECT
};


class CExecLike: public CExec
{
public:
  CExecLike(CLavaPEDoc* doc, QComboBox* list, LavaDECL * Ldecl, LavaDECL* sylDef=0);
  void ExecFormDef(LavaDECL ** pelDef, int level);
  void ExecDefs (LavaDECL ** pelDef, int level);
  bool CheckSupports(LavaDECL *basedecl, LavaDECL *sylDef);
  CLavaPEDoc* myDoc;
  QComboBox *List;
  LavaDECL * myDECL;
  LavaDECL * ContextDECL;
  LavaDECL * FormDECL;
  ~CExecLike() {}
};


#endif
