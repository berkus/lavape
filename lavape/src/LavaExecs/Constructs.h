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

#pragma hdrstop

#include "qwidget.h"
#include <QScrollArea>
#include "qstring.h"
#include "qmessagebox.h"
#include "qpainter.h"
#include <QList>
#include <QHash>
#include "LavaAppBase.h"
#include "LavaBaseDoc.h"
#include "LavaBaseStringInit.h"
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
class FuncStatement;
class Visitor;



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
    closedVarLevel=0;
  }
  ObjReference *objRef;
  CVarDesc *varDesc;
  unsigned closedVarLevel;
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
  QString *ReadCheck (CheckData &ckd, ObjReference *objRef,CWriteAccess *&writeAcc);
  QString *findMatchingAccess (CheckData &ckd,
                          CHE *&chp,
                          CVarDesc *refEntry,
                          bool nestedCall,
                          bool &isAssigned,
                          ObjReference *objRef,
                          CWriteAccess *&writeAcc);
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
       showComments, leftArrows, parmNames, ignored, singleFile;
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
  free0,
  free1,
  free2,
  isIniClauseVar,
  isOptionalExpr,
  isReverseLink,
  isClosed,
  isIniCallOrHandle};

extern void CDPExecFlags (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

enum WorkFlags {
  isBrkPnt,
  isTempPoint};

extern void CDPWorkFlags (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

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

  friend void CDPTComment (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTComment(pgf,cid,this,baseCDP); }
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
  FuncStatement *iniCall;
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
  virtual void MakeTable(address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where=0,CHAINX *chx=0,address searchData=0);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
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

  friend void CDPSynObject (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSynObject(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  virtual void CopyData (AnyType *from) {
    *this = *(TDOD*)from;
  }

  friend void CDPTDOD (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTDOD(pgf,cid,this,baseCDP); }
};

typedef CHAINX TDODC;

extern void CDPTDODC (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

class Expression : public SynObject {
  DECLARE_DYNAMIC_CLASS(Expression)


  public:

  Expression()
  {
    closedLevel=0;
  }
  int closedLevel;
  int ClosedLevel(CheckData &ckd,bool ofBaseVar=false);
  LavaDECL *formVType, *finalType;
  int sectionNumber;
  int vSectionNumber;
  bool isOuter;
  Category targetCat;
  virtual bool IsPlaceHolder()
  {
    return false;
  }
  virtual bool CallCheck(CheckData &ckd);
  virtual LavaDECL *FuncDecl()
  {
    return 0;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(Expression*)from;
  }

  friend void CDPExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPExpression(pgf,cid,this,baseCDP); }
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

  friend void CDPOperation (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPOperation(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);

  virtual void CopyData (AnyType *from) {
    *this = *(Reference*)from;
  }

  friend void CDPReference (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPReference(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool Check(CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);

  virtual void CopyData (AnyType *from) {
    *this = *(EnumConst*)from;
  }

  friend void CDPEnumConst (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEnumConst(pgf,cid,this,baseCDP); }
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
  VarName *PrimaryVar(CheckData &ckd);
  virtual VarRefContext Context();
  virtual bool AssignCheck(CheckData &ckd,VarRefContext cty);
  virtual bool CopyCheck(CheckData &ckd);
  virtual bool ReadCheck(CheckData &ckd);
  virtual bool CallCheck(CheckData &ckd);
  virtual bool ArrayTargetCheck(CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  virtual void CopyData (AnyType *from) {
    *this = *(ObjReference*)from;
  }

  friend void CDPObjReference (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPObjReference(pgf,cid,this,baseCDP); }
};

class VarName : public Expression {
  DECLARE_DYNAMIC_CLASS(VarName)


  public:
  STRING varName;
  TID varID;
  VarName *nextLocal;
  unsigned stackPos, varIndex;
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  VarName () {}

  virtual void CopyData (AnyType *from) {
    *this = *(VarName*)from;
  }

  friend void CDPVarName (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPVarName(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  virtual void CopyData (AnyType *from) {
    *this = *(FormParm*)from;
  }

  friend void CDPFormParm (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFormParm(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  virtual void CopyData (AnyType *from) {
    *this = *(FormParms*)from;
  }

  friend void CDPFormParms (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFormParms(pgf,cid,this,baseCDP); }
};

class BaseInit : public Expression {
  DECLARE_DYNAMIC_CLASS(BaseInit)


  public:
  NESTEDANY/*Reference*/ baseItf;
  NESTEDANY/*FuncStatement*/ initializerCall;
  NESTEDANY/*ObjReference*/ self;
  NESTEDANY/*Reference*/ baseItf2;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  BaseInit () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BaseInit*)from;
  }

  friend void CDPBaseInit (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBaseInit(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  virtual void CopyData (AnyType *from) {
    *this = *(SelfVar*)from;
  }

  friend void CDPSelfVar (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSelfVar(pgf,cid,this,baseCDP); }
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

  friend void CDPConstant (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPConstant(pgf,cid,this,baseCDP); }
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

  friend void CDPBoolConst (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBoolConst(pgf,cid,this,baseCDP); }
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

  friend void CDPNullConst (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPNullConst(pgf,cid,this,baseCDP); }
};

class SucceedStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(SucceedStatement)


  public:
  virtual bool Check(CheckData &ckd);

  SucceedStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SucceedStatement*)from;
  }

  friend void CDPSucceedStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSucceedStatement(pgf,cid,this,baseCDP); }
};

class FailStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(FailStatement)


  public:
  NESTEDANY/*Expression*/ exception;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  bool IsThrow()
  {
    return true;
  }

  FailStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(FailStatement*)from;
  }

  friend void CDPFailStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFailStatement(pgf,cid,this,baseCDP); }
};

class OldExpression : public Expression {
  DECLARE_DYNAMIC_CLASS(OldExpression)


  public:
  NESTEDANY/*Expression*/ paramExpr;
  unsigned iOldExpr;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  OldExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(OldExpression*)from;
  }

  friend void CDPOldExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPOldExpression(pgf,cid,this,baseCDP); }
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
  LavaDECL *FuncDecl()
  {
    return funcDecl;
  }
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  UnaryOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(UnaryOp*)from;
  }

  friend void CDPUnaryOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPUnaryOp(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  EvalExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(EvalExpression*)from;
  }

  friend void CDPEvalExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEvalExpression(pgf,cid,this,baseCDP); }
};

class EvalStatement : public EvalExpression {
  DECLARE_DYNAMIC_CLASS(EvalStatement)


  public:
  virtual bool Check(CheckData &ckd);

  EvalStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(EvalStatement*)from;
  }

  friend void CDPEvalStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEvalStatement(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  ArrayAtIndex () {}

  virtual void CopyData (AnyType *from) {
    *this = *(ArrayAtIndex*)from;
  }

  friend void CDPArrayAtIndex (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPArrayAtIndex(pgf,cid,this,baseCDP); }
};

class InvertOp : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(InvertOp)


  InvertOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(InvertOp*)from;
  }

  friend void CDPInvertOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPInvertOp(pgf,cid,this,baseCDP); }
};

class HandleOp : public Expression {
  DECLARE_DYNAMIC_CLASS(HandleOp)

  NESTEDANY/*ObjReference*/ operand;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  HandleOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(HandleOp*)from;
  }

  friend void CDPHandleOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPHandleOp(pgf,cid,this,baseCDP); }
};

class OrdOp : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(OrdOp)


  OrdOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(OrdOp*)from;
  }

  friend void CDPOrdOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPOrdOp(pgf,cid,this,baseCDP); }
};

class MinusOp : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(MinusOp)


  MinusOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(MinusOp*)from;
  }

  friend void CDPMinusOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPMinusOp(pgf,cid,this,baseCDP); }
};

class LogicalNot : public UnaryOp {
  DECLARE_DYNAMIC_CLASS(LogicalNot)


  public:
  virtual bool IsReadOnlyClause(SynObject *synObj)
  {
    return true;
  }
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  LogicalNot () {}

  virtual void CopyData (AnyType *from) {
    *this = *(LogicalNot*)from;
  }

  friend void CDPLogicalNot (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPLogicalNot(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  InSetStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(InSetStatement*)from;
  }

  friend void CDPInSetStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPInSetStatement(pgf,cid,this,baseCDP); }
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
  LavaDECL *FuncDecl()
  {
    return funcDecl;
  }
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  BinaryOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BinaryOp*)from;
  }

  friend void CDPBinaryOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBinaryOp(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  LavaDECL *FuncDecl()
  {
    return funcDecl;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(MultipleOp*)from;
  }

  friend void CDPMultipleOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPMultipleOp(pgf,cid,this,baseCDP); }
};

class SemicolonOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(SemicolonOp)


  SemicolonOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SemicolonOp*)from;
  }

  friend void CDPSemicolonOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSemicolonOp(pgf,cid,this,baseCDP); }
};

class AndOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(AndOp)


  AndOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(AndOp*)from;
  }

  friend void CDPAndOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPAndOp(pgf,cid,this,baseCDP); }
};

class OrOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(OrOp)


  OrOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(OrOp*)from;
  }

  friend void CDPOrOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPOrOp(pgf,cid,this,baseCDP); }
};

class XorOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(XorOp)


  XorOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(XorOp*)from;
  }

  friend void CDPXorOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPXorOp(pgf,cid,this,baseCDP); }
};

class BitAndOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(BitAndOp)


  BitAndOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BitAndOp*)from;
  }

  friend void CDPBitAndOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBitAndOp(pgf,cid,this,baseCDP); }
};

class BitOrOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(BitOrOp)


  BitOrOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BitOrOp*)from;
  }

  friend void CDPBitOrOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBitOrOp(pgf,cid,this,baseCDP); }
};

class BitXorOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(BitXorOp)


  BitXorOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(BitXorOp*)from;
  }

  friend void CDPBitXorOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBitXorOp(pgf,cid,this,baseCDP); }
};

class DivideOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(DivideOp)


  DivideOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(DivideOp*)from;
  }

  friend void CDPDivideOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDivideOp(pgf,cid,this,baseCDP); }
};

class ModulusOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(ModulusOp)


  ModulusOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(ModulusOp*)from;
  }

  friend void CDPModulusOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPModulusOp(pgf,cid,this,baseCDP); }
};

class LshiftOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(LshiftOp)


  LshiftOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(LshiftOp*)from;
  }

  friend void CDPLshiftOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPLshiftOp(pgf,cid,this,baseCDP); }
};

class RshiftOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(RshiftOp)


  RshiftOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(RshiftOp*)from;
  }

  friend void CDPRshiftOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPRshiftOp(pgf,cid,this,baseCDP); }
};

class PlusOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(PlusOp)


  PlusOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(PlusOp*)from;
  }

  friend void CDPPlusOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPPlusOp(pgf,cid,this,baseCDP); }
};

class MultOp : public MultipleOp {
  DECLARE_DYNAMIC_CLASS(MultOp)


  MultOp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(MultOp*)from;
  }

  friend void CDPMultOp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPMultOp(pgf,cid,this,baseCDP); }
};

class Assignment : public Expression {
  DECLARE_DYNAMIC_CLASS(Assignment)


  public:
  NESTEDANY/*ObjReference*/ targetObj;
  NESTEDANY/*Expression*/ exprValue;
  TargetType kindOfTarget;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  Assignment () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Assignment*)from;
  }

  friend void CDPAssignment (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPAssignment(pgf,cid,this,baseCDP); }
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
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  Parameter () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Parameter*)from;
  }

  friend void CDPParameter (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPParameter(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual LavaDECL *FuncDecl()
  {
    return funcDecl;
  }

  FuncExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(FuncExpression*)from;
  }

  friend void CDPFuncExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFuncExpression(pgf,cid,this,baseCDP); }
};

class FuncStatement : public FuncExpression {
  DECLARE_DYNAMIC_CLASS(FuncStatement)


  public:
  CHAINX outputs;
  SynObject *varName;

  FuncStatement()
  {
  }

  FuncStatement(Reference *ref);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  virtual void CopyData (AnyType *from) {
    *this = *(FuncStatement*)from;
  }

  friend void CDPFuncStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPFuncStatement(pgf,cid,this,baseCDP); }
};

struct Connect : public Expression {
  DECLARE_DYNAMIC_CLASS(Connect)


  public:
  NESTEDANY/*Expression*/ signalSender;
  NESTEDANY/*Expression*/ signalSenderClass;
  NESTEDANY/*Reference*/ signalFunction;
  NESTEDANY/*FuncStatement*/ callback;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  Connect () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Connect*)from;
  }

  friend void CDPConnect (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPConnect(pgf,cid,this,baseCDP); }
};

struct Disconnect : public Expression {
  DECLARE_DYNAMIC_CLASS(Disconnect)


  public:
  NESTEDANY/*Expression*/ signalSender;
  NESTEDANY/*Reference*/ signalFunction;
  NESTEDANY/*Expression*/ signalReceiver;
  NESTEDANY/*Reference*/ callbackFunction;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  Disconnect () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Disconnect*)from;
  }

  friend void CDPDisconnect (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDisconnect(pgf,cid,this,baseCDP); }
};

class Signal : public Expression {
  DECLARE_DYNAMIC_CLASS(Signal)


  public:
  NESTEDANY/*Expression*/ sCall;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  Signal () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Signal*)from;
  }

  friend void CDPSignal (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSignal(pgf,cid,this,baseCDP); }
};

class AssertStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(AssertStatement)


  public:
  NESTEDANY/*Expression*/ statement;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsReadOnlyClause(SynObject *synObj)
  {
    return true;
  }

  AssertStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(AssertStatement*)from;
  }

  friend void CDPAssertStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPAssertStatement(pgf,cid,this,baseCDP); }
};

class IgnoreStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(IgnoreStatement)


  public:
  CHAINX igVars;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsReadOnlyClause(SynObject *synObj)
  {
    return true;
  }

  IgnoreStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IgnoreStatement*)from;
  }

  friend void CDPIgnoreStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIgnoreStatement(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  IfThen () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IfThen*)from;
  }

  friend void CDPIfThen (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfThen(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  IfStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IfStatement*)from;
  }

  friend void CDPIfStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfStatement(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  IfxThen () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IfxThen*)from;
  }

  friend void CDPIfxThen (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfxThen(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  IfdefStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IfdefStatement*)from;
  }

  friend void CDPIfdefStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfdefStatement(pgf,cid,this,baseCDP); }
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

  friend void CDPCondExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCondExpression(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  virtual void CopyData (AnyType *from) {
    *this = *(IfExpression*)from;
  }

  friend void CDPIfExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIfExpression(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  virtual void CopyData (AnyType *from) {
    *this = *(ElseExpression*)from;
  }

  friend void CDPElseExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPElseExpression(pgf,cid,this,baseCDP); }
};

class Branch : public Expression {
  DECLARE_DYNAMIC_CLASS(Branch)


  public:
  CHAINX caseLabels;
  NESTEDANY/*Expression*/ thenPart;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsRepeatableClause(CHAINX *&chx);
  virtual bool IsExecutable()
  {
    return false;
  }

  Branch () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Branch*)from;
  }

  friend void CDPBranch (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPBranch(pgf,cid,this,baseCDP); }
};

class SwitchStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(SwitchStatement)


  public:
  NESTEDANY/*Expression*/ caseExpression;
  CHAINX branches;
  NESTEDANY/*Expression*/ elsePart;
  virtual bool NestedOptClause(SynObject *optClause);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  SwitchStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SwitchStatement*)from;
  }

  friend void CDPSwitchStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSwitchStatement(pgf,cid,this,baseCDP); }
};

class CatchClause : public Expression {
  DECLARE_DYNAMIC_CLASS(CatchClause)


  public:
  NESTEDANY/*Reference*/ exprType;
  NESTEDANY/*VarName*/ varName;
  NESTEDANY/*Expression*/ catchClause;
  LavaDECL *typeDecl;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsRepeatableClause(CHAINX *&chx);
  virtual bool IsExecutable()
  {
    return false;
  }

  CatchClause () {}

  virtual void CopyData (AnyType *from) {
    *this = *(CatchClause*)from;
  }

  friend void CDPCatchClause (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCatchClause(pgf,cid,this,baseCDP); }
};

class TryStatement : public Expression {
  DECLARE_DYNAMIC_CLASS(TryStatement)


  public:
  NESTEDANY/*Expression*/ tryStatement;
  CHAINX catchClauses;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  TryStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TryStatement*)from;
  }

  friend void CDPTryStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTryStatement(pgf,cid,this,baseCDP); }
};

class TypeBranch : public Expression {
  DECLARE_DYNAMIC_CLASS(TypeBranch)


  public:
  NESTEDANY/*Reference*/ exprType;
  NESTEDANY/*VarName*/ varName;
  NESTEDANY/*Expression*/ thenPart;
  LavaDECL *typeDecl;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsRepeatableClause(CHAINX *&chx);
  virtual bool IsExecutable()
  {
    return false;
  }

  TypeBranch () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TypeBranch*)from;
  }

  friend void CDPTypeBranch (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTypeBranch(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  TypeSwitchStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TypeSwitchStatement*)from;
  }

  friend void CDPTypeSwitchStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTypeSwitchStatement(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool Check(CheckData &ckd);

  AttachObject () {}

  virtual void CopyData (AnyType *from) {
    *this = *(AttachObject*)from;
  }

  friend void CDPAttachObject (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPAttachObject(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  NewExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(NewExpression*)from;
  }

  friend void CDPNewExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPNewExpression(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  CloneExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(CloneExpression*)from;
  }

  friend void CDPCloneExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCloneExpression(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  CopyStatement () {}

  virtual void CopyData (AnyType *from) {
    *this = *(CopyStatement*)from;
  }

  friend void CDPCopyStatement (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCopyStatement(pgf,cid,this,baseCDP); }
};

class EnumItem : public Expression {
  DECLARE_DYNAMIC_CLASS(EnumItem)


  public:
  NESTEDANY/*Expression*/ itemNo;
  NESTEDANY/*Reference*/ enumType;
  void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  EnumItem () {}

  virtual void CopyData (AnyType *from) {
    *this = *(EnumItem*)from;
  }

  friend void CDPEnumItem (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEnumItem(pgf,cid,this,baseCDP); }
};

class ExtendExpression : public Expression {
  DECLARE_DYNAMIC_CLASS(ExtendExpression)


  public:
  NESTEDANY/*Expression*/ extendObj;
  NESTEDANY/*Reference*/ extendType;
  LavaDECL *extendTypeDecl;
  void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  ExtendExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(ExtendExpression*)from;
  }

  friend void CDPExtendExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPExtendExpression(pgf,cid,this,baseCDP); }
};

class Run : public Expression {
  DECLARE_DYNAMIC_CLASS(Run)


  public:
  NESTEDANY/*Reference*/ initiator;
  CHAINX inputs;
  LavaDECL *execDECL, *typeDECL;
  unsigned nParams;
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  Run () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Run*)from;
  }

  friend void CDPRun (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPRun(pgf,cid,this,baseCDP); }
};

class QueryItf : public Expression {
  DECLARE_DYNAMIC_CLASS(QueryItf)


  public:
  NESTEDANY/*Reference*/ itf;
  NESTEDANY/*ObjReference*/ givenObj;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  QueryItf () {}

  virtual void CopyData (AnyType *from) {
    *this = *(QueryItf*)from;
  }

  friend void CDPQueryItf (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPQueryItf(pgf,cid,this,baseCDP); }
};

class GetUUID : public Expression {
  DECLARE_DYNAMIC_CLASS(GetUUID)


  public:
  NESTEDANY/*Reference*/ itf;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  GetUUID () {}

  virtual void CopyData (AnyType *from) {
    *this = *(GetUUID*)from;
  }

  friend void CDPGetUUID (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPGetUUID(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  IntegerInterval () {}

  virtual void CopyData (AnyType *from) {
    *this = *(IntegerInterval*)from;
  }

  friend void CDPIntegerInterval (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPIntegerInterval(pgf,cid,this,baseCDP); }
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
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  Quantifier () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Quantifier*)from;
  }

  friend void CDPQuantifier (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPQuantifier(pgf,cid,this,baseCDP); }
};

class QuantStmOrExp : public Expression {
  DECLARE_DYNAMIC_CLASS(QuantStmOrExp)


  public:
  CHAINX quantifiers;
  NESTEDANY/*Expression*/ primaryClause;
  unsigned nQuantVars;
  virtual bool IsExists()
  {
    return false;
  }
  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool Check(CheckData &ckd);
  virtual bool InitCheck(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  QuantStmOrExp () {}

  virtual void CopyData (AnyType *from) {
    *this = *(QuantStmOrExp*)from;
  }

  friend void CDPQuantStmOrExp (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPQuantStmOrExp(pgf,cid,this,baseCDP); }
};

class Declare : public QuantStmOrExp {
  DECLARE_DYNAMIC_CLASS(Declare)


  public:
  NESTEDANY/*Expression*/ secondaryClause;
  virtual bool NestedOptClause(SynObject *optClause);
  virtual bool IsDeclare()
  {
    return true;
  }

  Declare () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Declare*)from;
  }

  friend void CDPDeclare (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDeclare(pgf,cid,this,baseCDP); }
};

class Exists : public QuantStmOrExp {
  DECLARE_DYNAMIC_CLASS(Exists)


  public:
  NESTEDANY/*Expression*/ secondaryClause;
  virtual bool NestedOptClause(SynObject *optClause);
  virtual bool IsExists()
  {
    return true;
  }

  Exists () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Exists*)from;
  }

  friend void CDPExists (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPExists(pgf,cid,this,baseCDP); }
};

class Foreach : public Exists {
  DECLARE_DYNAMIC_CLASS(Foreach)


  Foreach () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Foreach*)from;
  }

  friend void CDPForeach (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPForeach(pgf,cid,this,baseCDP); }
};

class SelectExpression : public QuantStmOrExp {
  DECLARE_DYNAMIC_CLASS(SelectExpression)


  public:
  NESTEDANY/*Expression*/ addObject, resultSet;
  virtual void ExprGetFVType(CheckData &ckd,LavaDECL *&decl,Category &cat,SynFlags &ctxFlags);
  virtual bool Check(CheckData &ckd);
  virtual void Accept(Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);

  SelectExpression () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SelectExpression*)from;
  }

  friend void CDPSelectExpression (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSelectExpression(pgf,cid,this,baseCDP); }
};
;  //  end of the TYPE macro


/////////////////////////////////////////////////////////////////


class Visitor {
public:
  
  bool finished;
  
  Visitor () {
	  finished = false;
  }
  
  virtual void Eval (SynObject *self,SynObject *parent,address where,CHAINX *chxp) {}

  virtual bool evalHandle(FuncExpression *fs) {
	return true;
  }

  virtual void VisitSynObject (SynObject *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitExpression (Expression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitOperation (Operation *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitReference (Reference *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitEnumConst (EnumConst *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitObjReference (ObjReference *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitTDOD (TDOD *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitVarName (VarName *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitFormParm (FormParm *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitFormParms (FormParms *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitBaseInit (BaseInit *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitSelfVar (SelfVar *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitConstant (Constant *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitBoolConst (BoolConst *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitNullConst (NullConst *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitSucceedStatement (SucceedStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitFailStatement (FailStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitOldExpression (OldExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitUnaryOp (UnaryOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitEvalExpression (EvalExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitEvalStatement (EvalStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitArrayAtIndex (ArrayAtIndex *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitInvertOp (InvertOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitHandleOp (HandleOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitOrdOp (OrdOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitMinusOp (MinusOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitLogicalNot (LogicalNot *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitInSetStatement (InSetStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitBinaryOp (BinaryOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitMultipleOp (MultipleOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitSemicolonOp (SemicolonOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitAndOp (AndOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitOrOp (OrOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitXorOp (XorOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitBitAndOp (BitAndOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitBitOrOp (BitOrOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitBitXorOp (BitXorOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitDivideOp (DivideOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitModulusOp (ModulusOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitLshiftOp (LshiftOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitRshiftOp (RshiftOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitPlusOp (PlusOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitMultOp (MultOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitAssignment (Assignment *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitParameter (Parameter *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitFuncExpression (FuncExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitFuncStatement (FuncStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitSignal (Signal *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitConnect (Connect *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitDisconnect (Disconnect *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitAssertStatement (AssertStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitIgnoreStatement (IgnoreStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitIfThen (IfThen *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitIfStatement (IfStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitIfxThen (IfxThen *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitIfdefStatement (IfdefStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitCondExpression (CondExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitIfExpression (IfExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitElseExpression (ElseExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitBranch (Branch *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitSwitchStatement (SwitchStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitCatchClause (CatchClause *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitTryStatement (TryStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitTypeBranch (TypeBranch *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitTypeSwitchStatement (TypeSwitchStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitAttachObject (AttachObject *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitNewExpression (NewExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitCloneExpression (CloneExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitCopyStatement (CopyStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitEnumItem (EnumItem *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitExtendExpression (ExtendExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitRun (Run *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitQueryItf (QueryItf *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitGetUUID (GetUUID *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitIntegerInterval (IntegerInterval *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitQuantifier (Quantifier *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitQuantStmOrExp (QuantStmOrExp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitDeclare (Declare *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitExists (Exists *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitForeach (Foreach *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
  virtual void VisitSelectExpression (SelectExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0) {}
};

class TableVisitor : public Visitor {
public:
  address table;
  int inINCL;
  SynObject *parent;
  FuncStatement *currIniCall;
  CHE *currIniCallChp;
//  unsigned currIniOrder;
  TTableUpdate update;
  address where;
  CHAINX *chxp;
  address searchData;
  
  TableVisitor (
	    address table,
	    int inINCL,
	    TTableUpdate update,
	    address searchData) {
    this->table = table;
    this->inINCL = inINCL;
    this->update = update;
    this->searchData = searchData;
//    currIniCallChp = 0;
//    currIniCall = 0;
  }
	
  virtual void Eval (SynObject *self,SynObject *parent,address where,CHAINX *chxp);
  
  virtual void VisitSynObject (SynObject *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitEnumConst (EnumConst *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitParameter (Parameter *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitFormParm (FormParm *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitReference (Reference *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitTDOD (TDOD *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitObjReference (ObjReference *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitSelfVar (SelfVar *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitUnaryOp (UnaryOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitBinaryOp (BinaryOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitMultipleOp (MultipleOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitVarName (VarName *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitArrayAtIndex (ArrayAtIndex *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  //virtual void VisitIgnoreStatement (IgnoreStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class ClosedLevelVisitor : public Visitor {
public:
  CLavaBaseDoc *document;
  int maxClosedLevel;
  VarName *currIniVar;
  ClosedLevelVisitor *predecessor;

  ClosedLevelVisitor (CLavaBaseDoc *doc, VarName *iniVar, ClosedLevelVisitor *pred) {
    document = doc;
    maxClosedLevel = iniVar->varIndex;
    currIniVar = iniVar;
    predecessor = pred;
  }
  
  bool evalHandle(FuncExpression *fs) {
    return !fs->flags.Contains(isIniCallOrHandle);
  }

  bool inRecursion (VarName *vn) {
    if (vn == currIniVar) return true;
    if (predecessor && predecessor->inRecursion(vn)) return true;
    return false;
  }
	  
  virtual void VisitParameter (Parameter *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitObjReference (ObjReference *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitUnaryOp (UnaryOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitBinaryOp (BinaryOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitMultipleOp (MultipleOp *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitFuncExpression (FuncExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitFuncStatement (FuncStatement *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitOldExpression (OldExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitEvalExpression (EvalExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitArrayAtIndex (ArrayAtIndex *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitIfExpression (IfExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitElseExpression (ElseExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitNewExpression (NewExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitCloneExpression (CloneExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitExtendExpression (ExtendExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void VisitSelectExpression (SelectExpression *obj,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

#endif
