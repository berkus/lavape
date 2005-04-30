#ifndef __SigCoder
#define __SigCoder


#include "SYSTEM.h"
#include "IntCode.h"

/*****************************************************************/
/* In the following "cdp" stands for "coding/decoding procedure".*/
/* The cdp-procedures generate the calls to the elementary       */
/*                 X.409pp Cvt procedures:                       */
/*  {Cvt}{SEQUENCE|EOC|CARDINAL|INTEGER|REAL|CHAR|STRING}        */
/*****************************************************************/

#include "TypCoder.h"
#include "CdpCoder.h"
#include "ElemCode.h"
#include "Atoms.h"
#include "DString.h"
#include "DIO.h"



class SigCoderCLASS {

public:

  void INIT ();

  void SIGProcGeneration ();

/**********************************************************************/


private:

  unsigned arrayIndexLevel;


  void declareSIGsOfVariants(TypeTablePtr currentTypeEntry);

  void sigDataType (TreeNodePtr typeSpecifier,
        TreeNodePtr declarator);

  void sigCdp (TreeNodePtr sigAnnos,
               TreeNodePtr typeSpecifier,
               TreeNodePtr declarator);

  void sigArrayType (TreeNodePtr typeSpecifier,
                     TreeNodePtr subscripts);

  void sigChainType (TreeNodePtr node);

  void sigNestedType (TreeNodePtr node);

  void sigTypeRef (TreeNodePtr node);

  void sigStructType (TreeNodePtr node);

  void sigBaseList (TreeNodePtr node);

  void sigMemberList (TreeNodePtr node);

  void sigMemberDeclaration (TreeNodePtr node);

  void sigMemberDeclarator (TreeNodePtr node);

  void sigRecordCase (TreeNodePtr node);

  void sigVariant (TreeNodePtr node);

  void CaseLabelList (TreeNodePtr node);
};

extern SigCoderCLASS SigCoder;

#endif
