// ComboBar.h : header file
//

#ifndef __ComboBar
#define __ComboBar

#include "qcombobox.h"
#include "qpushbutton.h"
#include "qtoolbar.h"
#include "qpopupmenu.h"
#include "qstring.h"
#include "qpainter.h"
#include "SylTraversal.h"
#include "PEBaseDoc.h"
#include "Constructs.h"
#include "ComboBarDlg.h"
#include "CHAINANY.h"
//#include "Resource.h"


extern LAVAEXECS_DLL void ResetComboItems(QComboBox* box);

class CFieldsItem: public CListItem {
public:
  CFieldsItem(const DString& text) {setText(QString(text.c)); withClass = false;}
  CFieldsItem(CFieldsItem* item)
    {setText(item->text()); withClass = false; QName = item->QName; IDs = item->IDs;}
  DString QName;
  TDODC IDs;
  bool withClass;
};

class CStatFuncItem: public CListItem {
public:
  TID funcID;
  TID vtypeID;

  CStatFuncItem(const DString& text, int fID, int fINCL)
   {setText(QString(text.c)); funcID.nID = fID; funcID.nINCL = fINCL;}
  CStatFuncItem(const DString& text, int fID, int fINCL, int vID, int vINCL)
   {setText(QString(text.c)); funcID.nID = fID; funcID.nINCL = fINCL; vtypeID.nID = vID; vtypeID.nINCL = vINCL;}
};

enum TShowCombo {
  disableCombo,
  objCombo,
  objEnumCombo,
  objSetEnumCombo,
  objSetCombo,
  typeCombo,
  setTypeCombo,
  newCombo,        //Initializers only
  newAndCObjCombo, //Initializers and component object specs
  attachCombo,     //persistent component object specification 
  coiCombo,        //component object interfaces
  callCombo,       //Inititors
  callbackCombo,
  invalidateLast
};

/////////////////////////////////////////////////////////////////////////////
// CComboBar dialog

#undef QMainWindow
class LAVAEXECS_DLL CComboBar : public QDockWindow//QToolBar
{
public:
  CComboBar();   // standard constructor
  CComboBar(LavaDECL* execDecl, CPEBaseDoc* doc, QMainWindow* parent);  
  ~CComboBar() {}
  virtual QSize sizeHint() const {return m_ComboBarDlg->size();}
  void DeleteObjData(bool setCombo = true);
  LavaDECL *ExecDECL;
  LavaDECL *myDECL; //parent of exec
  LavaDECL *SelfTypeDECL;  //Implementation if myDECL function
  LavaDECL *IntfDECL;
  LavaDECL *FuncParentDecl;
  CPEBaseDoc *myDoc;
  bool lastOK;
  TShowCombo lastCombo;
  QString lastSelStr;
  TID lastSelTID;
  bool onShowStatic; //select an interface and then a static function defined in this interface

//  bool Create(QWidget* insertAfter, QWidget* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID );
  void ShowCombos(TShowCombo what, TID* pID=0); 
  void ShowSubObjects(LavaDECL* decl, const CContext &context);  //decl is type or field
  void ShowClassFuncs(CheckData &ckd, LavaDECL *decl, LavaDECL* eventDescDecl, const CContext &callCtx, bool withStatic=false);  //decl is interface or implementation
  void ShowStaticFuncs(CheckData &ckd); 
  void showIFFuncs();
  void ShowBaseInis(const TID& id);  //decl is interface or implementation
//  void ShowUpcasts(const TID& id, bool theTop = true);     //id is interface or implementation
  void ShowCompObjects(CheckData &ckd, LavaDECL *decl, const CContext &context, Category givenCat, bool forInput, bool forCopy=false); //compatible objects, iC is type
  void ShowCompaTypes(CheckData &ckd, LavaDECL *decl, const CContext &context); //compatible types
  void FuncsInSupports(CheckData &ckd, LavaDECL *decl, LavaDECL* conDECL, LavaDECL* vbaseDECL, LavaDECL* eventDescDecl, const CContext &evDescCtx, const CContext &callCtx, bool withStatic);
  bool IsInBox(QComboBox* combo, const DString& name, int id, int incl, bool& sameName);
  void RemoveLocals();
  void AddLocal(const TID& id, const DString& name, const TID& typeID, bool subst=false);
  void OnUpdate(LavaDECL *execDecl, bool externalHint);
  bool UsedName(const DString& name);
  void SetCombos(bool setVar, bool hideCombo);
  void TrackEnum();

  ComboBarDlg* m_ComboBarDlg;
  //static boxes
  QComboBox*  m_TypesCtrl;        
  QComboBox*  m_SetTypesCtrl;  
  QComboBox*  m_CallbackCtrl;   
  QComboBox*  m_BasicTypesCtrl;  
  QComboBox*  m_EnumsCtrl;        //Enumeration types, right
  QComboBox*  m_NewCtrl;          //creatable interfaces, left
  QComboBox*  m_SNewCtrl;         //initiator, component object specifications and, right
  QComboBox*  m_ObjectsCtrl;      //all objects, left
  QComboBox*  m_SetObjectsCtrl;   //all set objects, left
  QComboBox*  m_AttachCtrl;       //persistent component object specifications, left
  QComboBox*  m_CallIntCtrl;      //callable entities, left
  QComboBox*  m_CompoObjIntCtrl;  //component object interfaces, left
  //dynamic boxes
  QComboBox*  m_ClassFuncsCtrl;   //class functions, right
  QComboBox*  m_VFuncsCtrl;       //virtual functions, left
  QComboBox*  m_SubObjectsCtrl;   //subobjects, left
  QComboBox*  m_CompaObjectsCtrl; //compatible objects, left
  QComboBox*  m_BaseInisCtrl;     //initializer function of base classes, left
  QComboBox*  m_StaticFuncsCtrl;  //static functions of a selected interface, right
  QComboBox*  m_CompaTypesCtrl;   //compatible types     
  QComboBox*  m_CompaBTypesCtrl;  //compatible basic types

  QPushButton *m_NewFunc;
  QPushButton *m_NewPFunc;
  QPushButton *m_ButtonEnum;
  QPixmap BitmapEnums;
  QPixmap BmNewCFunc;
  QPixmap BmNewPFunc;
  QPopupMenu EnumMenu;

  enum combosEnum {v_Types, v_SetTypes, v_Callback, v_BasicTypes, v_Enums, v_New, v_SNew,
        v_Objects, v_SetObjects, v_Attach, v_CallInt, v_CompoObjInt, v_ClassFuncs,
        v_VFuncs, v_SubObjects, v_CompaObjects, v_BaseInis, v_StaticFuncs, v_CompaTypes,
        v_CompaBTypes, hcombosEnum};

  bool VisibleList [hcombosEnum];
  QComboBox*  LeftCombo;
  QComboBox*  RightCombo;
  QComboBox*  ThirdCombo;

  bool EnumsEnable;
  bool EnumsShow;
  bool NewFuncEnable;
  bool NewFuncShow;
  bool NewPFuncEnable;


//  bool SetToolText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );

protected:

/*
	void OnCloseupBasicTypes();
	void OnCloseupCompaBTypes();
	void OnCloseupComboAttach();
	void OnCloseupComboBaseInis();
	void OnCloseupCOMBOCall();
	void OnCloseupCOMBOCallback();
	void OnCloseupComboClassFuncs();
	void OnCloseupComboCompObjects();
	void OnCloseupComboEnums();
	void OnCloseupComboFuncs();
	void OnCloseupComboNew();
	void OnCloseupComboObjects();
	void OnCloseupComboSetObjects();
	void OnCloseupComboSetTypes();
	void OnCloseupComboSNew();
	void OnCloseupComboSubObjects();
	void OnCloseupComboTypes();
	void OnCloseupCompaTypes();
	void OnCloseupCompoInterf();
	void OnCloseupStaticFuncs();
*/
//  void OnSelendokComboStatFuncs();

public slots:
  void OnEnumMenu(int id);
  void OnSelendokComboObjects(int pos);
  void OnSelendokComboSetObjects(int pos);
  void OnSelendokComboCompObjects(int pos);
  void OnSelendokComboSubObjects(int pos);
  void OnSelendokComboTypes(int pos);
  void OnSelendokCompaTypes(int pos);
  void OnSelendokComboFuncs(int pos);
  void OnSelendokComboBaseInis(int pos);
  void OnSelendokBasicTypes(int pos);
  void OnSelendokCompaBTypes(int pos);
  void OnSelendokComboClassFuncs(int pos);
  void OnSelendokComboNew(int pos);
  void OnSelendokComboSNew(int pos);
  void OnSelendokComboAttach(int pos);
  void OnSelendokComboEnums(int pos);
  void OnSelendokComboSetTypes(int pos);
  void OnSelendokCompoInterf(int pos);
  void OnSelendokCOMBOCall(int pos);
  void OnSelendokCOMBOCallback(int pos);
  void OnSelendokStaticFuncs(int pos);

  void OnButtonEnum();
  void OnNewFunc();
  void OnNewPFunc();

private:
  Q_OBJECT
};


class CExecTypes: public CExec
{ //fill type and function boxes
public:
  CExecTypes(CComboBar* bar);
  void ExecDefs (LavaDECL ** pelDef, int level);
//  void ExecSHOW (LavaDECL ** pelDef, int level);
  CComboBar* Bar;
  QSize sz;
  QPainter *dc;
  int maxTW;
  int maxSTW;
  int maxIW;
  int maxSIW;
  int maxEnuW;
  int maxAtW;
  int maxCallW;
  int maxCOIW;
  int maxCBW;
  ~CExecTypes() {}
};


class  CExecFields
{ //fill object boxes
public:
  CExecFields(CComboBar* bar, LavaDECL* startDECL, const CContext &context,
              bool subO = false,  DString* startName = 0, TDODC* startIDs = 0);

  void AddToBox (LavaDECL** pdecl, DString& name, QComboBox* fieldList);
  bool SameField(TDODC& IDs1, TDODC& IDs2, bool& replace);
  void SetWidth(QComboBox* list, int wmax);
  CComboBar* Bar;
  QComboBox *FieldList;
  QSize sz;
  QPainter *dc;
  int maxW;
  int maxSetW;
  TDODC *pAccuIDs; //the accumulated chain of IDs
  void OnField(LavaDECL **pdecl, DString accuName, TDODC accuIDs, TIDs typeIDs, const CContext &context);
  void OnType(LavaDECL *decl, DString accuName, TDODC accuIDs, TIDs typeIDs, const CContext &context);
  void OnSupports(LavaDECL *decl, DString accuName, TDODC accuIDs, TIDs typeIDs, const CContext &context);
  ~CExecFields() {}
};


#endif
