// LavaPEDoc.h : interface of the CLavaPEDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LavaPEDoc_H__BC8733E1_2301_11D3_B7AF_000000000000__INCLUDED_)
#define AFX_LavaPEDoc_H__BC8733E1_2301_11D3_B7AF_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#include "SynIO.h"
#include "Syntax.h"
#include "SylTraversal.h"
#include "PEBaseDoc.h"
//#include "wxExport.h"
#include "qcombobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"


class CLavaPEDoc : public CPEBaseDoc
{
public:
  FACTORY(CLavaPEDoc)
  CLavaPEDoc();
  virtual ~CLavaPEDoc();
  virtual bool OnNewDocument();
  virtual bool OnOpenDocument(const QString& filename);
  wxDocument* FindOpenDoc(const DString& fn);

  DString GetTypeLabel(LavaDECL* elDef, bool goDown); 
  DString GetIOLabel(TAnnotation *anno);

  bool openFView(LavaDECL** pdecl);
  virtual LavaDECL* GetConstrDECL(LavaDECL* parentDecl,TDeclType type,bool makeDecl=true,bool makeExec=true);
  virtual CHE* GetConstrChe(LavaDECL* parentDecl,TDeclType type,bool makeIt=true);
  virtual CHE* SetConstrChe(LavaDECL* parentDecl,LavaDECL* execDecl);
  virtual bool OpenCView(LavaDECL* constrdecl);
  virtual void OnCloseLastExecView();
  bool openVTView(LavaDECL** pdecl, unsigned long autoUpdate);
  void SetTreeItemImage(LavaDECL* errDECL, bool hasErr);
  virtual void SetExecItemImage(LavaDECL* execDECL, bool empty, bool hasErrors);
  virtual void SetExecBarText(CSearchData& sData);
  bool openWizardView(CLavaBaseView* formView, LavaDECL** pdecl/*, unsigned long autoUpdate*/);

  bool CollectPattern(LavaDECL *paramDECL, LavaDECL* collectDECL);
  void ConcernForms(CLavaPEHint* hint);
  void ConcernImpls(CLavaPEHint* hint, LavaDECL* ppDECL, bool undo=false);
  void ConcernExecs(CLavaPEHint* hint);
  void UpdateMoveInDocs(const DString& clipDoc);
  void UpdateOtherDocs(wxDocument* skipOther, DString& inclFile, int newINCL, bool synDel, SynFlags flag=SETpp());
  bool IsSpecialOf(LavaDECL* decl, const TIDs& basicIDs, bool cDeriv=false);
                              //return : true :  id is an ancestor of upId
                              //         false : id is not an ancestor of upId
  virtual int MakeFunc(LavaDECL* decl, bool otherDoc, QWidget* parent);
  void MakeIniFunc(LavaDECL* ifDECL);
  LavaDECL* MakeSet(const TID& setID, LavaDECL* decl=0);
  void MakeOperator(LavaDECL* opDecl);
  virtual bool GetOperatorID(LavaDECL* decl, TOperator op, TID& ElID);
  virtual bool getOperatorID(LavaDECL* decl, TOperator op, TID& ElID);

  virtual bool MakeFormVT(LavaDECL *decl, CheckData* pckd = 0);
  virtual bool MakeVElems(LavaDECL *decl, CheckData* pckd = 0);
  void ResetVElems(LavaDECL *decl);
  bool CheckForm(LavaDECL* formDECL, int checkLevel);
  bool CheckImpl(LavaDECL* implDECL, int checkLevel=0);
  bool CheckOverInOut(LavaDECL* funcDECL, int checkLevel);

  virtual void FindReferences(DString& allNames, CFindData& fw);
  TID FindUIReference(LavaDECL* decl, CFindData& fw);
  void DownFind(LavaDECL* inDecl, CFindData& fw); //const DString& absFileNameconst, const TID& refTid, const DString& enumID, const CFindWhat& fw);
  void SetFindText(LavaDECL* inDecl, CFindData& fw);
  bool TrueReference(LavaDECL* decl, int refCase,const TID& refTid);
  void SetNameText(LavaDECL* inDecl, CFindData& fw);

  virtual void Modify(bool bModified);
  void ShowErrorBox(bool inOpen);
  virtual void SetPEError(const CHAINX& ErrChain, bool andShow);
  virtual void ResetError();
  virtual bool ErrorBarVisible();
  void AutoCorr(LavaDECL* decl);
  int AutoCorrBox(QString* errID);
  void SetLastHints(bool fromDragDrop, bool otherDocs);

  void OnRunLava();
  void OnUpdateRunLava(wxAction* action);
  void OnCheck();
  void OnTotalCheck();

  int nErrors;
  bool hasErrorsInTree;
  int nPlaceholders;
  bool hasHint;
  bool drawView;
  bool changeInUpdate;

protected: 
  bool openInTotalCheck;
  bool checkedInTotalCheck;

  bool CheckFuncImpl(LavaDECL* funcDECL, int checkLevel, bool& changed);
  LavaDECL* MakeOneSetGet(TypeFlag setgetflag, LavaDECL* implDECL,
                                     LavaDECL* propDecl, int checkLevel);
  bool OnEmptyDoc(const DString& Name);
  bool CollectP(const TIDs& paramIDs, LavaDECL* collectDECL);
  void ShrinkCollectDECL(LavaDECL* decl);
  bool collectPattern(LavaDECL *decl, const TIDs& paramIDs, const TIDs& refIDs);
  bool AddVElems(LavaDECL *decl, LavaDECL* baseDECL);
  bool AddVBase(LavaDECL *decl, LavaDECL* conDECL);
  bool MakeSetAndGets(LavaDECL* implDECL, LavaDECL* iFaceDECL, int checkLevel);
  bool CheckMenu(LavaDECL* formDECL, LavaDECL* classDECL);
  void OnUpdateFilePrint(wxAction* action);
private:
    Q_OBJECT;
};

#define FIRSTLAST(doc, firstlast) \
  if (((CLavaPEDoc*)doc)->hasHint) firstlast.EXCL(firstHint); \
  else firstlast.INCL(firstHint); \
  ((CLavaPEDoc*)doc)->hasHint = true

#endif // !defined(AFX_LavaPEDoc_H__BC8733E1_2301_11D3_B7AF_000000000000__INCLUDED_)
