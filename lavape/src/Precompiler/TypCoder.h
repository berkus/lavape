#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class TypCoderCLASS

 **********************************************************************/

#ifndef __TypCoder
#define __TypCoder


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"

#include "Atoms.h"

#include "IntCode.h"


/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/

#include "ElemCode.h"

#include "DParser.h"


/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class TypCoderCLASS {

public:  
  
  /**********************************************************************/
  /*         initialization proc. (the former "module body"):           */
  /**********************************************************************/
  void INIT ();


  void CaseLabelList (TreeNodePtr node);

  void Expression (TreeNodePtr node);

  void TypeDcls (TreeNodePtr node);

  void CodeAssOperator ();

  void CodeCHENST ();

  void CodeCHENSTs ();

  void CodeCHEdcl (TypeTablePtr typeEntry);

  void CodeNST0dcl (TypeTablePtr typeEntry);

  void CodeNSTdcl (TypeTablePtr typeEntry);
  
  void CodeNSTAdcl (TypeTablePtr typeEntry);
  
  void CompleteClassName (TreeNodePtr node);

/**********************************************************************/

private:

  bool isFunctionDef;
  

  void declaration (TreeNodePtr node);

  void declSpecifiers (TreeNodePtr node,
           bool& noDeclSpecifiers);

  void simpleType (TreeNodePtr node);

  void classSpecifier (TreeNodePtr node);

  void baseList (TreeNodePtr node);

  void memberList (TreeNodePtr node,
       bool isPublic);

  void memList (TreeNodePtr node,
    bool& isPublic);

  void memberDeclaration (TreeNodePtr node,
        bool& isPublic);

  void recordCase (TreeNodePtr node,
       bool& isPublic);

  void variant (TreeNodePtr node,
    bool& isPublic);

  void memberDeclarator (TreeNodePtr node);

  void enumSpecifier (TreeNodePtr node);

  void enumeration (TreeNodePtr node);

  void declaratorList (TreeNodePtr node);

  void declarator (TreeNodePtr node);

  void cvQualifierList (TreeNodePtr& node);

  void initializer (TreeNodePtr node);

  void term (TreeNodePtr node);

  void qualExpression (TreeNodePtr node);

  void suffixExpression (TreeNodePtr node);

  void expList (TreeNodePtr node);

  void balancedExp (TreeNodePtr node);
};

extern TypCoderCLASS TypCoder;

#endif
