#ifdef _GNUG__
#pragma interface
#endif

//////////////////////////////////////////////////////////////////////////////////7
// GUIProg.h



#ifndef __GUIProg
#define __GUIProg


#include "SYSTEM.h"
#include "ASN1.h"
#include "DString.h"
#include "SynIO.h"
#include "Syntax.h"

#include "GUIProgBase.h"
#include "CmdExec.h"
#include "Conv.h"
#include "LavaForm.h"
#include "MakeGUI.h"
#include "TreeSrch.h"
#include "LavaBaseDoc.h"



class LAVAGUI_DLL CGUIProg : public CGUIProgBase {

public:
  CmdExecCLASS CmdExec;
  LavaFormCLASS LavaForm;
  MakeGUICLASS MakeGUI;
  TreeSrchCLASS TreeSrch;
  //QWidget* ParentWin;
  CheckData ckd;
  CRuntimeException *ex;

  void Create(CLavaBaseDoc* doc, QWidget* view);
  void OnUpdate( LavaDECL* decl, LavaVariablePtr resultPtr);
  void OnTab(bool back, QWidget* win);
  void OnSetFocus(CHEFormNode* trp0);
  void RedrawForm();
  bool Redraw;
  void OnModified();
  virtual void SyncTree(CHEFormNode *node);
  virtual void NoteLastModified();
  CGUIProg();
  ~CGUIProg();

};

#endif
