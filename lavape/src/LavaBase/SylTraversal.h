
#ifdef __GNUC__
#pragma interface
#endif

#ifndef __SylTraversal
#define __SylTraversal


#include "Syntax.h"
#include "SynIO.h"
#include "LavaAppBase.h"
//#include "stdafx.h"
//#include "wxExport.h"
#include "qobject.h"
#include "qstring.h"

class LAVABASE_DLL CSylTraversal;
class LAVABASE_DLL CTabTraversal;
//class LAVABASE_DLL CFieldTraversal;

class LAVABASE_DLL  CExec {

public:
  CSylTraversal *Travers;
  CTabTraversal *TabTravers;
  //CFieldTraversal *FieldTravers;
  CExec() {Travers= 0; TabTravers=0; /*FieldTravers= 0;*/}

  virtual ~CExec(); 
  virtual void ExecDefs (LavaDECL ** , int /*level*/) {}
  virtual void ExecMember  (LavaDECL ** , int /*level*/) {}
  virtual void ExecFText  (LavaDECL ** , int /*level*/) {}
  virtual void ExecIn      (LavaDECL ** , int /*level*/) {}
  virtual void ExecOut     (LavaDECL ** , int /*level*/) {}
  virtual void ExecSHOW    (LavaDECL ** , int /*level*/) {}
  virtual void ExecConstraint(LavaDECL ** , int /*level*/) {}
  virtual void ExecLit     (LavaDECL** ) {}
  virtual void ExecUndef   (LavaDECL** , DString /*name*/) {}
  virtual void ExecStruct  (LavaDECL** , DString /*name*/) {}
  virtual void ExecNamed   (LavaDECL** , DString /*name*/) {}
  virtual void ExecBasic   (LavaDECL** , DString /*name*/) {}
  virtual void ExecEnum    (LavaDECL** , DString /*name*/) {}
  virtual void ExecEnumItem(CHEEnumSelId* , int /*level*/) {}

};


class LAVABASE_DLL CTabTraversal
{
  public:

  CExec *ExecTab;
  SynDef *mySynDef;
  bool Done;

  CTabTraversal() {}
  CTabTraversal (CExec *execTab, SynDef *syn) {ExecTab = execTab; mySynDef = syn; Done = FALSE;}
  void Run(bool withINCL = true, bool withStd = false);
};

class LAVABASE_DLL CSylTraversal //: public CSylBrowse
{
  public:
  SynDef *mySynDef;
  CExec *ExecSyl;
  bool Done;
  bool FillOut;
  int nINCL; 
  LavaDECL** parent_p_DECL;
  bool elRemoved; //indicates that the Exec... function has removed its element

  CSylTraversal() {Done = FALSE; nINCL = -1; }
  CSylTraversal(CExec *execSyl, SynDef *syn);

 int AllDefs(bool withImported, bool fillOut = false);
 void DownTree(LavaDECL **elDef, int level, DString& name);

};


class LAVABASE_DLL  CExecSetImpls : public CExec {

public:
  bool rs;
  TIDTable *IDTab;
  CExecSetImpls(SynDef *syn);
  virtual void ExecDefs (LavaDECL ** pelDef, int );
};


class LAVABASE_DLL CFindLikeForm : public CExec
{  //find any form derived from the pattern with the qualified name
  public:
  int inINCL;
  TID RefID;
  TID GUIID;
  LavaDECL *GUIDECL;
  LavaDECL **pdecl;
  TIDTable* IDTab;
  CFindLikeForm(SynDef *syn, const TID& classID, int inINC, LavaDECL *GUIDECL=0);
  void ExecSHOW(LavaDECL ** pelDef, int level);
};

#endif
