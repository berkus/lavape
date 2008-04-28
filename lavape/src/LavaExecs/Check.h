#ifndef __Check
#define __Check


#include "Constructs.h"
#include "SynIDTable.h"
#include "LavaBaseDoc.h"
#include <QList>

extern unsigned GetNumInputs(TIDTable *idt, const TID &tid);
extern CHE *GetFirstInput(LavaDECL *funcDecl);
extern CHE *GetFirstInput(TIDTable *idt, const TID &tid);
extern unsigned GetNumOutputs(TIDTable *idt, const TID &tid);
extern CHE *GetFirstOutput(LavaDECL *funcDecl);
extern CHE *GetFirstOutput(TIDTable *idt, const TID &tid);
extern bool compatibleTypes(CheckData &ckd, LavaDECL *decl1, const CContext &context1, LavaDECL *decl2, const CContext &context2);
extern bool sameType(CheckData &ckd, const TID &t1, const TID &t2);
extern bool compatibleInput(CheckData &ckd, CHE *actParm, CHE *formParm, const CContext &callCtx);
extern bool compatibleOutput(CheckData &ckd, CHE *actParm, CHE *formParm, const CContext &callCtx);
extern bool slotFunction(CheckData &ckd, LavaDECL *callbackDecl, const CContext &callbackCtx, LavaDECL *signalDecl, const CContext &signalCtx);

extern  bool VerifyObj(CheckData &ckd, CHE *DODs, DString& name, ObjReference *parent, LavaDECL *startDecl=0); 

#ifdef INTERPRETER
#define CASTOBJECT(object,sectionNumber) \
  (LavaObjectPtr)((object)-(*(object))->sectionOffset+(*(object))[sectionNumber].sectionOffset)
#endif

#define OWNID(decl) TID(decl->OwnID,decl->inINCL)

#endif
