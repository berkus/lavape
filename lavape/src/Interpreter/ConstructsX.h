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


#ifndef __ConstructsX
#define __ConstructsX

//#include "Constructs.h"

extern TToken OperandType (TToken primaryToken);
extern TToken PlaceHolderType (TToken primaryToken);


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
