#ifndef __Coder
#define __Coder


#include "SYSTEM.h"

#include "IntCode.h"


class CoderCLASS {

public:  
  
  void INIT ();

  void Coder ();

/**********************************************************************/

private:

  TreeNodePtr nodeModuleName;
  bool modGen, arbOrg, constOrg;
  unsigned savedIndent;




  /* ************************************************************************/
  /* declaration MACROS: */


  void fromMacro (TreeNodePtr node);


  /* ************************************************************************/
  /* executable MACROS: */


  void callMacro (TreeNodePtr node);


  void checkMacro (TreeNodePtr node);


  void checkpointMacro ();


  void dropMacro (TreeNodePtr node);


  void getMacro (TreeNodePtr node);


  void getPrivMacro (TreeNodePtr node);


  void orgunitMacro (TreeNodePtr node);


  void procedureMacro (TreeNodePtr node);


  void putMacro (TreeNodePtr node);


  void putPrivMacro (TreeNodePtr node);


  void returnMacro (TreeNodePtr node);


  void showMacro (TreeNodePtr node);


  void startMacro (TreeNodePtr node);


  void stopMacro ();
  

  bool notYetIncluded (const DString& module);


  bool firstImport;

  void appendImportIf (TGlobalFlag used,
           const char *module);


  void fixedImports ();


  void imports (TreeNodePtr& node);


  void constOrVar (DString oidVal,
                   TreeNodePtr node);


  void setAuxVar (DString oidVal,
                  TreeNodePtr node);


  void buildReceiverChain (TreeNodePtr node);


  void buildOrgUnitConst (TreeNodePtr orgUnitNode);


  void receivers ();
};

extern CoderCLASS Coder;

#endif
