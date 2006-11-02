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


#include "Constructs.h"
#ifndef INTERPRETER
#include "ExecView.h"
#endif
#include "LavaAppBase.h"
#include "PEBaseDoc.h"

#pragma hdrstop


#define ACCEPT(WHERE) \
	{visitor.Adjust((SynObject*)this,(address)&WHERE,0);\
  ((SynObject*)WHERE)->Accept(visitor);\
  if (visitor.finished) return;}

#define ACC_CHE(CHEP,CHXP) \
	{visitor.Adjust((SynObject*)this,(address)CHEP,CHXP);\
  ((SynObject*)CHEP->data)->Accept(visitor);\
  if (visitor.finished) return;}


void SynObject::Accept(Visitor &visitor) {
  visitor.Eval(this);

  visitor.VisitSynObject(this);
}

void EnumConst::Accept(Visitor &visitor) {
  visitor.Eval(this);

  visitor.VisitSynObject(this);
}

void Parameter::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(parameter.ptr);

  visitor.VisitParameter(this);
}

void FormParms::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);

  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&inputs);
  for (chp = (CHE*)outputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&outputs);

  visitor.VisitFormParms(this);
}

void FormParm::Accept(Visitor &visitor) {
  visitor.Eval(this);
  ACCEPT(formParm.ptr);

  visitor.VisitFormParm(this);
}

void Reference::Accept(Visitor &visitor) {
  visitor.Eval(this);

  visitor.VisitReference(this);
}

void TDOD::Accept(Visitor &visitor) {
  visitor.Eval(this);

  visitor.VisitTDOD(this);
}

void ObjReference::Accept(Visitor &visitor) {
  CHE *chp;

  visitor.Eval(this);
  for ( chp = (CHE*)refIDs.first;
        chp;
        chp = (CHE*)chp->successor)
    ACC_CHE(chp,&refIDs);

  visitor.VisitObjReference(this);
}


void MultipleOp::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);

  for (chp = (CHE*)operands.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&operands);

  visitor.VisitMultipleOp(this);
}

void BaseInit::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(baseItf.ptr);
  if (initializerCall.ptr)
    ACCEPT(initializerCall.ptr);

  visitor.VisitBaseInit(this);
}

void SelfVar::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  ACCEPT(execName.ptr);

  if (formParms.ptr)
    ACCEPT(formParms.ptr);

  for (chp = (CHE*)baseInitCalls.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&baseInitCalls);

  ACCEPT(body.ptr);

  visitor.VisitSelfVar(this);
}

void HandleOp::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(operand.ptr);

  visitor.VisitHandleOp(this);
}

void FailStatement::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  if ((SynObject*)exception.ptr)
    ACCEPT(exception.ptr);

  visitor.VisitFailStatement(this);
}

void OldExpression::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(paramExpr.ptr);

  visitor.VisitOldExpression(this);
}

void UnaryOp::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  if ((SynObject*)operand.ptr) // in MinusOp ptr==0 possible
    ACCEPT(operand.ptr);

  visitor.VisitUnaryOp(this);
}

void LogicalNot::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(operand.ptr);

  visitor.VisitLogicalNot(this);
}

void EvalExpression::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(operand.ptr);

  visitor.VisitEvalExpression(this);
}

void InSetStatement::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(operand1.ptr);
  ACCEPT(operand2.ptr);

  visitor.VisitInSetStatement(this);
}

void BinaryOp::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(operand1.ptr);
  ACCEPT(operand2.ptr);

  visitor.VisitBinaryOp(this);
}

void VarName::Accept(Visitor &visitor)
{
  visitor.Eval(this);

  visitor.VisitVarName(this);
}

void Assignment::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(targetObj.ptr);
  ACCEPT(exprValue.ptr);

  visitor.VisitAssignment(this);
}

void ArrayAtIndex::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(arrayObj.ptr);
  ACCEPT(arrayIndex.ptr);

  visitor.VisitArrayAtIndex(this);
}

void FuncExpression::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&inputs);

  if (handle.ptr)
    ACCEPT(handle.ptr);
  if (function.ptr)
    ACCEPT(function.ptr);

  visitor.VisitFuncExpression(this);
}

void FuncStatement::Accept(Visitor &visitor)
{
  CHE *chp;

  FuncExpression::Accept(visitor);

  for (chp = (CHE*)outputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&outputs);

  visitor.VisitFuncStatement(this);
}

void Signal::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(sCall.ptr);

  visitor.VisitSignal(this);
}

void Connect::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  if (signalSender.ptr)
    ACCEPT(signalSender.ptr)
  else
    ACCEPT(signalSenderClass.ptr)
  ACCEPT(signalFunction.ptr);
  ACCEPT(callback.ptr);

  visitor.VisitConnect(this);
}

void Disconnect::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(signalSender.ptr);
  ACCEPT(signalFunction.ptr);
  ACCEPT(signalReceiver.ptr);
  ACCEPT(callbackFunction.ptr);

  visitor.VisitDisconnect(this);
}

void IfThen::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(ifCondition.ptr);
  ACCEPT(thenPart.ptr);

  visitor.VisitIfThen(this);
}

void IfStatement::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  for (chp = (CHE*)ifThens.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&ifThens);

  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitIfStatement(this);
}

void IfxThen::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(ifCondition.ptr);
  ACCEPT(thenPart.ptr);

  visitor.VisitIfxThen(this);
}

void IfdefStatement::Accept(Visitor &visitor)
{
  visitor.Eval(this);

  CHE *chp;

  for (chp = (CHE*)ifCondition.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&ifCondition);

	if (thenPart.ptr)
    ACCEPT(thenPart.ptr);
  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitIfdefStatement(this);
}

void IfExpression::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  for (chp = (CHE*)ifThens.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&ifThens);

  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitIfExpression(this);
}

void ElseExpression::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(expr1.ptr);
  ACCEPT(expr2.ptr);

  visitor.VisitElseExpression(this);
}

void TypeBranch::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(exprType.ptr);
  ACCEPT(varName.ptr);

  if (thenPart.ptr)
    ACCEPT(thenPart.ptr);

  visitor.VisitTypeBranch(this);
}

void Branch::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  for ( chp = (CHE*)caseLabels.first;
        chp;
        chp = (CHE*)chp->successor)
    ACC_CHE(chp,&caseLabels);

  if (thenPart.ptr)
    ACCEPT(thenPart.ptr);

  visitor.VisitBranch(this);
}

void SwitchStatement::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  ACCEPT(caseExpression.ptr);
  for (chp = (CHE*)branches.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&branches);

  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitSwitchStatement(this);
}

void CatchClause::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(exprType.ptr);
  ACCEPT(varName.ptr);
  if (catchClause.ptr)
    ACCEPT(catchClause.ptr);

  visitor.VisitCatchClause(this);
}

void TryStatement::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  ACCEPT(tryStatement.ptr);
  for (chp = (CHE*)catchClauses.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&catchClauses);

  visitor.VisitTryStatement(this);
}

void TypeSwitchStatement::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  ACCEPT(caseExpression.ptr);
  for (chp = (CHE*)branches.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&branches);

  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitTypeSwitchStatement(this);
}

void AssertStatement::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(statement.ptr);

  visitor.VisitAssertStatement(this);
}

void AttachObject::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  if (objType.ptr)
    ACCEPT(objType.ptr);
  if (itf.ptr) // itf: because of Run/NewExpression
    ACCEPT(itf.ptr);
  if (url.ptr)
    ACCEPT(url.ptr);

  visitor.VisitAttachObject(this);
}

void Run::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  ACCEPT(initiator.ptr);
  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&inputs);

  visitor.VisitRun(this);
}

void QueryItf::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(itf.ptr);
  ACCEPT(givenObj.ptr);

  visitor.VisitQueryItf(this);
}

void GetUUID::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(itf.ptr);

  visitor.VisitGetUUID(this);
}

void IntegerInterval::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(from.ptr);
  ACCEPT(to.ptr);

  visitor.VisitIntegerInterval(this);
}

void Quantifier::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  if (elemType.ptr)
    ACCEPT(elemType.ptr);
  for (chp = (CHE*)quantVars.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&quantVars);

  if (set.ptr)
    ACCEPT(set.ptr);

  visitor.VisitQuantifier(this);
}

void QuantStmOrExp::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  for (chp = (CHE*)quantifiers.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&quantifiers);

  if (primaryClause.ptr)
    ACCEPT(primaryClause.ptr);

  if (IsExists()) {
    if (((Exists*)this)->secondaryClause.ptr)
      ACCEPT(((Exists*)this)->secondaryClause.ptr);
  }
  else if (IsDeclare()) {
    if (((Declare*)this)->secondaryClause.ptr)
      ACCEPT(((Declare*)this)->secondaryClause.ptr);
  }

  visitor.VisitQuantStmOrExp(this);
}

void SelectExpression::Accept(Visitor &visitor)
{
  CHE *chp;

  visitor.Eval(this);
  for (chp = (CHE*)quantifiers.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&quantifiers);

  if (primaryClause.ptr)
    ACCEPT(primaryClause.ptr);

  ACCEPT(addObject.ptr);
  ACCEPT(resultSet.ptr);

  visitor.VisitSelectExpression(this);
}

void NewExpression::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  if (objType.ptr)
    ACCEPT(objType.ptr);
  if (itf.ptr)
    ACCEPT(itf.ptr);
  if (url.ptr)
    ACCEPT(url.ptr);

  if (varName.ptr)
    ACCEPT(varName.ptr);
  if (initializerCall.ptr)
    ACCEPT(initializerCall.ptr);
  if (butStatement.ptr)
    ACCEPT(butStatement.ptr);

  visitor.VisitNewExpression(this);
}

void CloneExpression::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(varName.ptr);
  ACCEPT(fromObj.ptr);
  if (butStatement.ptr)
    ACCEPT(butStatement.ptr);

  visitor.VisitCloneExpression(this);
}

void CopyStatement::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(fromObj.ptr);
  ACCEPT(ontoObj.ptr);

  visitor.VisitCopyStatement(this);
}

void EnumItem::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(itemNo.ptr);
  ACCEPT(enumType.ptr);

  visitor.VisitEnumItem(this);
}

void ExtendExpression::Accept(Visitor &visitor)
{
  visitor.Eval(this);
  ACCEPT(extendObj.ptr);
  ACCEPT(extendType.ptr);

  visitor.VisitExtendExpression(this);
}
