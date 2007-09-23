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


#include "LavaAppBase.h"
#include "PEBaseDoc.h"

#pragma hdrstop

#include "Constructs.h"

#ifndef INTERPRETER
#include "ExecView.h"
#endif


void ClosedLevelVisitor::VisitParameter (Parameter *obj,SynObject *parent,address where,CHAINX *chxp) {
  obj->closedLevel = ((Expression*)obj->parameter.ptr)->closedLevel;
}

void ClosedLevelVisitor::VisitObjReference (ObjReference *obj,SynObject *parent,address where,CHAINX *chxp) {
  DWORD dw;
  TIDType idtype;
  VarName *vn;
  int maxLevel=-1;

  obj->closedLevel = 0;
  if (obj->refIDs.first == obj->refIDs.last) {
    dw = document->IDTable.GetVar(((TDOD*)((CHE*)obj->refIDs.first)->data)->ID,idtype);
    if (!dw) return;
    if (idtype == localID) {
      vn = (VarName*)dw;
      if (vn->parentObject->parentObject->IsDeclare()
      && ((Declare*)vn->parentObject->parentObject)->secondaryClause.ptr) {
        if (inRecursion(vn)) // stop recursion
          return;
        else if (vn->closedLevel == -1) { // CL still unknown, go into recursion
          ClosedLevelVisitor clv(document,vn,this);
          vn->iniCall->Accept(clv);
          vn->closedLevel = clv.maxClosedLevel;
          maxClosedLevel = qMax(clv.maxClosedLevel,maxClosedLevel);
          obj->closedLevel = vn->closedLevel;
        }
        else // CL is known already
          return;
      }
      else {
      }
    }
  }
}

void ClosedLevelVisitor::VisitUnaryOp (UnaryOp *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitBinaryOp (BinaryOp *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitMultipleOp (MultipleOp *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitFuncExpression (FuncExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
  CHE *chp;
  int maxLevel;

  maxLevel = ((Expression*)obj->handle.ptr)->closedLevel;

  for (chp = (CHE*)obj->inputs.first;
       chp;
       chp = (CHE*)chp->successor) {
    maxLevel = qMax(maxLevel,((Expression*)chp->data)->closedLevel);
  }
  obj->closedLevel = maxLevel;
}

void ClosedLevelVisitor::VisitFuncStatement (FuncStatement *obj,SynObject *parent,address where,CHAINX *chxp) {
  CHE *chp;
  int maxLevel;

  if (!obj->flags.Contains(isIniCallOrHandle))
    return;

  maxLevel = ((Expression*)obj->handle.ptr)->closedLevel;

  for (chp = (CHE*)obj->inputs.first;
       chp;
       chp = (CHE*)chp->successor) {
    maxLevel = qMax(maxLevel,((Expression*)chp->data)->closedLevel);
  }
  obj->closedLevel = maxLevel;
}

void ClosedLevelVisitor::VisitOldExpression (OldExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitEvalExpression (EvalExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitArrayAtIndex (ArrayAtIndex *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitIfExpression (IfExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitElseExpression (ElseExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitNewExpression (NewExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitCloneExpression (CloneExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitExtendExpression (ExtendExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitSelectExpression (SelectExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
}
