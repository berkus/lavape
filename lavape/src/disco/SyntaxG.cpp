//*******************************************************************
//
//   public procedures of structs/classes defined in Syntax.h
//
//*******************************************************************

#include "Syntax.h"


#include "CDPpp.h"


IMPLEMENT_DYNAMIC_CLASS(TSigRef,NULL)


void CDPTSigRef (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  TSigRef *vp = (TSigRef*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTbool(pgf,cid,vp->GlobalScope);
    vp->SigID.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTSigRef


IMPLEMENT_DYNAMIC_CLASS(TSigDef,NULL)


void CDPTSigDef (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  TSigDef *vp = (TSigDef*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTunsigned(pgf,cid,vp->Scope);
    vp->SigID.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTSigDef

ChainAnyElem* NewCHETID ()
{ return (ChainAnyElem*)(new CHETID); }


IMPLEMENT_DYNAMIC_CLASS(TID,NULL)


void CDPTID (PutGetFlag pgf, ASN1* cid, address varAddr,
             bool baseCDP)

{
  TID *vp = (TID*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTint(pgf,cid,vp->nINCL);
    CDPpp.CVTint(pgf,cid,vp->nID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTID


void CDPTIDs (PutGetFlag pgf, ASN1* cid, address varAddr,
              bool)

{
  TIDs *vp = (TIDs*)varAddr;
  if (cid->Skip()) return;

  vp->CDP(pgf,cid,CDPTID,NewCHETID);
} // END OF CDPTIDs


void CDPSynFlags (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool)

{
  SynFlags *vp = (SynFlags*)varAddr;
  if (cid->Skip()) return;

  vp->CDP(pgf,cid);
} // END OF CDPSynFlags


void CDPTOperator (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool)

{
  TOperator *vp = (TOperator*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TOperator)cid->FGETunsigned();
} // END OF CDPTOperator

ChainAnyElem* NewCHETVElem ()
{ return (ChainAnyElem*)(new CHETVElem); }


IMPLEMENT_DYNAMIC_CLASS(TVElem,NULL)


void CDPTVElem (PutGetFlag pgf, ASN1* cid, address varAddr,
                bool baseCDP)

{
  TVElem *vp = (TVElem*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTint(pgf,cid,vp->updateNo);
    CDPpp.CVTbool(pgf,cid,vp->ok);
    CDPSynFlags(pgf,cid,(address)&vp->TypeFlags);
    CDPTID(pgf,cid,(address)&vp->VTEl);
    CDPTID(pgf,cid,(address)&vp->VTBaseEl);
    CDPTID(pgf,cid,(address)&vp->VTClss);
    CDPTIDs(pgf,cid,(address)&vp->Ambgs);
    CDPTOperator(pgf,cid,(address)&vp->op);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTVElem


void CDPTCTVElem (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool)

{
  TCTVElem *vp = (TCTVElem*)varAddr;
  if (cid->Skip()) return;

  vp->CDP(pgf,cid,CDPTVElem,NewCHETVElem);
} // END OF CDPTCTVElem


IMPLEMENT_DYNAMIC_CLASS(TVElems,NULL)


void CDPTVElems (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  TVElems *vp = (TVElems*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTint(pgf,cid,vp->UpdateNo);
    CDPTCTVElem(pgf,cid,(address)&vp->VElems);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTVElems


IMPLEMENT_DYNAMIC_CLASS(CLavaError,NULL)


void CDPCLavaError (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  CLavaError *vp = (CLavaError*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    vp->textParam.CDP(pgf,cid);
    CDPpp.CVTbool(pgf,cid,vp->showAutoCorrBox);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPCLavaError


void CDPTEmphasis (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool)

{
  TEmphasis *vp = (TEmphasis*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TEmphasis)cid->FGETunsigned();
} // END OF CDPTEmphasis


void CDPTIndentation (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool)

{
  TIndentation *vp = (TIndentation*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TIndentation)cid->FGETunsigned();
} // END OF CDPTIndentation


void CDPTJustification (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool)

{
  TJustification *vp = (TJustification*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TJustification)cid->FGETunsigned();
} // END OF CDPTJustification


void CDPTAlignment (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool)

{
  TAlignment *vp = (TAlignment*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TAlignment)cid->FGETunsigned();
} // END OF CDPTAlignment


void CDPTBox (PutGetFlag pgf, ASN1* cid, address varAddr,
              bool)

{
  TBox *vp = (TBox*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TBox)cid->FGETunsigned();
} // END OF CDPTBox


void CDPTBasicType (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool)

{
  TBasicType *vp = (TBasicType*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TBasicType)cid->FGETunsigned();
} // END OF CDPTBasicType


void CDPTreeFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool)

{
  TreeFlag *vp = (TreeFlag*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TreeFlag)cid->FGETunsigned();
} // END OF CDPTreeFlag


void CDPTypeFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool)

{
  TypeFlag *vp = (TypeFlag*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TypeFlag)cid->FGETunsigned();
} // END OF CDPTypeFlag


void CDPSecondTFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool)

{
  SecondTFlag *vp = (SecondTFlag*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (SecondTFlag)cid->FGETunsigned();
} // END OF CDPSecondTFlag


void CDPWorkFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool)

{
  WorkFlag *vp = (WorkFlag*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (WorkFlag)cid->FGETunsigned();
} // END OF CDPWorkFlag


void CDPBasicFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool)

{
  BasicFlag *vp = (BasicFlag*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (BasicFlag)cid->FGETunsigned();
} // END OF CDPBasicFlag


void CDPIoSigFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool)

{
  IoSigFlag *vp = (IoSigFlag*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (IoSigFlag)cid->FGETunsigned();
} // END OF CDPIoSigFlag


void CDPIterFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool)

{
  IterFlag *vp = (IterFlag*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (IterFlag)cid->FGETunsigned();
} // END OF CDPIterFlag


void CDPPrintFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool)

{
  PrintFlag *vp = (PrintFlag*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (PrintFlag)cid->FGETunsigned();
} // END OF CDPPrintFlag

NESTEDINITCOPY(TDECLComment)

NESTEDCDP(TDECLComment)


IMPLEMENT_DYNAMIC_CLASS(TDECLComment,NULL)


void CDPTDECLComment (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP)

{
  TDECLComment *vp = (TDECLComment*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    vp->Comment.CDP(pgf,cid);
    CDPSynFlags(pgf,cid,(address)&vp->PrintFlags);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTDECLComment


void CDPTBoundRel (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool)

{
  TBoundRel *vp = (TBoundRel*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TBoundRel)cid->FGETunsigned();
} // END OF CDPTBoundRel

ChainAnyElem* NewCHEEnumSelId ()
{ return (ChainAnyElem*)(new CHEEnumSelId); }


IMPLEMENT_DYNAMIC_CLASS(EnumSelId,NULL)


void CDPEnumSelId (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  EnumSelId *vp = (EnumSelId*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    vp->Id.CDP(pgf,cid);
    vp->SelectionCode.CDP(pgf,cid);
    vp->DECLComment.CDP(pgf,cid,CDPTDECLComment);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPEnumSelId


void CDPTDeclType (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool)

{
  TDeclType *vp = (TDeclType*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TDeclType)cid->FGETunsigned();
} // END OF CDPTDeclType


void CDPTDeclDescType (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool)

{
  TDeclDescType *vp = (TDeclDescType*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (TDeclDescType)cid->FGETunsigned();
} // END OF CDPTDeclDescType


void CDPTSupports (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool)

{
  TSupports *vp = (TSupports*)varAddr;
  if (cid->Skip()) return;

  vp->CDP(pgf,cid,CDPTID,NewCHETID);
} // END OF CDPTSupports


IMPLEMENT_DYNAMIC_CLASS(TLength,NULL)


void CDPTLength (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  TLength *vp = (TLength*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTunsigned(pgf,cid,vp->Field);
    CDPpp.CVTunsigned(pgf,cid,vp->Data);
    CDPTBoundRel(pgf,cid,(address)&vp->FieldBoundRel);
    CDPTBoundRel(pgf,cid,(address)&vp->DataBoundRel);
    CDPpp.CVTunsigned(pgf,cid,vp->DecPoint);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTLength


void CDPAnnoExType (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool)

{
  AnnoExType *vp = (AnnoExType*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (AnnoExType)cid->FGETunsigned();
} // END OF CDPAnnoExType

ChainAnyElem* NewCHETAnnoEx ()
{ return (ChainAnyElem*)(new CHETAnnoEx); }


IMPLEMENT_DYNAMIC_CLASS(TAnnoEx,NULL)


void CDPTAnnoEx (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool baseCDP)

{
  TAnnoEx *vp = (TAnnoEx*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPAnnoExType(pgf,cid,(address)&vp->exType);
    if (cid->Skip()) return;
    switch (vp->exType) {
    case anno_Color:
    case anno_TextColor:
    case anno_PBColor:
      CDPpp.CVTbool(pgf,cid,vp->RgbBackValid);
      CDPpp.CVTbool(pgf,cid,vp->RgbForeValid);
      CDPpp.CVTunsigned(pgf,cid,vp->RgbBackColor);
      CDPpp.CVTunsigned(pgf,cid,vp->RgbForeColor);
      break;
    case anno_Pixmap:
      vp->xpmFile.CDP(pgf,cid);
      break;
    }
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTAnnoEx

NESTEDINITCOPY(TAnnotation)

NESTEDCDP(TAnnotation)


IMPLEMENT_DYNAMIC_CLASS(TAnnotation,NULL)


void CDPTAnnotation (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool baseCDP)

{
  TAnnotation *vp = (TAnnotation*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    vp->Iter.CDP(pgf,cid);
    vp->Prefixes.CDP(pgf,cid);
    vp->Suffixes.CDP(pgf,cid);
    vp->FA.CDP(pgf,cid);
    vp->PopupNode.CDP(pgf,cid);
    vp->MenuDECL.CDP(pgf,cid);
    vp->IterOrig.CDP(pgf,cid);
    CDPSynFlags(pgf,cid,(address)&vp->BasicFlags);
    CDPSynFlags(pgf,cid,(address)&vp->IoSigFlags);
    CDPSynFlags(pgf,cid,(address)&vp->IterFlags);
    vp->WidgetName.CDP(pgf,cid);
    CDPTLength(pgf,cid,(address)&vp->Length);
    CDPTSigDef(pgf,cid,(address)&vp->SigDef);
    vp->AnnoEx.CDP(pgf,cid,CDPTAnnoEx,NewCHETAnnoEx);
    CDPTEmphasis(pgf,cid,(address)&vp->Emphasis);
    CDPpp.CVTunsigned(pgf,cid,vp->Space);
    CDPpp.CVTint(pgf,cid,vp->TabPosition);
    CDPpp.CVTint(pgf,cid,vp->FrameSpacing);
    CDPTIndentation(pgf,cid,(address)&vp->IndType);
    CDPpp.CVTint(pgf,cid,vp->Indentation);
    CDPTJustification(pgf,cid,(address)&vp->JustType);
    CDPTAlignment(pgf,cid,(address)&vp->Alignment);
    CDPTBox(pgf,cid,(address)&vp->Box);
    vp->StringValue.CDP(pgf,cid);
    CDPTBasicType(pgf,cid,(address)&vp->BType);
    if (cid->Skip()) return;
    switch (vp->BType) {
    case B_Bool:
      CDPpp.CVTbool(pgf,cid,vp->B);
      break;
    case Enumeration:
      CDPpp.CVTunsigned(pgf,cid,vp->D);
      break;
    case Integer:
      CDPpp.CVTint(pgf,cid,vp->I);
      break;
    }
    vp->String1.CDP(pgf,cid);
    vp->String2.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTAnnotation

NESTEDINITCOPY(LavaDECL)

NESTEDCDP(LavaDECL)


IMPLEMENT_DYNAMIC_CLASS(LavaDECL,NULL)


void CDPLavaDECL (PutGetFlag pgf, ASN1* cid, address varAddr,
                  bool baseCDP)

{
  LavaDECL *vp = (LavaDECL*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    vp->LocalName.CDP(pgf,cid);
    vp->DECLComment.CDP(pgf,cid,CDPTDECLComment);
    CDPSynFlags(pgf,cid,(address)&vp->TreeFlags);
    CDPpp.CVTint(pgf,cid,vp->OwnID);
    vp->Annotation.CDP(pgf,cid,CDPTAnnotation);
    CDPSynFlags(pgf,cid,(address)&vp->TypeFlags);
    CDPSynFlags(pgf,cid,(address)&vp->SecondTFlags);
    CDPTID(pgf,cid,(address)&vp->RefID);
    CDPTDeclType(pgf,cid,(address)&vp->DeclType);
    CDPTOperator(pgf,cid,(address)&vp->op);
    CDPpp.CVTint(pgf,cid,vp->nInput);
    CDPpp.CVTint(pgf,cid,vp->nOutput);
    CDPTSupports(pgf,cid,(address)&vp->Supports);
    CDPTSupports(pgf,cid,(address)&vp->Inherits);
    vp->Items.CDP(pgf,cid,CDPEnumSelId,NewCHEEnumSelId);
    CDPTDeclDescType(pgf,cid,(address)&vp->DeclDescType);
    if (cid->Skip()) return;
    switch (vp->DeclDescType) {
    case BasicType:
      CDPTBasicType(pgf,cid,(address)&vp->BType);
      break;
    case EnumType:
      vp->EnumDesc.CDP(pgf,cid);
    case StructDesc:
      vp->NestedDecls.CDP(pgf,cid);
    case LiteralString:
      vp->LitStr.CDP(pgf,cid);
      break;
    case ExecDesc:
      vp->Exec.CDP(pgf,cid);
      break;
    }
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPLavaDECL


IMPLEMENT_DYNAMIC_CLASS(TEnumDescription,NULL)


void CDPTEnumDescription (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP)

{
  TEnumDescription *vp = (TEnumDescription*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPLavaDECL(pgf,cid,(address)&vp->EnumField);
    CDPLavaDECL(pgf,cid,(address)&vp->MenuTree);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTEnumDescription


IMPLEMENT_DYNAMIC_CLASS(TIteration,NULL)


void CDPTIteration (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  TIteration *vp = (TIteration*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPTBoundRel(pgf,cid,(address)&vp->BoundType);
    CDPpp.CVTunsigned(pgf,cid,vp->Bound);
    vp->IteratedExpr.CDP(pgf,cid,CDPLavaDECL);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPTIteration

ChainAnyElem* NewCHESimpleSyntax ()
{ return (ChainAnyElem*)(new CHESimpleSyntax); }


IMPLEMENT_DYNAMIC_CLASS(SimpleSyntax,NULL)


void CDPSimpleSyntax (PutGetFlag pgf, ASN1* cid, address varAddr,
                      bool baseCDP)

{
  SimpleSyntax *vp = (SimpleSyntax*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTint(pgf,cid,vp->nINCL);
    vp->UsersName.CDP(pgf,cid);
    vp->LocalTopName.CDP(pgf,cid);
    vp->TopDef.CDP(pgf,cid,CDPLavaDECL);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPSimpleSyntax


void CDPSyntaxDefinition (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool)

{
  SyntaxDefinition *vp = (SyntaxDefinition*)varAddr;
  if (cid->Skip()) return;

  vp->CDP(pgf,cid,CDPSimpleSyntax,NewCHESimpleSyntax);
} // END OF CDPSyntaxDefinition

ChainAnyElem* NewCHEClipSimpleSyntax ()
{ return (ChainAnyElem*)(new CHEClipSimpleSyntax); }


IMPLEMENT_DYNAMIC_CLASS(ClipSimpleSyntax,NULL)


void CDPClipSimpleSyntax (PutGetFlag pgf, ASN1* cid, address varAddr,
                          bool baseCDP)

{
  ClipSimpleSyntax *vp = (ClipSimpleSyntax*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTint(pgf,cid,vp->nINCL);
    vp->SyntaxName.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPClipSimpleSyntax


void CDPClipSyntaxDefinition (PutGetFlag pgf, ASN1* cid, address varAddr,
                              bool)

{
  ClipSyntaxDefinition *vp = (ClipSyntaxDefinition*)varAddr;
  if (cid->Skip()) return;

  vp->CDP(pgf,cid,CDPClipSimpleSyntax,NewCHEClipSimpleSyntax);
} // END OF CDPClipSyntaxDefinition


IMPLEMENT_DYNAMIC_CLASS(SynObjectBase,NULL)


void CDPSynObjectBase (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP)

{
  SynObjectBase *vp = (SynObjectBase*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPSynObjectBase


void CDPDbgCommand (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool)

{
  DbgCommand *vp = (DbgCommand*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (DbgCommand)cid->FGETunsigned();
} // END OF CDPDbgCommand


void CDPStopReason (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool)

{
  StopReason *vp = (StopReason*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (StopReason)cid->FGETunsigned();
} // END OF CDPStopReason


void CDPDbgContType (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool)

{
  DbgContType *vp = (DbgContType*)varAddr;
  if (cid->Skip()) return;

  if (pgf == PUT) cid->PUTunsigned(*vp);
  else *vp = (DbgContType)cid->FGETunsigned();
} // END OF CDPDbgContType


IMPLEMENT_DYNAMIC_CLASS(DDItemData,NULL)


void CDPDDItemData (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  DDItemData *vp = (DDItemData*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTbool(pgf,cid,vp->isPrivate);
    CDPpp.CVTbool(pgf,cid,vp->HasChildren);
    vp->Column0.CDP(pgf,cid);
    vp->Column1.CDP(pgf,cid);
    vp->Column2.CDP(pgf,cid);
    vp->Children.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPDDItemData

ChainAnyElem* NewCHEStackData ()
{ return (ChainAnyElem*)(new CHEStackData); }


IMPLEMENT_DYNAMIC_CLASS(StackData,NULL)


void CDPStackData (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  StackData *vp = (StackData*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTint(pgf,cid,vp->SynObjID);
    CDPTDeclType(pgf,cid,(address)&vp->ExecType);
    CDPTID(pgf,cid,(address)&vp->FuncID);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPStackData

ChainAnyElem* NewCHEProgPoint ()
{ return (ChainAnyElem*)(new CHEProgPoint); }

NESTEDINITCOPY(ProgPoint)

NESTEDCDP(ProgPoint)


IMPLEMENT_DYNAMIC_CLASS(ProgPoint,NULL)


void CDPProgPoint (PutGetFlag pgf, ASN1* cid, address varAddr,
                   bool baseCDP)

{
  ProgPoint *vp = (ProgPoint*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTint(pgf,cid,vp->SynObjID);
    CDPTDeclType(pgf,cid,(address)&vp->ExecType);
    CDPTID(pgf,cid,(address)&vp->FuncID);
    CDPpp.CVTbool(pgf,cid,vp->Activate);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPProgPoint


IMPLEMENT_DYNAMIC_CLASS(DbgStopData,NULL)


void CDPDbgStopData (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool baseCDP)

{
  DbgStopData *vp = (DbgStopData*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPStopReason(pgf,cid,(address)&vp->stopReason);
    CDPpp.CVTint(pgf,cid,vp->ActStackLevel);
    vp->StackChain.CDP(pgf,cid,CDPStackData,NewCHEStackData);
    vp->ObjectChain.CDP(pgf,cid);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPDbgStopData


IMPLEMENT_DYNAMIC_CLASS(DbgContData,NULL)


void CDPDbgContData (PutGetFlag pgf, ASN1* cid, address varAddr,
                     bool baseCDP)

{
  DbgContData *vp = (DbgContData*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTbool(pgf,cid,vp->ClearBrkPnts);
    vp->BrkPnts.CDP(pgf,cid,CDPProgPoint,NewCHEProgPoint);
    CDPDbgContType(pgf,cid,(address)&vp->ContType);
    vp->RunToPnt.CDP(pgf,cid,CDPProgPoint);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPDbgContData

NESTEDINITCOPY(ChObjRq)

NESTEDCDP(ChObjRq)


void CDPChObjRq (PutGetFlag pgf, ASN1* cid, address varAddr,
                 bool)

{
  ChObjRq *vp = (ChObjRq*)varAddr;
  if (cid->Skip()) return;

  vp->CDP(pgf,cid,CDPint,NewCHEint);
} // END OF CDPChObjRq


IMPLEMENT_DYNAMIC_CLASS(DbgMessage,NULL)


void CDPDbgMessage (PutGetFlag pgf, ASN1* cid, address varAddr,
                    bool baseCDP)

{
  DbgMessage *vp = (DbgMessage*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPDbgCommand(pgf,cid,(address)&vp->Command);
    if (cid->Skip()) return;
    switch (vp->Command) {
    case Dbg_StopData:
    case Dbg_Stack:
      vp->DbgData.CDP(pgf,cid);
      break;
    case Dbg_MemberData:
      vp->ObjData.CDP(pgf,cid);
      break;
    case Dbg_MemberDataRq:
      vp->ObjNr.CDP(pgf,cid,CDPChObjRq);
      break;
    case Dbg_StackRq:
      CDPpp.CVTint(pgf,cid,vp->CallStackLevel);
      break;
    case Dbg_Continue:
      vp->ContData.CDP(pgf,cid);
      break;
    }
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
} // END OF CDPDbgMessage

ChainAnyElem* NewCHESigNodePtr ()
{ return (ChainAnyElem*)(new CHESigNodePtr); }

ChainAnyElem* NewCHEFormNode ()
{ return (ChainAnyElem*)(new CHEFormNode); }
