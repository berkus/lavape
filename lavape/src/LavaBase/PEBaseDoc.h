
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
  bool changeNothing;
  CLavaBaseView *MainView;
  CLavaBaseView* DragView;
  DebugContData* ContinueData;
  int debugINCL;

  virtual void Serialize(QDataStream& ar);
  virtual bool OnSaveDocument(const QString& lpszPathName);
  virtual bool OnCloseDocument();
  CLavaBaseData* GetLBaseData();
  virtual void ResetVElems(LavaDECL* ,int) {}
  virtual void ExecViewPrivToPub(LavaDECL* , int) {}
  virtual int ReadSynDef(const QString& fn, SynDef* &sntx, ASN1* cid = 0);//fn has all links resolved
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
  virtual CHE* GetExecChe(LavaDECL* parentDecl,TDeclType type,bool makeIt=true) {return 0;}
  virtual CHE* SetExecChe(LavaDECL* parentDecl,LavaDECL* execDecl) {return 0;}
  virtual bool OpenExecView(LavaDECL* execDECL) {return false;}
  virtual void OnCloseLastExecView() {}
  virtual void SetExecBarText(CSearchData& /*sData*/) {}
  virtual void SetPEError(const CHAINX& /*ErrChain*/, bool /*andShow*/) {}
  virtual void ResetError() {}
  virtual bool ErrorBarVisible() {return false;}
  virtual void OnShowError() {}
  virtual wxDocument* FindOpenDoc(const QString& /*fn*/) {return 0;}
  virtual void UpdateMoveInDocs(const DString& /*clipDoc*/) {}
  virtual void UpdateOtherDocs(wxDocument* /*notOther*/, DString& /*inclFile*/, int ,bool, SynFlags flags=SETpp()) {}
  virtual int MakeFunc(LavaDECL* decl, bool otherDoc, QWidget* parent) {return -1;}
  void SetSelectDECL(CHE* elRemoved);
  bool InSection(CHE* el);
  virtual void IncludeHint(const QString& fullfn, CHESimpleSyntax* cheSyn);
  virtual void OnDebugLava() {}
private:
    Q_OBJECT

};

extern LAVABASE_DLL void SortCombo(QComboBox* lbox);

#endif
