// ConstrUpdate.h : header file
//

#ifndef __ConstrUpdate
#define __ConstrUpdate


#include "Constructs.h"
#include "PEBaseDoc.h"
#include "LavaAppBase.h"


extern SynObject *replacedObj, *toBeDrawn;
extern bool multipleUpdates;

class LAVAEXECS_DLL CConstrUpdate : public CBaseConstrUpdate {
public:
  void MakeExec(LavaDECL *myDECL);

  bool ChangeConstraint(CLavaPEHint* hint, wxDocument* doc, bool undo);
  void DeleteHint(CLavaPEHint *hint);
  void MakeSetGet(CPEBaseDoc *myDoc, LavaDECL *myDECL, TID &prop, TID &parm);
};


#endif
