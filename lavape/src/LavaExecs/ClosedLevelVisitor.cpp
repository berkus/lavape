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


void ClosedLevelVisitor::VisitParameter (Parameter *obj,SynObject *parent,address where,CHAINX *chxp) {
  obj->closedLevel = ((Expression*)obj->parameter.ptr)->closedLevel;
}

void ClosedLevelVisitor::VisitObjReference (ObjReference *obj,SynObject *parent,address where,CHAINX *chxp) {
  DWORD dw;
  TIDType idtype;
  VarName *vn;
  FuncExpression *funcCall;
  CHE *chp;
  int maxLevel=-1;

  if (obj->parentObject->IsFuncInvocation()) {// is call object of ini call or func expression/stm
    funcCall = (FuncExpression*)obj->parentObject;
    for (chp = (CHE*)funcCall->inputs.first;
         chp;
         chp = (CHE*)chp->successor) {
      maxLevel = qMax(maxLevel,((Expression*)chp->data)->closedLevel);
    }
    if (!obj->flags.Contains(isIniCallOrHandle)) {
      maxLevel = qMax(maxLevel,((Expression*)chp->data)->closedLevel);
    }
    obj->closedLevel = funcCall->closedLevel = maxLevel;
  }
  else if (obj->refIDs.first == obj->refIDs.last) {
    dw = document->IDTable.GetVar(((TDOD*)((CHE*)obj->refIDs.first)->data)->ID,idtype);
    if (!dw) return;
    else if (idtype == localID) {
      vn = (VarName*)dw;
      if (vn->parentObject->parentObject->IsDeclare()
      && ((Declare*)vn->parentObject->parentObject)->secondaryClause.ptr) {
        if (vn->closedLevel == -1) {
          funcCall = vn->iniCall;
        }
      }
      else {
      }
    }
  }
  else
    obj->closedLevel = 0;
}

void ClosedLevelVisitor::VisitUnaryOp (UnaryOp *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitBinaryOp (BinaryOp *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitMultipleOp (MultipleOp *obj,SynObject *parent,address where,CHAINX *chxp) {
}

void ClosedLevelVisitor::VisitFuncExpression (FuncExpression *obj,SynObject *parent,address where,CHAINX *chxp) {
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
