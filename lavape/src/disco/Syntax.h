#ifdef __GNUC__
#pragma interface
#endif

/* Lava programming environment and Lava interpreter.
   Copyright (C) 2001 Fraunhofer-Gesellschaft.
   The GNU GPL license applies. For details see the file
   "license.txt" in the top level directory. */

/**********************************************************************

                      #include file Syntax.ph

 **********************************************************************/

#ifndef __Syntax
#define __Syntax

#include "qstring.h"
#include "qfont.h"
#include "qwidget.h"
#include "wx_obj.h"
#include "qpixmap.h"

#define RELEASE  1

#undef VIEW

struct LavaDECL;


#include "SYSTEM.h"
#include "AnyType.h"

#include "ASN1pp.h"
#include "CHAINANY.h"
#include "NESTANY.h"
#include "SETpp.h"
#include "STR.h"


struct DISCO_DLL TSigRef : public DObject  {
  DECLARE_DYNAMIC_CLASS(TSigRef)

  bool GlobalScope;
  STRING SigID;

  TSigRef () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TSigRef*)from;
  }

  friend DISCO_DLL void CDPTSigRef (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTSigRef(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL TSigDef : public DObject  {
  DECLARE_DYNAMIC_CLASS(TSigDef)

  unsigned Scope;
  STRING SigID;

  TSigDef () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TSigDef*)from;
  }

  friend DISCO_DLL void CDPTSigDef (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTSigDef(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL TID : public DObject  {
  DECLARE_DYNAMIC_CLASS(TID)

  int nINCL;
  int nID;
  inline bool operator == (const TID &id2) const
  {
    return((nID==id2.nID)&&(nINCL==id2.nINCL));
  }
  inline bool operator != (const TID &id2) const
  {
    return((nID!=id2.nID)||(nINCL!=id2.nINCL));
  }

  TID()
  {
    nINCL=0;
    nID=-1;
  }

  TID(int id,int incl)
  {
    nID=id;
    nINCL=incl;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(TID*)from;
  }

  friend DISCO_DLL void CDPTID (PutGetFlag pgf, ASN1* cid, address varAddr,
                                bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTID(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL CHETID : ChainAnyElem {
  TID data;

  ChainAnyElem* Clone ()
  { return new CHETID(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHETID*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHETID ();

typedef CHAINANY/*TID*/ TIDs;

extern DISCO_DLL void CDPTIDs (PutGetFlag pgf, ASN1* cid, address varAddr,
                               bool baseCDP=false);

typedef SETpp SynFlags;

extern DISCO_DLL void CDPSynFlags (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

enum TOperator {
  OP_noOp,
  OP_equal,
  OP_notequal,
  OP_lessthen,
  OP_greaterthen,
  OP_lessequal,
  OP_greaterequal,
  OP_plus,
  OP_minus,
  OP_mult,
  OP_div,
  OP_mod,
  OP_bwAnd,
  OP_bwOr,
  OP_bwXor,
  OP_invert,
  OP_lshift,
  OP_rshift,
  OP_fis,
  OP_arrayGet,
  OP_arraySet,
  OP_high};

extern DISCO_DLL void CDPTOperator (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

class DISCO_DLL TVElem : public DObject  {
  DECLARE_DYNAMIC_CLASS(TVElem)


  TVElem()
  {
    ok=true;
    updateNo=0;
    TypeFlags=SETpp();
    op=OP_noOp;
    ElDECL=0;
    ClssDECL=0;
  }
  int updateNo;
  bool ok;
  SynFlags TypeFlags;
  TID VTEl;
  TID VTBaseEl;
  TID VTClss;
  TIDs Ambgs;
  TOperator op;
  LavaDECL *ElDECL;
  LavaDECL *ClssDECL;

  virtual void CopyData (AnyType *from) {
    *this = *(TVElem*)from;
  }

  friend DISCO_DLL void CDPTVElem (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTVElem(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL CHETVElem : ChainAnyElem {
  TVElem data;

  ChainAnyElem* Clone ()
  { return new CHETVElem(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHETVElem*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHETVElem ();

typedef CHAINANY/*TVElem*/ TCTVElem;

extern DISCO_DLL void CDPTCTVElem (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

class DISCO_DLL TVElems : public DObject  {
  DECLARE_DYNAMIC_CLASS(TVElems)

  int UpdateNo;
  TCTVElem VElems;

  TVElems () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TVElems*)from;
  }

  friend DISCO_DLL void CDPTVElems (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTVElems(pgf,cid,(address)this,baseCDP); }
};

class DISCO_DLL CLavaError : public DObject  {
  DECLARE_DYNAMIC_CLASS(CLavaError)


  public:
  QString *IDS;
  STRING textParam;
  bool showAutoCorrBox;

  CLavaError()
  {
    showAutoCorrBox=false;
  }

  CLavaError(CHAINX *errors,QString *ids,DString *text=0,bool autoC=false);

  virtual void CopyData (AnyType *from) {
    *this = *(CLavaError*)from;
  }

  friend DISCO_DLL void CDPCLavaError (PutGetFlag pgf, ASN1* cid, address varAddr,
                                       bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPCLavaError(pgf,cid,(address)this,baseCDP); }
};

enum TEmphasis {
  High,
  Low,
  NoEcho};

extern DISCO_DLL void CDPTEmphasis (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

enum TIndentation {
  RelIndent,
  AbsIndent};

extern DISCO_DLL void CDPTIndentation (PutGetFlag pgf, ASN1* cid, address varAddr,
                                       bool baseCDP=false);

enum TJustification {
  LeftJustified,
  Centered,
  RightJustified};

extern DISCO_DLL void CDPTJustification (PutGetFlag pgf, ASN1* cid, address varAddr,
                                         bool baseCDP=false);

enum TAlignment {
  UnAligned,
  LeftAligned,
  CenterAligned,
  RightAligned};

extern DISCO_DLL void CDPTAlignment (PutGetFlag pgf, ASN1* cid, address varAddr,
                                     bool baseCDP=false);

enum TBox {
  NoRules,
  HRules,
  VRules,
  FullBox,
  FocusBox};

extern DISCO_DLL void CDPTBox (PutGetFlag pgf, ASN1* cid, address varAddr,
                               bool baseCDP=false);

enum TBasicType {
  NonBasic,
  B_Object,
  Bitset,
  B_Bool,
  Char,
  Integer,
  Float,
  Double,
  VLString,
  Enumeration,
  B_Set,
  B_Chain,
  B_Che,
  B_Array,
  ComponentObj,
  B_Exception,
  B_HWException,
  B_LavaException,
  B_Callback,
  Identifier};

extern DISCO_DLL void CDPTBasicType (PutGetFlag pgf, ASN1* cid, address varAddr,
                                     bool baseCDP=false);

enum TreeFlag {
  isExpanded,
  ItemsExpanded,
  ParaExpanded,
  InExpanded,
  OutExpanded,
  DefsExpanded,
  MemsExpanded,
  hasEmptyOpt,
  leftArrows,
  FREE_FLAG9,
  FREE_FLAG10,
  FREE_FLAG11,
  FREE_FLAG12,
  FREE_FLAG13,
  FREE_FLAG14,
  FREE_FLAG15,
  FREE_FLAG16,
  FREE_FLAG17,
  dragOverrides,
  mixedDrag,
  dragVT,
  dragInt,
  dragImpl,
  dragIni,
  dragFuncDef,
  dragFuncImpl,
  dragPack,
  dragCompo,
  dragCOS,
  dragCO,
  dragInput,
  ShowExecComments};

extern DISCO_DLL void CDPTreeFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

enum TypeFlag {
  isOptional,
  isStatic,
  thisComponent,
  isComponent,
  substitutable,
  isProtected,
  isPlaceholder,
  isConst,
  thisCompoForm,
  isAbstract,
  forceOverride,
  isPropGet,
  isPropSet,
  Free_FLAG13,
  constituent,
  acquaintance,
  isInitializer,
  isPersistent,
  isGUIEdit,
  isGUI,
  sameAsSelf,
  isNative,
  defaultInitializer,
  isTransaction,
  execConcurrent,
  execIndependent,
  copyOnAccess,
  consumable,
  stateObject,
  hasSetGet,
  definesObjCat,
  trueObjCat};

extern DISCO_DLL void CDPTypeFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

enum SecondTFlag {
  FREE_FLAG0,
  FREE_FLAG1,
  FREE_FLAG2,
  FREE_FLAG3,
  FREE_FLAG4,
  FREE_FLAG5,
  FREE_FLAG6,
  FREE_FLAG7,
  FREE_FLAG8,
  enableName,
  overrides,
  funcImpl,
  inComponent,
  isEnum,
  isSet,
  isChain,
  isArray,
  isCallback,
  isEventDesc,
  isEventSpec,
  isException};

extern DISCO_DLL void CDPSecondTFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                                      bool baseCDP=false);

enum WorkFlag {
  isPattern,
  isPartOfPattern,
  runTimeOK,
  recalcVT,
  flagDrag,
  checkmark,
  selAfter,
  selEnum,
  selPara,
  selIn,
  selOut,
  selDefs,
  selMems,
  implRequired,
  hasDefaultIni,
  allowDEL,
  nonEmptyInvariant,
  isReferenced,
  skipOnCopy,
  skipOnDeleteID,
  fromPrivToPub,
  ImplFromBaseToEx,
  SupportsReady,
  poppedUp,
  formVTOK,
  newTreeNode,
  fromPubToPriv};

extern DISCO_DLL void CDPWorkFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);

enum BasicFlag {
  Atomic,
  useDefault,
  Text,
  Vertical,
  PopUp,
  Invisible,
  ButtonMenu,
  OptionMenu,
  PopUpMenu,
  MenuText,
  MenuFirst,
  BitmapLabel,
  BlankSelCode,
  Toggle,
  DefaultMenu,
  Popdown,
  RadioButtons,
  Groupbox,
  hasPixmap,
  beforeBaseType};

extern DISCO_DLL void CDPBasicFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

enum IoSigFlag {
  UnprotectedUser,
  UnprotectedProg,
  Flag_INPUT,
  Flag_OUTPUT,
  DONTPUT,
  Enter,
  EnterAsIs,
  Signature,
  SigIsValid,
  DontSign,
  trueValue,
  setViewBorder,
  firstUseOfFrame};

extern DISCO_DLL void CDPIoSigFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

enum IterFlag {
  Optional,
  IteratedItem,
  FixedCount,
  NoEllipsis,
  Ignore,
  Inserted,
  InsertAfter,
  Ellipsis};

extern DISCO_DLL void CDPIterFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                                   bool baseCDP=false);



struct TAnnotation;


/*************************************************************************/

const int DFTLengthBoolean = 1;
const int DFTLengthHexByte = 2;
const int DFTLengthCardinal = 5;
const int DFTLengthChar = 1;
const int DFTLengthString = 30;

const TEmphasis DFTEmphasisUnprotected = Low;
const TIndentation DFTIndType = RelIndent;
const int DFTIndentation = 2;
const TJustification DFTJustification = LeftJustified;
const TAlignment DFTAlignment = UnAligned;
const TBox DFTBox = NoRules;

/*************************************************************************/

enum TTableUpdate {
           onUndoDeleteID,
           onDeleteID,
           onAddID,
           onNewID,
           onChange,
           onUndoChange,
           onCopy,
           onMove,
           onConstrCopy,
           onTypeID,
		   onSearch,
           onSelect,
		   onSetSynOID,
           onSetBrkPnt,
           onSetRunToPnt
           };

  struct CSecTabBase {
    virtual void Destroy() {}
  };


enum PrintFlag {
  inLineComment,
  trailingComment};

extern DISCO_DLL void CDPPrintFlag (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

struct DISCO_DLL TDECLComment : public DObject  {
  DECLARE_DYNAMIC_CLASS(TDECLComment)

  STRING Comment;
  SynFlags PrintFlags;

  TDECLComment () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TDECLComment*)from;
  }

  friend DISCO_DLL void CDPTDECLComment (PutGetFlag pgf, ASN1* cid, address varAddr,
                                         bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTDECLComment(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL NST0TDECLComment {
protected:

  void init (const NST0TDECLComment&);

  void copy (const NST0TDECLComment&);

public:

  TDECLComment *ptr;

  NST0TDECLComment () { ptr = 0; }

  NST0TDECLComment (const NST0TDECLComment& n)
  { init(n); }

  void Destroy () { delete ptr; ptr = 0; }

  NST0TDECLComment& operator= (const NST0TDECLComment& n)
  { copy(n); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid, ConversionProc cdp);
};

struct DISCO_DLL NSTTDECLComment : NST0TDECLComment {
  NSTTDECLComment() {}

  NSTTDECLComment (const NSTTDECLComment& n)
  { init(n); }

  NSTTDECLComment& operator= (const NSTTDECLComment& n)
  { copy(n); return *this; }

  virtual ~NSTTDECLComment () { Destroy(); }
};

enum TBoundRel {
  GE,
  EQ,
  LE,
  LG};

extern DISCO_DLL void CDPTBoundRel (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

struct DISCO_DLL EnumSelId : public DObject  {
  DECLARE_DYNAMIC_CLASS(EnumSelId)

  STRING Id, SelectionCode;
  NSTTDECLComment DECLComment;
  bool selItem;

  EnumSelId()
  {
    selItem=false;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(EnumSelId*)from;
  }

  friend DISCO_DLL void CDPEnumSelId (PutGetFlag pgf, ASN1* cid, address varAddr,
                                      bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPEnumSelId(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL CHEEnumSelId : ChainAnyElem {
  EnumSelId data;

  ChainAnyElem* Clone ()
  { return new CHEEnumSelId(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHEEnumSelId*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHEEnumSelId ();

enum TDeclType {
  NoDef,
  Interface,
  Impl,
  Initiator,
  Function,
  Package,
  Component,
  CompObjSpec,
  CompObj,
  FormDef,
  Alias,
  VirtualType,
  BasicDef,
  Attr,
  IAttr,
  OAttr,
  ExecDef,
  FormText,
  PatternDef,
  Require,
  Ensure,
  DragParam,
  DragDef,
  DragFeature,
  DragFeatureF,
  DragIO,
  DragEnum,
  DragFText,
  UnDef,
  IsDef,
  SetDef,
  EnumDef};

extern DISCO_DLL void CDPTDeclType (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

enum TDeclDescType {
  BasicType,
  LiteralString,
  NamedType,
  StructDesc,
  EnumType,
  Undefined,
  ExecDesc,
  UnknownDDT};

extern DISCO_DLL void CDPTDeclDescType (PutGetFlag pgf, ASN1* cid, address varAddr,
                                        bool baseCDP=false);

typedef CHAINANY/*TID*/ TSupports;

extern DISCO_DLL void CDPTSupports (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

struct DISCO_DLL TLength : public DObject  {
  DECLARE_DYNAMIC_CLASS(TLength)

  unsigned Field;
  unsigned Data;
  TBoundRel FieldBoundRel, DataBoundRel;
  unsigned DecPoint;

  TLength () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TLength*)from;
  }

  friend DISCO_DLL void CDPTLength (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTLength(pgf,cid,(address)this,baseCDP); }
};

enum AnnoExType {
  anno_Color,
  anno_Pixmap,
  anno_TextColor,
  anno_PBColor};

extern DISCO_DLL void CDPAnnoExType (PutGetFlag pgf, ASN1* cid, address varAddr,
                                     bool baseCDP=false);

struct DISCO_DLL TAnnoEx : public DObject  {
  DECLARE_DYNAMIC_CLASS(TAnnoEx)

  /*  CASE */ AnnoExType exType; /* OF */

  //    anno_Color,anno_TextColor,anno_PBColor:
          bool RgbBackValid;
          bool RgbForeValid;
          unsigned RgbBackColor;
          unsigned RgbForeColor;

  //  | anno_Pixmap:
          STRING xpmFile;

  //  ELSE

  //  END

  TAnnoEx()
  {
    RgbBackValid=false;
    RgbForeValid=false;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(TAnnoEx*)from;
  }

  friend DISCO_DLL void CDPTAnnoEx (PutGetFlag pgf, ASN1* cid, address varAddr,
                                    bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTAnnoEx(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL CHETAnnoEx : ChainAnyElem {
  TAnnoEx data;

  ChainAnyElem* Clone ()
  { return new CHETAnnoEx(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHETAnnoEx*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHETAnnoEx ();

struct DISCO_DLL TAnnotation : public DObject  {
  DECLARE_DYNAMIC_CLASS(TAnnotation)

  LavaDECL *myDECL;
  NESTEDANY/*TIteration*/ Iter;
  CHAINX Prefixes, Suffixes;
  NESTEDANY/*TAnnotation*/ FA;
  NESTEDANY/*LavaDECL*/ PopupNode;
  NESTEDANY/*LavaDECL*/ MenuDECL;
  NESTEDANY/*TIteration*/ IterOrig;
  SynFlags BasicFlags, IoSigFlags, IterFlags;
  STRING WidgetName;
  TLength Length;
  TSigDef SigDef;
  CHAINANY/*TAnnoEx*/ AnnoEx;
  TEmphasis Emphasis;
  unsigned Space;
  int TabPosition;
  int FrameSpacing;
  TIndentation IndType;
  int Indentation;
  TJustification JustType;
  TAlignment Alignment;
  TBox Box;
  STRING StringValue;
  /*  CASE */ TBasicType BType; /* OF */

  //    B_Bool:
          bool B;

  //  | Enumeration:
          unsigned D;

  //  | Integer:
          int I;

  //  ELSE

  //  END
  STRING String1, String2;

  TAnnotation();
  CHETAnnoEx *GetAnnoEx(AnnoExType type,bool makeIt=false);
  void DelAnnoEx(AnnoExType type);

  virtual void CopyData (AnyType *from) {
    *this = *(TAnnotation*)from;
  }

  friend DISCO_DLL void CDPTAnnotation (PutGetFlag pgf, ASN1* cid, address varAddr,
                                        bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTAnnotation(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL NST0TAnnotation {
protected:

  void init (const NST0TAnnotation&);

  void copy (const NST0TAnnotation&);

public:

  TAnnotation *ptr;

  NST0TAnnotation () { ptr = 0; }

  NST0TAnnotation (const NST0TAnnotation& n)
  { init(n); }

  void Destroy () { delete ptr; ptr = 0; }

  NST0TAnnotation& operator= (const NST0TAnnotation& n)
  { copy(n); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid, ConversionProc cdp);
};

struct DISCO_DLL NSTTAnnotation : NST0TAnnotation {
  NSTTAnnotation() {}

  NSTTAnnotation (const NSTTAnnotation& n)
  { init(n); }

  NSTTAnnotation& operator= (const NSTTAnnotation& n)
  { copy(n); return *this; }

  virtual ~NSTTAnnotation () { Destroy(); }
};

struct DISCO_DLL LavaDECL : public DObject  {
  DECLARE_DYNAMIC_CLASS(LavaDECL)

  STRING FullName;
  STRING LocalName;
  CHAINX DECLError1;
  CHAINX DECLError2;
  NSTTDECLComment DECLComment;
  SynFlags TreeFlags;
  SynFlags WorkFlags;
  int OwnID;
  int inINCL;
  LavaDECL *ParentDECL;
  TVElems VElems;
  TBasicType fromBType;
  NSTTAnnotation Annotation;
  SynFlags TypeFlags;
  SynFlags SecondTFlags;
  TID RefID;
  TDeclType DeclType;
  TOperator op;
  int nInput;
  int nOutput;
  TSupports Supports;
  TSupports Inherits;
  CHAINANY/*EnumSelId*/ Items;
  /*  CASE */ TDeclDescType DeclDescType; /* OF */

  //    BasicType:
          TBasicType BType;

  //  | EnumType:
          NESTEDANY/*TEnumDescription*/ EnumDesc;

  //  | StructDesc:
          CHAINX NestedDecls;

  //  | LiteralString:
          STRING LitStr;

  //  | ExecDesc:
          NESTEDANY/*SynObjectBase*/ Exec;

  //  ELSE

  //  END
  TIDs ImplIDs;
  LavaDECL *RuntimeDECL;
  LavaDECL *RelatedDECL;
  CSecTabBase *SectionTabPtr;
  int nSection;
  int SectionInfo1;
  int SectionInfo2;
  int SectionInfo3;

  LavaDECL();

  ~LavaDECL();
  bool isInSubTree(LavaDECL *ancestor);
  bool usableIn(LavaDECL *decl);
  bool HasDefaultInitializer();
  void CompleteFullName();
  void ResetCheckmarks();
  int GetAppendPos(TDeclType declType);

  virtual void CopyData (AnyType *from) {
    *this = *(LavaDECL*)from;
  }

  friend DISCO_DLL void CDPLavaDECL (PutGetFlag pgf, ASN1* cid, address varAddr,
                                     bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPLavaDECL(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL NST0LavaDECL {
protected:

  void init (const NST0LavaDECL&);

  void copy (const NST0LavaDECL&);

public:

  LavaDECL *ptr;

  NST0LavaDECL () { ptr = 0; }

  NST0LavaDECL (const NST0LavaDECL& n)
  { init(n); }

  void Destroy () { delete ptr; ptr = 0; }

  NST0LavaDECL& operator= (const NST0LavaDECL& n)
  { copy(n); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid, ConversionProc cdp);
};

struct DISCO_DLL NSTLavaDECL : NST0LavaDECL {
  NSTLavaDECL() {}

  NSTLavaDECL (const NSTLavaDECL& n)
  { init(n); }

  NSTLavaDECL& operator= (const NSTLavaDECL& n)
  { copy(n); return *this; }

  virtual ~NSTLavaDECL () { Destroy(); }
};

struct DISCO_DLL TEnumDescription : public DObject  {
  DECLARE_DYNAMIC_CLASS(TEnumDescription)

  LavaDECL EnumField, MenuTree;

  TEnumDescription () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TEnumDescription*)from;
  }

  friend DISCO_DLL void CDPTEnumDescription (PutGetFlag pgf, ASN1* cid, address varAddr,
                                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTEnumDescription(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL TIteration : public DObject  {
  DECLARE_DYNAMIC_CLASS(TIteration)

  TBoundRel BoundType;
  unsigned Bound;
  NSTLavaDECL IteratedExpr;

  TIteration () {}

  virtual void CopyData (AnyType *from) {
    *this = *(TIteration*)from;
  }

  friend DISCO_DLL void CDPTIteration (PutGetFlag pgf, ASN1* cid, address varAddr,
                                       bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPTIteration(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL SimpleSyntax : public DObject  {
  DECLARE_DYNAMIC_CLASS(SimpleSyntax)

  int clipIncl;
  bool notFound;
  bool inWork;
  int nINCL;
  STRING UsersName;
  STRING SyntaxName;
  STRING LocalTopName;
  NSTLavaDECL TopDef;

  SimpleSyntax()
  {
    clipIncl=0;
    notFound=false;
    inWork=false;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(SimpleSyntax*)from;
  }

  friend DISCO_DLL void CDPSimpleSyntax (PutGetFlag pgf, ASN1* cid, address varAddr,
                                         bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSimpleSyntax(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL CHESimpleSyntax : ChainAnyElem {
  SimpleSyntax data;

  ChainAnyElem* Clone ()
  { return new CHESimpleSyntax(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHESimpleSyntax*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHESimpleSyntax ();

typedef CHAINANY/*SimpleSyntax*/ SyntaxDefinition;

extern DISCO_DLL void CDPSyntaxDefinition (PutGetFlag pgf, ASN1* cid, address varAddr,
                                           bool baseCDP=false);

struct DISCO_DLL ClipSimpleSyntax : public DObject  {
  DECLARE_DYNAMIC_CLASS(ClipSimpleSyntax)

  int clipIncl;
  bool notFound;
  bool inWork;
  int nINCL;
  STRING UsersName;
  STRING SyntaxName;
  STRING LocalTopName;
  NSTLavaDECL TopDef;

  ClipSimpleSyntax()
  {
    clipIncl=0;
    notFound=false;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(ClipSimpleSyntax*)from;
  }

  friend DISCO_DLL void CDPClipSimpleSyntax (PutGetFlag pgf, ASN1* cid, address varAddr,
                                             bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPClipSimpleSyntax(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL CHEClipSimpleSyntax : ChainAnyElem {
  ClipSimpleSyntax data;

  ChainAnyElem* Clone ()
  { return new CHEClipSimpleSyntax(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHEClipSimpleSyntax*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHEClipSimpleSyntax ();

typedef CHAINANY/*ClipSimpleSyntax*/ ClipSyntaxDefinition;

extern DISCO_DLL void CDPClipSyntaxDefinition (PutGetFlag pgf, ASN1* cid, address varAddr,
                                               bool baseCDP=false);

struct DISCO_DLL SynObjectBase : public DObject  {
  DECLARE_DYNAMIC_CLASS(SynObjectBase)

  virtual void MakeTable(address,int,SynObjectBase *,TTableUpdate,address where=0,CHAINX *chx=0,address searchData=0)
  {
  }
  virtual void MakeExpression()
  {
  }

  SynObjectBase () {}

  virtual void CopyData (AnyType *from) {
    *this = *(SynObjectBase*)from;
  }

  friend DISCO_DLL void CDPSynObjectBase (PutGetFlag pgf, ASN1* cid, address varAddr,
                                          bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPSynObjectBase(pgf,cid,(address)this,baseCDP); }
};

enum DbgCommand {
  Dbg_Nothing,
  Dbg_Continue,
  Dbg_StopData,
  Dbg_MemberDataRq,
  Dbg_MemberData,
  Dbg_StackRq,
  Dbg_Stack,
  Dbg_Exit};

extern DISCO_DLL void CDPDbgCommand (PutGetFlag pgf, ASN1* cid, address varAddr,
                                     bool baseCDP=false);

enum StopReason {
  Stop_BreakPoint,
  Stop_NextStm,
  Stop_NextFunc,
  Stop_NextOp,
  Stop_StepInto,
  Stop_StepOut,
  Stop_FunctionCall,
  Stop_Exception};

extern DISCO_DLL void CDPStopReason (PutGetFlag pgf, ASN1* cid, address varAddr,
                                     bool baseCDP=false);

enum DbgContType {
  dbg_Cont,
  dbg_Step,
  dbg_StepFunc,
  dbg_StepOut,
  dbg_StepInto,
  dbg_RunTo};

extern DISCO_DLL void CDPDbgContType (PutGetFlag pgf, ASN1* cid, address varAddr,
                                      bool baseCDP=false);

struct DISCO_DLL DDItemData : public DObject  {
  DECLARE_DYNAMIC_CLASS(DDItemData)


  DDItemData()
  {
  }

  ~DDItemData()
  {
  }
  bool isPrivate;
  bool HasChildren;
  STRING Column0;
  STRING Column1;
  STRING Column2;
  CHAINX Children;

  virtual void CopyData (AnyType *from) {
    *this = *(DDItemData*)from;
  }

  friend DISCO_DLL void CDPDDItemData (PutGetFlag pgf, ASN1* cid, address varAddr,
                                       bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDDItemData(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL StackData : public DObject  {
  DECLARE_DYNAMIC_CLASS(StackData)

  int SynObjID;
  TDeclType ExecType;
  TID FuncID;
  CSecTabBase ***Stack;
  SynObjectBase *SynObj;

  StackData () {}

  virtual void CopyData (AnyType *from) {
    *this = *(StackData*)from;
  }

  friend DISCO_DLL void CDPStackData (PutGetFlag pgf, ASN1* cid, address varAddr,
                                      bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPStackData(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL CHEStackData : ChainAnyElem {
  StackData data;

  ChainAnyElem* Clone ()
  { return new CHEStackData(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHEStackData*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHEStackData ();

struct DISCO_DLL ProgPoint : public DObject  {
  DECLARE_DYNAMIC_CLASS(ProgPoint)

  int SynObjID;
  TDeclType ExecType;
  TID FuncID;
  bool Activate;
  SynObjectBase *SynObj;
  DString FuncDocName;
  DString FuncDocDir;
  address FuncDoc;

  ProgPoint () {}

  virtual void CopyData (AnyType *from) {
    *this = *(ProgPoint*)from;
  }

  friend DISCO_DLL void CDPProgPoint (PutGetFlag pgf, ASN1* cid, address varAddr,
                                      bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPProgPoint(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL CHEProgPoint : ChainAnyElem {
  ProgPoint data;

  ChainAnyElem* Clone ()
  { return new CHEProgPoint(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHEProgPoint*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHEProgPoint ();

struct DISCO_DLL NST0ProgPoint {
protected:

  void init (const NST0ProgPoint&);

  void copy (const NST0ProgPoint&);

public:

  ProgPoint *ptr;

  NST0ProgPoint () { ptr = 0; }

  NST0ProgPoint (const NST0ProgPoint& n)
  { init(n); }

  void Destroy () { delete ptr; ptr = 0; }

  NST0ProgPoint& operator= (const NST0ProgPoint& n)
  { copy(n); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid, ConversionProc cdp);
};

struct DISCO_DLL NSTProgPoint : NST0ProgPoint {
  NSTProgPoint() {}

  NSTProgPoint (const NSTProgPoint& n)
  { init(n); }

  NSTProgPoint& operator= (const NSTProgPoint& n)
  { copy(n); return *this; }

  virtual ~NSTProgPoint () { Destroy(); }
};

struct DISCO_DLL DbgStopData : public DObject  {
  DECLARE_DYNAMIC_CLASS(DbgStopData)

  StopReason stopReason;
  int ActStackLevel;
  CHAINANY/*StackData*/ StackChain;
  CHAINX ObjectChain;

  DbgStopData()
  {
    ActStackLevel=0;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(DbgStopData*)from;
  }

  friend DISCO_DLL void CDPDbgStopData (PutGetFlag pgf, ASN1* cid, address varAddr,
                                        bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDbgStopData(pgf,cid,(address)this,baseCDP); }
};

struct DISCO_DLL DbgContData : public DObject  {
  DECLARE_DYNAMIC_CLASS(DbgContData)

  bool ClearBrkPnts;
  CHAINANY/*ProgPoint*/ BrkPnts;
  DbgContType ContType;
  NSTProgPoint RunToPnt;

  DbgContData()
  {
    ClearBrkPnts=false;
  }

  virtual void CopyData (AnyType *from) {
    *this = *(DbgContData*)from;
  }

  friend DISCO_DLL void CDPDbgContData (PutGetFlag pgf, ASN1* cid, address varAddr,
                                        bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDbgContData(pgf,cid,(address)this,baseCDP); }
};

typedef CHAINANY/*int*/ ChObjRq;

struct DISCO_DLL NST0ChObjRq {
protected:

  void init (const NST0ChObjRq&);

  void copy (const NST0ChObjRq&);

public:

  ChObjRq *ptr;

  NST0ChObjRq () { ptr = 0; }

  NST0ChObjRq (const NST0ChObjRq& n)
  { init(n); }

  void Destroy () { delete ptr; ptr = 0; }

  NST0ChObjRq& operator= (const NST0ChObjRq& n)
  { copy(n); return *this; }

  void CDP (PutGetFlag pgf, ASN1 *cid, ConversionProc cdp);
};

struct DISCO_DLL NSTChObjRq : NST0ChObjRq {
  NSTChObjRq() {}

  NSTChObjRq (const NSTChObjRq& n)
  { init(n); }

  NSTChObjRq& operator= (const NSTChObjRq& n)
  { copy(n); return *this; }

  virtual ~NSTChObjRq () { Destroy(); }
};

extern DISCO_DLL void CDPChObjRq (PutGetFlag pgf, ASN1* cid, address varAddr,
                                  bool baseCDP=false);

struct DISCO_DLL DbgMessage : public DObject  {
  DECLARE_DYNAMIC_CLASS(DbgMessage)

  /*  CASE */ DbgCommand Command; /* OF */

  //    Dbg_StopData,Dbg_Stack:
          NESTEDANY0/*DbgStopData*/ DbgData;

  //  | Dbg_MemberData:
          NESTEDANY0/*DDItemData*/ ObjData;

  //  | Dbg_MemberDataRq:
          NSTChObjRq ObjNr;

  //  | Dbg_StackRq:
          int CallStackLevel;

  //  | Dbg_Continue:
          NESTEDANY/*DbgContData*/ ContData;

  //  ELSE

  //  END
  void SetSendData(DbgCommand command,DbgStopData *data)
  {
    Command=command;
    DbgData.ptr=0;
    DbgData.ptr=data;
  }
  void SetSendData(DDItemData *obj)
  {
    Command=Dbg_MemberData;
    ObjData.ptr=obj;
  }

  DbgMessage(DbgCommand com,ChObjRq *oRq)
  {
    Command=com;
    ObjNr.ptr=oRq;
  }

  DbgMessage()
  {
  }
  void Destroy()
  {
    DbgData.ptr=0;
    ObjData.ptr=0;
    ObjNr.Destroy();
    ContData.Destroy();
  }

  virtual void CopyData (AnyType *from) {
    *this = *(DbgMessage*)from;
  }

  friend DISCO_DLL void CDPDbgMessage (PutGetFlag pgf, ASN1* cid, address varAddr,
                                       bool baseCDP=false);

  virtual void CDP (PutGetFlag pgf, ASN1* cid,
                    bool baseCDP=false)
  { CDPDbgMessage(pgf,cid,(address)this,baseCDP); }
};

/*************************************************************************/
/* Concrete syntactic objects: */

struct CHEFormNode;

class DISCO_DLL CContext 
{
public:
  SynFlags ContextFlags;  
  LavaDECL* iContext;
  LavaDECL* oContext;
  CContext() {iContext = 0; oContext = 0;}
  CContext(LavaDECL* iC, LavaDECL* oC) {iContext = iC; oContext = oC;}
};


enum TFontCase {
  ownFont,
  parentFont,
  mainFont};

struct DISCO_DLL Field : public AnyType  {
  QWidget *widget, *frameWidget, *popupShell;
  unsigned borderWidth, leng, visible;
  CHEFormNode *up;
  bool poppedUp;

  Field () {}

  virtual void CopyData (AnyType *from) {
    *this = *(Field*)from;
  }
};

typedef address SigNodePtr;

struct DISCO_DLL CHESigNodePtr : ChainAnyElem {
  SigNodePtr data;

  ChainAnyElem* Clone ()
  { return new CHESigNodePtr(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHESigNodePtr*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHESigNodePtr ();

struct DISCO_DLL FormNode : public AnyType  {
  Field FIP;
  LavaDECL *FormSyntax;
  LavaDECL *EnumField;
  NST0TAnnotation Annotation;
  TFontCase ownLFont;
  QFont LFont;
  TFontCase ownTFont;
  QFont TFont;
  QPixmap *Pixmap;
  bool ColorBValid;
  bool ColorFValid;
  QColor ColorB;
  QColor ColorF;
  STRING WidgetName;
  CHAINANY0/*SigNodePtr*/ SigNodes;
  SynFlags BasicFlags, IoSigFlags, IterFlags;
  STRING StringValue;
  bool Atomic;
  TBasicType BType;
  unsigned D;
  int I;
  bool B;
  float F;
  double Db;
  char Ch;
  CSecTabBase ***ResultVarPtr;
  CSecTabBase **HandleObjPtr;
  CHAINANY/*FormNode*/ SubTree;

  FormNode()
  {
    ResultVarPtr=0;
    HandleObjPtr=0;
    EnumField=0;
    ownLFont=mainFont;
    ownTFont=mainFont;
    Pixmap=0;
    ColorBValid=false;
    ColorFValid=false;
  }

  ~FormNode();
  unsigned GetLengthField();
  unsigned GetLengthDecPoint();

  virtual void CopyData (AnyType *from) {
    *this = *(FormNode*)from;
  }
};

struct DISCO_DLL CHEFormNode : ChainAnyElem {
  FormNode data;

  ChainAnyElem* Clone ()
  { return new CHEFormNode(*this); }

  void CopyData (ChainAnyElem* from)
  { this->data = ((CHEFormNode*)from)->data; }
};

extern DISCO_DLL ChainAnyElem* NewCHEFormNode ();

extern DISCO_DLL LavaDECL *NewLavaDECL();
extern DISCO_DLL bool RemoveErrCode(CHAINX* errors, QString* ids);
                      //returns true if removes an error

extern DISCO_DLL TAnnotation *NewTAnnotation();

#endif
