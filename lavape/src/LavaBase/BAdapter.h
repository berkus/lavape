#ifdef __GNUC__
#pragma interface
#endif


#ifndef __BAdapter
#define __BAdapter

#include "LavaBaseDoc.h"

//length of adapter table header
#define LAH  6 //adapterTable[0]: length of embedded basic type
               //adapterTable[1]: copy function
               //adapterTable[2]: compare function, this is not the "=="operator 
               //adapterTable[3]: serialize function 
               //adapterTable[4]: new function, used in AllocateObject
               //adapterTable[5]: refcount function, used in DEC_[FWD|REV]_CNT
               

typedef LAVABASE_DLL TAdapterFunc* (*FP)();

extern LAVABASE_DLL TAdapterFunc* StdAdapterTab [];
//extern LAVABASE_DLL LavaDECL* DECLTab [];
extern LAVABASE_DLL void NewQString(QString* pstr, const char* str);
extern LAVABASE_DLL bool HStringCopy(LavaObjectPtr s0, LavaObjectPtr s1);
extern LAVABASE_DLL bool HEnumSetVal(CheckData& ckd, LavaObjectPtr obj, unsigned num);

extern LAVABASE_DLL CRuntimeException* HSetInsertBefore(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool HSetFirst(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL CRuntimeException* HSetSucc(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool HSetContains(LavaObjectPtr setObj, LavaObjectPtr elemObj);
extern LAVABASE_DLL bool HSetUpdate(CheckData& ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr, bool& isNew);
extern LAVABASE_DLL void HSetOneLevelCopy(CheckData& ckd, LavaObjectPtr sourceSectionPtr, LavaObjectPtr resultSectionPtr);

extern LAVABASE_DLL bool HArrayMakeLen(LavaObjectPtr array, int len);
extern LAVABASE_DLL int HArrayGetLen(LavaObjectPtr array);
extern LAVABASE_DLL LavaObjectPtr HArrayGetEl(LavaObjectPtr array, int pos);
extern LAVABASE_DLL bool HArraySetEl(CheckData& ckd, LavaObjectPtr array, LavaObjectPtr elem, int pos);
extern LAVABASE_DLL bool HArrayUpdate(CheckData& ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr, bool& isNew);
extern LAVABASE_DLL void HArrayOneLevelCopy(CheckData& ckd, LavaObjectPtr sourceSectionPtr, LavaObjectPtr resultSectionPtr);

extern LAVABASE_DLL bool StringNewFunc(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool StringDecFunc(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool SetNewFunc(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool CheNewFunc(CheckData& ckd, LavaVariablePtr stack);

extern LAVABASE_DLL bool IntPlus(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool SetAdd(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool SetRemove(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool SetGet(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL void MakeStdAdapter();

extern LAVABASE_DLL bool DropException(CheckData& ckd, LavaVariablePtr stack);



#endif
