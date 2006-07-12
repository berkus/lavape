#ifndef __ExecTree
#define __ExecTree

#include "Syntax.h"
#include "SylTraversal.h"
#include "LavaPEView.h"
//#include "Constructs.h"
#include "qcombobox.h"
//Added by qt3to4:
#include <QPixmap>
#include "docview.h"
#include "qobject.h"
#include "qstring.h"


class CExecTree : public CExec
{
public:

  CLavaPEView *viewTree;
  CTreeItem* ActItem;
  CTreeItem* ParItem;
  int ActLevel;
  int skipLevel;
  bool downAwaited;
  bool isOnSkip;
  bool FinalUpdate;
  bool useAutoBox;
  bool noItems;
  CLavaPEDoc* Doc;
  int checkLevel;
  
  CExecTree(CLavaPEView *tree, bool autoBox, bool finalUpdate, int check = CHLV_noCheck);
  void ExecDefs(LavaDECL ** pelDef, int level);
  void ExecMember(LavaDECL ** pelDef, int level);
  void ExecIn(LavaDECL ** pelDef, int level);
  void ExecOut(LavaDECL ** pelDef, int level);
  void ExecFText(LavaDECL ** pelDef, int level);
  void ExecFormDef(LavaDECL ** pelDef, int level);
  void ExecExec(LavaDECL ** pelDef, int level);
  void ExecEnum    (LavaDECL** inEl, DString name);
  void ExecEnumItem(CHEEnumSelId * enumsel, int level);
  void ExecStruct(LavaDECL ** pinEl, DString fieldID);
  void AddExtends(LavaDECL* elDef, DString* lab);
  bool CalcPos(int level);
  void MakeItem(DString& label, int bm, CTreeItem* parent, LavaDECL** pelDef);
  CTreeItem* getSectionNode(CTreeItem* parent, TDeclType ncase);
  int GetPixmap(bool isParent, bool isAttr, TDeclType deftype, const SynFlags flag = (const unsigned long)0);
};

class CExecImpls : public CExec
{  //find all implementations of an updated interface and make the necessary changes
  public:
  CLavaPEHint *Hint;
  TID IFaceID;
  TID FuncID;
  TID AttrID;
  LavaDECL* ClassDECL;
  LavaDECL* HintDECL;
  bool inImpl;
  bool inSImpl;
  bool inFImpl;
  bool oneSetGet;
  int implLevel;
  CExecImpls(SynDef *lavaCode, const TID& ifaceID, const TID& funcID, CLavaPEHint* hint);
  virtual void ExecDefs(LavaDECL ** pelDef, int level);
};

class CExecOverrides : public CExec
{  //find all interfaces extending an updated interface and make the necessary changes
  public:
  CLavaPEHint *Hint;
  CLavaPEHint *NewHint;
  TID FuncID;
  TID IFaceID;
  LavaDECL* HintDECL;
  CExecOverrides(SynDef *lavaCode, const TID& ifaceID, const TID& funcID, CLavaPEHint* hint);
  virtual void ExecDefs(LavaDECL ** pelDef, int level);
  void ExecMember(LavaDECL ** pelDef, int level);
  void ExecIn(LavaDECL ** pelDef, int level);
  void ExecOut(LavaDECL ** pelDef, int level);
};

class CExecAllRefs : public CExec
{  // find all references of a declaration name or form name
   // the basic behaviour is 
   // on a declaration delete: to delete all objects having a reference to the definition
   //   if the object having the reference is also a declaration or form, this will cause
   //   a recursive behaviour,
   // on a form delete:    to change the reference to the basic declaration of the form
   // on name change:      to change the reference to the new name
public:
  DString QName;
  DString accName;
  CLavaPEHint *Hint;
  int level;
  int skipLevel;

  virtual void ExecDefs (LavaDECL ** pelDef, int level);
  virtual void ExecFormDef    (LavaDECL ** pelDef, int level);
  virtual void ExecExec(LavaDECL ** pelDef, int level);
  virtual void ExecNamed   (LavaDECL** pinEl, DString name);
  virtual void AddParentsCommand(LavaDECL ** pelDef, int level);
  void ChangeName(DString* nameToChange, DString& fullScopeName);

};

class CExecForms : public CExec
{  //find all forms derived from the structure with the qualified name
  public:
  CLavaPEHint *Hint;
  CExecForms(SynDef *lavaCode, CLavaPEHint *hint);
  virtual void ExecFormDef(LavaDECL ** pelDef, int level);
  int GetPosinForm(LavaDECL* decl);
};



class CExecChecks : public CExec
{  //find all forms derived from the structure with the qualified name
  public:
  CLavaPEDoc* Doc;
  bool inOpen;
  CExecChecks(CLavaPEDoc* doc, bool inopen = false);
  virtual void ExecExec(LavaDECL ** pelDef, int level);
  virtual void ExecFormDef (LavaDECL ** pelDef, int level);
  virtual void ExecMember(LavaDECL ** pelDef, int level);
  virtual void ExecDefs(LavaDECL ** pelDef, int level);
  virtual void ExecIn(LavaDECL ** pelDef, int /*level*/);
  virtual void ExecOut(LavaDECL ** pelDef, int /*level*/);
};


#endif
