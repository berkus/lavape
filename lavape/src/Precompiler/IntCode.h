#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class IntCodeCLASS

 **********************************************************************/

#ifndef __IntCode
#define __IntCode


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"

#include "Atoms.h"
#include "Scanner.h"
#include "ChainA.h"

extern char slash[2];

enum FlagIC {
  AnnotationNode,
  DONTPUTIC,
  Encrypt,
  FixedCountIC,
  HasATOM,
  HasCDP,
  HasFIO,
  HasSIG,
  HasSYN,
  Included,
  INPUTIC,
  InvisibleIC,
  ManyFields,
  NoEchoIC,
  NoEllipsisIC,
  NoGET,
  NoPrefix,
  NoBreak,
  OUTPUTIC,
  PopUpIC,
  RealizeIC};

typedef SET FlagSet;

const SET AndedAttributes;
/* these attributes are inherited in "upward" direction if a l l
   elements of a composite data type (e.g. RECORD) have the respective
   attribute */

const SET OredAttributes(HasCDP,HasFIO,HasSIG,HasSYN,HasATOM,-1);
/* these attributes are inherited in "upward" direction if at least one
   element of a composite data type (e.g. RECORD) has the respective
   attribute */

const SET DefaultAttributes(HasCDP,HasFIO,HasSYN,HasATOM,-1);
/* these attributes are assigned to atomic data types by default */

enum SyntacticType {
  STabsQualName,
  STactualParameters,
  STapplId,
  STarbitrary,
  STargumentList,
  STasn1Annotation,
  STbaseList,
  STbaseSpec,
  STbaseVariant,
  STbitmap,
  STbitstring,
  STbuttonmenu,
  STbytes,
  STcall,
  STcallSuspend,
  STcaseLabelList,
  STchain,
  STchainAny,
  STchain0,
  STchainAny0,
  STchainx,
  STcheck,
  STcheckpoint,
  STclass,
  STclassMemberPtr,
  STcompClassName,
  STconstructor,
  STdate,
  STdeclaration,
  STdeclarator,
  STdeclaratorList,
  STdeclSpecs,
  STdrop,
  STelseVariant,
  STenumeration,
  STenumItem,
  STerror,
  STexpression,
  STfieldId,
  STform,
  STformAnnotations,
  STfrom,
  STfunction,
  STfunctionBody,
  STget,
  STgetPrivate,
  STinclude,
  STindentation,
  STinitArgs,
  STinitializerList,
  STinitValue,
  STinputObjects,
  STlengthSpec,
  STmemberDeclaration,
  STmemberDeclarator,
  STmemberList,
  STmenu,
  STmoneyString,
  STnested,
  STnested0,
  STnestedAny,
  STnestedAny0,
  STnestedDeclarator,
  STnewLine,
  STobId,
  STobVal,
  SToptionmenu,
  STorgDest,
  STorgunit,
  SToutputObjects,
  STowner,
  STpopup,
  STpopupmenu,
  STprocedure,
  STput,
  STputPrivate,
  STqualExpression,
  STqualName,
  STqualTypeName,
  STreceiverList,
  STrecordCase,
  STreturn,
  STsenderReceiver,
  STset,
  STshow,
  STshowForm,
  STsigID,
  STsigIDlist,
  STsignature,
  STsimpleType,
  STspace,
  STstart,
  STstop,
  STstring,
  STstring0,
  STstruct,
  STsubscriptExpression,
  STsuffixExpression,
  STsuspend,
  STsysObId,
  STsysOrgDest,
  STtab,
  STtagField,
  STterm,
  STtime,
  STtoggle,
  STtype,
  STvariableSpec,
  STvariant,
  STversion,
  STwidgetName,
  STyear};

/* **********************************************************************/
/* TREE NODE TYPES: */


typedef struct TreeNode *TreeNodePtr;

struct TreeNode {
  TreeNodePtr Right, Annotation;
  unsigned SourceLine, SourceCol;
  FlagSet Flags;
  /* CASE */ bool Atomic; /* OF */
  //  true:
      AtomType Atom;
      TStringBufferPos StringBufferPos;

  //| false:
      SyntacticType SynType;
      TreeNodePtr Down;

  //END;
  unsigned CaseLabelOrd;
  /* ORD of enumeration type case label */
};

enum RefMode {
  FormRef,
  ChainAnyRef,
  NestedAnyRef,
  Nested0Ref,
  NestedRef,
  ChainAnyRefElseWhere,
  NestedAnyRefElseWhere,
  Nested0RefElseWhere,
  NestedRefElseWhere};

typedef SET SetOfRefModes;

typedef struct TypeTableEntry *TypeTablePtr;

struct CHETypeTablePtr : ChainAnyElem {
  TypeTablePtr data;

  ChainAnyElem* Clone ()
  { return new CHETypeTablePtr(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHETypeTablePtr*)from)->data; }
};

struct TypeTableEntry {
  TypeTablePtr successor, nextEntry, baseType, CDPfrom, SIGfrom;
  /* "successor" in global type list,
     "nextEntry" in partial type list corrsponding to a
     single hash table slot */
  ChainAny/*TypeTablePtr*/ variantTypes;
  TStringBufferPos id;
  SetOfRefModes refModes;
  FlagSet attributes, procGenAttributes;
  TreeNodePtr typeSpecifier, declarator;
  DString module;
  unsigned typeIndex, indexLevel;
  bool isForm, hasConstructor;
};

/* **********************************************************************/
/* OTHER TYPES: */

enum TGlobalFlag {
  PPescEncountered,
  IncludeFile,
  
  ASN1ppUsed,
  CHAINUsed,
  CHAINANYUsed,
  NESTEDANYUsed,
  DateTimeUsed,
  DTypesUsed,
  OrcServUsed,
  SETUsed,
  STRINGUsed,
  
  CDPgen,
  FIOgen,
  SIGNgen,
  SYNgen,
  
  DEPgen,
  MODgen};

typedef SET TGlobalFlags;

/* **********************************************************************/
/* VARIABLES: */

/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/

#include "PCerrors.h"
#include "DString.h"

static const unsigned stackSize = 50;

static const unsigned hashPrimeIC = 257;

/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class IntCodeCLASS {

public:

  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();

  struct TGlobalProps {
    TGlobalFlags Flags;
  } GlobalProperties;
    
  TreeNodePtr TreeRoot, CurrentDefType, CurrentRefType,
        CurrentTypeSpecifier, CurrentDeclarator,
        TagFieldNode, PopupNode;

  TypeTablePtr TypeTable, BaseTypeEntry, CDPfromEntry, SIGfromEntry,
               CurrentTypeEntry;
  
  unsigned typeInd, indexLevel;

  SetOfRefModes RefModes;

  FlagSet attributes, procGenAttributes, typeRefAttributes;
  
  unsigned IncludeLevel, ScopeNestingLevel;

  ChainAny/*TypeTablePtr*/ ProcTypes;

  bool ImplModIsCoded, DclsAreCoded,
      FunctionsAreToBeGenerated, IsForm, HasConstructor;
  
  
  /* **********************************************************************/
  /* TREE AND STACK MANAGEMENT PROCEDURES: */

  void DefTypeIdent ();

  void KeepDefType (TreeNodePtr node);

  void RefTypeIdent ();

  void Push (SyntacticType sT);
 
  void Interpose (SyntacticType sT);
 
  void InterposeDontPop (SyntacticType sT);

  void PushAnnotation (SyntacticType sT);

  void PushAtom ();

  void PushEmpty ();

  void Pop ();

  void PopAnnotation ();

  TreeNodePtr TopOfStack ();

  unsigned DepthOfStack ();

  void SaveStack (TreeNodePtr& tos);

  void RestoreStack (TreeNodePtr tos);

  void ResetStack ();

  TreeNodePtr FindAnnos (TreeNodePtr node,
                         SyntacticType annoType);

  /* **********************************************************************/
  /* TABLE MANAGEMENT PROCEDURES: */

  void TypeDef ();

  void CheckTypeDefined (bool noAlibi,
       bool shouldBeFormType);

  void TypeRef (FlagSet& attributes);

  void CheckAllTypesDefined ();
  /* called on eof since Chained/Nested types need not be defined 
     before the first reference (in order to support recursive types!)*/

  void LookUpType (TStringBufferPos id,
                   TypeTablePtr& entryFound);


  void SetCurrentTypeEntry (TreeNodePtr typeIdNode)
  {
    LookUpType(typeIdNode->StringBufferPos,CurrentTypeEntry);
  }


  void PrepareProcDcls ();



  void FindEndOfReferenceChain (TreeNodePtr startNode,
                                TreeNodePtr& endNode,
                                TreeNodePtr& indirRefNode,
                                bool& isLast);

/**********************************************************************/

private:

  TreeNodePtr stack[stackSize];
  unsigned iStack, indent;
  TreeNodePtr treeNodePtr;
  TypeTablePtr lastType;


  /* **********************************************************************/

  /* TREE AND STACK MANAGEMENT PROCEDURES: */


  void appendToStack ();


  void appendAnnot ();
  
  struct {
    TypeTablePtr firstEntry, lastEntry;
  } hashTable[hashPrimeIC-1+1];


  unsigned hashCode (char *s);

  unsigned hc; /* last hash table slot */


  void appendType (TypeTablePtr& newEntry);
};

extern IntCodeCLASS IntCode;

#endif
