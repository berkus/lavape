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


class LAVAGUI_DLL CmdExecCLASS  /*: public QObject*/ {
public:
  
  void INIT (CGUIProgBase *guiPr);
  CGUIProgBase *GUIProg;
  LavaVariablePtr Handler_Stack;
  CVFuncDesc* Handler_fDesc;
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
  CHEHandlerInfo* GetHandler(CHEFormNode* fNode, int eventType);
  bool OptHandlerCall(CHEFormNode* optNode, int eventType);
  bool ChainHandlerCall(LavaVariablePtr StackFrame, int eventType);
  bool EditHandlerCall(CHEFormNode* formNode, STRING newStr);
  bool ButtonHandlerCall(CHEFormNode* buttonNode, int enumNum);
  void ContinueExecThread();
  bool ConvertAndStore (CHEFormNode* trp);
  bool Toggl (CHEFormNode* trp);
  void SelectButton (CHEFormNode* button, CHEFormNode*& trp);
  void HandleButtonClick(CHEFormNode* button, CHEFormNode* trp);
  void HandlePopup(CHEFormNode* trp);
  CHEFormNode* GetOptNode(CHEFormNode* fNode);
  CHEFormNode* GetIterNode(CHEFormNode* fNode);
  CHEFormNode* InPopupShell(CHEFormNode* node);
 
  virtual bool GUIEvent(QEvent* ev);

private: 
  LavaDECL *H_FormSyn;
  CHEFormNode *H_ChainNode;
  CHEFormNode* H_InsertedNode;
  CHEFormNode* H_DelNode;
  //LavaObjectPtr H_Handle;
  CHEFormNode* H_BeforeNode;
  CHEFormNode* H_ParNode;
  CHEFormNode* H_ElliNode;
  CHEFormNode* H_OptNode;
  CHEFormNode* H_EditNode;
  CHEFormNode* H_ButtonNode;
  LavaVariablePtr H_EditPtr;
  LavaObjectPtr StackFrame[SFH+7]; 
  int LastEvent;

//private:
//  Q_OBJECT

};


#endif
