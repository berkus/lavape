#ifdef __GNUC__
#pragma implementation "Constructs.h"
#endif

//*******************************************************************
//
//   public procedures of structs/classes defined in Constructs.h
//
//*******************************************************************

#include "Constructs.h"


#include "CDPpp.h"

ChainAnyElem* NewCHETokenNode ()
{ return (ChainAnyElem*)(new CHETokenNode); }

ChainAnyElem* NewCHECVarDesc ()
{ return (ChainAnyElem*)(new CHECVarDesc); }


void CDPConstrFlags (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool)

{
  ConstrFlags *vp = (ConstrFlags*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (ConstrFlags)cid->FGETunsigned();
} // END OF CDPConstrFlags

NESTEDINITCOPY(TComment)

NESTEDCDP(TComment)


IMPLEMENT_DYNAMIC_CLASS(TComment,NULL)


void CDPTComment (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  TComment *vp = (TComment*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    vp->str.CDP(pgf,cid);
    CDPpp.CVTbool(pgf,cid,vp->inLine);
    CDPpp.CVTbool(pgf,cid,vp->trailing);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTComment


IMPLEMENT_DYNAMIC_CLASS(SynObject,SynObjectBase)


void CDPSynObject (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  SynObject *vp = (SynObject*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPSynObjectBase(pgf,cid,(address)(SynObjectBase*)vp,true);
    CDPTToken(pgf,cid,(address)&vp->primaryToken);
    CDPTToken(pgf,cid,(address)&vp->type);
    CDPTToken(pgf,cid,(address)&vp->replacedType);
    vp->comment.CDP(pgf,cid,CDPTComment);
    vp->flags.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPSynObject


IMPLEMENT_DYNAMIC_CLASS(TDOD,SynObject)


void CDPTDOD (PutGetFlag pgf, ASN1* cid, address varAddr,
              bool baseCDP)

{
  TDOD *vp = (TDOD*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPSynObject(pgf,cid,(address)(SynObject*)vp,true);
    CDPTID(pgf,cid,(address)&vp->ID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTDOD


void CDPTDODC (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool)

{
  TDODC *vp = (TDODC*)varAddr;
  if (cid->Skip()) return;

  vp->CDP(pgf,cid);
} // END OF CDPTDODC


IMPLEMENT_DYNAMIC_CLASS(Expression,SynObject)


void CDPExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  Expression *vp = (Expression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPSynObject(pgf,cid,(address)(SynObject*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPExpression


IMPLEMENT_DYNAMIC_CLASS(Operation,Expression)


void CDPOperation (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  Operation *vp = (Operation*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    CDPTID(pgf,cid,(address)&vp->opFunctionID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPOperation


IMPLEMENT_DYNAMIC_CLASS(Reference,SynObject)


void CDPReference (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  Reference *vp = (Reference*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPSynObject(pgf,cid,(address)(SynObject*)vp,true);
    CDPTID(pgf,cid,(address)&vp->refID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPReference


IMPLEMENT_DYNAMIC_CLASS(EnumConst,Expression)


void CDPEnumConst (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  EnumConst *vp = (EnumConst*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    CDPTID(pgf,cid,(address)&vp->refID);
    vp->Id.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPEnumConst


IMPLEMENT_DYNAMIC_CLASS(ObjReference,Expression)


void CDPObjReference (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP)

{
  ObjReference *vp = (ObjReference*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    CDPTDODC(pgf,cid,(address)&vp->refIDs);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPObjReference


IMPLEMENT_DYNAMIC_CLASS(VarName,Expression)


void CDPVarName (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  VarName *vp = (VarName*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->varName.CDP(pgf,cid);
    CDPTID(pgf,cid,(address)&vp->varID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPVarName


IMPLEMENT_DYNAMIC_CLASS(FormParm,Expression)


void CDPFormParm (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  FormParm *vp = (FormParm*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->parmType.CDP(pgf,cid);
    vp->formParm.CDP(pgf,cid);
    CDPTID(pgf,cid,(address)&vp->formParmID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPFormParm


IMPLEMENT_DYNAMIC_CLASS(FormParms,Expression)


void CDPFormParms (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  FormParms *vp = (FormParms*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->inputs.CDP(pgf,cid);
    vp->outputs.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPFormParms


IMPLEMENT_DYNAMIC_CLASS(BaseInit,Expression)


void CDPBaseInit (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  BaseInit *vp = (BaseInit*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->baseItf.CDP(pgf,cid);
    vp->initializerCall.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPBaseInit


IMPLEMENT_DYNAMIC_CLASS(SelfVar,VarName)


void CDPSelfVar (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  SelfVar *vp = (SelfVar*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPVarName(pgf,cid,(address)(VarName*)vp,true);
    CDPTID(pgf,cid,(address)&vp->typeID);
    vp->execName.CDP(pgf,cid);
    vp->baseInitCalls.CDP(pgf,cid);
    vp->body.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPSelfVar


IMPLEMENT_DYNAMIC_CLASS(Constant,Expression)


void CDPConstant (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  Constant *vp = (Constant*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    CDPTBasicType(pgf,cid,(address)&vp->constType);
    vp->str.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPConstant


IMPLEMENT_DYNAMIC_CLASS(BoolConst,Expression)


void CDPBoolConst (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  BoolConst *vp = (BoolConst*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    CDPpp.CVTbool(pgf,cid,vp->boolValue);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPBoolConst


IMPLEMENT_DYNAMIC_CLASS(NullConst,Expression)


void CDPNullConst (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  NullConst *vp = (NullConst*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPNullConst


IMPLEMENT_DYNAMIC_CLASS(SucceedStatement,Expression)


void CDPSucceedStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP)

{
  SucceedStatement *vp = (SucceedStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPSucceedStatement


IMPLEMENT_DYNAMIC_CLASS(FailStatement,Expression)


void CDPFailStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP)

{
  FailStatement *vp = (FailStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->exception.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPFailStatement


IMPLEMENT_DYNAMIC_CLASS(OldExpression,Expression)


void CDPOldExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP)

{
  OldExpression *vp = (OldExpression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->paramExpr.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPOldExpression


IMPLEMENT_DYNAMIC_CLASS(UnaryOp,Operation)


void CDPUnaryOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  UnaryOp *vp = (UnaryOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPOperation(pgf,cid,(address)(Operation*)vp,true);
    vp->operand.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPUnaryOp


IMPLEMENT_DYNAMIC_CLASS(EvalExpression,UnaryOp)


void CDPEvalExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP)

{
  EvalExpression *vp = (EvalExpression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPUnaryOp(pgf,cid,(address)(UnaryOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPEvalExpression


IMPLEMENT_DYNAMIC_CLASS(EvalStatement,EvalExpression)


void CDPEvalStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP)

{
  EvalStatement *vp = (EvalStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPEvalExpression(pgf,cid,(address)(EvalExpression*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPEvalStatement


IMPLEMENT_DYNAMIC_CLASS(ArrayAtIndex,Operation)


void CDPArrayAtIndex (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP)

{
  ArrayAtIndex *vp = (ArrayAtIndex*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPOperation(pgf,cid,(address)(Operation*)vp,true);
    vp->arrayObj.CDP(pgf,cid);
    vp->arrayIndex.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPArrayAtIndex


IMPLEMENT_DYNAMIC_CLASS(InvertOp,UnaryOp)


void CDPInvertOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  InvertOp *vp = (InvertOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPUnaryOp(pgf,cid,(address)(UnaryOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPInvertOp


IMPLEMENT_DYNAMIC_CLASS(HandleOp,Expression)


void CDPHandleOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  HandleOp *vp = (HandleOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->operand.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPHandleOp


IMPLEMENT_DYNAMIC_CLASS(OrdOp,UnaryOp)


void CDPOrdOp (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool baseCDP)

{
  OrdOp *vp = (OrdOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPUnaryOp(pgf,cid,(address)(UnaryOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPOrdOp


IMPLEMENT_DYNAMIC_CLASS(MinusOp,UnaryOp)


void CDPMinusOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  MinusOp *vp = (MinusOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPUnaryOp(pgf,cid,(address)(UnaryOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPMinusOp


IMPLEMENT_DYNAMIC_CLASS(LogicalNot,UnaryOp)


void CDPLogicalNot (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  LogicalNot *vp = (LogicalNot*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPUnaryOp(pgf,cid,(address)(UnaryOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPLogicalNot


IMPLEMENT_DYNAMIC_CLASS(InSetStatement,Expression)


void CDPInSetStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP)

{
  InSetStatement *vp = (InSetStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->operand1.CDP(pgf,cid);
    vp->operand2.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPInSetStatement


IMPLEMENT_DYNAMIC_CLASS(BinaryOp,Operation)


void CDPBinaryOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  BinaryOp *vp = (BinaryOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPOperation(pgf,cid,(address)(Operation*)vp,true);
    vp->operand1.CDP(pgf,cid);
    vp->operand2.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPBinaryOp


IMPLEMENT_DYNAMIC_CLASS(MultipleOp,Operation)


void CDPMultipleOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  MultipleOp *vp = (MultipleOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPOperation(pgf,cid,(address)(Operation*)vp,true);
    vp->operands.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPMultipleOp


IMPLEMENT_DYNAMIC_CLASS(SemicolonOp,MultipleOp)


void CDPSemicolonOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool baseCDP)

{
  SemicolonOp *vp = (SemicolonOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPSemicolonOp


IMPLEMENT_DYNAMIC_CLASS(AndOp,MultipleOp)


void CDPAndOp (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool baseCDP)

{
  AndOp *vp = (AndOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPAndOp


IMPLEMENT_DYNAMIC_CLASS(OrOp,MultipleOp)


void CDPOrOp (PutGetFlag pgf, ASN1* cid, address varAddr,
              bool baseCDP)

{
  OrOp *vp = (OrOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPOrOp


IMPLEMENT_DYNAMIC_CLASS(XorOp,MultipleOp)


void CDPXorOp (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool baseCDP)

{
  XorOp *vp = (XorOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPXorOp


IMPLEMENT_DYNAMIC_CLASS(BitAndOp,MultipleOp)


void CDPBitAndOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  BitAndOp *vp = (BitAndOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPBitAndOp


IMPLEMENT_DYNAMIC_CLASS(BitOrOp,MultipleOp)


void CDPBitOrOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  BitOrOp *vp = (BitOrOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPBitOrOp


IMPLEMENT_DYNAMIC_CLASS(BitXorOp,MultipleOp)


void CDPBitXorOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  BitXorOp *vp = (BitXorOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPBitXorOp


IMPLEMENT_DYNAMIC_CLASS(DivideOp,MultipleOp)


void CDPDivideOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  DivideOp *vp = (DivideOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPDivideOp


IMPLEMENT_DYNAMIC_CLASS(ModulusOp,MultipleOp)


void CDPModulusOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  ModulusOp *vp = (ModulusOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPModulusOp


IMPLEMENT_DYNAMIC_CLASS(LshiftOp,MultipleOp)


void CDPLshiftOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  LshiftOp *vp = (LshiftOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPLshiftOp


IMPLEMENT_DYNAMIC_CLASS(RshiftOp,MultipleOp)


void CDPRshiftOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  RshiftOp *vp = (RshiftOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPRshiftOp


IMPLEMENT_DYNAMIC_CLASS(PlusOp,MultipleOp)


void CDPPlusOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                bool baseCDP)

{
  PlusOp *vp = (PlusOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPPlusOp


IMPLEMENT_DYNAMIC_CLASS(MultOp,MultipleOp)


void CDPMultOp (PutGetFlag pgf, ASN1* cid, address varAddr,
                bool baseCDP)

{
  MultOp *vp = (MultOp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPMultipleOp(pgf,cid,(address)(MultipleOp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPMultOp


IMPLEMENT_DYNAMIC_CLASS(Assignment,Expression)


void CDPAssignment (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  Assignment *vp = (Assignment*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->targetObj.CDP(pgf,cid);
    vp->exprValue.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPAssignment


IMPLEMENT_DYNAMIC_CLASS(Parameter,Expression)


void CDPParameter (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  Parameter *vp = (Parameter*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->parameter.CDP(pgf,cid);
    CDPTID(pgf,cid,(address)&vp->formParmID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPParameter


IMPLEMENT_DYNAMIC_CLASS(FuncExpression,Expression)


void CDPFuncExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP)

{
  FuncExpression *vp = (FuncExpression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->inputs.CDP(pgf,cid);
    vp->function.CDP(pgf,cid);
    vp->handle.CDP(pgf,cid);
    CDPTID(pgf,cid,(address)&vp->vtypeID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPFuncExpression


IMPLEMENT_DYNAMIC_CLASS(FuncStatement,FuncExpression)


void CDPFuncStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP)

{
  FuncStatement *vp = (FuncStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPFuncExpression(pgf,cid,(address)(FuncExpression*)vp,true);
    vp->outputs.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPFuncStatement


IMPLEMENT_DYNAMIC_CLASS(Callback,Expression)


void CDPCallback (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  Callback *vp = (Callback*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->callbackType.CDP(pgf,cid);
    vp->callback.CDP(pgf,cid);
    vp->onEvent.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPCallback


IMPLEMENT_DYNAMIC_CLASS(AssertStatement,Expression)


void CDPAssertStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP)

{
  AssertStatement *vp = (AssertStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->statement.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPAssertStatement


IMPLEMENT_DYNAMIC_CLASS(ThrowStatement,Expression)


void CDPThrowStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP)

{
  ThrowStatement *vp = (ThrowStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->error.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPThrowStatement


IMPLEMENT_DYNAMIC_CLASS(IfThen,Expression)


void CDPIfThen (PutGetFlag pgf, ASN1* cid, address varAddr,
                bool baseCDP)

{
  IfThen *vp = (IfThen*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->ifCondition.CDP(pgf,cid);
    vp->thenPart.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPIfThen


IMPLEMENT_DYNAMIC_CLASS(IfStatement,Expression)


void CDPIfStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool baseCDP)

{
  IfStatement *vp = (IfStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->ifThens.CDP(pgf,cid);
    vp->elsePart.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPIfStatement


IMPLEMENT_DYNAMIC_CLASS(IfxThen,Expression)


void CDPIfxThen (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  IfxThen *vp = (IfxThen*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->ifCondition.CDP(pgf,cid);
    vp->thenPart.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPIfxThen


IMPLEMENT_DYNAMIC_CLASS(IfExpression,Expression)


void CDPIfExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP)

{
  IfExpression *vp = (IfExpression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->ifThens.CDP(pgf,cid);
    vp->elsePart.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPIfExpression


IMPLEMENT_DYNAMIC_CLASS(Branch,Expression)


void CDPBranch (PutGetFlag pgf, ASN1* cid, address varAddr,
                bool baseCDP)

{
  Branch *vp = (Branch*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->caseLabels.CDP(pgf,cid);
    vp->thenPart.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPBranch


IMPLEMENT_DYNAMIC_CLASS(SwitchStatement,Expression)


void CDPSwitchStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP)

{
  SwitchStatement *vp = (SwitchStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->caseExpression.CDP(pgf,cid);
    vp->branches.CDP(pgf,cid);
    vp->elsePart.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPSwitchStatement


IMPLEMENT_DYNAMIC_CLASS(CatchClause,Expression)


void CDPCatchClause (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool baseCDP)

{
  CatchClause *vp = (CatchClause*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->catchClause.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPCatchClause


IMPLEMENT_DYNAMIC_CLASS(TryStatement,Expression)


void CDPTryStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP)

{
  TryStatement *vp = (TryStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->tryStatement.CDP(pgf,cid);
    vp->catchClauses.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTryStatement


IMPLEMENT_DYNAMIC_CLASS(TypeBranch,Expression)


void CDPTypeBranch (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  TypeBranch *vp = (TypeBranch*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->exprType.CDP(pgf,cid);
    vp->varName.CDP(pgf,cid);
    vp->thenPart.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTypeBranch


IMPLEMENT_DYNAMIC_CLASS(TypeSwitchStatement,Expression)


void CDPTypeSwitchStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                             bool baseCDP)

{
  TypeSwitchStatement *vp = (TypeSwitchStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->caseExpression.CDP(pgf,cid);
    vp->branches.CDP(pgf,cid);
    vp->elsePart.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTypeSwitchStatement


IMPLEMENT_DYNAMIC_CLASS(AttachObject,Expression)


void CDPAttachObject (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP)

{
  AttachObject *vp = (AttachObject*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->objType.CDP(pgf,cid);
    vp->itf.CDP(pgf,cid);
    vp->url.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPAttachObject


IMPLEMENT_DYNAMIC_CLASS(NewExpression,AttachObject)


void CDPNewExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP)

{
  NewExpression *vp = (NewExpression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPAttachObject(pgf,cid,(address)(AttachObject*)vp,true);
    vp->varName.CDP(pgf,cid);
    vp->initializerCall.CDP(pgf,cid);
    vp->butStatement.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPNewExpression


IMPLEMENT_DYNAMIC_CLASS(CloneExpression,Expression)


void CDPCloneExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP)

{
  CloneExpression *vp = (CloneExpression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->varName.CDP(pgf,cid);
    vp->fromObj.CDP(pgf,cid);
    vp->butStatement.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPCloneExpression


IMPLEMENT_DYNAMIC_CLASS(CopyStatement,Expression)


void CDPCopyStatement (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP)

{
  CopyStatement *vp = (CopyStatement*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->fromObj.CDP(pgf,cid);
    vp->ontoObj.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPCopyStatement


IMPLEMENT_DYNAMIC_CLASS(EnumItem,Expression)


void CDPEnumItem (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  EnumItem *vp = (EnumItem*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->itemNo.CDP(pgf,cid);
    vp->enumType.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPEnumItem


IMPLEMENT_DYNAMIC_CLASS(ExtendExpression,Expression)


void CDPExtendExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP)

{
  ExtendExpression *vp = (ExtendExpression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->extendObj.CDP(pgf,cid);
    vp->extendType.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPExtendExpression


IMPLEMENT_DYNAMIC_CLASS(Run,Expression)


void CDPRun (PutGetFlag pgf, ASN1* cid, address varAddr,
             bool baseCDP)

{
  Run *vp = (Run*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->initiator.CDP(pgf,cid);
    vp->inputs.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPRun


IMPLEMENT_DYNAMIC_CLASS(QueryItf,Expression)


void CDPQueryItf (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  QueryItf *vp = (QueryItf*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->itf.CDP(pgf,cid);
    vp->givenObj.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPQueryItf


IMPLEMENT_DYNAMIC_CLASS(GetUUID,Expression)


void CDPGetUUID (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  GetUUID *vp = (GetUUID*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->itf.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPGetUUID


IMPLEMENT_DYNAMIC_CLASS(IntegerInterval,SynObject)


void CDPIntegerInterval (PutGetFlag pgf, ASN1* cid, address varAddr,
                         bool baseCDP)

{
  IntegerInterval *vp = (IntegerInterval*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPSynObject(pgf,cid,(address)(SynObject*)vp,true);
    vp->from.CDP(pgf,cid);
    vp->to.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPIntegerInterval


IMPLEMENT_DYNAMIC_CLASS(Quantifier,SynObject)


void CDPQuantifier (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  Quantifier *vp = (Quantifier*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPSynObject(pgf,cid,(address)(SynObject*)vp,true);
    vp->elemType.CDP(pgf,cid);
    vp->quantVars.CDP(pgf,cid);
    vp->set.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPQuantifier


IMPLEMENT_DYNAMIC_CLASS(QuantStmOrExp,Expression)


void CDPQuantStmOrExp (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP)

{
  QuantStmOrExp *vp = (QuantStmOrExp*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExpression(pgf,cid,(address)(Expression*)vp,true);
    vp->quantifiers.CDP(pgf,cid);
    vp->statement.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPQuantStmOrExp


IMPLEMENT_DYNAMIC_CLASS(Declare,QuantStmOrExp)


void CDPDeclare (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  Declare *vp = (Declare*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPQuantStmOrExp(pgf,cid,(address)(QuantStmOrExp*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPDeclare


IMPLEMENT_DYNAMIC_CLASS(Exists,QuantStmOrExp)


void CDPExists (PutGetFlag pgf, ASN1* cid, address varAddr,
                bool baseCDP)

{
  Exists *vp = (Exists*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPQuantStmOrExp(pgf,cid,(address)(QuantStmOrExp*)vp,true);
    vp->updateStatement.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPExists


IMPLEMENT_DYNAMIC_CLASS(Foreach,Exists)


void CDPForeach (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  Foreach *vp = (Foreach*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPExists(pgf,cid,(address)(Exists*)vp,true);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPForeach


IMPLEMENT_DYNAMIC_CLASS(SelectExpression,QuantStmOrExp)


void CDPSelectExpression (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP)

{
  SelectExpression *vp = (SelectExpression*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPQuantStmOrExp(pgf,cid,(address)(QuantStmOrExp*)vp,true);
    vp->addObject.CDP(pgf,cid);
    vp->resultSet.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPSelectExpression
