#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

               #include file for class ContextCKCLASS

 **********************************************************************/

#ifndef __ContextCK
#define __ContextCK

/**********************************************************************/
/*                                                                    */
/*                            #includes                               */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"

#include "IntCode.h"


/**********************************************************************/
/*                                                                    */
/*                             #includes                              */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/



#include "Atoms.h"



#include "PCerrors.h"

#include "DString.h"


/**********************************************************************/
/*                                                                    */
/*                         types/constants                            */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/



/**********************************************************************/
/*                                                                    */
/*                        constants/PP-types                          */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/



/**********************************************************************/
/*                                                                    */
/*           DEFINITION OF CLASS (~ Modula-2 - MODULE)                */
/*                                                                    */
/**********************************************************************/

class ContextCKCLASS {

public:

/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/

  void INIT ();

  /*************************************************************************/
  /* context dependent checks: */

  void CheckIndexType (TreeNodePtr indexNode,
                       TreeNodePtr referencedIndexType);

  void CheckCaseLabelExpr (TreeNodePtr node,
         TreeNodePtr tagFieldNode);

  void CheckDecimalConstExpression (TreeNodePtr node);

  void CheckDecimalConstSimpleExpression (TreeNodePtr node);

  void CheckTypeCompatibility (TreeNodePtr formType,
                               TreeNodePtr baseType);


/**********************************************************************/


private:


/**********************************************************************/
/*                                                                    */
/*                        non-PP-types                                */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/


/**********************************************************************/


  bool Equivalent (TreeNodePtr formType,
                      TreeNodePtr baseType);


  void cmpStringBuffer (TreeNodePtr formType,
                        TreeNodePtr baseType);


  void cmpArrayType (TreeNodePtr formType,
                     TreeNodePtr baseType);


  void cmpArrayIndices (TreeNodePtr formType,
                        TreeNodePtr baseType);


  bool existenceDifference (TreeNodePtr formType,
                               TreeNodePtr baseType,
                               TreeNodePtr precedingFormType);


  bool succExistenceDifference (TreeNodePtr& formType,
                                   TreeNodePtr& baseType);


  void cmpChainType (TreeNodePtr formType,
                     TreeNodePtr baseType);


  void skipPureTextItems (TreeNodePtr& item,
                          TreeNodePtr& id,
                          TreeNodePtr& precedingItem,
                          TreeNodePtr& precedingId);


  void cmpEnumeration (TreeNodePtr formType,
                       TreeNodePtr baseType);


  void cmpRecordType (TreeNodePtr formType,
                      TreeNodePtr baseType);


  void cmpFieldList (TreeNodePtr formType,
                     TreeNodePtr baseType);


  void cmpFieldIdentList (TreeNodePtr formType,
                          TreeNodePtr baseType);


  void cmpRecordCase (TreeNodePtr formType,
                      TreeNodePtr baseType);


  void cmpVariant (TreeNodePtr formType,
                   TreeNodePtr baseType);


  void cmpCaseLabelList (TreeNodePtr formType,
                         TreeNodePtr baseType);


  void cmpCaseLabel (TreeNodePtr formType,
                     TreeNodePtr baseType);


  void cmpSubrange (TreeNodePtr formType,
                    TreeNodePtr baseType);


  void cmpExpression (TreeNodePtr formType,
                      TreeNodePtr baseType);


  void cmpSimpleExpression (TreeNodePtr formType,
                            TreeNodePtr baseType);


  void cmpTerm (TreeNodePtr formType,
                TreeNodePtr baseType);


  void cmpFactor (TreeNodePtr formType,
                  TreeNodePtr baseType);
};

extern ContextCKCLASS ContextCK;

#endif
