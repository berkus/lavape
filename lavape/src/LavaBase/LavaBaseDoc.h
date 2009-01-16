#ifndef __CLavaBaseDoc
#define __CLavaBaseDoc


#include "SynIO.h"
#include "Syntax.h"
#include "SynIDTable.h"
#include "LavaAppBase.h"
#include "LavaThread.h"
#include "docview.h"
#include "BASEMACROS.h"
#include "qobject.h"
#include "qstring.h"
#include "qlist.h"
#include "qhash.h"
//Added by qt3to4:
#include <QMouseEvent>
#ifndef WIN32
#include <setjmp.h>
#include <signal.h>
#endif

#define ALLOCOBJLIST

//#ifdef __Darwin
//#define FPE_INTOVF FPE_FLTOVF
//#define FPE_INTDIV FPE_FLTDIV
//#endif

struct CheckData;

enum ContextFlag {
  staticContext,
  selfiContext,
  selfoContext,
  multiContext,
  undefContext
};


enum Category {
  anyCategory,
  stateObj,
  valueObj,
  unknownCategory
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
  bool debugOn;
  bool openForDebugging;  //for runtime only
  LavaObjectPtr DocObjects[3]; //Runtime: [0] = GUI object,
                               //         [1] = start object,
                               //         [2] = show result 0bject
//  DString ObjName;  //Runtime: name of object file
  wxView *RuntimeView; //used from *.ldoc only
  QWidget* ActLavaDialog;
  CThreadList *ThreadList;
  CLavaThread* m_execThreadPtr;
  QWidget* DumpFrame;
  int numAllocObjects;
#ifdef ALLOCOBJLIST
  QList<LavaObjectPtr> allocatedObjects;
#endif
  CHESimpleSyntax* AddSyntax(SynDef *syntaxIncl, const QString& fn, bool& errEx, int hint=0);
  bool AllowThrowType(LavaDECL* decl, TID throwID, int inINCL);
  CHESimpleSyntax* AttachSyntax(CheckData& ckd, QString& fn); //Runtime include

  void CalcNames(const QString& FileName);
  virtual bool CheckCompObj(LavaDECL* compoDECL);
  QString* CheckException(LavaDECL *funcDecl, CheckData* pckd);
  QString* CheckFormEl(LavaDECL *formEl, LavaDECL *classEl);
  LavaDECL* CheckGetFinalMType(LavaDECL* inDECL, LavaDECL* typeDECL=0, CheckData* pckd=0);
  virtual bool CheckForm(CheckData& ckd, LavaDECL* /*formDECL*/, int checkLevel = 0) {return false;}
  virtual bool CheckImpl(CheckData& callingCkd, LavaDECL* /*classDECL*/, LavaDECL* specDECL=0) {return false;}
  QString* CheckScope(LavaDECL* elDef); //returns error code
  QString* CommonContext(LavaDECL* paramDECL);
  int CheckHandlerIO(LavaDECL* funcDECL, LavaDECL* ClientType);

  virtual void DelSyntax(CHESimpleSyntax* delSyn) {}
  QString* ExtensionAllowed(LavaDECL* decl, LavaDECL* baseDECL, CheckData* pckd);
  LavaDECL* FindInSupports(const DString& name, LavaDECL *self, LavaDECL *decl, bool down = false, bool onTop = true);
  DString GetAbsSynFileName();          //get the document syl-file name
  SynFlags GetCategoryFlags(LavaDECL* memDECL, bool& catErr);
//  virtual int GetContextsSectionNumber(LavaDECL* /*classDECL*/, LavaDECL* /*baseDECL*/, bool /*outer*/) {return -1;}
  virtual LavaDECL* GetExecDECL(LavaDECL* parentDecl,TDeclType type,bool makeDecl=true,bool makeExec=true);
  LavaDECL* GetFinalMVType(LavaDECL* decl, CContext &context, Category &cat, CheckData* pckd);
  LavaDECL* GetFinalMVType(const TID& id, int inINCL, CContext &context, Category &cat, CheckData* pckd);
  LavaDECL* GetFinalMTypeAndContext(const TID& id, int inINCL, CContext &context, CheckData* pckd);
  LavaDECL** GetFormpDECL(LavaDECL* decl);
  TID GetGUIDataTypeID(LavaDECL* GUIclass);
  TID GetMappedVType(TID paramID, CContext &context, CheckData* pckd);
  virtual int GetMemsSectionNumber(CheckData& ckd, LavaDECL* /*classDECL*/, LavaDECL* /*memDECL*/, bool putErr=true) {return -1;}
  virtual bool GetOperatorID(LavaDECL* , TOperator , TID& ) {return false;}
  virtual bool getOperatorID(LavaDECL* , TOperator , TID& ) {return false;}
  virtual int GetSectionNumber(CheckData& ckd, const TID& /*classID*/, const TID& /*baseclassID*/) {return -1;}
  virtual int GetSectionNumber(CheckData& ckd, LavaDECL* /*classDECL*/, LavaDECL* /*baseDECL*/, bool putErr=true) {return -1;}
  LavaDECL* GetType(LavaDECL *decl);
  LavaDECL* GetTypeAndContext(LavaDECL* decl, CContext &context);
  virtual int GetVTSectionNumber(CheckData& ckd, const CContext& /*context*/, LavaDECL* /*paramDECL*/, bool& /*outer*/) {return -1;}

  virtual void IncludeHint(const QString& /*fullfn*/, CHESimpleSyntax* ) {}
  CHESimpleSyntax* IncludeSyntax(const QString& fn, bool& isNew, int hint=0);
  virtual bool IsCDerivation(LavaDECL *decl, LavaDECL *baseDecl, CheckData* pckd=0);
//  virtual bool IsOuterParam(paramDECL, typeDECL) {return false;}
  //virtual void LavaEnd(bool doClose = true) {}
  virtual void LavaError(CheckData& /*ckd*/, bool /*setEx*/, LavaDECL*, QString* /*nresourceID*/, LavaDECL* /*refDECL = 0*/) {}
  virtual bool MakeFormVT(LavaDECL* , CheckData* pckd) {return false;}
  virtual bool MakeVElems(LavaDECL* , CheckData* pckd) {return false;}
  bool MemberTypeContext(LavaDECL *memDECL, CContext &context, CheckData* pckd);
  void NextContext(LavaDECL *decl, CContext &context);
  virtual bool OnSaveModified() {return wxDocument::OnSaveModified();}
  bool okPattern(LavaDECL* decl);
  virtual LavaObjectPtr OpenObject(CheckData& ckd, LavaObjectPtr urlObj) {return 0;}
  virtual bool OverriddenMatch(LavaDECL *decl, bool setName=false, CheckData* pckd=0);
  virtual int ReadSynDef(const QString& fn, SynDef* &sntx, ASN1* cid = 0);//fn has all links resolved
//  void ReduceType(TID& id, int inINCL, int& refs);
  virtual void ResetError() {}
  virtual bool SaveObject(CheckData& ckd, LavaObjectPtr object) {return false;}
  virtual bool SelectLcom(bool emptyDoc) {return false;}
  QString* TestValOfVirtual(LavaDECL* vdecl, LavaDECL* valDECL);
  QString* TypeForMem(LavaDECL* memdecl, LavaDECL* typeDECL, CheckData* pckd);

private:
    Q_OBJECT

};


class LAVABASE_DLL CLavaBaseView : public wxView
{
public:
//  VIEWFACTORY(CLavaBaseView)
  CLavaBaseView(QWidget *parent,wxDocument *doc, const char*  name): wxView(parent,doc,name) {}
  virtual void on_whatNextAction_triggered();
//  virtual void howTo();

  virtual void OnEditCut() {}
  virtual void OnEditCopy(){}
  virtual void OnEditPaste(){}
  virtual void OnFindReferences() {}
  virtual void OnExpandAll() {}
  virtual void OnCollapseAll() {}
  virtual void OnShowOptionals() {}
  virtual void OnShowAllEmptyOpt() {}
  virtual void OnRemoveAllEmptyOpt() {}
  virtual void OnNextComment() {}
  virtual void OnPrevComment() {}
  virtual void OnNextError() {}
  virtual void OnPrevError() {}
  virtual void OnNewInclude() {}
  virtual void OnNewPackage() {}
  virtual void OnNewinitiator() {}
  virtual void OnNewInterface() {}
  virtual void OnNewImpl() {}
  virtual void OnNewCSpec() {}
  virtual void OnNewComponent() {}
  virtual void OnNewset() {}
  virtual void OnNewenum() {}
  virtual void OnNewVirtualType() {}
  virtual void OnNewfunction() {}
  virtual void OnNewmember() {}
  virtual void OnNewEnumItem() {}
  virtual void OnEditSel() {}
  virtual void OnComment() {}
  virtual void OnGotoDecl() {}
  virtual void OnGotoImpl() {}
  virtual void OnShowOverridables() {}
  virtual void OnShowGUIview() {}
  virtual void OnMakeGUI() {}
  virtual void OnNewLitStr() {}
  virtual void OnInsertOpt() {}
  virtual void OnDeleteOpt() {}
  virtual void OnOK() {}
  virtual void OnReset() {}
  virtual void OnCancel() {}
	virtual void OnTogglestate() {}
  virtual void OnDelete() {}
  virtual void OnOverride() {}
  virtual void OnGenHtml() {}
  virtual void OnGenHtmlS() {}

  virtual void on_DbgAction_triggered() {}
  virtual void on_DbgBreakpointAct_triggered() {}
  virtual void on_DbgClearBreakpointsAct_triggered() {};
  virtual void on_DbgStepNextAct_triggered() {}
  virtual void on_DbgStepNextFunctionAct_triggered() {}
  virtual void on_DbgStepintoAct_triggered() {}
  virtual void on_DbgStepoutAct_triggered() {}
  virtual void on_DbgStopAction_triggered() {}
  virtual void on_DbgRunToSelAct_triggered() {}

  virtual void OnAnd() {}
  virtual void OnBitAnd() {}
  virtual void OnBitOr() {}
  virtual void OnBitXor() {}
  virtual void OnClone() {}
  virtual void OnSelect() {}
//  virtual void OnDelete() {}
  virtual void OnDivide() {}
//  virtual void OnEditCut() {}
//  virtual void OnEditCopy() {}
//  virtual void OnEditPaste() {}
  virtual void OnEditUndo() {}
  virtual void OnEditRedo() {}
  virtual void OnEq() {}
  virtual void OnDeclare() {}
  virtual void OnForeach() {}
  virtual void OnFunctionCall() {}
  virtual void OnGe() {}
  virtual void OnGt() {}
  virtual void OnIf() {}
  virtual void OnIfdef() {}
  virtual void OnIfExpr() {}
  virtual void OnElseExpr() {}
  virtual void OnIn() {}
  virtual void OnInsert() {}
  virtual void OnInsertBefore() {}
  virtual void OnInvert() {}
//  virtual void OnLButtonDblClk(UINT nFlags, QPoint point);
  virtual void OnLButtonDown(QMouseEvent *e) {}
  virtual void OnLe() {}
  virtual void OnLshift() {}
  virtual void OnLt() {}
  virtual void OnPlusMinus() {}
  virtual void OnMult() {}
  virtual void OnNe() {}
  virtual void OnShowComments() {}
  virtual void OnNot() {}
  virtual void OnNull() {}
  virtual void OnOr() {}
  virtual void OnPlus() {}
  virtual void OnRshift() {}
  virtual void OnSwitch() {}
  virtual void OnXor() {}
//  virtual void OnEditSel() {}
//  virtual void OnComment() {}
  virtual void OnToggleArrows() {}
  virtual void OnNewLine() {}
//  virtual void OnShowOptionals() {}
//  virtual void OnGotoDecl() {}
//  virtual void OnGotoImpl() {}
  virtual void OnModulus() {}
  virtual void OnIgnore() {}
  virtual void OnAttach() {}
  virtual void OnQueryItf() {}
  virtual void OnStaticCall() {}
  virtual void OnAssign() {}
  virtual void OnConnect() {}
  virtual void OnDisconnect() {}
  virtual void OnEmitSignal() {}
  virtual void OnTypeSwitch() {}
  virtual void OnInterval() {}
  virtual void OnUuid() {}
  virtual void OnCall() {}
  virtual void OnExists() {}
  virtual void OnCreateObject() {}
  virtual void OnAssert() {}
  virtual void OnIgnoreStm() {}
//  virtual void OnNextError() {}
//  virtual void OnPrevError() {}
//  virtual void OnNextComment() {}
//  virtual void OnPrevComment() {}
  virtual void OnConflict() {}
  virtual void OnRefresh() {}
  virtual void OnToggleCategory() {}
  virtual void OnToggleSubstitutable() {}
  virtual void OnToggleClosed() {}
  virtual void OnToggleParmNames() {}
  virtual void OnCopy() {}
  virtual void OnEvaluate() {}
  virtual void OnInputArrow() {}
//  virtual void OnSetFocus(QWidget* pOldWnd);
//  virtual void OnKillFocus(QWidget* pNewWnd);
  virtual void OnHandle() {}
//  virtual void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnOptLocalVar() {}
	virtual void OnItem() {}
	virtual void OnQua() {}
	virtual void OnSucceed() {}
	virtual void OnFail() {}
	virtual void OnOld() {}
	virtual void OnTrue() {}
	virtual void OnFalse() {}
	virtual void OnOrd() {}
	virtual void OnTryStatement() {}

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
    synError = 0;
    hint = 0;
    undoRedo = 0;
    refTable = 0;
    currentStackLevel = 0;
    stackFrameSize = 0;
    stackFrame[0] = stackFrame[1] = stackFrame[2] = 0;
    nPlaceholders = 0;
    nErrors = 0;
    errorCode = 0;
    iArg = 0;
    inINCL = 0;
    stateObj = false;
    concernExecs = false;
    criticalScope = false;
    handleOpd = false;
    inQuant = false;
    inInitialUpdate = false;
    inIniClause = false;
    iniVar = 0;
    immediateReturn = false;
    precedingIniCall = 0;
  };

  ~CheckData() { if (synError) delete synError; }

  CLavaBaseDoc *document;
  LavaDECL *myDECL, *selfTypeDECL;
  CVAttrDesc *attrDesc;
  LavaObjectPtr lastException, stackFrame[3];
  QString callStack, exceptionMsg;
  SynObjectBase *selfVar, *succeed, *iniVar;
  SynErr *synError;
  CContext lpc;          //vorher *execIC, *execOC
  CContext tempCtx;  //vorher *iC, *oC
  wxView *execView;
  CLavaPEHint *hint;
  int undoRedo;
  void *refTable; // actually of type Constructs.ph::RefTable*
  unsigned
    currentStackLevel,
    currVarIndex,
    stackFrameSize,
    nPlaceholders,
    nErrors,
    inINCL,
	  iArg;
  QString *errorCode;
  Category callObjCat;
  bool stateObj, iniCheck, concernExecs, criticalScope, handleOpd,
       inQuant, inInitialUpdate, inIniClause, immediateReturn, exceptionThrown,
	     checkClosedParmLevel;
  SET flags;
  SynObjectBase *firstIniCall;
  CHE *precedingIniCall;
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


extern LAVABASE_DLL bool INC_FWD_CNT(CheckData &ckd, LavaObjectPtr object);
extern LAVABASE_DLL bool INC_REV_CNT(CheckData &ckd, LavaObjectPtr object);
extern LAVABASE_DLL bool DEC_FWD_CNT(CheckData &ckd, LavaObjectPtr object);
extern LAVABASE_DLL bool DEC_REV_CNT(CheckData &ckd, LavaObjectPtr object);


class LAVABASE_DLL DumpEventData {
public:
  DumpEventData(CLavaBaseDoc* d, LavaVariablePtr stack, CLavaThread* thread)
  {doc = d; object = stack[SFH]; name = *((QString*)(stack[SFH+1]+LSH)); currentThread = thread;}
  CLavaBaseDoc* doc;
  LavaObjectPtr object;
  QString name;
  CLavaThread *currentThread;

  ~DumpEventData() {}
};

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

enum SectionFlag {
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
  TAdapterFunc* adapterTab;
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
enum RTerrorCodes {
  memory_ex,
  check_ex,
  NullCallObject_ex,
  NullMandatory_ex,
  NullParent_ex,
  NullException_ex,
  AssertionViolation_ex,
  InvariantViolation_ex,
  PreconditionViolation_ex,
  PreconditionConsistency_ex,
  PostconditionViolation_ex,
  PostconditionConsistency_ex,
  IntegerRange_ex,
  EnumOrdLow_ex,
  EnumOrdHigh_ex,
  ElemNotInSet_ex,
  RunTimeException_ex,
  DLLError_ex,
  ldocNotOpened_ex,
  CompoNotFound_ex,
  ldocIncompatible_ex,
  IncompatibleCategory_ex,
  NoPicture_ex,
  CorruptObject_ex,
  ldocNotStored_ex,
  ArrayXOutOfRange_ex,
  CanceledForm_ex,
  AssigToFrozen_ex,
  ZombieAccess_ex,
  UnfinishedObject_ex,
  ExecutionFailed_ex,
  OutFunctionFailed_ex
};

//
//Errorcodes corresponding to lava hardware exceptions
//
enum HWerrorCodes {
  seg_violation_ex,
  float_div_by_zero_ex,
  float_overflow_ex,
  float_underflow_ex,
  float_inexact_result_ex,
  float_invalid_op_ex,
  float_subscript_out_of_range_ex,
  float_quietNAN_ex,
  integer_div_by_zero_ex,
  integer_overflow_ex,
  other_hardware_ex
};

extern LAVABASE_DLL bool forceZombify (CheckData &ckd, LavaObjectPtr object, bool aquaintancesToo);
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


enum ObjectStateFlag {
   sectEstablished,
   //finished,        //global object flag (in object+1)
   stateObjFlag,
   useDefaults,     //used in LavaGUI
   insertedItem,    //used in LavaGUI
   deletedItem,     //used in LavaGUI
   localCheckmark,  //temporary check mark
   dontSave,        //global object flag (in object+1)
   zombified,       //global object flag (in object+1)
   releaseFinished, //indicator that all sub-objects have been released/decremented
//   marked,          //used in InterprBase::forceRelease
   compoPrim,        //the creation section of component
   objectModified,   //object is modified 
   objectLocked      //
};

extern LAVABASE_DLL LavaObjectPtr AllocateObject(CheckData &ckd, LavaDECL* typeDECL, bool stateObj, LavaObjectPtr urlObj=0);
extern LAVABASE_DLL LavaObjectPtr AttachLavaObject(CheckData &ckd, LavaObjectPtr urlObj, LavaDECL* specDECL, LavaDECL* classDECL, bool stateObj);
extern LAVABASE_DLL LavaObjectPtr CreateObject(CheckData &ckd, LavaObjectPtr urlObj, LavaDECL* specDECL, LavaDECL* classDECL, bool stateObj);
extern LAVABASE_DLL bool CallDefaultInit(CheckData &ckd, LavaObjectPtr object);
extern LAVABASE_DLL CRuntimeException* CopyObject(CheckData &ckd, LavaVariablePtr sourceVarPtr, LavaVariablePtr resultVarPtr, bool stateObj = false, LavaDECL* resultClassDECL = 0, CCopied* copied = 0);
extern LAVABASE_DLL bool EqualObjects(CheckData &ckd, LavaObjectPtr leftVarPtr, LavaObjectPtr rightVarPtr, int specialEQ);
extern LAVABASE_DLL bool UpdateObject(CheckData &ckd, LavaObjectPtr& origObj, LavaVariablePtr updatePtr, bool& objModf);
extern LAVABASE_DLL bool OneLevelCopy(CheckData& ckd, LavaObjectPtr& object);
extern LAVABASE_DLL TAdapterFunc* GetAdapterTable(CheckData &ckd, LavaDECL* classDECL, LavaDECL* specDECL);
extern LAVABASE_DLL LavaObjectPtr CastTo(LavaDECL* classDECL, LavaObjectPtr obj);
extern LAVABASE_DLL LavaObjectPtr CastEnumType(CheckData& ckd, LavaObjectPtr eTypeObjPtr);
extern LAVABASE_DLL LavaObjectPtr CastChainType(CheckData& ckd, LavaObjectPtr chainTypeObjPtr/*, LavaDECL* chainTypeDECL*/);
extern LAVABASE_DLL LavaObjectPtr CastSetType(CheckData& ckd, LavaObjectPtr setTypeObjPtr);
extern LAVABASE_DLL LavaObjectPtr CastArrayType(CheckData& ckd, LavaObjectPtr arrayTypeObjPtr);
extern LAVABASE_DLL void ToggleObjectCat(LavaObjectPtr obj);
/////////////////////////////////////////////////////////////////////////////

enum DebugStep {
  nextStm,
  nextFunc,
  stepInto,
  stepOut,
  noStep};

enum StackFlag {
  inAttachedAssertion,
  bptEncountered};

extern LAVABASE_DLL DebugStep nextDebugStep;

extern LAVABASE_DLL unsigned stepOutStackDepth;
extern LAVABASE_DLL unsigned currentStackDepth;


class Receiver {
public:
  Receiver (LavaObjectPtr rcv, LavaDECL *cb, SynObjectBase *cbfs) {
    receiver = rcv;
    callbackDecl = cb;
    cbFuncStm = cbfs;
  }

  bool matches (LavaObjectPtr rcvr, LavaDECL *cbDecl);

  bool IsFuncInvocation(){ return true; }
  //QString DebugStop(CheckData &ckd,SynObjectBase *rcv,LavaVariablePtr stack,QString excMsg,StopReason sr,LavaVariablePtr calleeStack,LavaDECL *calleeFunc){ return QString::null; }

  LavaObjectPtr receiver;
  LavaDECL *callbackDecl;
  SynObjectBase *cbFuncStm;

  bool callCallback(CheckData &ckd, LavaVariablePtr stackFrame, unsigned oldExprLevel, SynObjectBase *sCall);
};

typedef QList<Receiver*> ReceiverList;
typedef QList<CSectionDesc**> LavaObjectList;

class Callback {
public:
  Callback (LavaObjectPtr sdr, LavaDECL *sdrClass, LavaDECL *cb) {
    sender = sdr;
    senderClass = sdrClass;
    callbackDecl = cb;
  }

  bool matches (LavaObjectPtr sdr, LavaDECL *sigDecl);

  LavaObjectPtr sender;
  LavaDECL *senderClass, *callbackDecl;
};

typedef QList<Callback*> CallbackList;

class RunTimeData {
public:
  RunTimeData() {
    urlObj = 0;
//    receiverDict.setAutoDelete(true);
//    callbackDict.setAutoDelete(true);
  }

  LavaObjectPtr urlObj;

  QHash<LavaDECL*,ReceiverList*> receiverDict;  // key = LavaDECL *signalDecl
  QHash<LavaDECL*,CallbackList*> callbackDict;  // key = LavaDECL *signalDecl
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
public:
  CHWException(){};
#ifdef WIN32
    CHWException(unsigned n);
#else
    CHWException(int sig_num, siginfo_t *info);
#endif
    bool SetLavaException(CheckData& ckd);
    ~CHWException() {}
    unsigned codeHW;
    unsigned lavaCode;
    QString message;
};

class LAVABASE_DLL CFPException : public CHWException
{
public:
    CFPException(bool isQuietNaN);
};

#ifndef WIN32
extern LAVABASE_DLL jmp_buf contOnHWexception;
extern LAVABASE_DLL CHWException hwException;
#endif

#endif
