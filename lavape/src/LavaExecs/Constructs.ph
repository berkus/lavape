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


$INCLUDE "Tokens.ph"
$INCLUDE "Syntax.ph"

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
class FuncStatement;
class Visitor;


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


$TYPE {

class VarAction {
public:
  CLavaBaseDoc *doc;
  unsigned tempNo;
  bool inIgnore, inExecHeader, inFormParms, inBaseInits, inParameter, inForeach;

  VarAction ();
  virtual bool Action (CheckData &ckd, VarName *vn, TID &typeTid)=0;
  virtual void CheckLocalScope(CheckData &ckd, SynObject *synObj);
};

class LocalVarSearch : public VarAction {
public:
  TID *toBeFound;
  bool found;

  LocalVarSearch (CLavaBaseDoc *d, TID *tid);
  virtual bool Action (CheckData &ckd, VarName *vn, TID &tid);
};

class LocalVarByNameSearch : public VarAction {
public:
  STRING toBeFound;
  VarName *varName;
  bool found;

  LocalVarByNameSearch (STRING name);
  virtual bool Action (CheckData &ckd, VarName *vn, TID &tid);
};

class TokenNode {
public:
  SynObject *synObject;
  TToken token;
  QString str;
  QRect rect;
  SET flags;
  int newLines, indent;

  TokenNode ();
  bool OptionalClauseToken (SynObject *&optClause);
};

typedef CHAINANY<TokenNode> CHToken;

class CVarDesc {
public:
  CVarDesc (){}

  TID varID;
  CVarDesc *parent;
  bool writeAccess;
  CHAINANY<CVarDesc> subTree;
};

class CRefEntry {
public:
  CRefEntry (){}

  virtual bool IsBranch () { return false; }
  virtual bool IsBranchStm () { return false; }
  virtual bool IsEndBranchStm () { return false; }
  virtual bool IsWriteAccess () { return false; }
  virtual bool IsFailSucceed () { return false; }
};

class CBranch : public CRefEntry {
public:
  CHE *branchStm, *precedingBranch;

  CBranch () {}

  CBranch (CHE *brStm,CHE *lastBr) { branchStm = brStm; precedingBranch = lastBr; }

  virtual bool IsBranch () { return true; }
};

class CBranchStm : public CBranch {
public:
  CBranchStm () {}

  virtual bool IsBranchStm () { return true; }
};

class CEndBranchStm : public CBranch {
public:
  CEndBranchStm (CHE *brStm,CHE *lastBr) { branchStm = brStm; precedingBranch = lastBr; }

  virtual bool IsEndBranchStm () { return true; }
};

class CWriteAccess : public CRefEntry {
public:
  CWriteAccess (){
    closedVarLevel = 0;
  }

  ObjReference *objRef;
  CVarDesc *varDesc;
  unsigned closedVarLevel;

  virtual bool IsWriteAccess () { return true; }
  bool Contains (ObjReference *objRef);
};

class CFailSucceed : public CRefEntry {
public:
  CFailSucceed (){}

  virtual bool IsFailSucceed () { return true; }
};

typedef CHAINANY<CVarDesc> CObjRefTable;
typedef CHAINANY<unsigned> CBranchStack;
typedef CHAINX/*CRefEntry*/ CRefTableEntries;

} //end of $TYPE-macro


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


$TYPE +CDP {

enum ExecFlags {
  staticCall,         //0
  ignoreSynObj,       //1
  newLine,            //2
  isOpt,              //3
  isDisabled,         //4
  primToken,          //5
  abstractPP,         //6
  brokenRef,          //7
  insertBlank,        //8
  isLocalVar,         //9
  isTempVar,          //10
  isStateObjMember,   //11
  isInputVar,         //12
  isOutputVar,        //13
  isSelfVar,          //14
  isMemberVar,        //15
  isDeclareVar,       //16
  isVariable,         //17
  inExecHdr,          //18
  inputArrow,         //19
  isSameAsSelf,       //20
  isSubstitutable,    //21
  isUnknownCat,       //22
  isInForeach,        //23
  free0,              //24
  free1,              //25
  free2,              //26
  isIniClauseVar,     //27
  isOptionalExpr,     //28
  isReverseLink,      //29
  isClosed,           //30
  isIniCallOrHandle   //31
};

enum WorkFlags {
  isBrkPnt,
  isTempPoint
};


class TComment {
public:
  STRING str;
  bool inLine, trailing;

  TComment() { inLine = false; trailing = false; }
};

class SynObject : public SynObjectBase {
public:
  QWidget *execView;
  TToken primaryToken, type, replacedType;
  SynObject-- *parentObject;
  FuncStatement-- *iniCall;
  NESTED<TComment> comment;
  address-- whereInParent;
  CHAINX-- *containingChain;
  SET flags; //ExecFlags
  SET-- workFlags;
  CHETokenNode-- *startToken, *endToken, *primaryTokenNode;
  CHAINX-- errorChain;
  CHE-- *oldError;
  unsigned-- oldColor, synObjectID;
  int-- startPos, startPosHTML, currentIndent;
  QString-- *lastError;
  bool-- errorChanged;

  SynObject();
  virtual ~SynObject() {}

  virtual void MakeExpression() {
    primaryToken = Exp_T;
    type = Exp_T;
    replacedType = Exp_T;
  }

  virtual void NLincIndent (CProgTextBase &text);
  virtual void NLdecIndent (CProgTextBase &text);

  bool BoolAdmissibleOnly (CheckData &ckd);
  bool EnumAdmissibleOnly (CheckData &ckd);
  bool NullAdmissible (CheckData &ckd);
  bool ExpressionSelected (CHETokenNode *currentSelection);
  virtual bool IsOptional(CheckData &ckd) { return flags.Contains(isOptionalExpr); }
  bool HasOptionalParts ();
  bool IsDefChecked(CheckData &ckd);
  virtual ROContext ReadOnlyContext();
  virtual bool IsReadOnlyClause(SynObject *synObj) {
    return false; }
  virtual bool InOldExpression();
  virtual bool InInitializer (CheckData &ckd);
  virtual bool InHiddenIniClause (CheckData &ckd, SynObject *&synObj);
  virtual bool IsArrayObj();
  virtual bool IsAssigTarget();
  virtual bool IsBinaryOp () { return false; }
  virtual bool IsInSetStm () { return false; }
  virtual bool IsIntIntv () { return false; }
  virtual bool IsEnumType () { return false; }
  virtual bool IsConstant () { return false; }
  virtual bool IsEditableConstant () { return false; }
  virtual bool IsDeclare () { return false; }
  virtual bool IsExpression ();
  virtual bool IsIfStmExpr () {return false; }
  bool IsFuncHandle ();
  virtual bool IsFuncInvocation () { return false; }
  bool IsIfClause ();
  virtual bool IsMultOp () { return false; }
  bool IsObjRef ();
  bool IsOutputParam ();
  virtual bool IsOperation () { return false; }
  virtual bool IsPlaceHolder () { return true; }
  virtual bool IsRepeatableClause (CHAINX *&chx) { return false; }
  virtual bool IsExecutable() { return true; }
  virtual bool IsSelfVar() { return false; }
  bool IsStatement ();
  virtual bool IsThrow () { return false; }
  virtual bool IsUnaryOp () { return false; }
  virtual bool NestedOptClause (SynObject *optClause) { return false; }
  bool StatementSelected (CHETokenNode *currentSelection);
  bool SameExec (LavaDECL *decl);

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable(address table, int inINCL, SynObjectBase* parent, TTableUpdate update, address where=0, CHAINX *chx=0, address searchData=0);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  bool UpdateReference (CheckData &ckd);
  virtual SynObject *InsertOptionals () { return 0; }
  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx) {}
  virtual void InsertBranch (SynObject *&branch, CHAINX *&chx) {}
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored){}
  void SetError(CheckData &ckd,QString *error,char *textParam=0);
  void SetRTError(CheckData &ckd,QString *error,LavaVariablePtr stackFrame,const char *textParam=0);
  QString DebugStop(CheckData &ckd,LavaVariablePtr stack,QString excMsg,StopReason sr,LavaVariablePtr calleeStack,LavaDECL *calleeFunc);
  QString LocationOfConstruct ();
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) { decl = 0; cat = unknownCategory; }
  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
  virtual LavaObjectPtr Evaluate(CheckData  &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel);
  virtual bool Recursion (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel, CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet=0) { return false;};
  virtual QString whatsThisText();
  virtual QString whatNextText();
};


struct TDOD : public SynObject {
  STRING-- name;
  TID ID;  // ID of the field definition
  LavaDECL-- *fieldDecl;
  int-- sectionNumber;  //section number of member in the previous effective type
  bool-- isProperty;
  CContext-- context;
  LavaDECL-- *vType; //(virtual) type of field
  LavaDECL-- *eType; //effective type of field
  int-- vSectionNumber; //section number where FMVT is defined (in analogy to funcSectionNumber)
  bool-- isOuter;  //virtual type(FMVT) of formal parameter is in the outer virtual type table of call object

  TDOD(); // required for InitCheck in Interpreter
  virtual bool IsPlaceHolder () { return false; };
  bool IsStateObject (CheckData &ckd);
  virtual bool IsExecutable() { return false; }
  bool accessTypeOK (SynFlags accessFlags);
  bool ReplaceWithLocalParm(CheckData &ckd, LavaDECL *funcDecl,TDeclType declType);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

typedef CHAINX/*TDOD*/ TDODC;


class Expression : public SynObject {
public:

  Expression () {
    closedLevel = 0;
  }
  
  int-- closedLevel;
  
  int ClosedLevel(CheckData &ckd, bool ofBaseVar=false);
  
  LavaDECL-- *formVType, *finalType;
  int-- sectionNumber; // section number of formal parameter type in the type of actual parameter
  int-- vSectionNumber; //section number where FMVT is defined (in analogy to funcSectionNumber)
  bool-- isOuter;  //virtual type(formVType) of formal parameter is in the outer virtual type table of call object
  Category-- targetCat;

  virtual bool IsPlaceHolder () { return false; }
  virtual bool CallCheck (CheckData &ckd);
  virtual LavaDECL* FuncDecl () {
    return 0;
  }
};

class Operation : public Expression {
public:
  TID opFunctionID;
  LavaDECL-- *funcDecl;
  unsigned-- funcSectionNumber;
  CContext-- callCtx;

  virtual bool IsOperation () { return true; }
};

class Reference : public SynObject {
public:
  Reference () {}
  Reference (TToken token,TID id,const char *name);

  STRING-- refName;
  TID refID;
  LavaDECL-- *refDecl;

  virtual bool IsPlaceHolder () { return false; }
  virtual bool IsExecutable();
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
};

class EnumConst : public Expression {
public:
  TID refID; // of enum type
  STRING Id; // item name
  LavaDECL-- *refDecl;
  int-- enumItem;  // item number
  LavaObjectPtr-- value, enumBaseObj;

  EnumConst () {}

  virtual bool IsConstant () { return true; };
  virtual bool IsEditableConstant () { return false; };
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool Check (CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
};

class ObjReference : public Expression {
public:
  STRING-- refName;
  TDODC refIDs;
  unsigned-- stackPos;
  ObjReference-- *conflictingAssig;
  LavaDECL-- *myFinalVType;
  CContext-- myContext;
  Category-- myCategory;

  ObjReference (){}
  ObjReference (TDODC &refIDs,const char *refName); // required for InitCheck in Interpreter
  virtual ~ObjReference() {}

  bool operator== (ObjReference &objRef);
  virtual bool IsPlaceHolder () { return false; }
  virtual bool IsOptional (CheckData &ckd);
  bool InConstituent (CheckData &ckd);
  bool Inherited (CheckData &ckd);
  bool OutOfScope (CheckData &ckd);
  VarName *PrimaryVar (CheckData &ckd);
  virtual VarRefContext Context ();
  virtual bool AssignCheck (CheckData &ckd,VarRefContext cty);
  virtual bool CopyCheck (CheckData &ckd);
  virtual bool ReadCheck (CheckData &ckd);
  virtual bool CallCheck (CheckData &ckd);
  virtual bool ArrayTargetCheck (CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class VarName : public Expression {
public:
  STRING varName;
  TID varID;
  VarName-- *nextLocal;
  unsigned-- stackPos, iniOrder;

  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  Reference *TypeRef ();
  virtual bool IsPlaceHolder () { return false; }
  virtual bool IsExecutable() { return false; }
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class FormParm : public Expression {
public:
  FormParm () {}
  FormParm (bool);

  NESTEDANY<Reference> parmType;
  NESTEDANY<ObjReference> formParm;
  TID formParmID;

  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
}--;

class FormParms : public Expression {
public:
  FormParms () {}
  FormParms (SynObject *selfVar);

  CHAINX/*FormParm*/ inputs, outputs;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
}--;

class BaseInit : public Expression {
public:
  NESTEDANY<Reference> baseItf;
  NESTEDANY<FuncStatement> initializerCall;
  NESTEDANY<ObjReference>-- self; //for virtual base classes
  NESTEDANY<Reference>-- baseItf2; //for virtual base classes

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class SelfVar : public VarName {
public:
  TID typeID;
  NESTEDANY<Reference> execName;
  NESTEDANY<FormParms>-- formParms;
  CHAINX/*BaseInits*/ baseInitCalls; // non-empty only in initializer execs
  NESTEDANY<Expression> body;
  FormParms-- *oldFormParms;
  LavaDECL-- *execDECL, *funcDECL, *itfDECL, *selfType;
  unsigned-- nParams, nInputs, nOutputs, stackFrameSize, firstOldExprExec, inINCL;
  bool-- concernExecs, checked, isFuncBody, isInvariant;
  CContext-- selfCtx;

  SelfVar () {
    checked = false;
    concernExecs = false;
    execView = 0;
    isFuncBody = false;
    isInvariant = false;
  }
  virtual bool IsEmptyExec();
  virtual bool IsSelfVar() { return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  bool BaseInitsCheck (CheckData &ckd);
  virtual bool Check (CheckData &ckd);
  bool InitCheck (CheckData &ckd, bool inSelfCheck=true);
  bool InputCheck (CheckData &ckd);
  bool OutputCheck (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class Constant : public Expression {
public:
  TBasicType constType;
  STRING str;
  LavaObjectPtr-- value;
  LavaDECL-- *typeDECL;

  Constant () { value = 0; }

  virtual bool Check (CheckData &ckd);
  virtual bool IsConstant () { return true; };
  virtual bool IsEditableConstant () { return true; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
};

class BoolConst : public Expression {
public:
  bool boolValue;
  LavaObjectPtr-- value;
  LavaDECL-- *typeDECL;

  BoolConst () { value = 0; }

  virtual bool Check (CheckData &ckd);
  virtual bool IsConstant () { return true; };
  virtual bool IsEditableConstant () { return false; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
};

class NullConst : public Expression {
public:

  virtual bool Check (CheckData &ckd);
  virtual bool IsConstant () { return true; };
  virtual bool IsEditableConstant () { return true; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
};

class SucceedStatement : public Expression {
public:
  virtual bool Check (CheckData &ckd);
};

class FailStatement : public Expression {
public:
  NESTEDANY<Expression> exception;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  bool IsThrow() { return true; }
};

class OldExpression : public Expression {
public:
  NESTEDANY<Expression> paramExpr;
  unsigned-- iOldExpr;

  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class UnaryOp : public Operation {
public:
  NESTEDANY<Expression> operand;

  virtual bool IsUnaryOp () { return true; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  LavaDECL* FuncDecl () {
    return funcDecl;
  }
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class EvalExpression : public UnaryOp {
public:

  virtual bool IsUnaryOp() { return false; }
  virtual bool IsReadOnlyClause(SynObject *synObj) { return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class EvalStatement : public EvalExpression {
public:
  virtual bool Check (CheckData &ckd);
};

class ArrayAtIndex : public Operation {
public:
  NESTEDANY<Expression> arrayObj, arrayIndex;
  int-- funcSectionNumber;
  bool-- setCase;

  virtual bool IsPlaceHolder () { return false; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class InvertOp : public UnaryOp {
};

class HandleOp : public Expression {
  NESTEDANY<ObjReference> operand;

  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class OrdOp : public UnaryOp {
};

class MinusOp : public UnaryOp {
};

class LogicalNot : public UnaryOp {
public:
  virtual bool IsReadOnlyClause(SynObject *synObj) {
    return true; }
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class InSetStatement : public Expression {
public:
  NESTEDANY<Expression> operand1, operand2;

  virtual bool IsInSetStm () { return true; };
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class BinaryOp : public Operation {
public:
  NESTEDANY<Expression> operand1, operand2;

  virtual bool IsBinaryOp () { return true; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  LavaDECL* FuncDecl () {
    return funcDecl;
  }
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class MultipleOp : public Operation {
public:
  CHAINX/*SynObject*/ operands;

  MultipleOp () {};
  void MultipleOpInit (TToken primToken);

  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool IsMultOp () { return true; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  LavaDECL* FuncDecl () {
    return funcDecl;
  }
};

class SemicolonOp : public MultipleOp {
};

class AndOp : public MultipleOp {
};

class OrOp : public MultipleOp {
};

class XorOp : public MultipleOp {
};

class BitAndOp : public MultipleOp {
};

class BitOrOp : public MultipleOp {
};

class BitXorOp : public MultipleOp {
};

class DivideOp : public MultipleOp {
};

class ModulusOp : public MultipleOp {
};

class LshiftOp : public MultipleOp {
};

class RshiftOp : public MultipleOp {
};

class PlusOp : public MultipleOp {
};

class MultOp : public MultipleOp {
};

class Assignment : public Expression {
public:
  NESTEDANY<ObjReference> targetObj;
  NESTEDANY<Expression> exprValue;
  TargetType-- kindOfTarget;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class Parameter : public Expression {
public:
  NESTEDANY<Expression> parameter;
  TID formParmID;
  TargetType-- kindOfTarget;  // for output parameters: field/property/array elem.

  virtual bool IsOptional (CheckData &ckd) { return ((SynObject*)parameter.ptr)->IsOptional(ckd); };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class FuncExpression : public Expression {
public:
  CHAINX/*Parameter*/ inputs;
  unsigned-- nParams /* self counts as a parameter, too */;
  NESTEDANY<Reference> function;
  NESTEDANY<ObjReference> handle;
  TID vtypeID;
  int-- funcSectionNumber;
  int-- vBaseSectionNumber;
  bool-- vBaseIsOuter;
  LavaDECL-- *funcDecl, *objTypeDecl,  *vBaseType;
  CContext-- callCtx;
  Category-- callObjCat;
//  bool-- isStateObj;
  SynFlags-- myCtxFlags;

  virtual bool IsFuncInvocation () { return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual LavaDECL* FuncDecl () {
    return funcDecl; //ckd.document->IDTable.GetDECL(((Reference*)function.ptr)->refID,ckd.inINCL);
  }
};

class FuncStatement : public FuncExpression {
public:
  CHAINX/*Parameter*/ outputs;
  SynObject-- *varName; // reverse link to var[PH] decl from ini-clauses of declare

  FuncStatement (){};
  FuncStatement (Reference *ref);

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

struct Connect : public Expression {
public:
  NESTEDANY<Expression> signalSender;
  NESTEDANY<Expression> signalSenderClass;
  NESTEDANY<Reference> signalFunction;
  NESTEDANY<FuncStatement> callback;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

struct Disconnect : public Expression {
public:
  NESTEDANY<Expression> signalSender;
  NESTEDANY<Reference> signalFunction;
  NESTEDANY<Expression> signalReceiver;
  NESTEDANY<Reference> callbackFunction;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class Signal : public Expression {
public:
  NESTEDANY<Expression> sCall;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class AssertStatement : public Expression {
public:
  NESTEDANY<Expression> statement;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsReadOnlyClause(SynObject *synObj) {
    return true; }
};

class IgnoreStatement : public Expression {
public:
  CHAINX/*ObjReference*/ igVars;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsReadOnlyClause(SynObject *synObj) {
    return true; }
};

class IfThen : public Expression {
public:
  NESTEDANY<Expression> ifCondition;
  NESTEDANY<Expression> thenPart;
  CHETokenNode-- *thenToken;

  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool IsRepeatableClause (CHAINX *&chx);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class IfStatement : public Expression {
public:
  CHAINX/*IfThen*/ ifThens;
  NESTEDANY<Expression> elsePart;

  virtual bool IsIfStmExpr () {return true; }
  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class IfxThen : public Expression {
public:
  NESTEDANY<Expression> ifCondition;
  NESTEDANY<Expression> thenPart;
  CHETokenNode-- *thenToken;

  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool IsRepeatableClause (CHAINX *&chx);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class IfdefStatement : public Expression {
public:
  CHAINX/*ObjReference*/ ifCondition;
  NESTEDANY<Expression> thenPart;
  NESTEDANY<Expression> elsePart;

  bool Checks (ObjReference &objRef);
  virtual bool IsIfStmExpr () {return true; }
  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class CondExpression : public Expression {
public:
  LavaDECL-- *targetDecl;
  CContext-- targetCtx;
  Category-- callObjCat;
  bool-- isOpt;

  CondExpression ();
};

class IfExpression : public CondExpression {
public:
  CHAINX/*IfxThen*/ ifThens;
  NESTEDANY<Expression> elsePart;

  IfExpression () {};

  virtual bool IsIfStmExpr () {return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class ElseExpression : public CondExpression {
public:
  NESTEDANY<Expression> expr1, expr2;

  ElseExpression () {};

  virtual bool IsIfStmExpr () {return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class Branch : public Expression {
public:
  CHAINX/*Constant*/ caseLabels;
  NESTEDANY<Expression> thenPart;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsRepeatableClause (CHAINX *&chx);
  virtual bool IsExecutable() { return false; }
};

class SwitchStatement : public Expression {
public:
  NESTEDANY<Expression> caseExpression;
  CHAINX/*Branch*/ branches;
  NESTEDANY<Expression> elsePart;

  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class CatchClause : public Expression {
public:
  NESTEDANY<Reference> exprType;
  NESTEDANY<VarName> varName;
  NESTEDANY<Expression> catchClause;
  LavaDECL-- *typeDecl;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsRepeatableClause (CHAINX *&chx);
  virtual bool IsExecutable() { return false; }
};

class TryStatement : public Expression {
public:
  NESTEDANY<Expression> tryStatement;
  CHAINX/*CatchClause*/ catchClauses;
//  NESTEDANY<Expression> elsePart;

//  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class TypeBranch : public Expression {
public:
  NESTEDANY<Reference> exprType;
  NESTEDANY<VarName> varName;
  NESTEDANY<Expression> thenPart;
  LavaDECL-- *typeDecl;
//  int-- sectionNumber;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool IsRepeatableClause (CHAINX *&chx);
  virtual bool IsExecutable() { return false; }
};

class TypeSwitchStatement : public Expression {
public:
  NESTEDANY<Expression> caseExpression;
  CHAINX/*TypeBranch*/ branches;
  NESTEDANY<Expression> elsePart;
  LavaDECL-- *declSwitchExpression;
  Category-- catSwitchExpression;

  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class AttachObject : public Expression {
public:
  NESTEDANY<Reference> objType;
  NESTEDANY<Reference> itf;
  NESTEDANY<Expression> url;
  LavaDECL-- *typeDECL, *execDECL;
  Category-- attachCat;

  virtual bool NestedOptClause (SynObject *optClause);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
  virtual bool Check (CheckData &ckd);
};

class NewExpression : public AttachObject {
public:
  NESTEDANY<VarName> varName;
  NESTEDANY<FuncStatement> initializerCall;
  NESTEDANY<Expression> butStatement;
  bool-- errorInInitializer;

  virtual bool NestedOptClause (SynObject *optClause);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class CloneExpression : public Expression {
  NESTEDANY<VarName> varName;
  NESTEDANY<Expression> fromObj;
  NESTEDANY<Expression> butStatement;
  LavaDECL-- *execDECL;

  virtual bool IsOptional (CheckData &ckd) { return ((SynObject*)fromObj.ptr)->IsOptional(ckd); };
  virtual bool NestedOptClause (SynObject *optClause);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class CopyStatement : public Expression {
  NESTEDANY<Expression> fromObj;
  NESTEDANY<ObjReference> ontoObj;
  TargetType-- kindOfTarget;
  LavaDECL-- *ontoTypeDecl;

  virtual bool IsOptional (CheckData &ckd) { return ((SynObject*)fromObj.ptr)->IsOptional(ckd); };
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class EnumItem : public Expression {
public:
  NESTEDANY<Expression> itemNo;
  NESTEDANY<Reference> enumType;

  void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class ExtendExpression : public Expression {
public:
  NESTEDANY<Expression> extendObj;
  NESTEDANY<Reference> extendType;
  LavaDECL-- *extendTypeDecl;

  void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class Run : public Expression {
public:
  NESTEDANY<Reference> initiator;
  CHAINX/*Parameter*/ inputs;
  LavaDECL-- *execDECL, *typeDECL;
  unsigned-- nParams;

  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class QueryItf : public Expression {
public:
  NESTEDANY<Reference> itf;
  NESTEDANY<ObjReference> givenObj;

  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class GetUUID : public Expression {
public:
  NESTEDANY<Reference> itf;

  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class IntegerInterval : public SynObject {
public:
  NESTEDANY<Expression> from, to;

  virtual bool IsPlaceHolder () { return false; }
  virtual bool IsIntIntv () { return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class Quantifier : public SynObject {
public:
  NESTEDANY<Reference> elemType;
  CHAINX/*VarName*/ quantVars;
  NESTEDANY<ObjReference> set;
  LavaDECL-- *typeDecl, *setElemDecl;
//  int-- sectionNumber;

  virtual bool IsPlaceHolder () { return false; }
  bool IsRepeatableClause (CHAINX *&chx);
  virtual bool IsExecutable() { return false; }
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class QuantStmOrExp : public Expression {
public:
  CHAINX/*Quantifier*/ quantifiers;
  NESTEDANY<Expression> primaryClause;
  unsigned-- nQuantVars;

  virtual bool IsExists () { return false; }
  virtual bool IsReadOnlyClause(SynObject *synObj);
  virtual bool Check (CheckData &ckd);
  virtual bool InitCheck (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

class Declare : public QuantStmOrExp {
public:
  NESTEDANY<Expression> secondaryClause;
  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool IsDeclare () { return true; }
};

class Exists : public QuantStmOrExp {
public:
  NESTEDANY<Expression> secondaryClause;

  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool IsExists () { return true; }
};

class Foreach : public Exists {
};

class SelectExpression : public QuantStmOrExp {
public:
  NESTEDANY<Expression> addObject, resultSet;

  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void Accept (Visitor &visitor,SynObject *parent=0,address where=0,CHAINX *chxp=0);
};

};  //  end of the TYPE macro



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

class IgnoreStatementV : public IgnoreStatement {
public:
  IgnoreStatementV () {}
  IgnoreStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Ignore a mandatory input parameter</p>");}
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

class IgnoreStatementX : public IgnoreStatement {
public:
  IgnoreStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel) { return true; };
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
  unsigned currIniOrder;
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
  currIniCallChp = 0;
  currIniCall = 0;
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
    maxClosedLevel = iniVar->iniOrder;
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
