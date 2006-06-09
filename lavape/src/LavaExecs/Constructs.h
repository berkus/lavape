/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
	 (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#ifndef __Constructs
#define __Constructs



#include "SYSTEM.h"
#include "AnyType.h"

#include "ASN1pp.h"
#include "CHAINANY.h"
#include "NESTANY.h"
#include "SETpp.h"
#include "STR.h"

#include "Tokens.h"

#include "Syntax.h"


#include "qwidget.h"
#include <QScrollArea>
#include "qstring.h"
#include "qmessagebox.h"
#include "qpainter.h"
#include <QList>
#include <QHash>
#include "LavaBaseDoc.h"
#include "SynIDTable.h"
#include "wx_obj.h"

#ifdef WIN32
#ifdef LAVAEXECS_EXPORT
#define LAVAEXECS_DLL __declspec( dllexport )
#else
#define LAVAEXECS_DLL __declspec( dllimport )
#endif
#else
#define LAVAEXECS_DLL
#endif


#define LEFTMARGIN 17
#define INDENT 3

#ifdef EXECVIEW
#undef IMPLEMENT_DYNAMIC_CLASS
// Single inheritance with one base class
#define IMPLEMENT_DYNAMIC_CLASS(name, basename) \
wxObject *wxConstructorFor##name(void) \
   { return new name##V; }\
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), wxConstructorFor##name);
#endif

#ifdef INTERPRETER
#undef IMPLEMENT_DYNAMIC_CLASS
// Single inheritance with one base class
#define IMPLEMENT_DYNAMIC_CLASS(name, basename) \
wxObject *wxConstructorFor##name(void) \
   { return new name##X; }\
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), wxConstructorFor##name);
#endif

extern TToken OperandType (TToken primaryToken);
extern TToken PlaceHolderType (TToken primaryToken);


enum ContextFlags {
  InLogNot,
  InXor,
  InIfCond,
  InForEach,
  InBranch,
  InBut
};


enum TargetType {
  field,
  property,
  arrayElem
};

enum VarRefContext {
  copyTarget,
  assignmentTarget,
  funcHandle,
  arrayTarget,
  inputParam,
  funcOutput
};

enum ROContext {
  noROContext,
  roExec,
  assertion,
  roClause
};


class SynObject;
class ObjReference;
class VarName;
class SelfVar;


#ifdef INTERPRETER

class RTAssertionData { // run time assertion data
public:
  unsigned oldExprLevel;
  bool requireChecked, requireOK, ensureChecked, ensureOK, invariantChecked;

  RTAssertionData(unsigned oel) {
    oldExprLevel = oel;
    requireChecked = false;
    ensureChecked = false;
    invariantChecked = false;
  }
};

class AssertionData;
typedef QHash<AssertionData*,RTAssertionData*> RTAssDataDict;

class AssertionData : public AnyType { // check time assertion data
public:
  unsigned maxFrameSize, stackFrameSize, nOwnOldExpr, nOvrOldExpr, nTotalOldExpr;
  bool hasOrInheritsPreconditions, hasOrInheritsPostconditions, hasOrInheritsInvariants;
  LavaDECL *funcDECL, *requireDECL, *ensureDECL, *requireDECLimpl, *ensureDECLimpl,
           *invariantDECL, *invariantDECLimpl;
  QList<AssertionData*> overridden;
  QList<class OldExpression*> oldExpressions; // pointers to OldExpression for eval.on method entry

  AssertionData(CheckData &ckd, LavaDECL *funcDECL);

  unsigned PrepareAssertions(CheckData &ckd, SelfVar *selfVar);
  bool EvalOldExpressions (CheckData &ckd, RTAssDataDict &rtadDict, LavaVariablePtr stackFrame, unsigned &oldExprLevel);
  bool EvalPreConditions (CheckData &ckd, RTAssDataDict &rtadDict, LavaVariablePtr stackFrame, AssertionData *parent, bool parentOK);
  bool EvalPostConditions (CheckData &ckd, RTAssDataDict &rtadDict, LavaVariablePtr stackFrame, AssertionData *parent);
};

class InvarData;
typedef QHash<InvarData*,LavaDECL*> RTInvDataDict;
// run time invariant dict;
// is used only to recognize if an ivariant has already been checked

class InvarData : public AnyType { // check time invariant data
public:
  unsigned maxFrameSize, stackFrameSize;
  LavaDECL *itfDECL, *invariantDECL, *invariantDECLimpl;
  bool hasOrInheritsInvariants;
  QList<InvarData*> overridden;

  InvarData(CheckData &ckd, LavaDECL *itfDECL);

  unsigned PrepareInvariants(CheckData &ckd, SelfVar *selfVar);
  bool EvalInvariants (CheckData &ckd, RTInvDataDict &rtInvDict, LavaVariablePtr stackFrame);
};

#endif



class VarAction : public AnyType  {

  public:
  CLavaBaseDoc *doc;
  unsigned tempNo;
  bool inIgnore, inExecHeader, inFormParms, inBaseInits, inParameter, inForeach;

  VarAction();
  virtual bool Action(CheckData &ckd,VarName *vn,TID &typeTid) =0;
  virtual void CheckLocalScope(CheckData &ckd,SynObject *synObj);

  virtual void CopyData (AnyType *from) {
    *this = *(VarAction*)from;
  }
};

class LocalVarSearch : public VarAction {

  public:
  TID *toBeFound;
  bool found;

  LocalVarSearch(CLavaBaseDoc *d,TID *tid);
  virtual bool Action(CheckData &ckd,VarName *vn,TID &tid);

  virtual void CopyData (AnyType *from) {
    *this = *(LocalVarSearch*)from;
  }
};

class LocalVarByNameSearch : public VarAction {

  public:
  STRING toBeFound;
  VarName *varName;
  bool found;

  LocalVarByNameSearch(STRING name);
  virtual bool Action(CheckData &ckd,VarName *vn,TID &tid);

  virtual void CopyData (AnyType *from) {
    *this = *(LocalVarByNameSearch*)from;
  }
};

class TokenNode : public AnyType  {

  public:
  SynObject *synObject;
  TToken token;
  QString str;
  QRect rect;
  SETpp flags;
  int newLines, indent;

  TokenNode();
  bool OptionalClauseToken(SynObject *&optClause);

  virtual void CopyData (AnyType *from) {
    *this = *(TokenNode*)from;
  }
};

struct CHETokenNode : ChainAnyElem {
  TokenNode data;

  ChainAnyElem* Clone ()
  { return new CHETokenNode(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHETokenNode*)from)->data; }
};

extern ChainAnyElem* NewCHETokenNode ();

typedef CHAINANY/*TokenNode*/ CHToken;

class CVarDesc : public AnyType  {

  public:

  CVarDesc()
  {
  }
  TID varID;
  CVarDesc *parent;
  bool writeAccess;
  CHAINANY/*CVarDesc*/ subTree;

  virtual void CopyData (AnyType *from) {
    *this = *(CVarDesc*)from;
  }
};

struct CHECVarDesc : ChainAnyElem {
  CVarDesc data;

  ChainAnyElem* Clone ()
  { return new CHECVarDesc(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHECVarDesc*)from)->data; }
};

extern ChainAnyElem* NewCHECVarDesc ();

class CRefEntry : public AnyType  {

  public:

  CRefEntry()
  {
  }
  virtual bool IsBranch()
  {
    return false;
  }
  virtual bool IsBranchStm()
  {
    return false;
  }
  virtual bool IsEndBranchStm()
  {
    return false;
  }
  virtual bool IsWriteAccess()
  {
    return false;
  }
  virtual bool IsFailSucceed()
  {
    return false;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(CRefEntry*)from;
  }
};

class CBranch : public CRefEntry {

  public:
  CHE *branchStm, *precedingBranch;

  CBranch()
  {
  }

  CBranch(CHE *brStm,CHE *lastBr)
  {
    branchStm=brStm;
    precedingBranch=lastBr;
  }
  virtual bool IsBranch()
  {
    return true;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(CBranch*)from;
  }
};

class CBranchStm : public CBranch {

  public:

  CBranchStm()
  {
  }
  virtual bool IsBranchStm()
  {
    return true;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(CBranchStm*)from;
  }
};

class CEndBranchStm : public CBranch {

  public:

  CEndBranchStm(CHE *brStm,CHE *lastBr)
  {
    branchStm=brStm;
    precedingBranch=lastBr;
  }
  virtual bool IsEndBranchStm()
  {
    return true;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(CEndBranchStm*)from;
  }
};

class CWriteAccess : public CRefEntry {

  public:

  CWriteAccess()
  {
  }
  ObjReference *objRef;
  CVarDesc *varDesc;
  virtual bool IsWriteAccess()
  {
    return true;
  }
  bool Contains(ObjReference *objRef);

  virtual void CopyData (AnyType *from) {
    *this = *(CWriteAccess*)from;
  }
};

class CFailSucceed : public CRefEntry {

  public:

  CFailSucceed()
  {
  }
  virtual bool IsFailSucceed()
  {
    return true;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(CFailSucceed*)from;
  }
};

typedef CHAINANY/*CVarDesc*/ CObjRefTable;

typedef CHAINANY/*unsigned*/ CBranchStack;

typedef CHAINX CRefTableEntries;
//end of $TYPE-macro


class RefTable {
public:
  CObjRefTable objRefTable;
  CBranchStack branchStack;
  CRefTableEntries refTableEntries;

  QString *AssignCheck (CheckData &ckd, ObjReference *objRef);
  QString *ReadCheck (CheckData &ckd, ObjReference *objRef);
  QString *findMatchingAccess (CheckData &ckd,
                          CHE *&chp,
                          CVarDesc *refEntry,
                          bool nestedCall,
                          bool &isAssigned,
                          ObjReference *objRef,
                          bool backward=true);
  void findObjRef (CheckData &ckd,
                   CObjRefTable &orTbl,
                   CVarDesc *oldVarDesc,
                   CVarDesc *&newdVarDesc,
                   CHE *che);
  bool assigned (CheckData &ckd,
                 CObjRefTable &oRefTbl,
                 CHE *che);
  void NewBranchStm (CHE *&branchStm,CHE *&precedingBranch);
  void NewBranch (CHE *branchStm,CHE *&precedingBranch);
  void EndBranchStm (CHE *branchStm,CHE *precedingBranch);
  void EndOfExec ();
};



class CProgTextBase {
public:
#ifdef EXECVIEW
  void INIT ();
  virtual void NewLine ();
	CProgTextBase () { INIT(); }
  virtual void Insert(TToken token,bool isPrimToken=false,bool isOpt=false)=0;
  virtual void SynObjNewLine()=0;
  virtual void Blank ()=0;
#endif

  virtual ~CProgTextBase () {};

  CHToken tokenChain;
  int currentPos, posInLine, newLines, lastIndent;
  unsigned currentSynObjID;
  CHETokenNode *currentToken,
    *newSelection, *currentSelection, *firstMoved;
  SynObject *currentSynObj, *selectAt, *selectAfter;
  CLavaBaseDoc *document;
  CheckData ckd;

  int insertedChars;
  bool plhSearch, htmlGen,
       showComments, leftArrows, ignored, singleFile;
};



enum ExecFlags {
  staticCall,
  ignoreSynObj,
  newLine,
  isOpt,
  isDisabled,
  primToken,
  abstractPP,
  brokenRef,
  insertBlank,
  isLocalVar,
  isTempVar,
  isStateObjMember,
  isInputVar,
  isOutputVar,
  isSelfVar,
  isMemberVar,
  isDeclareVar,
  isVariable,
  inExecHdr,
  inputArrow,
  isSameAsSelf,
  isSubstitutable,
  isUnknownCat,
  isInForeach,
  unfinishedAllowed,
  free1,
  free2,
  free3,
  isOptionalExpr,
  isReverseLink,
  isClosed};

extern void CDPExecFlags (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

enum WorkFlags {
  isBrkPnt,
  isTempPoint};

extern void CDPWorkFlags (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

class TComment : public DObject  {
  DECLARE_DYNAMIC_CLASS(TComment)


  public:
  STRING str;
  bool inLine, trailing;

  TComment()
  {
    inLine=false;
    trailing=false;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(TComment*)from;
  }

  friend void CDPTComment (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTComment(pgf,cid,(address)this,baseCDP); }
};

struct NST0TComment {
protected:

  void init (const NST0TComment&);

  void copy (const NST0TComment&);

public:

  TComment *ptr;

  NST0TComment () { ptr = 0; }

  NST0TComment (const NST0TComment& n)
  { init(n); }

  void Destroy () { delete ptr; ptr = 0; }

  NST0TComment& operator= (const NST0TComment& n)
  { copy(n); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid, ConversionProc cdp);
};

struct NSTTComment : NST0TComment {
  NSTTComment() {}

  NSTTComment (const NSTTComment& n)
  { init(n); }

  NSTTComment& operator= (const NSTTComment& n)
  { copy(n); return *this; }

  virtual ~NSTTComment () { Destroy(); }
};

class SynObject : public SynObjectBase {
  DECLARE_DYNAMIC_CLASS(SynObject)


  public:
  QWidget *execView;
  TToken primaryToken, type, replacedType;
  SynObject *parentObject;
  NSTTComment comment;
  address whereInParent;
  CHAINX *containingChain;
  SETpp flags;
  SETpp workFlags;
  CHETokenNode *startToken, *endToken, *primaryTokenNode;
  CHAINX errorChain;
  CHE *oldError;
  unsigned oldColor, synObjectID;
  int startPos, startPosHTML, currentIndent;
  QString *lastError;
  bool errorChanged;

  SynObject();

  virtual ~SynObject()
  {
  }
  virtual void MakeExpression()
  {
    primaryToken=Exp_T;
    type=Exp_T;
    replacedType=Exp_T;
  }
  virtual void NLincIndent(CProgTextBase &text);
  virtual void NLdecIndent(CProgTextBase &text);
  bool BoolAdmissibleOnly(CheckData &ckd);
  bool EnumAdmissibleOnly(CheckData &ckd);
  bool NullAdmissible(CheckData &ckd);
  bool ExpressionSelected(CHETokenNode *currentSelection);
  virtual bool IsOptional(CheckData &ckd)
  {
    return flags.Contains(isOptionalExpr);
  }
  virtual bool IsClosed(CheckData &ckd)
  {
    return flags.Contains(isClosed);
  }
  bool HasOptionalParts();
  bool IsDefChecked(CheckData &ckd);
  virtual ROContext ReadOnlyContext();
  virtual bool IsReadOnlyClause(SynObject *synObj)
  {
    return false;
  }
  virtual bool InOldExpression();
  virtual bool InInitializer(CheckData &ckd);
  virtual bool InHiddenIniClause(CheckData &ckd,SynObject *&synObj);
  virtual bool IsArrayObj();
  virtual bool IsAssigTarget();
  virtual bool IsBinaryOp()
  {
    return false;
  }
  virtual bool IsInSetStm()
  {
    return false;
  }
  virtual bool IsIntIntv()
  {
    return false;
  }
  virtual bool IsEnumType()
  {
    return false;
  }
  virtual bool IsConstant()
  {
    return false;
  }
  virtual bool IsEditableConstant()
  {
    return false;
  }
  virtual bool IsDeclare()
  {
    return false;
  }
  virtual bool IsExpression();
  virtual bool IsIfStmExpr()
  {
    return false;
  }
  bool IsFuncHandle();
  virtual bool IsFuncInvocation()
  {
    return false;
  }
  bool IsIfClause();
  virtual bool IsMultOp()
  {
    return false;
  }
  bool IsObjRef();
  bool IsOutputParam();
  virtual bool IsOperation()
  {
    return false;
  }
  virtual bool IsPlaceHolder()
  {
    return true;
  }
  virtual bool IsRepeatableClause(CHAINX *&chx)
  {
    return false;
  }
  virtual bool IsExecutable()
  {
    return true;
  }
  virtual bool IsSelfVar()
  {
    return false;
  }
  bool IsStatement();
  virtual bool IsThrow()
  {
    return false;
  }
  virtual bool IsUnaryOp()
  {
    return false;
  }
  virtual bool NestedOptClause(SynObject *optClause)
  {
    return false;
  }
  bool StatementSelected(CHETokenNode *currentSelection);
  bool SameExec(LavaDECL *decl);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address,int inINCL,SynObjectBase *,TTableUpdate,address,CHAINX *,address searchData=0);
  bool UpdateReference(CheckData &ckd);
  virtual SynObject *InsertOptionals()
  {
    return 0;
  }
  virtual void Insert2Optionals(SynObject *&elsePart,SynObject *&branch,CHAINX *&chx)
  {
  }
  virtual void InsertBranch(SynObject *&branch,CHAINX *&chx)
  {
  }
  virtual void Draw(CProgTextBase &text,address where,CHAINX *chxp,bool ignored)
  {
  }
  void SetError(CheckData &ckd,QString *error,char *textParam=0);
  void SetRTError(CheckData &ckd,QString *error,LavaVariablePtr stackFrame,const char *textParam=0);
  QString DebugStop(CheckData &ckd,LavaVariablePtr stack,QString excMsg,StopReason sr,LavaVariablePtr calleeStack,LavaDECL *calleeFunc);
  QString LocationOfConstruct();
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags)
  {
    decl=0;
    cat=unknownCategory;
  }
  virtual bool Execute(CheckData &ckd,LavaVariablePtr stackFrame,unsigned oldExprLevel);
  virtual LavaObjectPtr Evaluate(CheckData &ckd,LavaVariablePtr stackFrame,unsigned oldExprLevel);
  virtual bool Recursion(CheckData &ckd,LavaVariablePtr stackFrame,unsigned oldExprLevel,CHE *cheQuant,CHE *cheVar,LavaObjectPtr rSet=0)
  {
    return false;
  }
  virtual QString whatsThisText();
  virtual QString whatNextText();

  virtual void CopyData (AnyType *from) {
    *this = *(SynObject*)from;
  }

  friend void CDPSynObject (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSynObject(pgf,cid,(address)this,baseCDP); }
};

struct TDOD : public SynObject {
  DECLARE_DYNAMIC_CLASS(TDOD)

  STRING name;
  TID ID;
  LavaDECL *fieldDecl;
  int sectionNumber;
  bool isProperty;
  CContext context;
  LavaDECL *vType;
  LavaDECL *eType;
  int vSectionNumber;
  bool isOuter;

  TDOD();
  virtual bool IsPlaceHolder()
  {
    return false;
  }
  bool IsStateObject(CheckData &ckd);
  virtual bool IsExecutable()
  {
    return false;
  }
  bool accessTypeOK(SynFlags accessFlags);
  bool ReplaceWithLocalParm(CheckData &ckd,LavaDECL *funcDecl,TDeclType declType);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(TDOD*)from;
  }

  friend void CDPTDOD (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTDOD(pgf,cid,(address)this,baseCDP); }
};

typedef CHAINX TDODC;

extern void CDPTDODC (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP=false);

class Expression : public SynObject {
  DECLARE_DYNAMIC_CLASS(Expression)


  public:
  LavaDECL *formVType, *finalType;
  int sectionNumber;
  int vSectionNumber;
  bool isOuter;
  Category targetCat;
  virtual bool IsPlaceHolder()
  {
    return false;
  }

  Expression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Expression*)from;
  }

  friend void CDPExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPExpression(pgf,cid,(address)this,baseCDP); }
};

class Operation : public Expression {
  DECLARE_DYNAMIC_CLASS(Operation)


  public:
  TID opFunctionID;
  LavaDECL *funcDecl;
  unsigned funcSectionNumber;
  CContext callCtx;
  virtual bool IsOperation()
  {
    return true;
  }

  Operation () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Operation*)from;
  }

  friend void CDPOperation (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPOperation(pgf,cid,(address)this,baseCDP); }
};

class Reference : public SynObject {
  DECLARE_DYNAMIC_CLASS(Reference)


  public:

  Reference()
  {
  }

  Reference(TToken token,TID id,const char *name);
  STRING refName;
  TID refID;
  LavaDECL *refDecl;
  virtual bool IsPlaceHolder()
  {
    return false;
  }
  virtual bool IsExecutable();
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);

  virtual void CopyData (AnyType *from) {
    *this = *(Reference*)from;
  }

  friend void CDPReference (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPReference(pgf,cid,(address)this,baseCDP); }
};

class EnumConst : public Expression {
  DECLARE_DYNAMIC_CLASS(EnumConst)


  public:
  TID refID;
  STRING Id;
  LavaDECL *refDecl;
  int enumItem;
  LavaObjectPtr value, enumBaseObj;

  EnumConst()
  {
  }
  virtual bool IsConstant()
  {
    return true;
  }
  virtual bool IsEditableConstant()
  {
    return false;
  }
  virtual void MakeTable(address,int inINCL,SynObjectBase *,TTableUpdate,address,CHAINX *,address);
  virtual bool Check(CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);

  virtual void CopyData (AnyType *from) {
    *this = *(EnumConst*)from;
  }

  friend void CDPEnumConst (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEnumConst(pgf,cid,(address)this,baseCDP); }
};

class ObjReference : public Expression {
  DECLARE_DYNAMIC_CLASS(ObjReference)


  public:
  STRING refName;
  TDODC refIDs;
  unsigned stackPos;
  ObjReference *conflictingAssig;
  LavaDECL *myFinalVType;
  CContext myContext;
  Category myCategory;

  ObjReference()
  {
  }

  ObjReference(TDODC &refIDs,const char *refName);

  virtual ~ObjReference()
  {
  }
  bool operator == (ObjReference &objRef);
  virtual bool IsPlaceHolder()
  {
    return false;
  }
  virtual bool IsOptional(CheckData &ckd);
  bool InConstituent(CheckData &ckd);
  bool Inherited(CheckData &ckd);
  bool OutOfScope(CheckData &ckd);
  virtual VarRefContext Context();
  virtual bool AssignCheck(CheckData &ckd,VarRefContext cty);
  virtual bool CopyCheck(CheckData &ckd);
  virtual bool ReadCheck(CheckData &ckd);
  virtual bool CallCheck(CheckData &ckd);
  virtual bool ArrayTargetCheck(CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(ObjReference*)from;
  }

  friend void CDPObjReference (PutGetFlag pgf, ASN1* cid, address varAddr,
                               bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPObjReference(pgf,cid,(address)this,baseCDP); }
};

class VarName : public Expression {
  DECLARE_DYNAMIC_CLASS(VarName)


  public:
  STRING varName;
  TID varID;
  VarName *nextLocal;
  unsigned stackPos;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  Reference *TypeRef();
  virtual bool IsPlaceHolder()
  {
    return false;
  }
  virtual bool IsExecutable()
  {
    return false;
  }
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  VarName () {}

  virtual void CopyData (AnyType *from) {
    *this = *(VarName*)from;
  }

  friend void CDPVarName (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPVarName(pgf,cid,(address)this,baseCDP); }
};

class FormParm : public Expression {
  DECLARE_DYNAMIC_CLASS(FormParm)


  public:

  FormParm()
  {
  }

  FormParm(bool);
  NESTEDANY/*Reference*/ parmType;
  NESTEDANY/*ObjReference*/ formParm;
  TID formParmID;
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(FormParm*)from;
  }

  friend void CDPFormParm (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFormParm(pgf,cid,(address)this,baseCDP); }
};

class FormParms : public Expression {
  DECLARE_DYNAMIC_CLASS(FormParms)


  public:

  FormParms()
  {
  }

  FormParms(SynObject *selfVar);
  CHAINX inputs, outputs;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(FormParms*)from;
  }

  friend void CDPFormParms (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFormParms(pgf,cid,(address)this,baseCDP); }
};

class BaseInit : public Expression {
  DECLARE_DYNAMIC_CLASS(BaseInit)


  public:
  NESTEDANY/*Reference*/ baseItf;
  NESTEDANY/*FuncStatement*/ initializerCall;
  NESTEDANY/*ObjReference*/ self;
  NESTEDANY/*Reference*/ baseItf2;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  BaseInit () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BaseInit*)from;
  }

  friend void CDPBaseInit (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBaseInit(pgf,cid,(address)this,baseCDP); }
};

class SelfVar : public VarName {
  DECLARE_DYNAMIC_CLASS(SelfVar)


  public:
  TID typeID;
  NESTEDANY/*Reference*/ execName;
  NESTEDANY/*FormParms*/ formParms;
  CHAINX baseInitCalls;
  NESTEDANY/*Expression*/ body;
  FormParms *oldFormParms;
  LavaDECL *execDECL, *funcDECL, *itfDECL, *selfType;
  unsigned nParams, nInputs, nOutputs, stackFrameSize, firstOldExprExec, inINCL;
  bool concernExecs, checked, isFuncBody, isInvariant;
  CContext selfCtx;

  SelfVar()
  {
    checked=false;
    concernExecs=false;
    execView=0;
    isFuncBody=false;
    isInvariant=false;
  }
  virtual bool IsEmptyExec();
  virtual bool IsSelfVar()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  bool BaseInitsCheck(CheckData &ckd);
  virtual bool Check(CheckData &ckd);
  bool InitCheck(CheckData &ckd,bool inSelfCheck=true);
  bool InputCheck(CheckData &ckd);
  bool OutputCheck(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(SelfVar*)from;
  }

  friend void CDPSelfVar (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSelfVar(pgf,cid,(address)this,baseCDP); }
};

class Constant : public Expression {
  DECLARE_DYNAMIC_CLASS(Constant)


  public:
  TBasicType constType;
  STRING str;
  LavaObjectPtr value;
  LavaDECL *typeDECL;

  Constant()
  {
    value=0;
  }
  virtual bool Check(CheckData &ckd);
  virtual bool IsConstant()
  {
    return true;
  }
  virtual bool IsEditableConstant()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);

  virtual void CopyData (AnyType *from) {
    *this = *(Constant*)from;
  }

  friend void CDPConstant (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPConstant(pgf,cid,(address)this,baseCDP); }
};

class BoolConst : public Expression {
  DECLARE_DYNAMIC_CLASS(BoolConst)


  public:
  bool boolValue;
  LavaObjectPtr value;
  LavaDECL *typeDECL;

  BoolConst()
  {
    value=0;
  }
  virtual bool Check(CheckData &ckd);
  virtual bool IsConstant()
  {
    return true;
  }
  virtual bool IsEditableConstant()
  {
    return false;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);

  virtual void CopyData (AnyType *from) {
    *this = *(BoolConst*)from;
  }

  friend void CDPBoolConst (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBoolConst(pgf,cid,(address)this,baseCDP); }
};

class NullConst : public Expression {
  DECLARE_DYNAMIC_CLASS(NullConst)


  public:
  virtual bool Check(CheckData &ckd);
  virtual bool IsConstant()
  {
    return true;
  }
  virtual bool IsEditableConstant()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);

  NullConst () {}

  virtual void CopyData (AnyType *from) {
    *this = *(NullConst*)from;
  }

  friend void CDPNullConst (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPNullConst(pgf,cid,(address)this,baseCDP); }
};

class SucceedStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(SucceedStatement)


  public:
  virtual bool Check(CheckData &ckd);

  SucceedStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SucceedStatement*)from;
  }

  friend void CDPSucceedStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSucceedStatement(pgf,cid,(address)this,baseCDP); }
};

class FailStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(FailStatement)


  public:
  NESTEDANY/*Expression*/ exception;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  bool IsThrow()
  {
    return true;
  }

  FailStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(FailStatement*)from;
  }

  friend void CDPFailStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFailStatement(pgf,cid,(address)this,baseCDP); }
};

class OldExpression : public Expression {
  DECLARE_DYNAMIC_CLASS(OldExpression)


  public:
  NESTEDANY/*Expression*/ paramExpr;
  unsigned iOldExpr;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  OldExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(OldExpression*)from;
  }

  friend void CDPOldExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPOldExpression(pgf,cid,(address)this,baseCDP); }
};

class UnaryOp : public Operation {
  DECLARE_DYNAMIC_CLASS(UnaryOp)


  public:
  NESTEDANY/*Expression*/ operand;
  virtual bool IsUnaryOp()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  UnaryOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(UnaryOp*)from;
  }

  friend void CDPUnaryOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPUnaryOp(pgf,cid,(address)this,baseCDP); }
};

class EvalExpression : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(EvalExpression)


  public:
  virtual bool IsUnaryOp()
  {
    return false;
  }
  virtual bool IsReadOnlyClause(SynObject *synObj)
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  EvalExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(EvalExpression*)from;
  }

  friend void CDPEvalExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                 bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEvalExpression(pgf,cid,(address)this,baseCDP); }
};

class EvalStatement : public EvalExpression {
  DECLARE_DYNAMIC_CLASS(EvalStatement)


  public:
  virtual bool Check(CheckData &ckd);

  EvalStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(EvalStatement*)from;
  }

  friend void CDPEvalStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEvalStatement(pgf,cid,(address)this,baseCDP); }
};

class ArrayAtIndex : public Operation {
  DECLARE_DYNAMIC_CLASS(ArrayAtIndex)


  public:
  NESTEDANY/*Expression*/ arrayObj, arrayIndex;
  int funcSectionNumber;
  bool setCase;
  virtual bool IsPlaceHolder()
  {
    return false;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  ArrayAtIndex () {}

  virtual void CopyData (AnyType *from) {
    *this = *(ArrayAtIndex*)from;
  }

  friend void CDPArrayAtIndex (PutGetFlag pgf, ASN1* cid, address varAddr,
                               bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPArrayAtIndex(pgf,cid,(address)this,baseCDP); }
};

class InvertOp : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(InvertOp)


  InvertOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(InvertOp*)from;
  }

  friend void CDPInvertOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPInvertOp(pgf,cid,(address)this,baseCDP); }
};

class HandleOp : public Expression {
  DECLARE_DYNAMIC_CLASS(HandleOp)

  NESTEDANY/*ObjReference*/ operand;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  HandleOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(HandleOp*)from;
  }

  friend void CDPHandleOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPHandleOp(pgf,cid,(address)this,baseCDP); }
};

class OrdOp : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(OrdOp)


  OrdOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(OrdOp*)from;
  }

  friend void CDPOrdOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPOrdOp(pgf,cid,(address)this,baseCDP); }
};

class MinusOp : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(MinusOp)


  MinusOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(MinusOp*)from;
  }

  friend void CDPMinusOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPMinusOp(pgf,cid,(address)this,baseCDP); }
};

class LogicalNot : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(LogicalNot)


  public:
  virtual bool IsReadOnlyClause(SynObject *synObj)
  {
    return true;
  }
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  LogicalNot () {}

  virtual void CopyData (AnyType *from) {
    *this = *(LogicalNot*)from;
  }

  friend void CDPLogicalNot (PutGetFlag pgf, ASN1* cid, address varAddr,
                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPLogicalNot(pgf,cid,(address)this,baseCDP); }
};

class InSetStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(InSetStatement)


  public:
  NESTEDANY/*Expression*/ operand1, operand2;
  virtual bool IsInSetStm()
  {
    return true;
  }
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  InSetStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(InSetStatement*)from;
  }

  friend void CDPInSetStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                 bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPInSetStatement(pgf,cid,(address)this,baseCDP); }
};

class BinaryOp : public Operation {
  DECLARE_DYNAMIC_CLASS(BinaryOp)


  public:
  NESTEDANY/*Expression*/ operand1, operand2;
  virtual bool IsBinaryOp()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  BinaryOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BinaryOp*)from;
  }

  friend void CDPBinaryOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBinaryOp(pgf,cid,(address)this,baseCDP); }
};

class MultipleOp : public Operation {
  DECLARE_DYNAMIC_CLASS(MultipleOp)


  public:
  CHAINX operands;

  MultipleOp()
  {
  }
  void MultipleOpInit(TToken primToken);
  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool IsMultOp()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(MultipleOp*)from;
  }

  friend void CDPMultipleOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPMultipleOp(pgf,cid,(address)this,baseCDP); }
};

class SemicolonOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(SemicolonOp)


  SemicolonOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SemicolonOp*)from;
  }

  friend void CDPSemicolonOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                              bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSemicolonOp(pgf,cid,(address)this,baseCDP); }
};

class AndOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(AndOp)


  AndOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(AndOp*)from;
  }

  friend void CDPAndOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPAndOp(pgf,cid,(address)this,baseCDP); }
};

class OrOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(OrOp)


  OrOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(OrOp*)from;
  }

  friend void CDPOrOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPOrOp(pgf,cid,(address)this,baseCDP); }
};

class XorOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(XorOp)


  XorOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(XorOp*)from;
  }

  friend void CDPXorOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPXorOp(pgf,cid,(address)this,baseCDP); }
};

class BitAndOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(BitAndOp)


  BitAndOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BitAndOp*)from;
  }

  friend void CDPBitAndOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBitAndOp(pgf,cid,(address)this,baseCDP); }
};

class BitOrOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(BitOrOp)


  BitOrOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BitOrOp*)from;
  }

  friend void CDPBitOrOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBitOrOp(pgf,cid,(address)this,baseCDP); }
};

class BitXorOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(BitXorOp)


  BitXorOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BitXorOp*)from;
  }

  friend void CDPBitXorOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBitXorOp(pgf,cid,(address)this,baseCDP); }
};

class DivideOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(DivideOp)


  DivideOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(DivideOp*)from;
  }

  friend void CDPDivideOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDivideOp(pgf,cid,(address)this,baseCDP); }
};

class ModulusOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(ModulusOp)


  ModulusOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(ModulusOp*)from;
  }

  friend void CDPModulusOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPModulusOp(pgf,cid,(address)this,baseCDP); }
};

class LshiftOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(LshiftOp)


  LshiftOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(LshiftOp*)from;
  }

  friend void CDPLshiftOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPLshiftOp(pgf,cid,(address)this,baseCDP); }
};

class RshiftOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(RshiftOp)


  RshiftOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(RshiftOp*)from;
  }

  friend void CDPRshiftOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPRshiftOp(pgf,cid,(address)this,baseCDP); }
};

class PlusOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(PlusOp)


  PlusOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(PlusOp*)from;
  }

  friend void CDPPlusOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPPlusOp(pgf,cid,(address)this,baseCDP); }
};

class MultOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(MultOp)


  MultOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(MultOp*)from;
  }

  friend void CDPMultOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPMultOp(pgf,cid,(address)this,baseCDP); }
};

class Assignment : public Expression {
  DECLARE_DYNAMIC_CLASS(Assignment)


  public:
  NESTEDANY/*ObjReference*/ targetObj;
  NESTEDANY/*Expression*/ exprValue;
  TargetType kindOfTarget;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  Assignment () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Assignment*)from;
  }

  friend void CDPAssignment (PutGetFlag pgf, ASN1* cid, address varAddr,
                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPAssignment(pgf,cid,(address)this,baseCDP); }
};

class Parameter : public Expression {
  DECLARE_DYNAMIC_CLASS(Parameter)


  public:
  NESTEDANY/*Expression*/ parameter;
  TID formParmID;
  TargetType kindOfTarget;
  virtual bool IsOptional(CheckData &ckd)
  {
    return((SynObject *)parameter.ptr)->IsOptional(ckd);
  }
  virtual bool IsClosed(CheckData &ckd)
  {
    return((SynObject *)parameter.ptr)->IsClosed(ckd);
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  Parameter () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Parameter*)from;
  }

  friend void CDPParameter (PutGetFlag pgf, ASN1* cid, address varAddr,
                            bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPParameter(pgf,cid,(address)this,baseCDP); }
};

class FuncExpression : public Expression {
  DECLARE_DYNAMIC_CLASS(FuncExpression)


  public:
  CHAINX inputs;
  unsigned nParams;
  NESTEDANY/*Reference*/ function;
  NESTEDANY/*ObjReference*/ handle;
  TID vtypeID;
  int funcSectionNumber;
  int vBaseSectionNumber;
  bool vBaseIsOuter;
  LavaDECL *funcDecl, *objTypeDecl, *vBaseType;
  CContext callCtx;
  Category callObjCat;
  SynFlags myCtxFlags;
  virtual bool IsFuncInvocation()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  FuncExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(FuncExpression*)from;
  }

  friend void CDPFuncExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                 bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFuncExpression(pgf,cid,(address)this,baseCDP); }
};

class FuncStatement : public FuncExpression {
  DECLARE_DYNAMIC_CLASS(FuncStatement)


  public:
  CHAINX outputs;

  FuncStatement()
  {
  }

  FuncStatement(Reference *ref);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(FuncStatement*)from;
  }

  friend void CDPFuncStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFuncStatement(pgf,cid,(address)this,baseCDP); }
};

struct Connect : public Expression {
  DECLARE_DYNAMIC_CLASS(Connect)


  public:
  NESTEDANY/*Expression*/ signalSender;
  NESTEDANY/*Expression*/ signalSenderClass;
  NESTEDANY/*Reference*/ signalFunction;
  NESTEDANY/*FuncStatement*/ callback;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  Connect () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Connect*)from;
  }

  friend void CDPConnect (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPConnect(pgf,cid,(address)this,baseCDP); }
};

struct Disconnect : public Expression {
  DECLARE_DYNAMIC_CLASS(Disconnect)


  public:
  NESTEDANY/*Expression*/ signalSender;
  NESTEDANY/*Reference*/ signalFunction;
  NESTEDANY/*Expression*/ signalReceiver;
  NESTEDANY/*Reference*/ callbackFunction;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  Disconnect () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Disconnect*)from;
  }

  friend void CDPDisconnect (PutGetFlag pgf, ASN1* cid, address varAddr,
                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDisconnect(pgf,cid,(address)this,baseCDP); }
};

class Signal : public Expression {
  DECLARE_DYNAMIC_CLASS(Signal)


  public:
  NESTEDANY/*Expression*/ sCall;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  Signal () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Signal*)from;
  }

  friend void CDPSignal (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSignal(pgf,cid,(address)this,baseCDP); }
};

class AssertStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(AssertStatement)


  public:
  NESTEDANY/*Expression*/ statement;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool IsReadOnlyClause(SynObject *synObj)
  {
    return true;
  }

  AssertStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(AssertStatement*)from;
  }

  friend void CDPAssertStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                  bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPAssertStatement(pgf,cid,(address)this,baseCDP); }
};

class IfThen : public Expression {
  DECLARE_DYNAMIC_CLASS(IfThen)


  public:
  NESTEDANY/*Expression*/ ifCondition;
  NESTEDANY/*Expression*/ thenPart;
  CHETokenNode *thenToken;
  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool IsRepeatableClause(CHAINX *&chx);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  IfThen () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IfThen*)from;
  }

  friend void CDPIfThen (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfThen(pgf,cid,(address)this,baseCDP); }
};

class IfStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(IfStatement)


  public:
  CHAINX ifThens;
  NESTEDANY/*Expression*/ elsePart;
  virtual bool IsIfStmExpr()
  {
    return true;
  }
  virtual bool NestedOptClause(SynObject *optClause);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  IfStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IfStatement*)from;
  }

  friend void CDPIfStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                              bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfStatement(pgf,cid,(address)this,baseCDP); }
};

class IfxThen : public Expression {
  DECLARE_DYNAMIC_CLASS(IfxThen)


  public:
  NESTEDANY/*Expression*/ ifCondition;
  NESTEDANY/*Expression*/ thenPart;
  CHETokenNode *thenToken;
  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool IsRepeatableClause(CHAINX *&chx);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  IfxThen () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IfxThen*)from;
  }

  friend void CDPIfxThen (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfxThen(pgf,cid,(address)this,baseCDP); }
};

class IfdefStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(IfdefStatement)


  public:
  CHAINX ifCondition;
  NESTEDANY/*Expression*/ thenPart;
  NESTEDANY/*Expression*/ elsePart;
  bool Checks(ObjReference &objRef);
  virtual bool IsIfStmExpr()
  {
    return true;
  }
  virtual bool NestedOptClause(SynObject *optClause);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  IfdefStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IfdefStatement*)from;
  }

  friend void CDPIfdefStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                 bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfdefStatement(pgf,cid,(address)this,baseCDP); }
};

class CondExpression : public Expression {
  DECLARE_DYNAMIC_CLASS(CondExpression)


  public:
  LavaDECL *targetDecl;
  CContext targetCtx;
  Category callObjCat;
  bool isOpt;

  CondExpression();

  virtual void CopyData (AnyType *from) {
    *this = *(CondExpression*)from;
  }

  friend void CDPCondExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                 bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCondExpression(pgf,cid,(address)this,baseCDP); }
};

class IfExpression : public CondExpression {
  DECLARE_DYNAMIC_CLASS(IfExpression)


  public:
  CHAINX ifThens;
  NESTEDANY/*Expression*/ elsePart;

  IfExpression()
  {
  }
  virtual bool IsIfStmExpr()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(IfExpression*)from;
  }

  friend void CDPIfExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                               bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfExpression(pgf,cid,(address)this,baseCDP); }
};

class ElseExpression : public CondExpression {
  DECLARE_DYNAMIC_CLASS(ElseExpression)


  public:
  NESTEDANY/*Expression*/ expr1, expr2;

  ElseExpression()
  {
  }
  virtual bool IsIfStmExpr()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  virtual void CopyData (AnyType *from) {
    *this = *(ElseExpression*)from;
  }

  friend void CDPElseExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                 bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPElseExpression(pgf,cid,(address)this,baseCDP); }
};

class Branch : public Expression {
  DECLARE_DYNAMIC_CLASS(Branch)


  public:
  CHAINX caseLabels;
  NESTEDANY/*Expression*/ thenPart;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool IsRepeatableClause(CHAINX *&chx);
  virtual bool IsExecutable()
  {
    return false;
  }

  Branch () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Branch*)from;
  }

  friend void CDPBranch (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBranch(pgf,cid,(address)this,baseCDP); }
};

class SwitchStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(SwitchStatement)


  public:
  NESTEDANY/*Expression*/ caseExpression;
  CHAINX branches;
  NESTEDANY/*Expression*/ elsePart;
  virtual bool NestedOptClause(SynObject *optClause);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  SwitchStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SwitchStatement*)from;
  }

  friend void CDPSwitchStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                  bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSwitchStatement(pgf,cid,(address)this,baseCDP); }
};

class CatchClause : public Expression {
  DECLARE_DYNAMIC_CLASS(CatchClause)


  public:
  NESTEDANY/*Reference*/ exprType;
  NESTEDANY/*VarName*/ varName;
  NESTEDANY/*Expression*/ catchClause;
  LavaDECL *typeDecl;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool IsRepeatableClause(CHAINX *&chx);
  virtual bool IsExecutable()
  {
    return false;
  }

  CatchClause () {}

  virtual void CopyData (AnyType *from) {
    *this = *(CatchClause*)from;
  }

  friend void CDPCatchClause (PutGetFlag pgf, ASN1* cid, address varAddr,
                              bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCatchClause(pgf,cid,(address)this,baseCDP); }
};

class TryStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(TryStatement)


  public:
  NESTEDANY/*Expression*/ tryStatement;
  CHAINX catchClauses;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  TryStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TryStatement*)from;
  }

  friend void CDPTryStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                               bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTryStatement(pgf,cid,(address)this,baseCDP); }
};

class TypeBranch : public Expression {
  DECLARE_DYNAMIC_CLASS(TypeBranch)


  public:
  NESTEDANY/*Reference*/ exprType;
  NESTEDANY/*VarName*/ varName;
  NESTEDANY/*Expression*/ thenPart;
  LavaDECL *typeDecl;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool IsRepeatableClause(CHAINX *&chx);
  virtual bool IsExecutable()
  {
    return false;
  }

  TypeBranch () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TypeBranch*)from;
  }

  friend void CDPTypeBranch (PutGetFlag pgf, ASN1* cid, address varAddr,
                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTypeBranch(pgf,cid,(address)this,baseCDP); }
};

class TypeSwitchStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(TypeSwitchStatement)


  public:
  NESTEDANY/*Expression*/ caseExpression;
  CHAINX branches;
  NESTEDANY/*Expression*/ elsePart;
  LavaDECL *declSwitchExpression;
  Category catSwitchExpression;
  virtual bool NestedOptClause(SynObject *optClause);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  TypeSwitchStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TypeSwitchStatement*)from;
  }

  friend void CDPTypeSwitchStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                      bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTypeSwitchStatement(pgf,cid,(address)this,baseCDP); }
};

class AttachObject : public Expression {
  DECLARE_DYNAMIC_CLASS(AttachObject)


  public:
  NESTEDANY/*Reference*/ objType;
  NESTEDANY/*Reference*/ itf;
  NESTEDANY/*Expression*/ url;
  LavaDECL *typeDECL, *execDECL;
  Category attachCat;
  virtual bool NestedOptClause(SynObject *optClause);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool Check(CheckData &ckd);

  AttachObject () {}

  virtual void CopyData (AnyType *from) {
    *this = *(AttachObject*)from;
  }

  friend void CDPAttachObject (PutGetFlag pgf, ASN1* cid, address varAddr,
                               bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPAttachObject(pgf,cid,(address)this,baseCDP); }
};

class NewExpression : public AttachObject {
  DECLARE_DYNAMIC_CLASS(NewExpression)


  public:
  NESTEDANY/*VarName*/ varName;
  NESTEDANY/*FuncStatement*/ initializerCall;
  NESTEDANY/*Expression*/ butStatement;
  bool errorInInitializer;
  virtual bool NestedOptClause(SynObject *optClause);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  NewExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(NewExpression*)from;
  }

  friend void CDPNewExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPNewExpression(pgf,cid,(address)this,baseCDP); }
};

class CloneExpression : public Expression {
  DECLARE_DYNAMIC_CLASS(CloneExpression)

  NESTEDANY/*VarName*/ varName;
  NESTEDANY/*Expression*/ fromObj;
  NESTEDANY/*Expression*/ butStatement;
  LavaDECL *execDECL;
  virtual bool IsOptional(CheckData &ckd)
  {
    return((SynObject *)fromObj.ptr)->IsOptional(ckd);
  }
  virtual bool NestedOptClause(SynObject *optClause);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  CloneExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(CloneExpression*)from;
  }

  friend void CDPCloneExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                  bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCloneExpression(pgf,cid,(address)this,baseCDP); }
};

class CopyStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(CopyStatement)

  NESTEDANY/*Expression*/ fromObj;
  NESTEDANY/*ObjReference*/ ontoObj;
  TargetType kindOfTarget;
  LavaDECL *ontoTypeDecl;
  virtual bool IsOptional(CheckData &ckd)
  {
    return((SynObject *)fromObj.ptr)->IsOptional(ckd);
  }
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  CopyStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(CopyStatement*)from;
  }

  friend void CDPCopyStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                                bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCopyStatement(pgf,cid,(address)this,baseCDP); }
};

class EnumItem : public Expression {
  DECLARE_DYNAMIC_CLASS(EnumItem)


  public:
  NESTEDANY/*Expression*/ itemNo;
  NESTEDANY/*Reference*/ enumType;
  void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  EnumItem () {}

  virtual void CopyData (AnyType *from) {
    *this = *(EnumItem*)from;
  }

  friend void CDPEnumItem (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEnumItem(pgf,cid,(address)this,baseCDP); }
};

class ExtendExpression : public Expression {
  DECLARE_DYNAMIC_CLASS(ExtendExpression)


  public:
  NESTEDANY/*Expression*/ extendObj;
  NESTEDANY/*Reference*/ extendType;
  LavaDECL *extendTypeDecl;
  void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  ExtendExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(ExtendExpression*)from;
  }

  friend void CDPExtendExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPExtendExpression(pgf,cid,(address)this,baseCDP); }
};

class Run : public Expression {
  DECLARE_DYNAMIC_CLASS(Run)


  public:
  NESTEDANY/*Reference*/ initiator;
  CHAINX inputs;
  LavaDECL *execDECL, *typeDECL;
  unsigned nParams;
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  Run () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Run*)from;
  }

  friend void CDPRun (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPRun(pgf,cid,(address)this,baseCDP); }
};

class QueryItf : public Expression {
  DECLARE_DYNAMIC_CLASS(QueryItf)


  public:
  NESTEDANY/*Reference*/ itf;
  NESTEDANY/*ObjReference*/ givenObj;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  QueryItf () {}

  virtual void CopyData (AnyType *from) {
    *this = *(QueryItf*)from;
  }

  friend void CDPQueryItf (PutGetFlag pgf, ASN1* cid, address varAddr,
                           bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPQueryItf(pgf,cid,(address)this,baseCDP); }
};

class GetUUID : public Expression {
  DECLARE_DYNAMIC_CLASS(GetUUID)


  public:
  NESTEDANY/*Reference*/ itf;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  GetUUID () {}

  virtual void CopyData (AnyType *from) {
    *this = *(GetUUID*)from;
  }

  friend void CDPGetUUID (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPGetUUID(pgf,cid,(address)this,baseCDP); }
};

class IntegerInterval : public SynObject {
  DECLARE_DYNAMIC_CLASS(IntegerInterval)


  public:
  NESTEDANY/*Expression*/ from, to;
  virtual bool IsPlaceHolder()
  {
    return false;
  }
  virtual bool IsIntIntv()
  {
    return true;
  }
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  IntegerInterval () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IntegerInterval*)from;
  }

  friend void CDPIntegerInterval (PutGetFlag pgf, ASN1* cid, address varAddr,
                                  bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIntegerInterval(pgf,cid,(address)this,baseCDP); }
};

class Quantifier : public SynObject {
  DECLARE_DYNAMIC_CLASS(Quantifier)


  public:
  NESTEDANY/*Reference*/ elemType;
  CHAINX quantVars;
  NESTEDANY/*ObjReference*/ set;
  LavaDECL *typeDecl, *setElemDecl;
  virtual bool IsPlaceHolder()
  {
    return false;
  }
  bool IsRepeatableClause(CHAINX *&chx);
  virtual bool IsExecutable()
  {
    return false;
  }
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  Quantifier () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Quantifier*)from;
  }

  friend void CDPQuantifier (PutGetFlag pgf, ASN1* cid, address varAddr,
                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPQuantifier(pgf,cid,(address)this,baseCDP); }
};

class QuantStmOrExp : public Expression {
  DECLARE_DYNAMIC_CLASS(QuantStmOrExp)


  public:
  CHAINX quantifiers;
  NESTEDANY/*Expression*/ statement;
  unsigned nQuantVars;
  virtual bool IsExists()
  {
    return false;
  }
  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool Check(CheckData &ckd);
  virtual bool InitCheck(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  QuantStmOrExp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(QuantStmOrExp*)from;
  }

  friend void CDPQuantStmOrExp (PutGetFlag pgf, ASN1* cid, address varAddr,
                                bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPQuantStmOrExp(pgf,cid,(address)this,baseCDP); }
};

class Declare : public QuantStmOrExp {
  DECLARE_DYNAMIC_CLASS(Declare)


  public:
  virtual bool IsDeclare()
  {
    return true;
  }

  Declare () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Declare*)from;
  }

  friend void CDPDeclare (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDeclare(pgf,cid,(address)this,baseCDP); }
};

class Exists : public QuantStmOrExp {
  DECLARE_DYNAMIC_CLASS(Exists)


  public:
  NESTEDANY/*Expression*/ updateStatement;
  virtual bool NestedOptClause(SynObject *optClause);
  virtual bool IsExists()
  {
    return true;
  }

  Exists () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Exists*)from;
  }

  friend void CDPExists (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPExists(pgf,cid,(address)this,baseCDP); }
};

class Foreach : public Exists {
  DECLARE_DYNAMIC_CLASS(Foreach)


  Foreach () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Foreach*)from;
  }

  friend void CDPForeach (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPForeach(pgf,cid,(address)this,baseCDP); }
};

class SelectExpression : public QuantStmOrExp {
  DECLARE_DYNAMIC_CLASS(SelectExpression)


  public:
  NESTEDANY/*Expression*/ addObject, resultSet;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);

  SelectExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SelectExpression*)from;
  }

  friend void CDPSelectExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSelectExpression(pgf,cid,(address)this,baseCDP); }
};
;  //  end of the TYPE macro



/////////////////////////////////////////////////////////////////



#ifdef EXECVIEW


enum InsertMode {
  InsNested,
  InsChain,
  InsMult,
  InsFlags,
  ToggleArrows,
  ChgComment,
  ChgOp,
  DelNested,
  ReDelNested,
  DelChain,
  DelMult,
  DelFlags,
  PlusMinus
};

class CProgText : public CProgTextBase {
public:
  CProgText () { INIT(); }

	bool insBlank;

  QWidget *redCtl;
	QScrollArea *sv;


  void Insert(TToken token,bool isPrimToken=false,bool isOpt=false);
  void SynObjNewLine();
  void Blank ();

  void NewSel (const QPoint *point);
  void Select (SynObject *selObj=0);
  CHETokenNode *FindNextPlaceholder();
  CHETokenNode *FindPrecedingPlaceholder();
};

class CProgHTML : public CProgTextBase {
public:
  unsigned selBeg, selEnd;

  CProgHTML () {
    INIT();
    selBeg = 0;
    selEnd = 0;
  }

  void Insert(TToken token,bool isPrimToken=false,bool isOpt=false);
  virtual void NewLine ();
  void SynObjNewLine();
  unsigned GetLineIndent ();
  void Blank ();
  void WriteIndent ();
};

class TCommentV : public TComment {
};

class SynObjectV : public SynObject {
public:
  SynObjectV();
  SynObjectV(TToken t);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

struct TDODV : public TDOD {
  TDODV(){}
  TDODV(bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QString(QObject::tr("<p>This is a <a href=\"../ObjectLifeCycle.htm\">variable</a> reference</p>")); }
};

class ExpressionV : public Expression {
};

class OperationV : public Operation {
};

class ReferenceV : public Reference {
public:
  ReferenceV (){}
  ReferenceV (TToken token,TID refID,const char *refName);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class EnumConstV : public EnumConst {
public:
  EnumConstV (){}
  EnumConstV (TToken token,TID &id,QString &name);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ObjReferenceV : public ObjReference {
public:
  ObjReferenceV (){}
  ObjReferenceV (TDODC &refIDs,const char *refName);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class VarNameV : public VarName {
public:
  VarNameV (){}
  VarNameV (const char *vn);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QString(QObject::tr("<p>This is a <a href=\"Declare.htm\">local variable</a></p>")); }
};

class FormParmV : public FormParm {
public:
  FormParmV (){}
  FormParmV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class FormParmsV : public FormParms {
public:
  FormParmsV (){}
  FormParmsV (SynObject *selfVar);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class BaseInitV : public BaseInit {
public:
  BaseInitV (){}
  BaseInitV (address idt,SelfVar *selfVar,LavaDECL *formBase);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class SelfVarV : public SelfVar {
public:
  SelfVarV (){}

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ConstantV : public Constant {
public:
  ConstantV (){}
  ConstantV (const char *c);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class BoolConstV : public BoolConst {
public:
  BoolConstV (){};
  BoolConstV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class NullConstV : public NullConst {
public:
  NullConstV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class SucceedStatementV : public SucceedStatement {
public:
  SucceedStatementV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"FailSucceed.htm\">Succeed</a>: immediate affirmative/successful return from an <a href=\"../EditExec.htm#exec\">exec</a></p>"); }
};

class FailStatementV : public FailStatement {
public:
  FailStatementV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"FailSucceed.htm\">Fail</a>: immediate negative/unsuccessful return from an <a href=\"../EditExec.htm#exec\">exec</a>,"
    " optionally throw an exception</p>"); }
};

class OldExpressionV : public OldExpression {
public:
  OldExpressionV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Old.htm\">Old</a> value of a variable or expression (on entry to function)</p>");}
};

class UnaryOpV : public UnaryOp {
};

class InvertOpV : public InvertOp {
public:
  InvertOpV () {}
  InvertOpV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class HandleOpV : public HandleOp {
public:
  HandleOpV () {}
  HandleOpV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class OrdOpV : public OrdOp {
public:
  OrdOpV () {}
  OrdOpV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class MinusOpV : public MinusOp {
public:
  MinusOpV () {}
  MinusOpV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class LogicalNotV : public LogicalNot {
public:
  LogicalNotV () {}
  LogicalNotV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class EvalExpressionV : public EvalExpression {
public:
  EvalExpressionV () {}
  EvalExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class EvalStatementV : public EvalStatement {
public:
  EvalStatementV () {}
  EvalStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class InSetStatementV : public InSetStatement {
public:
  InSetStatementV () {}
  InSetStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
   return QObject::tr("<p><a href=\"InSet.htm\">Element in set</a> test</p>"); };
};

class BinaryOpV : public BinaryOp {
public:
  BinaryOpV () {}
  BinaryOpV (TToken primToken);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class MultipleOpV : public MultipleOp {
public:
  MultipleOpV (){}
  MultipleOpV (TToken primToken);
};


extern void Draw (MultipleOp *self,CProgTextBase &t,address where,CHAINX *chxp,bool ignored);

class SemicolonOpV : public SemicolonOp {
public:
  SemicolonOpV () { MultipleOpInit(Semicolon_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class AndOpV : public AndOp {
public:
  AndOpV () { MultipleOpInit(and_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class OrOpV : public OrOp {
public:
  OrOpV () { MultipleOpInit(or_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class XorOpV : public XorOp {
public:
  XorOpV () { MultipleOpInit(xor_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class BitAndOpV : public BitAndOp {
public:
  BitAndOpV () { MultipleOpInit(BitAnd_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class BitOrOpV : public BitOrOp {
public:
  BitOrOpV () { MultipleOpInit(BitOr_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class BitXorOpV : public BitXorOp {
public:
  BitXorOpV () { MultipleOpInit(BitXor_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class DivideOpV : public DivideOp {
public:
  DivideOpV () { MultipleOpInit(Slash_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class ModulusOpV : public ModulusOp {
public:
  ModulusOpV () { MultipleOpInit(Percent_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class LshiftOpV : public LshiftOp {
public:
  LshiftOpV () { MultipleOpInit(Lshift_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class RshiftOpV : public RshiftOp {
public:
  RshiftOpV () { MultipleOpInit(Rshift_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class PlusOpV : public PlusOp {
public:
  PlusOpV () { MultipleOpInit(Plus_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class MultOpV : public MultOp {
public:
  MultOpV () { MultipleOpInit(Mult_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class AssignmentV : public Assignment {
public:
  AssignmentV ();
  AssignmentV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Assign.htm\">Assignment statement</a></p>"); }
};

class ArrayAtIndexV : public ArrayAtIndex {
public:
  ArrayAtIndexV () {}
  ArrayAtIndexV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ParameterV : public Parameter {
public:
  ParameterV () {}
  ParameterV (SynObject *param);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class FuncExpressionV : public FuncExpression {
public:
  FuncExpressionV () {}
  FuncExpressionV (bool funcDisabled, bool staticFunc=false);
  FuncExpressionV (Reference *ref);
  FuncExpressionV (ObjReference *ref);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class FuncStatementV : public FuncStatement {
public:
  FuncStatementV ();
  FuncStatementV (bool funcDisabled, bool out=false, bool staticFunc=false);
  FuncStatementV (Reference *ref);
  FuncStatementV (ObjReference *ref, bool out=false);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>This is a <a href=\"MemberFunctions.htm\">Function</a> call statement</p>"); }
};

class ConnectV : public Connect {
public:
  ConnectV (){};
  ConnectV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Use the <b>connect</b> statement to connect a "
    "<a href=\"../Callbacks.htm\">software signal</a> to a signal handler (\"callback\")</p>");}
};

class DisconnectV : public Disconnect {
public:
  DisconnectV (){};
  DisconnectV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Use the <b>disconnect</b> statement to disconnect a "
    "<a href=\"../Callbacks.htm\">software signal</a> from a signal handler (\"callback\")</p>");}
};

class SignalV : public Signal {
public:
  SignalV(){}
  SignalV(bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Use the <b>signal</b> statement to emit a "
    "<a href=\"../Callbacks.htm\">software signal</a>, i.e., to call the signal handlers"
    " (\"callbacks\") connected to the signal</p>");}
};

class AssertStatementV : public AssertStatement {
public:
  AssertStatementV () {}
  AssertStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>An <a href=\"Assert.htm\">embedded assertion</a> is embedded anywhwere in executable code"
    " (in contrast to <a href=\"../DBC.htm\">attached assertions</a>)"
    " and throws a specific exception in case of violation</p>"); }
};

class IfThenV : public IfThen {
public:
  IfThenV () {}
  IfThenV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class IfStatementV : public IfStatement {
public:
  IfStatementV () {}
  IfStatementV (bool);

  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"IfStm.htm\">if-then-elsif-else</a> statement with\noptional elsif and else branches</p>"); }
};

class IfxThenV : public IfxThen {
public:
  IfxThenV () {}
  IfxThenV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"IfExpr.htm\">if-then-elsif-else</a> conditional expression with optional\nelsif and else branches</p>");}
};

class IfdefStatementV : public IfdefStatement {
public:
  IfdefStatementV () {}
  IfdefStatementV (bool);

  virtual void Insert2Optionals (SynObject *&thenPart,SynObject *&elsePart);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"IfdefElse.htm\">ifdef</a> statement with optional else branch: if variable != null then ...</p>"); }
};

class CondExpressionV : public CondExpression {
public:
  CondExpressionV () {}
};

class IfExpressionV : public IfExpression {
public:
  IfExpressionV () {}
  IfExpressionV (bool);

  virtual void InsertBranch (SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"IfExpr.htm\">if-then-elsif-else</a> conditional expression with optional\nelsif and else branches</p>");}
};

class ElseExpressionV : public ElseExpression {
public:
  ElseExpressionV () {}
  ElseExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>\"x <a href=\"IfdefElse.htm\">else</a> y\" yields x if x != null, else y</p>");}
};

class BranchV : public Branch {
public:
  BranchV () {}
  BranchV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Switch.htm\">switch</a> statement with optional else branch</p>");}
};

class SwitchStatementV : public SwitchStatement {
public:
  SwitchStatementV () {}
  SwitchStatementV (bool);

  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Switch.htm\">switch</a> statement with optional else branch</p>");}
};

class CatchClauseV : public CatchClause {
public:
  CatchClauseV () {}
  CatchClauseV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class TryStatementV : public TryStatement {
public:
  TryStatementV () {}
  TryStatementV (bool);

  void InsertBranch (SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Try.htm\">Try</a> a statement, catch exceptions</p>"); }
};

class TypeBranchV : public TypeBranch {
public:
  TypeBranchV () {}
  TypeBranchV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"TypeSwitch.htm\">type switch</a> statement with optional else branch</p>");}
};

class TypeSwitchStatementV : public TypeSwitchStatement {
public:
  TypeSwitchStatementV () {}
  TypeSwitchStatementV (bool);

  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"TypeSwitch.htm\">type switch</a> statement with optional else branch</p>");}
};

class AttachObjectV : public AttachObject {
public:
  AttachObjectV () {}
  AttachObjectV (bool);
  AttachObjectV (Reference *ref);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Attach.htm\">Attach</a> an existing <b><i><font color=\"red\">Lava</font></i></b> component object through one of its interfaces</p>"); }
};

class NewExpressionV : public NewExpression {
public:
  NewExpressionV () {}
  NewExpressionV (bool);
  NewExpressionV (Reference *ref,bool withItf,bool withLoc);
  NewExpressionV (FuncStatement *ref,bool withItf,bool withLoc);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Create a <a href=\"New.htm\">new</a> object</p>");}
};

class CloneExpressionV : public CloneExpression {
public:
  CloneExpressionV () {}
  CloneExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Clone.htm\">Clone</a> an existing <b><i><font color=\"red\">Lava</font></i></b> object</p>");}
};

class CopyStatementV : public CopyStatement {
public:
  CopyStatementV () {}
  CopyStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Copy.htm\">Copy</a> an existing <b><i><font color=\"red\">Lava</font></i></b> object onto another object</p>");}
};

class EnumItemV : public EnumItem {
public:
  EnumItemV () {}
  EnumItemV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Get an enumeration <a href=\"EnumItem.htm\">item</a> from its index</p>");}
};

class ExtendExpressionV : public ExtendExpression {
public:
  ExtendExpressionV () {}
  ExtendExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Add a <a href=\"Scale.htm\">scale</a> (e.g. \"Meters\", derived from float/double) to a raw object (e.g. \"3.5\"): 3.5 Meters</p>"); }
};

class RunV : public Run {
public:
  RunV () {}
  RunV (bool);
  RunV (Reference *ref);
  RunV (FuncStatement *ref);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Run.htm\">Run</a> a nested <a href=\"../Packages.htm#initiator\">main program</a></p>");}
};

class QueryItfV : public QueryItf {
public:
  QueryItfV () {}
  QueryItfV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"QryItf.htm\">Query interface:</a> Make another interface of a component object accessible, starting from a known interface</p>");}
};

class GetUUIDV : public GetUUID {
public:
  GetUUIDV () {}
  GetUUIDV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class IntegerIntervalV : public IntegerInterval {
public:
  IntegerIntervalV () {}
  IntegerIntervalV (bool);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class QuantifierV : public Quantifier {
public:
  QuantifierV () {}
  QuantifierV (bool withSet);

  virtual SynObject *InsertOptionals ();
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class QuantStmOrExpV : public QuantStmOrExp {
public:
  QuantStmOrExpV () {}
};

class DeclareV : public Declare {
public:
  DeclareV () {}
  DeclareV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Declare.htm\">Declare</a> local variables</p>"); };
};

class ExistsV : public Exists {
public:
  ExistsV () {}
  ExistsV (bool);

  virtual SynObject *InsertOptionals ();
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
   return QObject::tr("<p><a href=\"Exists.htm\">Existential quantifier</a> ranging \nover a finite set</p>"); };
};

class ForeachV : public Foreach {
public:
  ForeachV () {}
  ForeachV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
   return QObject::tr("<p><a href=\"Foreach.htm\">Universal quantifier</a> ranging \nover a finite set</p>"); };
};

class SelectExpressionV : public SelectExpression {
public:
  SelectExpressionV () {}
  SelectExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Select.htm\">Select</a> quantifier ranging \nover a finite set</p>"); }
};


extern LAVAEXECS_DLL void DrawExec (DString file,bool singleFile,CLavaBaseDoc *doc,LavaDECL *myDECL,SynObject *newObj);
extern LAVAEXECS_DLL void UpdateParameters (CheckData &ckd);

#endif


/////////////////////////////////////////////////////////////////


#ifdef INTERPRETER


class LAVAEXECS_DLL CVFuncDescX : public CVFuncDesc {
public:
  virtual bool Execute(SynObjectBase* obj, CheckData& ckd, LavaVariablePtr newStackFrame);
};

class TCommentX : public TComment {
public:
  TCommentX() {}
};

class SynObjectX : public SynObject {
public:
  SynObjectX() {}
};

struct TDODX : public TDOD {
public:
  TDODX() {}
};

class ExpressionX : public Expression {
public:
  ExpressionX() {}
};

class OperationX : public Operation {
public:
  OperationX() {}
};

class ReferenceX : public Reference {
public:
};

class EnumConstX : public EnumConst {
public:
  EnumConstX () { value = 0; enumItem = 0; }
  ~EnumConstX();

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class ObjReferenceX : public ObjReference {
public:
  ObjReferenceX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
  LavaObjectPtr GetPropertyInfo(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos, LavaDECL*& setExec, unsigned oldExprLevel);
  LavaObjectPtr GetMemberObjPtr(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos, unsigned oldExprLevel);
  LavaVariablePtr GetMemberVarPtr(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos, unsigned oldExprLevel);
  bool assign (SynObject *source,
               LavaObjectPtr object,
               TargetType targetType,
               unsigned sectionNumber,
               CheckData &ckd,
               LavaVariablePtr stackFrame,
               unsigned oldExprLevel);
};

class VarNameX : public VarName {
public:
  VarNameX() {}
};

class FormParmX : public FormParm {
public:
  FormParmX() {}
};

class FormParmsX : public FormParms {
public:
  FormParmsX() {}
};

class BaseInitX : public BaseInit {
public:
  BaseInitX() {}
};

class LAVAEXECS_DLL SelfVarX : public SelfVar {
public:
  SelfVarX() { 
    stackFrameSize = 0;
    funcDECL = 0;
    itfDECL = 0;
  }

  bool ExecBaseInits (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class ConstantX : public Constant {
public:
  ConstantX() {}
  ~ConstantX();

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class BoolConstX : public BoolConst {
public:
  BoolConstX() {}
  ~BoolConstX();

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class NullConstX : public NullConst {
public:
  NullConstX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) { return 0; }
};

class SucceedStatementX : public SucceedStatement {
public:
  SucceedStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class FailStatementX : public FailStatement {
public:
  FailStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class OldExpressionX : public OldExpression {
public:
  OldExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class UnaryOpX : public UnaryOp {
public:
  UnaryOpX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class InvertOpX : public UnaryOpX {
public:
  InvertOpX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class HandleOpX : public HandleOp {
public:
  HandleOpX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class OrdOpX : public UnaryOpX {
public:
  OrdOpX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class MinusOpX : public UnaryOpX {
public:
  MinusOpX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class LogicalNotX : public LogicalNot {
public:
  LogicalNotX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) {
    return !((SynObjectX*)operand.ptr)->Execute(ckd,stackFrame,oldExprLevel);}
};

class EvalExpressionX : public EvalExpression {
public:
  EvalExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class EvalStatementX : public EvalStatement {
public:
  EvalStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class InSetStatementX : public InSetStatement {
  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class BinaryOpX : public BinaryOp {
  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class MultipleOpX : public MultipleOp {
public:
  MultipleOpX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class SemicolonOpX : public MultipleOpX {
public:
  SemicolonOpX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class AndOpX : public SemicolonOpX {
public:
  AndOpX() {}
};

class OrOpX : public MultipleOpX {
public:
  OrOpX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class XorOpX : public MultipleOpX {
public:
  XorOpX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class BitAndOpX : public MultipleOpX {
public:
  BitAndOpX() {}
};

class BitOrOpX : public MultipleOpX {
public:
  BitOrOpX() {}
};

class BitXorOpX : public MultipleOpX {
public:
  BitXorOpX() {}
};

class DivideOpX : public MultipleOpX {
public:
  DivideOpX() {}
};

class ModulusOpX : public MultipleOpX {
public:
  ModulusOpX() {}
};

class LshiftOpX : public MultipleOpX {
public:
  LshiftOpX() {}
};

class RshiftOpX : public MultipleOpX {
public:
  RshiftOpX() {}
};

class PlusOpX : public MultipleOpX {
public:
  PlusOpX() {}
};

class MultOpX : public MultipleOpX {
public:
  MultOpX() {}
};

class AssignmentX : public Assignment {
public:
  AssignmentX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class ArrayAtIndexX : public ArrayAtIndex {
public:
  ArrayAtIndexX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class ParameterX : public Parameter {
public:
  ParameterX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class FuncExpressionX : public FuncExpression {
public:
  FuncExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class FuncStatementX : public FuncStatement {
public:
  FuncStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class ConnectX : public Connect {
public:
  ConnectX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class DisconnectX : public Disconnect {
public:
  DisconnectX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class SignalX : public Signal {
public:
  SignalX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class AssertStatementX : public AssertStatement {
public:
  AssertStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class IfThenX : public IfThen {
public:
  IfThenX() {}
};

class IfStatementX : public IfStatement {
public:
  IfStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class IfxThenX : public IfxThen {
public:
  IfxThenX() {}
};

class IfdefStatementX : public IfdefStatement {
public:
  IfdefStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class CondExpressionX : public CondExpression {
public:
  CondExpressionX() {}
};

class IfExpressionX : public IfExpression {
public:
  IfExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class ElseExpressionX : public ElseExpression {
public:
  ElseExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class BranchX : public Branch {
public:
  BranchX() {}
};

class SwitchStatementX : public SwitchStatement {
public:
  SwitchStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class TypeBranchX : public TypeBranch {
public:
  TypeBranchX() {}
};

class TypeSwitchStatementX : public TypeSwitchStatement {
public:
  TypeSwitchStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class CatchClauseX : public CatchClause {
public:
  CatchClauseX() {}

//  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class TryStatementX : public TryStatement {
public:
  TryStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class AttachObjectX : public AttachObject {
public:
  AttachObjectX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class NewExpressionX : public NewExpression {
public:
  NewExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class CloneExpressionX : public CloneExpression {
public:
  CloneExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class CopyStatementX : public CopyStatement {
public:
  CopyStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class EnumItemX : public EnumItem {
public:
  EnumItemX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class ExtendExpressionX : public ExtendExpression {
public:
  ExtendExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class RunX : public Run {
public:
  RunX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class QueryItfX : public QueryItf {
public:
  QueryItfX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class GetUUIDX : public GetUUID {
//public:
//  GetUUIDX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class IntegerIntervalX : public IntegerInterval {
//public:
//  IntegerIntervalX() {}
};

class QuantifierX : public Quantifier {
//public:
//  QuantifierX() {}
};

class DeclareX : public Declare {
public:
//  DeclareX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
};

class QuantStmOrExpX : public QuantStmOrExp {
//public:
//  QuantStmOrExpX() {}
};

class ExistsX : public Exists {
public:
//  ExistsX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
  bool Recursion (CheckData &ckd, LavaVariablePtr stackFrame,unsigned oldExprLevel,CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet=0);
};

class ForeachX : public Foreach {
public:
//  ForeachX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
  bool Recursion (CheckData &ckd, LavaVariablePtr stackFrame,unsigned oldExprLevel,CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet=0);
};

class SelectExpressionX : public SelectExpression {
public:
//  SelectExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
  bool Recursion (CheckData &ckd, LavaVariablePtr stackFrame,unsigned oldExprLevel,CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet);
};

#endif

#endif
