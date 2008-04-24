#ifndef __BAdapter
#define __BAdapter

#include "LavaBaseDoc.h"

enum adapterPos {adapterPos_len, adapterPos_copy, adapterPos_compare, adapterPos_serialize, adapterPos_newFunc,
adapterPos_refCount, adapterPos_DD};              

typedef LAVABASE_DLL TAdapterFunc* (*FP)();

extern LAVABASE_DLL TAdapterFunc* StdAdapterTab [];
extern LAVABASE_DLL TAdapterFunc GUIAdapter[];

//extern LAVABASE_DLL LavaDECL* DECLTab [];
extern LAVABASE_DLL void NewQString(QString* pstr, const char* str);
extern LAVABASE_DLL bool HStringCopy(LavaObjectPtr s0, LavaObjectPtr s1);
extern LAVABASE_DLL bool HEnumSetVal(CheckData& ckd, LavaObjectPtr obj, unsigned num);

extern LAVABASE_DLL CRuntimeException* HSetInsertBefore(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool HSetFirst(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL CRuntimeException* HSetSucc(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL bool HSetContains(LavaObjectPtr setObj, LavaObjectPtr elemObj);
extern LAVABASE_DLL bool HSetUpdate(CheckData& ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr, bool& isNew, bool& objModf);
extern LAVABASE_DLL bool HSetOneLevelCopy(CheckData& ckd, LavaObjectPtr sourceSectionPtr, LavaObjectPtr resultSectionPtr);

extern LAVABASE_DLL bool HArrayMakeLen(LavaObjectPtr array, int len);
extern LAVABASE_DLL int HArrayGetLen(LavaObjectPtr array);
extern LAVABASE_DLL LavaObjectPtr HArrayGetEl(LavaObjectPtr array, int pos);
extern LAVABASE_DLL bool HArraySetEl(CheckData& ckd, LavaObjectPtr array, LavaObjectPtr elem, int pos);
extern LAVABASE_DLL bool HArrayUpdate(CheckData& ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr, bool& isNew, bool& objModf);
extern LAVABASE_DLL void HArrayOneLevelCopy(CheckData& ckd, LavaObjectPtr sourceSectionPtr, LavaObjectPtr resultSectionPtr);
extern LAVABASE_DLL bool GUIData(CheckData& ckd, LavaVariablePtr stack);
extern LAVABASE_DLL void HGUISetData(CheckData& ckd, LavaObjectPtr guiService, LavaVariablePtr dataPtr, QWidget* dialog);
extern LAVABASE_DLL void HGUISetUnused(CheckData& ckd, LavaObjectPtr guiService);
extern LAVABASE_DLL QWidget* HGUIGetDialog(CheckData& ckd, LavaObjectPtr guiService);

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
