
#ifdef __GNUC__
#pragma interface
#endif

#ifndef __CPEBaseDoc
#define __CPEBaseDoc


#include "SynIO.h"
#include "Syntax.h"
#include "LavaAppBase.h"
#include "LavaBaseDoc.h"
#include "SylTraversal.h"
#include "SynIDTable.h"
#include "qcstring.h"
//#include "wxExport.h"
#include "qcombobox.h"
#include "docview.h"
#include "qobject.h"
#include "qstring.h"
#include "qlistbox.h"

class CListItem: public QListBoxText
{
public:
  CListItem(const QString& text, const TID& id) {setText(text); ID = id;}
  CListItem(const DString& text, const TID& id) {setText(QString(text.c)); ID = id;}
  CListItem(char* text, const TID& id) {setText(QString(text)); ID = id;}
  CListItem(const DString& text, unsigned* flags) {setText(QString(text.c)); Flags = flags;}
  CListItem(const QString& text, unsigned* flags) {setText(text); Flags = flags;}
  CListItem() {}
  TID itemData() {return ID;}
  unsigned* flags() {return Flags;}
  virtual void setText(const QString& text) {QListBoxText::setText(text);}
protected:
  TID ID;
  unsigned* Flags;
};

class CLavaBaseView;
class LAVABASE_DLL CPEBaseDoc : public CLavaBaseDoc
{
protected: 
  CPEBaseDoc();
public:
  virtual ~CPEBaseDoc();

  CUndoMem UndoMem;
  bool modified;
  int UpdateNo;
  bool asked;
  bool isReadOnly;
  CLavaBaseView *MainView;
  CLavaBaseView* DragView;

  virtual void Serialize(QDataStream& ar);
  virtual bool OnSaveDocument(const QString& lpszPathName);
  virtual bool OnCloseDocument();
  CLavaBaseData* GetLBaseData();
  virtual void ResetVElems(LavaDECL* ) {}
  virtual int ReadSynDef(DString& fn, SynDef* &sntx, ASN1* cid = 0);//fn has all links resolved
  virtual void SetExecItemImage(LavaDECL* /*execDECL*/, bool /*empty*/, bool /*hasErrors*/) {}
  void MakeBasicBox(QComboBox* cbox, TDeclType defType, bool with, bool skipServices = false); //with: with B_Object and B_Service
  bool Step(CLavaPEHint* hint, LavaDECL* parDECL, CHE*& relElem);
  CLavaPEHint* InsDelDECL(CLavaPEHint* hint, bool undo, bool redo/*, CHE *elRemoved*/);
  void ChangeDECL(CLavaPEHint* hint, bool undo);
  void UndoDelSyntax(CLavaPEHint* hint);
  virtual void DelSyntax(CHESimpleSyntax* delSyn);
  bool UpdateDoc(CLavaBaseView *Sender, bool undo, CLavaPEHint* doHint = 0, bool redo = FALSE);
  void SetLastHint(bool otherDocs = true);
  void OnDestroyMainView(CLavaBaseView* mview);
  bool Undo(bool redo = FALSE);
  virtual bool OnSaveModified();
  virtual LavaDECL* GetConstrDECL(LavaDECL* decl,bool makeExec) {return 0;}
  virtual bool OpenCView(LavaDECL* /*execDECL*/) {return false;}
  virtual void OnCloseLastExecView() {}
  virtual void SetExecBarText(CSearchData& /*sData*/) {}
  virtual void SetPEError(const CHAINX& /*ErrChain*/, bool /*andShow*/) {}
  virtual void ResetError() {}
  virtual bool ErrorBarVisible() {return false;}
  virtual void OnShowError() {}
  virtual wxDocument* FindOpenDoc(const DString& /*fn*/) {return 0;}
  virtual void UpdateMoveInDocs(const DString& /*clipDoc*/) {}
  virtual void UpdateOtherDocs(wxDocument* /*notOther*/, DString& /*inclFile*/, int ,bool, SynFlags flags=SETpp()) {}
  virtual int MakeFunc(LavaDECL* decl, bool otherDoc) {return -1;}
  void SetSelectDECL(CHE* elRemoved);
  bool InSection(CHE* el);
  virtual void IncludeHint(const DString& fullfn, CHESimpleSyntax* cheSyn);
private:
    Q_OBJECT

};

extern LAVABASE_DLL void SortCombo(QComboBox* lbox);

class LAVABASE_DLL CLavaBaseView : public wxView
{
public:
//  VIEWFACTORY(CLavaBaseView)
  CLavaBaseView(QWidget *parent,wxDocument *doc, const char*  name): wxView(parent,doc,name) {}
  virtual void OnEditCut() {}
  virtual void OnEditCopy(){}
  virtual void OnEditPaste(){}
  virtual void OnFindreferences() {}
  virtual void OnExpandAll() {}
  virtual void OnCollapsAll() {}
  virtual void OnShowOptionals() {}
  virtual void OnShowAllEmptyOpt() {}
  virtual void OnRemoveAllEmptyOpt() {}
  virtual void OnNextComment() {}
  virtual void OnPrevComment() {}
  virtual void OnNextError() {}
  virtual void OnPrevError() {}
  virtual void OnNewInclude() {}
  virtual void OnNewNamespace() {}
  virtual void OnNewinitiator() {}
  virtual void OnNewclassintf() {}
  virtual void OnNewclassimpl() {}
  virtual void OnCSpec() {}
  virtual void OnComponent() {}
  virtual void OnNewset() {}
  virtual void OnNewenum() {}
  virtual void OnNewVirtualType() {}
  virtual void OnNewfunction() {}
  virtual void OnNewmember() {}
  virtual void OnNewEnumItem() {}
  virtual void OnEditSel() {}
  virtual void OnComment() {}
  virtual void OnGotodef() {}
  virtual void OnGotoImpl() {}
  virtual void OnShowOverridables() {}
  virtual void OnShowformview() {}
  virtual void OnLitStr() {}  
  virtual void OnInsertOpt() {}  
  virtual void OnDeleteOpt() {}  
  virtual void OnOK() {}
  virtual void OnCancel() {}
	virtual void OnTogglestate() {}
  virtual void OnDelete() {}
  virtual void OnOverride() {}
  virtual void OnGenHtml() {}
  virtual void OnGenHtmlS() {}

  virtual void OnAnd() {}
  virtual void OnBitAnd() {}
  virtual void OnBitOr() {}
  virtual void OnBitXor() {}
  virtual void OnClone() {}
  virtual void OnSelect() {}
//  virtual void OnDelete() {}
  virtual void OnDivide() {}
//  virtual void OnEditCut() {}
//  virtual void OnEditCopy() {}
//  virtual void OnEditPaste() {}
  virtual void OnEditUndo() {}
  virtual void OnEditRedo() {}
  virtual void OnEq() {}
  virtual void OnDeclare() {}
  virtual void OnForeach() {}
  virtual void OnFunctionCall() {}
  virtual void OnGe() {}
  virtual void OnGt() {}
  virtual void OnIf() {}
  virtual void OnIfExpr() {}
  virtual void OnIn() {}
  virtual void OnInsert() {}
  virtual void OnInsertBefore() {}
  virtual void OnInvert() {}
//  virtual void OnLButtonDblClk(UINT nFlags, QPoint point);
  virtual void OnLButtonDown(QMouseEvent *e) {}
  virtual void OnLe() {}
  virtual void OnLshift() {}
  virtual void OnLt() {}
  virtual void OnPlusMinus() {}
  virtual void OnMult() {}
  virtual void OnNe() {}
  virtual void OnShowComments() {}
  virtual void OnNot() {}
  virtual void OnNull() {}
  virtual void OnOr() {}
  virtual void OnPlus() {}
  virtual void OnRshift() {}
  virtual void OnSwitch() {}
  virtual void OnXor() {}
//  virtual void OnEditSel() {}
//  virtual void OnComment() {}
  virtual void OnToggleArrows() {}
  virtual void OnNewLine() {}
//  virtual void OnShowOptionals() {}
//  virtual void OnGotodef() {}
//  virtual void OnGotoImpl() {}
  virtual void OnFindReferences() {}
  virtual void OnModulus() {}
  virtual void OnIgnore() {}
  virtual void OnAttach() {}
  virtual void OnQueryItf() {}
  virtual void OnStaticCall() {}
  virtual void OnAssign() {}
  virtual void OnCallback() {}
  virtual void OnTypeSwitch() {}
  virtual void OnInterval() {}
  virtual void OnUuid() {}
  virtual void OnCall() {}
  virtual void OnExists() {}
  virtual void OnCreateObject() {}
  virtual void OnAssert() {}
//  virtual void OnNextError() {}
//  virtual void OnPrevError() {}
//  virtual void OnNextComment() {}
//  virtual void OnPrevComment() {}
  virtual void OnConflict() {}
  virtual void OnRefresh() {}
  virtual void OnToggleCategory() {}
  virtual void OnToggleSubstitutable() {}
  virtual void OnCopy() {}
  virtual void OnEvaluate() {}
  virtual void OnInputArrow() {}
//  virtual void OnSetFocus(QWidget* pOldWnd);
//  virtual void OnKillFocus(QWidget* pNewWnd);
  virtual void OnHandle() {}
//  virtual void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnOptLocalVar() {}
	virtual void OnItem() {}
	virtual void OnQua() {}
	virtual void OnSucceed() {}
	virtual void OnFail() {}
	virtual void OnTrue() {}
	virtual void OnFalse() {}
	virtual void OnOrd() {}
	virtual void OnTryStatement() {}

private:
    Q_OBJECT
};

#endif
