#ifndef __LBMACROS
#define __LBMACROS

#ifdef WIN32
#ifdef LAVABASE_EXPORT
#define LAVABASE_DLL __declspec( dllexport )
#else
#define LAVABASE_DLL __declspec( dllimport )
#endif
#else
#define LAVABASE_DLL
#endif


//#ifndef max
#define lmax(a,b)            (((a) > (b)) ? (a) : (b))
//#endif

//#ifndef min
#define lmin(a,b)            (((a) < (b)) ? (a) : (b))
//#endif


#define UNDEF_VARIABLE (LavaObjectPtr)0
#define LSH 2 //Length of section header in object
#define SFH 3 //Length of stack frame header (function calls)
  //LavaObjectPtr : pointer to section in section table
  //LavaObjectPtr+1: objects section flags

#define LOH 2 //Length of header in object:
  // LavaObjectPtr-1: reference counts (2 x unsigned short);
  // changed: use LavaObjectPtr+1 in the main section of the object
  // instead of LavaObjectPtr-2 for object-global flags;
  // LavaObjectPtr-2: pointer to the object's run time data,
  // e.g. urlObj, sender/receiver lists for [dis]connect and signal primitives

//length of adapter table header
#define LAH  7 //adapterTable[0]: length of native data, lava-attributes behind the native part of the section
               //adapterTable[1]: copy function
               //adapterTable[2]: compare function, this is not the "=="operator
               //adapterTable[3]: serialize function
               //adapterTable[4]: new function, used in AllocateObject
               //adapterTable[5]: refcount function, used in DEC_[FWD|REV]_CNT
               //adapterTable[6]: dump&debug function, returns a DDMakeClass-object


#define CHLV_noCheck       10
#define CHLV_inUpdateLow   11
#define CHLV_inUpdateLowFinal 12
#define CHLV_inUpdateHigh  13 //fit but no call of NewID() and no setting of modified-flag
#define CHLV_showError     14
#define CHLV_fit           15


#define IFC(OBJ) {if (!INC_FWD_CNT(ckd,OBJ)) return false;}
#define IRC(OBJ) {if (!INC_REV_CNT(ckd,OBJ)) return false;}
#define DFC(OBJ) {if (!DEC_FWD_CNT(ckd,OBJ)) return false;}
#define DRC(OBJ) {if (!DEC_REV_CNT(ckd,OBJ)) return false;}


#endif


