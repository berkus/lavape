#ifndef __ExecUpdate
#define __ExecUpdate


#include "Constructs.h"
#include "PEBaseDoc.h"
#include "LavaAppBase.h"


extern SynObject *replacedObj, *toBeDrawn;
extern bool multipleUpdates;

class LAVAEXECS_DLL CExecUpdate : public CBaseExecUpdate {
public:
  void MakeExec(LavaDECL *myDECL);

  bool ChangeExec(CLavaPEHint* hint, wxDocument* doc, bool undo);
  void DeleteHint(CLavaPEHint *hint);
  void MakeSetGet(CPEBaseDoc *myDoc, LavaDECL *myDECL, TID &prop, TID &parm);
};


#endif
