#ifndef __FioCoder
#define __FioCoder


#include "SYSTEM.h"

#include "IntCode.h"


/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/

/* ****************************************************************/
/* In the following "fio" stands for "forms input/output".        */
/* The fio-procedures generate calls to the elementary            */
/* ASN1 Put/Get procedures                                        */
/* ****************************************************************/

#include "Atoms.h"



class FioCoderCLASS {

public:

  void INIT ();


  void FIOProcGeneration ();

  bool IsMenu (TreeNodePtr node,
                  bool& menuFirst,
                  AtomType& menuType);
                  
/**********************************************************************/

private:

  unsigned indexLevel;
//  unsigned arrayIndexLevel, recordIndexLevel;

  TStringBufferPos currentTypeName; /* for fipEnumType! */
  bool firstFieldList, pureINPUT;


  void declareFIOsOfVariants(TypeTablePtr currentTypeEntry);

  void fipDataType (TreeNodePtr typeSpecifier,
        TreeNodePtr declarator);

  void fopDataType (TreeNodePtr typeSpecifier,
        TreeNodePtr declarator);

  void fipSimpleType (TreeNodePtr node);

  void fopSimpleType (TreeNodePtr node);

  void fipDateType ();

  void fopDateType (TreeNodePtr node);

  void fipTimeType ();

  void fopTimeType (TreeNodePtr node);

  void fipYearType ();

  void fopYearType (TreeNodePtr node);

  void fipEnumType (TreeNodePtr node);

  void fopEnumType (TreeNodePtr node);

  void fipArrayType (TreeNodePtr typeSpecifier,
                     TreeNodePtr subscripts);

  void fopArrayType (TreeNodePtr typeSpecifier,
                     TreeNodePtr subscripts);

  void fioChainNestedType (TreeNodePtr node);
  
  void fioSignatureType (TreeNodePtr node);

  void fipStructType (TreeNodePtr node,
          TreeNodePtr structNode);

  void fioBaseList (TreeNodePtr node);

  void fipMemberList (TreeNodePtr node,
          TreeNodePtr structNode);

  void fipMemberDeclaration (TreeNodePtr node,
           TreeNodePtr structNode,
           unsigned& iField);

  void fipMemberDeclarator (TreeNodePtr declarator,
          TreeNodePtr structNode,
          unsigned& iField);

  void skipPrefixStrings (TreeNodePtr& nodeFieldId,
                          unsigned& prefixCount);

  void fipRecordCase (TreeNodePtr node,
          TreeNodePtr structNode,
          unsigned& iField);

  void fipVariant (TreeNodePtr node);

  void fopStructType (TreeNodePtr node,
          TreeNodePtr structNode);

  void fopMemberList (TreeNodePtr node,
          TreeNodePtr structNode);

  void fopMemberDeclaration (TreeNodePtr node);

  void fopMemberDeclarator (TreeNodePtr declarator);

  void fopRecordCase (TreeNodePtr caseNode);

  void fopVariant (TreeNodePtr node);
};

extern FioCoderCLASS FioCoder;

#endif
