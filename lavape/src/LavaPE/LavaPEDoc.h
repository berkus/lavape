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
  int nErrors;
  bool hasErrorsInTree;
  int nPlaceholders;
  bool hasHint;
  bool drawView;
  bool changeInUpdate;

  void AutoCorr(LavaDECL* decl);
  int AutoCorrBox(QString* errID);
  bool CheckForm(LavaDECL* formDECL, int checkLevel);
  bool CheckImpl(LavaDECL* implDECL, int checkLevel=0);
  bool CheckOverInOut(LavaDECL* funcDECL, int checkLevel);
  bool CollectPattern(LavaDECL *paramDECL, LavaDECL* collectDECL);
  void ConcernExecs(CLavaPEHint* hint);
  void ConcernForms(CLavaPEHint* hint);
  void ConcernImpls(CLavaPEHint* hint, LavaDECL* ppDECL, bool undo=false);
  void DownFind(LavaDECL* inDecl, CFindData& fw); //const DString& absFileNameconst, const TID& refTid, const DString& enumID, const CFindWhat& fw);
  virtual bool ErrorBarVisible();
  virtual void ExecViewPrivToPub(LavaDECL* func, int delID);
  void SetCom8();
  wxDocument* FindOpenDoc(const DString& fn);
  virtual void FindReferences(DString& allNames, CFindData& fw);
  TID FindUIReference(LavaDECL* decl, CFindData& fw);
  virtual CHE* GetExecChe(LavaDECL* parentDecl,TDeclType type,bool makeIt=true);
  DString GetIOLabel(TAnnotation *anno);
  virtual bool GetOperatorID(LavaDECL* decl, TOperator op, TID& ElID);
  virtual bool getOperatorID(LavaDECL* decl, TOperator op, TID& ElID);
  DString GetTypeLabel(LavaDECL* elDef, bool goDown); 
  bool IsSpecialOf(LavaDECL* decl, const TIDs& basicIDs, bool cDeriv=false);
                              //return : true :  id is an ancestor of upId
                              //         false : id is not an ancestor of upId
  virtual bool MakeFormVT(LavaDECL *decl, CheckData* pckd = 0);
  virtual int MakeFunc(LavaDECL* decl, bool otherDoc, QWidget* parent);
  LavaDECL* MakeGUI(LavaDECL* relDECL, LavaDECL** pparent, int& pos, LavaDECL* guiInterface=0);
  void MakeGUIFuncs(LavaDECL* guiInterface);
  void MakeIniFunc(LavaDECL* ifDECL);
  void MakeOperator(LavaDECL* opDecl);
  LavaDECL* MakeSet(const TID& setID, LavaDECL* decl=0);
  virtual bool MakeVElems(LavaDECL *decl, CheckData* pckd = 0);
  virtual void Modify(bool bModified);
  void OnCheck();
  virtual void OnCloseLastExecView();
  virtual bool OnNewDocument();
  virtual bool OnOpenDocument(const QString& filename);
  void OnRunLava();
  void OnTotalCheck();
  void OnUpdateRunLava(wxAction* action);
  bool OpenGUIView(LavaDECL** pdecl);
  bool OpenVTView(LavaDECL** pdecl, unsigned long autoUpdate);
  virtual bool OpenExecView(LavaDECL* execDecl);
  bool OpenWizardView(CLavaBaseView* formView, LavaDECL** pdecl/*, unsigned long autoUpdate*/);
  virtual void ResetError();
  void ResetVElems(LavaDECL *decl);

  virtual void SetExecBarText(CSearchData& sData);
  virtual CHE* SetExecChe(LavaDECL* parentDecl,LavaDECL* execDecl);
  virtual void SetExecItemImage(LavaDECL* execDECL, bool empty, bool hasErrors);
  void SetFindText(LavaDECL* inDecl, CFindData& fw);
  void SetLastHints(bool fromDragDrop, bool otherDocs);
  void SetNameText(LavaDECL* inDecl, CFindData& fw);
  virtual void SetPEError(const CHAINX& ErrChain, bool andShow);
  void SetTreeItemImage(LavaDECL* errDECL, bool hasErr);
  void ShowErrorBox(bool inOpen);
  bool TrueReference(LavaDECL* decl, int refCase,const TID& refTid);

  void UpdateMoveInDocs(const DString& clipDoc);
  void UpdateOtherDocs(wxDocument* skipOther, DString& inclFile, int newINCL, bool synDel, SynFlags flag=SETpp());

protected: 
  bool AddVElems(LavaDECL *decl, LavaDECL* baseDECL);
  bool AddVBase(LavaDECL *decl, LavaDECL* conDECL);
  bool checkedInTotalCheck;
  bool CheckFuncImpl(LavaDECL* funcDECL, int checkLevel, bool& changed);
  bool CheckMenu(LavaDECL* formDECL, LavaDECL* classDECL);
  bool CollectP(const TIDs& paramIDs, LavaDECL* collectDECL);
  bool collectPattern(LavaDECL *decl, const TIDs& paramIDs, const TIDs& refIDs);

  LavaDECL* MakeOneSetGet(TypeFlag setgetflag, LavaDECL* implDECL,
                                     LavaDECL* propDecl, int checkLevel);
  bool MakeSetAndGets(LavaDECL* implDECL, LavaDECL* iFaceDECL, int checkLevel);
  bool OnEmptyDoc(const DString& Name);
  void OnUpdateFilePrint(wxAction* action);
  bool openInTotalCheck;
  void ShrinkCollectDECL(LavaDECL* decl);

private:
    Q_OBJECT;
};



#define FIRSTLAST(doc, firstlast) \
  if (((CLavaPEDoc*)doc)->hasHint) firstlast.EXCL(firstHint); \
  else firstlast.INCL(firstHint); \
  ((CLavaPEDoc*)doc)->hasHint = true

#endif // !defined(AFX_LavaPEDoc_H__BC8733E1_2301_11D3_B7AF_000000000000__INCLUDED_)
