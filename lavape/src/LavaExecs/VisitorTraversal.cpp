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

#define ENTRY \
  visitor.Eval(this,parent,where,chxp);\
  if (visitor.finished) return;

#define ACCEPT(WHERE) \
  {((SynObject*)WHERE)->Accept(visitor,(SynObject*)this,(address)&WHERE,0);\
  if (visitor.finished) return;}

#define ACC_CHE(CHEP,CHXP) \
  {((SynObject*)CHEP->data)->Accept(visitor,(SynObject*)this,(address)CHEP,CHXP);\
  if (visitor.finished) return;}


void SynObject::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp) {
  ENTRY

  visitor.VisitSynObject(this,parent,where,chxp);
}

void EnumConst::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp) {
  ENTRY

  visitor.VisitEnumConst(this,parent,where,chxp);
}

void Parameter::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(parameter.ptr);

  visitor.VisitParameter(this,parent,where,chxp);
}

void FormParms::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY

  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&inputs);
  for (chp = (CHE*)outputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&outputs);

  visitor.VisitFormParms(this,parent,where,chxp);
}

void FormParm::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp) {
  ENTRY
  ACCEPT(formParm.ptr);

  visitor.VisitFormParm(this,parent,where,chxp);
}

void Reference::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp) {
  ENTRY

  visitor.VisitReference(this,parent,where,chxp);
}

void TDOD::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp) {
  ENTRY

  visitor.VisitTDOD(this,parent,where,chxp);
}

void ObjReference::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp) {
  CHE *chp;

  ENTRY
  for ( chp = (CHE*)refIDs.first;
        chp;
        chp = (CHE*)chp->successor)
    ACC_CHE(chp,&refIDs);

  visitor.VisitObjReference(this,parent,where,chxp);
}


void MultipleOp::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY

  for (chp = (CHE*)operands.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&operands);

  visitor.VisitMultipleOp(this,parent,where,chxp);
}

void BaseInit::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(baseItf.ptr);
  if (initializerCall.ptr)
    ACCEPT(initializerCall.ptr);

  visitor.VisitBaseInit(this,parent,where,chxp);
}

void SelfVar::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  //ENTRY
  ACCEPT(execName.ptr);

  if (formParms.ptr)
    ACCEPT(formParms.ptr);

  for (chp = (CHE*)baseInitCalls.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&baseInitCalls);

  ACCEPT(body.ptr);

  visitor.VisitSelfVar(this,parent,where,chxp);
}

void HandleOp::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(operand.ptr);

  visitor.VisitHandleOp(this,parent,where,chxp);
}

void FailStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  if ((SynObject*)exception.ptr)
    ACCEPT(exception.ptr);

  visitor.VisitFailStatement(this,parent,where,chxp);
}

void OldExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(paramExpr.ptr);

  visitor.VisitOldExpression(this,parent,where,chxp);
}

void UnaryOp::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  if ((SynObject*)operand.ptr) // in MinusOp ptr==0 possible
    ACCEPT(operand.ptr);

  visitor.VisitUnaryOp(this,parent,where,chxp);
}

void LogicalNot::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(operand.ptr);

  visitor.VisitLogicalNot(this,parent,where,chxp);
}

void EvalExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(operand.ptr);

  visitor.VisitEvalExpression(this,parent,where,chxp);
}

void InSetStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(operand1.ptr);
  ACCEPT(operand2.ptr);

  visitor.VisitInSetStatement(this,parent,where,chxp);
}

void BinaryOp::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(operand1.ptr);
  ACCEPT(operand2.ptr);

  visitor.VisitBinaryOp(this,parent,where,chxp);
}

void VarName::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY

  visitor.VisitVarName(this,parent,where,chxp);
}

void Assignment::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(targetObj.ptr);
  ACCEPT(exprValue.ptr);

  visitor.VisitAssignment(this,parent,where,chxp);
}

void ArrayAtIndex::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(arrayObj.ptr);
  ACCEPT(arrayIndex.ptr);

  visitor.VisitArrayAtIndex(this,parent,where,chxp);
}

void FuncExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY

  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&inputs);
// inputs must be processed before handle since closedLevel of handle
// depends on closedLevels of actual inputs
  if (handle.ptr)
    ACCEPT(handle.ptr);

  if (function.ptr)
    ACCEPT(function.ptr);

  visitor.VisitFuncExpression(this,parent,where,chxp);
}

void FuncStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  FuncExpression::Accept(visitor,parent,where,chxp);

  for (chp = (CHE*)outputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&outputs);

  visitor.VisitFuncStatement(this,parent,where,chxp);
}

void Signal::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(sCall.ptr);

  visitor.VisitSignal(this,parent,where,chxp);
}

void Connect::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  if (signalSender.ptr)
    ACCEPT(signalSender.ptr)
  else
    ACCEPT(signalSenderClass.ptr)
  ACCEPT(signalFunction.ptr);
  ACCEPT(callback.ptr);

  visitor.VisitConnect(this,parent,where,chxp);
}

void Disconnect::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(signalSender.ptr);
  ACCEPT(signalFunction.ptr);
  ACCEPT(signalReceiver.ptr);
  ACCEPT(callbackFunction.ptr);

  visitor.VisitDisconnect(this,parent,where,chxp);
}

void IfThen::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(ifCondition.ptr);
  ACCEPT(thenPart.ptr);

  visitor.VisitIfThen(this,parent,where,chxp);
}

void IfStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  for (chp = (CHE*)ifThens.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&ifThens);

  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitIfStatement(this,parent,where,chxp);
}

void IfxThen::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(ifCondition.ptr);
  ACCEPT(thenPart.ptr);

  visitor.VisitIfxThen(this,parent,where,chxp);
}

void IfdefStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY

  CHE *chp;

  for (chp = (CHE*)ifCondition.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&ifCondition);

	if (thenPart.ptr)
    ACCEPT(thenPart.ptr);
  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitIfdefStatement(this,parent,where,chxp);
}

void IfExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  for (chp = (CHE*)ifThens.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&ifThens);

  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitIfExpression(this,parent,where,chxp);
}

void ElseExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(expr1.ptr);
  ACCEPT(expr2.ptr);

  visitor.VisitElseExpression(this,parent,where,chxp);
}

void TypeBranch::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(exprType.ptr);
  ACCEPT(varName.ptr);

  if (thenPart.ptr)
    ACCEPT(thenPart.ptr);

  visitor.VisitTypeBranch(this,parent,where,chxp);
}

void Branch::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  for ( chp = (CHE*)caseLabels.first;
        chp;
        chp = (CHE*)chp->successor)
    ACC_CHE(chp,&caseLabels);

  if (thenPart.ptr)
    ACCEPT(thenPart.ptr);

  visitor.VisitBranch(this,parent,where,chxp);
}

void SwitchStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  ACCEPT(caseExpression.ptr);
  for (chp = (CHE*)branches.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&branches);

  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitSwitchStatement(this,parent,where,chxp);
}

void CatchClause::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(exprType.ptr);
  ACCEPT(varName.ptr);
  if (catchClause.ptr)
    ACCEPT(catchClause.ptr);

  visitor.VisitCatchClause(this,parent,where,chxp);
}

void TryStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  ACCEPT(tryStatement.ptr);
  for (chp = (CHE*)catchClauses.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&catchClauses);

  visitor.VisitTryStatement(this,parent,where,chxp);
}

void TypeSwitchStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  ACCEPT(caseExpression.ptr);
  for (chp = (CHE*)branches.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&branches);

  if (elsePart.ptr)
    ACCEPT(elsePart.ptr);

  visitor.VisitTypeSwitchStatement(this,parent,where,chxp);
}

void AssertStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(statement.ptr);

  visitor.VisitAssertStatement(this,parent,where,chxp);
}

void IgnoreStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  for (chp = (CHE*)igVars.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&igVars);

  visitor.VisitIgnoreStatement(this,parent,where,chxp);
}

void AttachObject::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  if (objType.ptr)
    ACCEPT(objType.ptr);
  if (itf.ptr) // itf: because of Run/NewExpression
    ACCEPT(itf.ptr);
  if (url.ptr)
    ACCEPT(url.ptr);

  visitor.VisitAttachObject(this,parent,where,chxp);
}

void Run::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  ACCEPT(initiator.ptr);
  for (chp = (CHE*)inputs.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&inputs);

  visitor.VisitRun(this,parent,where,chxp);
}

void QueryItf::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(itf.ptr);
  ACCEPT(givenObj.ptr);

  visitor.VisitQueryItf(this,parent,where,chxp);
}

void GetUUID::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(itf.ptr);

  visitor.VisitGetUUID(this,parent,where,chxp);
}

void IntegerInterval::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(from.ptr);
  ACCEPT(to.ptr);

  visitor.VisitIntegerInterval(this,parent,where,chxp);
}

void Quantifier::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  if (elemType.ptr)
    ACCEPT(elemType.ptr);
  for (chp = (CHE*)quantVars.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&quantVars);

  if (set.ptr)
    ACCEPT(set.ptr);

  visitor.VisitQuantifier(this,parent,where,chxp);
}

void QuantStmOrExp::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
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

  visitor.VisitQuantStmOrExp(this,parent,where,chxp);
}

void SelectExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  CHE *chp;

  ENTRY
  for (chp = (CHE*)quantifiers.first;
       chp;
       chp = (CHE*)chp->successor)
    ACC_CHE(chp,&quantifiers);

  if (primaryClause.ptr)
    ACCEPT(primaryClause.ptr);

  ACCEPT(addObject.ptr);
  ACCEPT(resultSet.ptr);

  visitor.VisitSelectExpression(this,parent,where,chxp);
}

void NewExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
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

  visitor.VisitNewExpression(this,parent,where,chxp);
}

void CloneExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(varName.ptr);
  ACCEPT(fromObj.ptr);
  if (butStatement.ptr)
    ACCEPT(butStatement.ptr);

  visitor.VisitCloneExpression(this,parent,where,chxp);
}

void CopyStatement::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(fromObj.ptr);
  ACCEPT(ontoObj.ptr);

  visitor.VisitCopyStatement(this,parent,where,chxp);
}

void EnumItem::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(itemNo.ptr);
  ACCEPT(enumType.ptr);

  visitor.VisitEnumItem(this,parent,where,chxp);
}

void ExtendExpression::Accept(Visitor &visitor,SynObject *parent,address where,CHAINX *chxp)
{
  ENTRY
  ACCEPT(extendObj.ptr);
  ACCEPT(extendType.ptr);

  visitor.VisitExtendExpression(this,parent,where,chxp);
}
