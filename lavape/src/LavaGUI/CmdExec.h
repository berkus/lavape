#ifndef __CmdExec
#define __CmdExec


/*--------------------------------------------------------------------------*/
/*- Basic Concepts:                                                         */
/*--------------------------------------------------------------------------*/
/*
 *  This module supports operations on syntactical objects in forms to
 *
 *  -  find different kinds of fields on screen and/or in syntactical form
 *     (unprotected, protected, iterations, menus, iterated items...),
 *  -  position the cursor on a field,
 *  -  change screen origin,
 *  -  scroll from input field to input field in different modi
 *                                                  (physical,logical,cursor),
 *  -  append an iterated/optional item,
 * -  delete an iterated/optional item,
 *  -  select variants of selection menus,
 *  -  change modi.
 */


#include "SYSTEM.h"
#include "DString.h"
#include "GUIProgBase.h"
#include "Syntax.h"
#include "qobject.h"


class CmdExecCLASS  : public QObject {
public:
  
  void INIT (CGUIProgBase *guiPr);
  CGUIProgBase *GUIProg;
/*
  bool okBType (TBasicType btype, int in);
   // converse procedure to ConvertAndStore for transmission phase.
   // Used to convert a type in internal representation to s string.
*/

  void DeleteOptionalItem (CHEFormNode* fNode);
  void InsertOptionalItem (CHEFormNode* fNode);
  void InsertIterItem (CHEFormNode* fNode);
  void DeleteIterItem (CHEFormNode* fNode);
//  bool ChangeAtomicObject(LavaVariablePtr resultVarPtr);
  bool ChainHandlerCall(CHEFormNode* chainNode, LavaVariablePtr StackFrame, int eventType);
  bool EditHandlerCall(CHEFormNode* formNode, STRING newStr);
  bool ConvertAndStore (CHEFormNode* trp);
  bool Toggl (CHEFormNode* trp);
  void SelectButton (CHEFormNode* button, CHEFormNode*& trp);
  void HandleButtonClick(CHEFormNode* button, CHEFormNode* trp);
  void HandlePopup(CHEFormNode* trp);
  CHEFormNode* GetOptNode(CHEFormNode* fNode);
  CHEFormNode* GetIterNode(CHEFormNode* fNode);
  CHEFormNode* InPopupShell(CHEFormNode* node);
 
  virtual bool Event(QEvent* ev);

private: 
  CHEFormNode* insertedNode;
  CHEFormNode* delNode;
  CHEFormNode* beforeNode;
  CHEFormNode* parNode;
  CHEFormNode* elliNode;
  CHEFormNode* optNode;

private:
  Q_OBJECT

};


#endif
