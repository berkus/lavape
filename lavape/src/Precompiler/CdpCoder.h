#ifndef __CdpCoder
#define __CdpCoder


#include "SYSTEM.h"

#include "IntCode.h"

/* ****************************************************************/
/* In the following "cdp" stands for "coding/decoding procedure". */
/* The cdp-procedures generate the calls to the elementary        */
/* X.409pp Cvt procedures:                            */
/* {CVT}{SEQUENCE|EOC|unsigned|int|float|...}                     */
/* ****************************************************************/


class CdpCoderCLASS {

public:

  DString pgf;
  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  bool SigCoderCalling;


  void CDProcGeneration ();

  void declareCDPsOfVariants(TypeTablePtr currentTypeEntry);

  void CdpDataType (TreeNodePtr typeSpecifier, 
        TreeNodePtr declarator);
  /* used in SigCoder */

  void SkipPrefixStrings (TreeNodePtr& node);

  void CaseLabelList (TreeNodePtr node);

  void TypeName (TreeNodePtr node, bool cdpName=false);
  /* used in FioCoder */

/**********************************************************************/

private:

  unsigned scopeNestingLevel, arrayIndexLevel;

  void altTag (TreeNodePtr node);

  void cdpSimpleType (TreeNodePtr node);

  void cdpChainNestedType (TreeNodePtr node);

  void cdpDateType ();

  void cdpTimeType ();

  void cdpYearType ();

  void cdpSignatureType ();

  void cdpArrayType (TreeNodePtr typeSpecifier,
                     TreeNodePtr declarator);

  void cdpEnumType (TreeNodePtr node);

  void cdpStructType (TreeNodePtr node);

  void cdpBaseList (TreeNodePtr node);

  void cdpMemberList (TreeNodePtr node);

  void cdpMemberDeclaration (TreeNodePtr node);

  void cdpMemberDeclarator (TreeNodePtr node);

  void cdpRecordCase (TreeNodePtr node);

  void cdpVariant (TreeNodePtr node);
};

extern CdpCoderCLASS CdpCoder;

#endif
