#ifndef __DParser
#define __DParser

#include "SYSTEM.h"
#include "ChString.h"
#include "IntCode.h"
#include "CmdError.h"
#include "Scanner.h"
#include "Atoms.h"
#include "ContxtCK.h"
#include "PCerrors.h"


class DParserCLASS {

public:
  enum {isClass,isEnum,isTypedef} typeOfType;
 
  ChainOfString IncludedFiles;

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  void ImportStatements ();

  void TypeAndFormStatements ();

  void NonPPstatements ();

  void Expression ();

  unsigned Value (TreeNodePtr node);

/**********************************************************************/

private:

  unsigned scopeNestingLevel, declaratorLevel;

  DString msgText;

  bool isCase, isTagField, isChain, isNested, isMoneyString;
  
  unsigned typeIndex;

  bool emptyAnnotation;



  /* ************************************************************************/
  /* declaration MACROS: */



  void includeMacro ();

  void include (const DString& filename);

  bool notYetIncluded (const DString& filename);

  void typeMacro ();

  void declaration ();

  void declSpecifiers (FlagSet& attributes,
             bool& isFunctionDef);

  void qualifiedName ();

  void classSpecifier (FlagSet& attributes);

  void baseVariant ();

  void baseList ();

  void memberList (FlagSet& attributes);

  void memList (FlagSet& attributes,
    unsigned& fieldCount,
    bool& noPrefix);

  void memberDeclaration (FlagSet& attributes,
                      unsigned& fieldCount,
                      bool& noPrefix,
                      bool& isFunctionDef);

  void memberDeclarator (bool& hasFieldId,
       bool& noPrefix,
                     bool& isFunctionDef);

  void enumSpecifier (FlagSet& attributes);

  void qualName ();

  void qualifiedTypeName ();

  void completeClassName ();

  void annotations (FlagSet& attributes);

  void simpleAnnotations (FlagSet& attributes);

  void formAnnotations (FlagSet& attributes);

  void asn1Tag ();

  void signatureType (FlagSet& attributes);

  void sigRefs ();

  void stringType (FlagSet& attributes);

  void chainType (FlagSet& attributes);

  void nestedType (FlagSet& attributes);

  void simpleType (FlagSet& attributes);

  void enumeration (bool parenth,
                    FlagSet& attributes);

  void explanationStrings (unsigned& explanStrCount);

  void recordCase (FlagSet& attributes);

  void variant (FlagSet& attributes,
                TreeNodePtr tagFieldNode);

  void elseVariant (FlagSet& attributes);

  void caseLabelList (TreeNodePtr tagFieldNode);

  void caseLabel (TreeNodePtr tagFieldNode);

  void tagField (FlagSet& attributes, bool& invisible);

  void setType (FlagSet& attributes);

  void declaratorList (FlagSet& attributes);

  void declarator (bool& hasFieldId,
       bool& noPrefix,
       bool optional,
       FlagSet& attributes,
       bool& isFunctionDef);

  void fieldIdentifier(bool optional,
           bool afterId,
             bool& hasFieldId,
             bool& noPrefix);

  void fieldId (TreeNodePtr tos,
    bool& noPrefix,
    unsigned& explanStrCount);

  void nestedDeclarator (bool& hasFieldId,
             bool& noPrefix,
       FlagSet& attributes);

  void popupDeclarator (bool& hasFieldId,
      bool& noPrefix,
      FlagSet& attributes);

  void initializer ();

  void cvQualifierList ();

  void identOrQualName ();

  void balancedExp (SyntacticType sT,
          AtomType lParenth,
          AtomType rParenth);

  void constantExpression ();

  void intConstOrName ();

  void term ();

  void qualExpression ();

  bool infixOp ();

  void suffixExpression ();

  void actualParameters ();

  void expList ();

  void lengthSpec ();

  void chainLength ();

  void ellipsisText ();
};

extern DParserCLASS DParser;

#endif
