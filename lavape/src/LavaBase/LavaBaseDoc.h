// CLavaBaseDoc.h : interface of the CLavaBaseDoc class
//
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUC__
#pragma interface
#endif

#ifndef __CLavaBaseDoc
#define __CLavaBaseDoc


#include "SynIO.h"
#include "Syntax.h"
#include "SynIDTable.h"
#include "LavaAppBase.h"
#include "LavaThread.h"
#include "docview.h"
#include "qcstring.h"
#include "qobject.h"
#include "qstring.h"
#ifndef WIN32
#include <setjmp.h>
#include <signal.h>
//#include <fenv.h>
#endif


struct CheckData;


enum ContextFlag {
  staticContext,
  selfiContext,
  selfoContext,
  multiContext,
  undefContext
};


enum Category {
  unknownCategory,
  stateObj,
  valueObj,
  sameAsSelfObj
};

class CSectionDesc;
typedef LAVABASE_DLL CSectionDesc **LavaObjectPtr;

typedef LAVABASE_DLL LavaObjectPtr *LavaVariablePtr;


class LAVABASE_DLL ASN1tofromAr: public ASN1
{ public:
   ASN1tofromAr(QDataStream* ar, const QString& file) {Ar = ar; FileName = file; };
   ASN1tofromAr(QDataStream* ar, const DString& file) {Ar = ar; FileName = file.c; };
   ASN1tofromAr(QDataStream* ar) {Ar = ar; actPos = 0; actLen = 0;};
  
  virtual void putChar (const unsigned char&);
  virtual void getChar (unsigned char&);
   void GETCString (QString* s);
   void PUTCString (QString* s);
   QDataStream* Ar;
   int actPos;
   unsigned actLen;
   QString FileName;
};

class LAVABASE_DLL CException
{
};

class LAVABASE_DLL CUserException : public CException
{
};

class LAVABASE_DLL CNotSupportedException : public CException
{
};

class LAVABASE_DLL CRuntimeException
{
public:
  CRuntimeException() {code = 0; message = "";}
  CRuntimeException(int co, const QString *mess) {code = co; message = *mess;}
  CRuntimeException(int co, QString *err_code);
  bool SetLavaException(CheckData& ckd);
  ~CRuntimeException() {}
  int code;
  QString message;
};

class LAVABASE_DLL CHWException
{
private:
//    CHWException() {}
//    CHWException( CHWException& ) {}
public:
#ifdef WIN32
    CHWException(int n);
#else
    CHWException(int sig_num, siginfo_t *info);
#endif
    bool SetLavaException(CheckData& ckd);
    ~CHWException() {}
    int codeHW;
    int lavaCode;
    QString message;
};


class LAVABASE_DLL CLavaBaseDoc : public wxDocument
{
public:
  CLavaBaseDoc();
  virtual bool DeleteContents();
  virtual ~CLavaBaseDoc();

  DString PathName;
  TIDTable IDTable;
  SynDef *mySynDef;
  LavaDECL* DECLTab [Identifier]; //runtime only
  bool hasIncludes; //indicates that includes where read from seperate files.
  bool isStd;

  bool throwError;
  bool isObject;
  bool inSaveProc;
  bool Redraw;
  LavaObjectPtr DocObjects[3]; //Runtime: [0] = GUI object,
                               //         [1] = start object,
                               //         [2] = show result 0bject
//  DString ObjName;  //Runtime: name of object file
  wxView *RuntimeView; //used from *.ldoc only
  CThreadList *ThreadList;

  void CalcNames(const DString& FileName);
  QString* CheckFormEl(LavaDECL *formEl, LavaDECL *classEl);
  bool AllowThrowType(LavaDECL* decl, TID throwID, int inINCL);
  QString* CheckException(LavaDECL *funcDecl, CheckData* pckd);
  DString GetAbsSynFileName();          //get the document syl-file name
  virtual int ReadSynDef(DString& fn, SynDef* &sntx, ASN1* cid = 0);//fn has all links resolved
  CHESimpleSyntax* AddSyntax(SynDef *syntaxIncl, DString& fn, bool& errEx, int hint=0);
  CHESimpleSyntax* IncludeSyntax(DString& fn, bool& isNew, int hint=0);
  CHESimpleSyntax* AttachSyntax(CheckData& ckd, DString& fn); //Runtime include
//  void ReduceType(TID& id, int inINCL, int& refs);
  virtual bool CheckCompObj(LavaDECL* compoDECL);
  virtual bool OverriddenMatch(LavaDECL *decl, bool setName=false, CheckData* pckd=0);
  virtual bool IsCDerivation(LavaDECL *decl, LavaDECL *baseDecl, CheckData* pckd=0);
  virtual bool OnSaveModified() {return wxDocument::OnSaveModified();}
  QString* ExtensionAllowed(LavaDECL* decl, LavaDECL* baseDECL, CheckData* pckd);
  QString* TypeForMem(LavaDECL* memdecl, LavaDECL* typeDECL, CheckData* pckd);
  QString* TestValOfVirtual(LavaDECL* vdecl, LavaDECL* valDECL);
  SynFlags GetCategoryFlags(LavaDECL* memDECL, bool& catErr);
  LavaDECL** GetFormpDECL(LavaDECL* decl);
  LavaDECL* GetType(LavaDECL *decl);
  bool MemberTypeContext(LavaDECL *memDECL, CContext &context, CheckData* pckd);
  void NextContext(LavaDECL *decl, CContext &context);
  LavaDECL* GetTypeAndContext(LavaDECL* decl, CContext &context);
  LavaDECL* GetFinalMTypeAndContext(const TID& id, int inINCL, CContext &context, CheckData* pckd);
  LavaDECL* GetFinalMVType(LavaDECL* decl, CContext &context, Category &cat, CheckData* pckd);
  LavaDECL* GetFinalMVType(const TID& id, int inINCL, CContext &context, Category &cat, CheckData* pckd);
  LavaDECL* FindInSupports(const DString& name, LavaDECL *self, LavaDECL *decl, bool down = false, bool onTop = true);
  TID GetMappedVType(TID paramID, CContext &context, CheckData* pckd);
  LavaDECL* CheckGetFinalMType(LavaDECL* inDECL, LavaDECL* typeDECL=0, CheckData* pckd=0);
  QString* CheckScope(LavaDECL* elDef); //returns error code
  bool okPattern(LavaDECL* decl);
  QString* CommonContext(LavaDECL* paramDECL);

  virtual void IncludeHint(const DString& /*fullfn*/, CHESimpleSyntax* ) {}
  virtual bool GetOperatorID(LavaDECL* , TOperator , TID& ) {return false;}
  virtual bool getOperatorID(LavaDECL* , TOperator , TID& ) {return false;}
  virtual int GetMemsSectionNumber(CheckData& ckd, LavaDECL* /*classDECL*/, LavaDECL* /*memDECL*/, bool putErr=true) {return -1;}
  virtual int GetVTSectionNumber(CheckData& ckd, const CContext& /*context*/, LavaDECL* /*paramDECL*/, bool& /*outer*/) {return -1;}
//  virtual int GetContextsSectionNumber(LavaDECL* /*classDECL*/, LavaDECL* /*baseDECL*/, bool /*outer*/) {return -1;}
  virtual int GetSectionNumber(CheckData& ckd, const TID& /*classID*/, const TID& /*baseclassID*/) {return -1;}
  virtual int GetSectionNumber(CheckData& ckd, LavaDECL* /*classDECL*/, LavaDECL* /*baseDECL*/, bool putErr=true) {return -1;}
  virtual bool CheckImpl(CheckData& callingCkd, LavaDECL* /*classDECL*/) {return false;}
  virtual bool CheckForm(CheckData& ckd, LavaDECL* /*formDECL*/, int checkLevel = 0) {return false;}
  virtual bool MakeFormVT(LavaDECL* , CheckData* pckd) {return false;}
  virtual bool MakeVElems(LavaDECL* , CheckData* pckd) {return false;}
  virtual LavaDECL* GetConstrDECL(LavaDECL* parentDecl,TDeclType type,bool makeDecl=true,bool makeExec=true);
//  virtual bool IsOuterParam(paramDECL, typeDECL) {return false;}

  virtual void LavaError(CheckData& /*ckd*/, bool /*setEx*/, LavaDECL*, QString* /*nresourceID*/, LavaDECL* /*refDECL = 0*/) {}
  //virtual void LavaEnd(bool doClose = true) {}
  virtual void ResetError() {}
  virtual bool SelectLcom(bool emptyDoc) {return false;}
  virtual LavaObjectPtr OpenObject(CheckData& ckd, LavaObjectPtr urlObj) {return 0;}
  virtual bool SaveObject(CheckData& ckd, LavaObjectPtr object) {return false;}
  virtual void DelSyntax(CHESimpleSyntax* delSyn) {}
private:
    Q_OBJECT

};

class CVAttrDesc;

struct LAVABASE_DLL CheckData {
  CheckData () {
    document = 0;
    myDECL = 0;
    selfTypeDECL = 0;
    attrDesc = 0;
    lastException = 0;
    exceptionThrown = false;
    selfVar = 0;
    succeed = 0;
    execView = 0;
    hint = 0;
    undoRedo = 0;
    refTable = 0;
    currentStackLevel = 0;
    stackFrameSize = 0;
    nPlaceholders = 0;
    nErrors = 0;
    errorCode = 0;
    inINCL = 0;
    stateObj = false;
    concernExecs = false;
    criticalScope = false;
    handleOpd = false;
    inQuant = false;
    inInitialUpdate = false;
    immediateReturn = false;
  };

  CLavaBaseDoc *document;
  LavaDECL *myDECL, *selfTypeDECL;
  CVAttrDesc *attrDesc;
  LavaObjectPtr lastException;
  QString callStack;
  SynObjectBase /*SelfVar*/ *selfVar, /*SuccStatement*/*succeed;
  CContext lpc;          //vorher *execIC, *execOC
  CContext tempCtx;  //vorher *iC, *oC
  wxView *execView;
  CLavaPEHint *hint;
  int undoRedo;
  void *refTable; // actually of type Constructs.ph::RefTable*
  unsigned
    currentStackLevel,
    stackFrameSize,
    nPlaceholders,
    nErrors,
    inINCL;
  QString *errorCode;
  Category callObjCat;
  bool stateObj, iniCheck, concernExecs, criticalScope, handleOpd, 
       inQuant, inInitialUpdate, immediateReturn, exceptionThrown;
  SET flags;
};



class LAVABASE_DLL CCopied {
public:
  LavaObjectPtr sourceObjPtr;
  LavaObjectPtr resultObjPtr;
  CCopied* next;
  CCopied* last;
  CCopied(LavaObjectPtr sc, LavaObjectPtr rs);
  void Add(LavaObjectPtr sc, LavaObjectPtr rs);
  LavaObjectPtr Find(LavaObjectPtr sc);
  void Destroy();
};



#define UNDEF_VARIABLE (LavaObjectPtr)0
#define LSH 2 //Lenght of section header in object
  //LavaObjectPtr : pointer to section in section table
  //LavaObjectPtr+1: objects section flags

#define LOH 2 //Lenght of header in object:
  // LavaObjectPtr-1: reference counts (2 x unsigned short)
  // changed: use LavaObjectPtr+1 in the main section of the object
  // instead of LavaObjectPtr-2 for object-global flags
  // LavaObjectPtr-2: LavaObjectPtr urlObj of attached object

extern LAVABASE_DLL int allocatedObjects;

extern LAVABASE_DLL bool INC_FWD_CNT(CheckData &ckd, LavaObjectPtr object);
extern LAVABASE_DLL bool INC_REV_CNT(CheckData &ckd, LavaObjectPtr object);
extern LAVABASE_DLL bool DEC_FWD_CNT(CheckData &ckd, LavaObjectPtr object);
extern LAVABASE_DLL bool DEC_REV_CNT(CheckData &ckd, LavaObjectPtr object);

#define IFC(OBJ) {if (!INC_FWD_CNT(ckd,OBJ)) return false;}
#define IRC(OBJ) {if (!INC_REV_CNT(ckd,OBJ)) return false;}
#define DFC(OBJ) {if (!DEC_FWD_CNT(ckd,OBJ)) return false;}
#define DRC(OBJ) {if (!DEC_REV_CNT(ckd,OBJ)) return false;}


typedef LAVABASE_DLL bool (*TAdapterFunc)(CheckData&, LavaVariablePtr);  


class LAVABASE_DLL CVFuncDesc {
  // virtual function descriptor
public:
  bool isNative;
  union {
    LavaDECL *funcExec;  // function body
    TAdapterFunc funcPtr;       //address of adapter function
  };
  int stackFrameSize;
  int delta;  // delta for adjusting "self" to the function implementation,
              //   relativ to this section
  CVFuncDesc() {stackFrameSize = 0;}
  ~CVFuncDesc() {}
  virtual bool Execute(SynObjectBase* , CheckData& , LavaVariablePtr /*newStackFrame*/) {return false;}
};

class LAVABASE_DLL CVAttrDesc {
  // virtual attribute descriptor
public:
  LavaDECL *attrDECL; // attribute/property definition
  LavaDECL *setExec;  // function body of property set
  LavaDECL *getExec;  // function body of property get
  int delta;  // delta for adjusting "self" to the function implementation,
              //   relativ to this section
  ~CVAttrDesc() {}
};


class LAVABASE_DLL CVTypeDesc {
  // virtual type descriptor
public:
  LavaDECL *paramDECL; // virtual type definition
  int delta;           // like function table delta
  bool isparamRef;           // true if referenced type is also virtual type
        // remember: the RefID-DECL is the RuntimeDECL in paramDECL
  LavaDECL *fValue;  //final value of paramDECL    
/*
  struct {
    bool outerParam; // if isparamRef
    int paramSecNo;  // remember: SectionInfo1 in paramDECL->RuntimeDECL is pos in section
                     //           SectionInfo2 in paramDECL->RuntimeDECL is section number
  };
*/
  ~CVTypeDesc() {}
};

//"typedef CVFuncDesc FuncTab[]";

enum LAVABASE_DLL SectionFlag {
  SectPrimary, // this is the first appearance of the section in the entire section table
  ElemsConstituent,  //used in set/array section
  ElemsAcquaintance, //used in set/array section
  SectChainType    //used in set section if type is chain

};

class LAVABASE_DLL CSectionDesc : public CSecTabBase {
  // SectionTable = new CSectionDesc [nSections];
  // nSection is calculated like non vitual inheritance of base classes
  // main section comes first, base sections follow, ==> first offset
  // = main section offset
public:
  LavaDECL* implDECL;          
  LavaDECL* classDECL;  //or packageDECL        
  int sectionOffset;       // offset of this section in the entire object
  CVFuncDesc *funcDesc;    // pointer to the first virtual function of this section
  CVAttrDesc *attrDesc;    // pointer to the first attribute description of this section
  CVTypeDesc *vtypeDesc;   // pointer to the first virtual type description of this section
  LavaDECL* innerContext;  //package or interface of inner virtual types
  LavaDECL* outerContext;  //package or interface of outer virtual types
  int nSections;           // number of sections belonging to this class
  SynFlags SectionFlags; 
  CSectionDesc();
  virtual void Destroy();
};

class LAVABASE_DLL COpenObjectParms {
public:
  COpenObjectParms(CheckData *cp,LavaObjectPtr uobj,CLavaThread *t) {
	  ckdPtr = cp;
		urlObj = uobj;
		thr = t;
	}
	
	LavaObjectPtr obj, urlObj;
	CheckData *ckdPtr;
	CLavaThread *thr;
};

//
//Errorcodes corresponding to lava runtime exceptions
//
#define memory_ex 0                    
#define check_ex 1
#define NullCallObject_ex 2
#define NullMandatory_ex 3
#define NullParent_ex 4
#define NullException_ex 5
#define AssertionViolation_ex 6
#define IntegerRange_ex 7
#define EnumOrdLow_ex 8
#define EnumOrdHigh_ex 9
#define ElemNotInSet_ex 10
#define RunTimeException_ex 11
#define DLLError_ex 12
#define ldocNotOpened_ex 13
#define CompoNotFound_ex 14
#define ldocIncompatible_ex 15
#define IncompatibleCategory_ex 16
#define NoPicture_ex 17
#define CorruptObject_ex 18
#define ldocNotStored_ex 19
#define ArrayXOutOfRange_ex 20
#define CanceledForm_ex 21
#define AssigToFrozen_ex 22
#define ZombieAccess_ex 23
#define UnfinishedObject_ex 24
#define ExecutionFailed_ex 25
#define OutFunctionFailed_ex 26

//
//Errorcodes corresponding to lava hardware exceptions
//
#define seg_violation_ex 0
#define float_div_by_zero_ex 1
#define float_overflow_ex 2
#define float_underflow_ex 3
#define float_inexact_result_ex 4
#define float_invalid_op_ex 5
#define float_subscript_out_of_range_ex 6
#define integer_div_by_zero_ex 7
#define integer_overflow_ex 8
#define other_hardware_ex 9


extern LAVABASE_DLL bool SetLavaException(CheckData& ckd, int code, const QString& mess);
//extern LAVABASE_DLL bool SetLavaException(CheckData& ckd, int code, QString *strCode);
  
extern LAVABASE_DLL void AbsPathName(DString& fn, const DString& dirName);
extern LAVABASE_DLL void RelPathName(DString& fn, const DString& dirName);
extern LAVABASE_DLL void CalcDirName(DString& dirName);
extern LAVABASE_DLL bool SameDir(const DString& dir1, const DString& dir2);
extern LAVABASE_DLL bool SameFile(const DString& absName1, const DString& absName2);
extern LAVABASE_DLL bool SameFile(const DString& relName1, const DString& dirName1, const DString& absName2);
extern LAVABASE_DLL bool SameFile(const DString& relName1, const DString& dirName1, const DString& relName2, const DString& dirName2);
extern LAVABASE_DLL bool isPartOf(const DString& qname, const DString& ancestorName);


//Functions in InterprBase.cpp


enum LAVABASE_DLL ObjectStateFlag {
   sectEstablished,
   finished,        //global object flag (in object+1)
   stateObjFlag,
   useDefaults,     //used in LavaGUI
   insertedItem,    //used in LavaGUI
   deletedItem,     //used in LavaGUI
//   notFromSource,  //used in LavaGUI
//   dontReplace,    //used in LavaGUI
   localCheckmark,  //temporary check mark
   dontSave,        //global object flag (in object+1)
   zombified,       //global object flag (in object+1)
   releaseFinished, //indicator that all sub-objects have been released/decremented
   marked           //used in InterprBase::forceRelease
};

extern LAVABASE_DLL LavaObjectPtr AllocateObject(CheckData &ckd, LavaDECL* typeDECL, bool stateObj);
extern LAVABASE_DLL LavaObjectPtr AttachLavaObject(CheckData &ckd, LavaObjectPtr urlObj, LavaDECL* specDECL, LavaDECL* classDECL, bool stateObj);
extern LAVABASE_DLL LavaObjectPtr CreateObject(CheckData &ckd, LavaObjectPtr urlObj, LavaDECL* specDECL, LavaDECL* classDECL, bool stateObj);
extern LAVABASE_DLL bool CallDefaultInit(CheckData &ckd, LavaObjectPtr object);
extern LAVABASE_DLL CRuntimeException* CopyObject(CheckData &ckd, LavaVariablePtr sourceVarPtr, LavaVariablePtr resultVarPtr, bool stateObj = false, LavaDECL* resultClassDECL = 0, CCopied* copied = 0);
extern LAVABASE_DLL bool EqualObjects(CheckData &ckd, LavaObjectPtr leftVarPtr, LavaObjectPtr rightVarPtr, int specialEQ);
extern LAVABASE_DLL bool UpdateObject(CheckData &ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr);
extern LAVABASE_DLL void OneLevelCopy(CheckData& ckd, LavaObjectPtr& object);
extern LAVABASE_DLL TAdapterFunc* GetAdapterTable(CheckData &ckd, LavaDECL* classDECL);
extern LAVABASE_DLL LavaObjectPtr CastEnumType(CheckData& ckd, LavaObjectPtr eTypeObjPtr);
extern LAVABASE_DLL LavaObjectPtr CastChainType(CheckData& ckd, LavaObjectPtr chainTypeObjPtr/*, LavaDECL* chainTypeDECL*/);
extern LAVABASE_DLL LavaObjectPtr CastSetType(CheckData& ckd, LavaObjectPtr setTypeObjPtr);
extern LAVABASE_DLL LavaObjectPtr CastArrayType(CheckData& ckd, LavaObjectPtr arrayTypeObjPtr);
extern LAVABASE_DLL void ToggleObjectCat(LavaObjectPtr obj);
/////////////////////////////////////////////////////////////////////////////

#ifndef WIN32
extern LAVABASE_DLL jmp_buf contOnHWexception;
extern LAVABASE_DLL CHWException *hwException;
#endif

#endif
