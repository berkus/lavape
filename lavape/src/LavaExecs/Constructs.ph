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

#ifdef __GNUC__
#pragma interface
#endif

$INCLUDE "Tokens.ph"
$INCLUDE "Syntax.ph"

#include "qwidget.h"
#include "qscrollview.h"
#include "qstring.h"
#include "qmessagebox.h"
#include "qpainter.h"
#include "LavaBaseDoc.h"
#include "SynIDTable.h"
#include "wx_obj.h"
//#include "StdAfx.h"

#ifdef WIN32
#ifdef LAVAEXECS_EXPORT
#define LAVAEXECS_DLL __declspec( dllexport )
#else
#define LAVAEXECS_DLL __declspec( dllimport )
#endif
#else
#define LAVAEXECS_DLL
#endif


#define leftMargin 5
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
  InBranch
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
  inputParam
};


class SynObject;
class ObjReference;
class VarName;

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
  CWriteAccess (){}

  ObjReference *objRef;
  CVarDesc *varDesc;

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
  QString *ReadCheck (CheckData &ckd, ObjReference *objRef);
  QString *findMatchingAccess (CheckData &ckd,
                          CHE *&chp,
                          CVarDesc *refEntry,
                          bool nestedCall,
                          bool &isAssigned,
                          ObjReference *objRef);
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
  virtual void Insert(TToken token,bool isPrimToken=false,bool isOpt=false/*CHE *pred*/)=0;
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


$TYPE +CDP {

enum ConstrFlags {
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
  unused2,            //22
  isInForeach,        //23
  unfinishedAllowed,  //24
  isHexConst,         //25
  isOctConst,         //26
  isUnsafeMandatory,  //27
  isOptionalExpr,     //28
  isReverseLink       //29
};

class TComment {
public:
  STRING str;
  bool inLine, trailing;

  TComment() { inLine = false; trailing = false; }
};

class SynObject : public SynObjectBase {
public:
  TToken primaryToken, type, replacedType;
  SynObject-- *parentObject;
  NESTED<TComment> comment;
  address-- whereInParent;
  CHAINX-- *containingChain;
  SET flags;
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
  bool HasOptionalParts ();
  virtual bool InReadOnlyContext(); // = InReadOnlyClause or in read-only Exec
  virtual bool InReadOnlyClause();
  virtual bool IsReadOnlyClause(SynObject *synObj, bool &roExec) {
    roExec = false; return false; }
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
  virtual bool IsOptional (CheckData &ckd) { return true; };
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
  virtual bool IsSelfVar() { return false; }
  bool IsStatement ();
  virtual bool IsThrow () { return false; }
  virtual bool IsUnaryOp () { return false; }
  virtual bool NestedOptClause (SynObject *optClause) { return false; }
  bool StatementSelected (CHETokenNode *currentSelection);
  bool SameExec (LavaDECL *decl);

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address,int inINCL,SynObjectBase *,TTableUpdate,address,CHAINX *,address searchData=0);
  bool UpdateReference (CheckData &ckd);
  virtual SynObject *InsertOptionals () { return 0; }
  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx) {}
  virtual void InsertBranch (SynObject *&branch, CHAINX *&chx) {}
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored){}
  void SetError(CheckData &ckd,QString *error,char *textParam=0);
  void SetRTError(CheckData &ckd,QString *error,LavaVariablePtr stackFrame,const char *textParam=0);
  QString CallStack(CheckData &ckd,LavaVariablePtr stack);
  QString LocationOfConstruct ();
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags) { decl = 0; cat = unknownCategory; }
  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
  virtual LavaObjectPtr Evaluate(CheckData  &ckd, LavaVariablePtr stackFrame);
  virtual bool Recursion (CheckData &ckd, LavaVariablePtr stackFrame, CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet=0) { return false;};
  virtual void whatNext() {
    QMessageBox::critical(qApp->mainWidget(),qApp->name(),QObject::tr("\"What next\" help not yet available for this construct"),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton); }
  virtual QString whatsThisText() {
    return QString(QObject::tr("<p>\"What's this\" help not yet available for this construct</p>")); }
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
  virtual bool IsOptional (CheckData &ckd) { return flags.Contains(isOptionalExpr); }
  bool IsStateObject (CheckData &ckd);
  bool accessTypeOK (SynFlags accessFlags);
  bool ReplaceWithLocalParm(CheckData &ckd, LavaDECL *funcDecl,TDeclType declType);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

typedef CHAINX/*TDOD*/ TDODC;


class Expression : public SynObject {
public:
  LavaDECL-- *formVType, *finalType;
  int-- sectionNumber; // section number of formal parameter type in the type of actual parameter
  int-- vSectionNumber; //section number where FMVT is defined (in analogy to funcSectionNumber)
  bool-- isOuter;  //virtual type(formVType) of formal parameter is in the outer virtual type table of call object
  Category-- targetCat;

  virtual bool IsPlaceHolder () { return false; }
};

class Operation : public Expression {
public:
  TID opFunctionID;
  LavaDECL-- *funcDecl;
  unsigned-- funcSectionNumber;
  CContext-- callCtx;

  virtual bool IsPlaceHolder () { return false; }
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
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
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
  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual bool IsEditableConstant () { return false; };
  virtual void MakeTable (address,int inINCL,SynObjectBase *,TTableUpdate,address,CHAINX *,address);
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
  virtual bool IsPlaceHolder () { return false; }
  virtual bool IsOptional (CheckData &ckd);
  bool InConstituent (CheckData &ckd);
  bool Inherited (CheckData &ckd);
  bool OutOfScope (CheckData &ckd);
  virtual VarRefContext Context ();
  virtual bool AssignCheck (CheckData &ckd,VarRefContext cty);
  virtual bool CopyCheck (CheckData &ckd);
  virtual bool ReadCheck (CheckData &ckd);
  virtual bool CallCheck (CheckData &ckd);
  virtual bool ArrayTargetCheck (CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class VarName : public Expression {
public:
  STRING varName;
  TID varID;
  VarName-- *nextLocal;
  unsigned-- stackPos;

  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  Reference *TypeRef ();
  virtual bool IsPlaceHolder () { return false; }
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class FormParm : public Expression {
public:
  FormParm () {}
  FormParm (bool);

  NESTEDANY<Reference> parmType;
  NESTEDANY<ObjReference> formParm;
  TID formParmID;

  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
}--;

class FormParms : public Expression {
public:
  FormParms () {}
  FormParms (SynObject *selfVar);

  CHAINX/*FormParm*/ inputs, outputs;

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
}--;

class BaseInit : public Expression {
public:
  NESTEDANY<Reference> baseItf;
  NESTEDANY<FuncStatement> initializerCall;
  NESTEDANY<ObjReference>-- self; //for virtual base classes
  NESTEDANY<Reference>-- baseItf2; //for virtual base classes

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class SelfVar : public VarName {
public:
  TID typeID;
  NESTEDANY<Reference> execName;
  NESTEDANY<FormParms>-- formParms;
  CHAINX/*BaseInits*/ baseInitCalls; // non-empty only in initializer execs
  NESTEDANY<Expression> body;
  FormParms-- *oldFormParms;
  LavaDECL-- *execDECL, *myDECL, *selfType;
  wxView-- *myView;
  unsigned-- nParams, nInputs, nOutputs, stackFrameSize, inINCL;
  bool-- concernExecs, checked;
  CContext-- selfCtx;

  SelfVar () { checked = false; concernExecs = false; myView = 0;}
  virtual bool IsEmptyExec();
  virtual bool IsSelfVar() { return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  bool BaseInitsCheck (CheckData &ckd);
  virtual bool Check (CheckData &ckd);
  bool InitCheck (CheckData &ckd, bool inSelfCheck=true);
  bool InputCheck (CheckData &ckd);
  bool OutputCheck (CheckData &ckd);
  virtual bool IsReadOnlyClause(SynObject *synObj, bool &roExec);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
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
  virtual bool IsOptional (CheckData &ckd) { return false; };
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
  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual bool IsEditableConstant () { return false; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
};

class NullConst : public Expression {
public:

  virtual bool Check (CheckData &ckd);
  virtual bool IsConstant () { return true; };
  virtual bool IsOptional (CheckData &ckd) { return false; };
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
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class UnaryOp : public Operation {
public:
  NESTEDANY<Expression> operand;

  virtual bool IsUnaryOp () { return true; };
  virtual bool IsOptional (CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class EvalExpression : public UnaryOp {
public:

  virtual bool IsUnaryOp() { return false; }
  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual bool IsReadOnlyClause(SynObject *synObj, bool &roExec) {
    roExec = false; return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
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
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class InvertOp : public UnaryOp {
};

class HandleOp : public Expression {
  NESTEDANY<ObjReference> operand;

  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class OrdOp : public UnaryOp {
};

class MinusOp : public UnaryOp {
};

class LogicalNot : public UnaryOp {
public:
  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual bool IsReadOnlyClause(SynObject *synObj, bool &roExec) {
    roExec = false; return true; }
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class InSetStatement : public Expression {
public:
  NESTEDANY<Expression> operand1, operand2;

  virtual bool IsInSetStm () { return true; };
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class BinaryOp : public Operation {
public:
  NESTEDANY<Expression> operand1, operand2;

  virtual bool IsBinaryOp () { return true; };
  virtual bool IsOptional (CheckData &ckd);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class MultipleOp : public Operation {
public:
  CHAINX/*SynObject*/ operands;

  MultipleOp () {};
  void MultipleOpInit (TToken primToken);

  virtual bool IsOptional (CheckData &ckd);
  virtual bool IsReadOnlyClause(SynObject *synObj, bool &roExec);
  virtual bool IsMultOp () { return true; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
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
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class Parameter : public Expression {
public:
  NESTEDANY<Expression> parameter;
  TID formParmID;
  TargetType-- kindOfTarget;  // for output parameters: field/property/array elem.

  virtual bool IsOptional (CheckData &ckd) { return ((SynObject*)parameter.ptr)->IsOptional(ckd); };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
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

  virtual bool IsOptional (CheckData &ckd);
  virtual bool IsFuncInvocation () { return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class FuncStatement : public FuncExpression {
public:
  CHAINX/*Parameter*/ outputs;

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

struct Callback : public Expression {
  NESTEDANY<Reference> callbackType;
  NESTEDANY<FuncStatement> callback;
  NESTEDANY<Expression> onEvent;

  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class AssertStatement : public Expression {
public:
  NESTEDANY<Expression> statement;

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class ThrowStatement : public Expression {
public:
  NESTEDANY<Expression> error;

  virtual bool IsThrow () { return true; }
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class IfThen : public Expression {
public:
  NESTEDANY<Expression> ifCondition;
  NESTEDANY<Expression> thenPart;
  CHETokenNode-- *thenToken;

  virtual bool IsReadOnlyClause(SynObject *synObj, bool &roExec);
  virtual bool IsRepeatableClause (CHAINX *&chx);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class IfStatement : public Expression {
public:
  CHAINX/*IfThen*/ ifThens;
  NESTEDANY<Expression> elsePart;

  virtual bool IsIfStmExpr () {return true; }
  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class IfxThen : public Expression {
public:
  NESTEDANY<Expression> ifCondition;
  NESTEDANY<Expression> thenPart;
  CHETokenNode-- *thenToken;

  virtual bool IsReadOnlyClause(SynObject *synObj, bool &roExec);
  virtual bool IsRepeatableClause (CHAINX *&chx);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class IfExpression : public Expression {
public:
  CHAINX/*IfxThen*/ ifThens;
  NESTEDANY<Expression> elsePart;
  LavaDECL-- *targetDecl;
  CContext-- targetCtx;
  Category-- callObjCat;
  bool-- isOpt;

  IfExpression ();

  virtual bool IsIfStmExpr () {return true; }
  virtual bool IsOptional (CheckData &ckd) { return true; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class Branch : public Expression {
public:
  CHAINX/*Constant*/ caseLabels;
  NESTEDANY<Expression> thenPart;

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool IsRepeatableClause (CHAINX *&chx);
};

class SwitchStatement : public Expression {
public:
  NESTEDANY<Expression> caseExpression;
  CHAINX/*Branch*/ branches;
  NESTEDANY<Expression> elsePart;

  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class CatchClause : public Expression {
public:
  NESTEDANY<Expression> catchClause;

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool IsRepeatableClause (CHAINX *&chx);
};

class TryStatement : public Expression {
public:
  NESTEDANY<Expression> tryStatement;
  CHAINX/*Expression*/ catchClauses;

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class TypeBranch : public Expression {
public:
  NESTEDANY<Reference> exprType;
  NESTEDANY<VarName> varName;
  NESTEDANY<Expression> thenPart;
  LavaDECL-- *typeDecl;
//  int-- sectionNumber;

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool IsRepeatableClause (CHAINX *&chx);
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
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class AttachObject : public Expression {
public:
  NESTEDANY<Reference> objType;
  NESTEDANY<Reference> itf;
  NESTEDANY<Expression> url;
  LavaDECL-- *typeDECL, *execDECL;
  Category-- attachCat;

  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual bool NestedOptClause (SynObject *optClause);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
  virtual bool Check (CheckData &ckd);
};

class NewExpression : public AttachObject {
public:
  NESTEDANY<VarName> varName;
  NESTEDANY<FuncStatement> initializerCall;
  NESTEDANY<Expression> butStatement;
  bool-- errorInInitializer;

  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual bool NestedOptClause (SynObject *optClause);
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
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
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class CopyStatement : public Expression {
  NESTEDANY<Expression> fromObj;
  NESTEDANY<ObjReference> ontoObj;
  TargetType-- kindOfTarget;
  LavaDECL-- *ontoTypeDecl;

  virtual bool IsOptional (CheckData &ckd) { return ((SynObject*)fromObj.ptr)->IsOptional(ckd); };
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class EnumItem : public Expression {
public:
  NESTEDANY<Expression> itemNo;
  NESTEDANY<Reference> enumType;

  void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class ExtendExpression : public Expression {
public:
  NESTEDANY<Expression> extendObj;
  NESTEDANY<Reference> extendType;
  LavaDECL-- *extendTypeDecl;

  void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class Run : public AttachObject {
public:
  NESTEDANY<FuncStatement> initializerCall;
  NESTEDANY<VarName> varName;
  CHAINX/*Parameter*/ inputs;
  LavaDECL-- *execDECL;
  unsigned-- nParams;

  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class QueryItf : public Expression {
public:
  NESTEDANY<Reference> itf;
  NESTEDANY<ObjReference> givenObj;

  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class GetUUID : public Expression {
public:
  NESTEDANY<Reference> itf;

  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class IntegerInterval : public SynObject {
public:
  NESTEDANY<Expression> from, to;

  virtual bool IsPlaceHolder () { return false; }
  virtual bool IsIntIntv () { return true; }
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
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
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class QuantStmOrExp : public Expression {
public:
  CHAINX/*Quantifier*/ quantifiers;
  NESTEDANY<Expression> statement;
  unsigned-- nQuantVars;

  virtual bool IsExists () { return false; }
  virtual bool IsReadOnlyClause(SynObject *synObj, bool &roExec);
  virtual bool Check (CheckData &ckd);
  virtual bool InitCheck (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
};

class Declare : public QuantStmOrExp {
public:
  virtual bool IsDeclare () { return true; }
  virtual QString whatsThisText() {
   return "<p><a href=\"Declare.htm\">Declare</a> local variables</p>"; };
};

class Exists : public QuantStmOrExp {
public:
  NESTEDANY<Expression> updateStatement;

  virtual bool NestedOptClause (SynObject *optClause);
  virtual bool IsExists () { return true; }
};

class Foreach : public Exists {
};

class SelectExpression : public QuantStmOrExp {
public:
  NESTEDANY<Expression> addObject, resultSet;

  virtual bool IsOptional (CheckData &ckd) { return false; };
  virtual void ExprGetFVType(CheckData &ckd, LavaDECL *&decl, Category &cat, SynFlags& ctxFlags);
  virtual bool Check (CheckData &ckd);
  virtual void MakeTable (address table,int inINCL,SynObjectBase *parent,TTableUpdate update,address where,CHAINX *chxp,address searchData=0);
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
	QScrollView *sv;


  void Insert(TToken token,bool isPrimToken=false,bool isOpt=false/*CHE *pred*/);
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

  void Insert(TToken token,bool isPrimToken=false,bool isOpt=false/*CHE *pred*/);
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
};

class FailStatementV : public FailStatement {
public:
  FailStatementV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
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
};

class AndOpV : public AndOp {
public:
  AndOpV () { MultipleOpInit(and_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class OrOpV : public OrOp {
public:
  OrOpV () { MultipleOpInit(or_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class XorOpV : public XorOp {
public:
  XorOpV () { MultipleOpInit(xor_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
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
  FuncStatementV () {}
  FuncStatementV (bool funcDisabled, bool out=false, bool staticFunc=false);
  FuncStatementV (Reference *ref);
  FuncStatementV (ObjReference *ref, bool out=false);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class CallbackV : public Callback {
public:
  CallbackV () {}
  CallbackV (ObjReference *handle);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class AssertStatementV : public AssertStatement {
public:
  AssertStatementV () {}
  AssertStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ThrowStatementV : public ThrowStatement {
public:
  ThrowStatementV () {}
  ThrowStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
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
};

class IfxThenV : public IfxThen {
public:
  IfxThenV () {}
  IfxThenV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class IfExpressionV : public IfExpression {
public:
  IfExpressionV () {}
  IfExpressionV (bool);

  virtual void InsertBranch (SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class BranchV : public Branch {
public:
  BranchV () {}
  BranchV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class SwitchStatementV : public SwitchStatement {
public:
  SwitchStatementV () {}
  SwitchStatementV (bool);

  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
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

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class TypeBranchV : public TypeBranch {
public:
  TypeBranchV () {}
  TypeBranchV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class TypeSwitchStatementV : public TypeSwitchStatement {
public:
  TypeSwitchStatementV () {}
  TypeSwitchStatementV (bool);

  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class AttachObjectV : public AttachObject {
public:
  AttachObjectV () {}
  AttachObjectV (bool);
  AttachObjectV (Reference *ref);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class NewExpressionV : public NewExpression {
public:
  NewExpressionV () {}
  NewExpressionV (bool);
  NewExpressionV (Reference *ref,bool withItf,bool withLoc);
  NewExpressionV (FuncStatement *ref,bool withItf,bool withLoc);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class CloneExpressionV : public CloneExpression {
public:
  CloneExpressionV () {}
  CloneExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class CopyStatementV : public CopyStatement {
public:
  CopyStatementV () {}
  CopyStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class EnumItemV : public EnumItem {
public:
  EnumItemV () {}
  EnumItemV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ExtendExpressionV : public ExtendExpression {
public:
  ExtendExpressionV () {}
  ExtendExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class RunV : public Run {
public:
  RunV () {}
  RunV (bool);
  RunV (Reference *ref);
  RunV (FuncStatement *ref);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class QueryItfV : public QueryItf {
public:
  QueryItfV () {}
  QueryItfV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
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
};

class ExistsV : public Exists {
public:
  ExistsV () {}
  ExistsV (bool);

  virtual SynObject *InsertOptionals ();
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ForeachV : public Foreach {
public:
  ForeachV () {}
  ForeachV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class SelectExpressionV : public SelectExpression {
public:
  SelectExpressionV () {}
  SelectExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
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

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class ObjReferenceX : public ObjReference {
public:
  ObjReferenceX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
  LavaObjectPtr GetPropertyInfo(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos, LavaDECL*& setExec);
  LavaObjectPtr GetMemberObjPtr(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos);
  LavaVariablePtr GetMemberVarPtr(CheckData &ckd, LavaVariablePtr stackFrame, unsigned stackPos);
  bool assign (SynObject *source,
               LavaObjectPtr object,
               TargetType targetType,
               unsigned sectionNumber,
               CheckData &ckd,
               LavaVariablePtr stackFrame);
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
  SelfVarX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class ConstantX : public Constant {
public:
  ConstantX() {}
  ~ConstantX();

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class BoolConstX : public BoolConst {
public:
  BoolConstX() {}
  ~BoolConstX();

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class NullConstX : public NullConst {
public:
  NullConstX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame) { return 0; }
};

class SucceedStatementX : public SucceedStatement {
public:
  SucceedStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
    ckd.immediateReturn = true; return true; }
};

class FailStatementX : public FailStatement {
public:
  FailStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class UnaryOpX : public UnaryOp {
public:
  UnaryOpX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class InvertOpX : public UnaryOpX {
public:
  InvertOpX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class HandleOpX : public HandleOp {
public:
  HandleOpX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class OrdOpX : public UnaryOpX {
public:
  OrdOpX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class MinusOpX : public UnaryOpX {
public:
  MinusOpX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class LogicalNotX : public LogicalNot {
public:
  LogicalNotX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame) {
    return !((SynObjectX*)operand.ptr)->Execute(ckd,stackFrame);}
};

class EvalExpressionX : public EvalExpression {
public:
  EvalExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class EvalStatementX : public EvalStatement {
public:
  EvalStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class InSetStatementX : public InSetStatement {
  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class BinaryOpX : public BinaryOp {
  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class MultipleOpX : public MultipleOp {
public:
  MultipleOpX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class SemicolonOpX : public MultipleOpX {
public:
  SemicolonOpX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class AndOpX : public SemicolonOpX {
public:
  AndOpX() {}
};

class OrOpX : public MultipleOpX {
public:
  OrOpX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class XorOpX : public MultipleOpX {
public:
  XorOpX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
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

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class ArrayAtIndexX : public ArrayAtIndex {
public:
  ArrayAtIndexX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class ParameterX : public Parameter {
public:
  ParameterX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class FuncExpressionX : public FuncExpression {
public:
  FuncExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class FuncStatementX : public FuncStatement {
public:
  FuncStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class CallbackX : public Callback {
public:
  CallbackX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class AssertStatementX : public AssertStatement {
public:
  AssertStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class ThrowStatementX : public ThrowStatement {
public:
  ThrowStatementX() {}

  //virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class IfThenX : public IfThen {
public:
  IfThenX() {}
};

class IfStatementX : public IfStatement {
public:
  IfStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class IfxThenX : public IfxThen {
public:
  IfxThenX() {}
};

class IfExpressionX : public IfExpression {
public:
  IfExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class BranchX : public Branch {
public:
  BranchX() {}
};

class SwitchStatementX : public SwitchStatement {
public:
  SwitchStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class TypeBranchX : public TypeBranch {
public:
  TypeBranchX() {}
};

class TypeSwitchStatementX : public TypeSwitchStatement {
public:
  TypeSwitchStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class CatchClauseX : public CatchClause {
public:
  CatchClauseX() {}
};

class TryStatementX : public TryStatement {
public:
  TryStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class AttachObjectX : public AttachObject {
public:
  AttachObjectX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class NewExpressionX : public NewExpression {
public:
  NewExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class CloneExpressionX : public CloneExpression {
public:
  CloneExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class CopyStatementX : public CopyStatement {
public:
  CopyStatementX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class EnumItemX : public EnumItem {
public:
  EnumItemX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class ExtendExpressionX : public ExtendExpression {
public:
  ExtendExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class RunX : public Run {
public:
  RunX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class QueryItfX : public QueryItf {
public:
  QueryItfX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
};

class GetUUIDX : public GetUUID {
//public:
//  GetUUIDX() {}

  //LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
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

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
};

class QuantStmOrExpX : public QuantStmOrExp {
//public:
//  QuantStmOrExpX() {}
};

class ExistsX : public Exists {
public:
//  ExistsX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
  bool Recursion (CheckData &ckd, LavaVariablePtr stackFrame,CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet=0);
};

class ForeachX : public Foreach {
public:
//  ForeachX() {}

  virtual bool Execute (CheckData &ckd, LavaVariablePtr stackFrame);
  bool Recursion (CheckData &ckd, LavaVariablePtr stackFrame,CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet=0);
};

class SelectExpressionX : public SelectExpression {
public:
//  SelectExpressionX() {}

  LavaObjectPtr Evaluate (CheckData &ckd, LavaVariablePtr stackFrame);
  bool Recursion (CheckData &ckd, LavaVariablePtr stackFrame,CHE *cheQuant, CHE *cheVar, LavaObjectPtr rSet);
};

#endif

#endif
