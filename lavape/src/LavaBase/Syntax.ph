#ifndef __Syntax
#define __Syntax
/*
#ifdef __GNUC__
#pragma interface
#endif
*/
/* Lava programming environment and Lava interpreter.
   Copyright (C) 2001 Fraunhofer-Gesellschaft.
   The GNU GPL license applies. For details see the file
   "license.txt" in the top level directory. */


#include "BASEMACROS.h"
#include "qstring.h"
#include "qfont.h"
#include "qwidget.h"
#include "wx_obj.h"
#include "qpixmap.h"
#include "qmetatype.h"
//#include "LavaBaseDoc.h"

#define RELEASE 5

#undef VIEW

struct LavaDECL;


#define Ev_ValueChanged 1
#define Ev_ChainInsert 2
#define Ev_ChainDelete 3
#define Ev_OptInsert 4
#define Ev_OptDelete 5

$TYPE +CDP {

  struct TSigRef {
    bool GlobalScope;
    STRING SigID;
  };

  struct TSigDef {
    unsigned Scope; /* iteration levels from the signature upward */
    STRING SigID;
  };


  struct TID {
    int nINCL;//the number of the included syntax in the document referencing this object
    int nID;  //the OwnID of the referenced object

    inline bool operator == (const TID& id2) const
    {
      return ((nID == id2.nID) && (nINCL == id2.nINCL));
    }

    inline bool operator != (const TID& id2) const
    {
      return ((nID != id2.nID) || (nINCL != id2.nINCL));
    }

    TID() {nINCL=0; nID=-1;}
    TID(int id, int incl)  {nID=id; nINCL=incl;}
  };

  typedef CHAINANY<TID> TIDs;
  typedef SET SynFlags;


  enum TOperator {
    OP_noOp,
  OP_equal,
  OP_notequal,
  OP_lessthan,
  OP_greaterthan,
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
  OP_high
  };

  class CLavaError {
  public:
    QString* IDS;
    STRING textParam;
    bool showAutoCorrBox;
    CLavaError() {showAutoCorrBox=false;}
    CLavaError(CHAINX* errors, QString* ids, DString *text=0, bool autoC = false);
  };

  class TVElem {
    TVElem() {ok=true; updateNo=0; TypeFlags = SETpp(); op = OP_noOp; ElDECL=0; ClssDECL=0; }
    int updateNo; //the update state
    bool ok;
    SynFlags TypeFlags;
    TID VTEl;     //the element
    TID VTBaseEl;  //the overridden element in the base class VTClss (LavaPE only)
    TID VTClss;   //the virtual table (base class) from which it is an element
    TIDs Ambgs;   //ambiguities
    TOperator op; //if a function is an operator
    LavaDECL-- *ElDECL; //at runtime only
    LavaDECL-- *ClssDECL; //at runtime only
  };

  typedef CHAINANY<TVElem> TCTVElem;

  class TVElems {
    int UpdateNo; //LavaPE:the update state,
                  //Lava: =0: not maked, > 0: maked, ok, <0: maked, not ok
  TCTVElem VElems;
  };

  enum TEmphasis {
    High,
    Low,
    NoEcho};

  enum TIndentation {
    RelIndent,
    AbsIndent};

  enum TJustification {
    LeftJustified,
    Centered,
    RightJustified};

  enum TAlignment {
    UnAligned,
    LeftAligned,
    CenterAligned,
    RightAligned};

  enum TBox {
    NoRules,
    HRules,
    VRules,
    FullBox,
    FocusBox};

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
		B_RTException,
    B_GUI,
    Identifier //this must be the last
  };

  enum TreeFlag {
    isExpanded,
    //0, 1, X0000 0001
    ItemsExpanded,
    //1, 2, X0000 0002
    ParaExpanded,
    //2, 4, X0000 0004
    InExpanded,
    //3, 8, X0000 0008
    OutExpanded,
    //4, 16, X0000 0010
    DefsExpanded,
    //5, 32, X0000 0020
    MemsExpanded,
    //6, 64, X0000 0040
    hasEmptyOpt,
    //7, 128, X0000 0080
    leftArrows,
    //8, 256, X0000 0100 in exec: assignment arrows from right to left
    parmNames,
    //9, 512, X0000 0200 in exec: show formal parameter names in function calls

    FREE_TREEFLAG10,//, X0000 0400
    FREE_TREEFLAG11,//, X0000 0800
    FREE_TREEFLAG12,//, X0000 1000
    FREE_TREEFLAG13,//, X0000 20000
    FREE_TREEFLAG14,//, X0000 4000
    FREE_TREEFLAG15,//, X0000 8000
    FREE_TREEFLAG16,//, X0001 0000
    FREE_TREEFLAG17,//, X0002 0000
    dragOverrides,  //overrides on clipboard
    //18//, X0004 0000,
    mixedDrag,
    //19//, X0008 0000
    dragVT,   //VirtualType on clipboard
    //20//, X0010 0000
    dragInt,  //Interface on clipboard
    //21//, X0020 0000
    dragImpl,  //Implementation on clipboard
    //22//, X0040 0000
    dragIni,      //Initiator on clipboard
    //23//, X0080 0000
    dragFuncDef,  //Function on clipboard
    //24//, X0100 0000
    dragFuncImpl, //Function on clipboard
    //25//, X0200 0000
    dragPack, //Package on clipboard
    //26//, X0400 0000
    dragCompo,//Component on clipboard
    //27//, X0800 0000
    dragCOS,  //CompObjSpec on clipboard
    //28//, X1000 0000
    dragCO,   //CompObj on clipboard
    //29//, X2000 0000
    dragInput, //input (from initiator) on clipboard
    //30//, X4000 0000
    ShowExecComments
    //31//, X8000 0000
  };


  enum TypeFlag {
    isOptional,
    // 0/1, X0000 0001, optional input or output or attribute
    isStatic,
    // 1, X0000 0002 , static function
    thisComponent,
   //2/ X0000 0004, the internal interface of the component object
    isComponent,
    // 3/ X0000 0008, Component object interface
    substitutable,
    // 4/16, X0000 0010 substitutable type (C- and P-derivations) of Attr, IAttr or OAttr
    isProtected,
    // 5/ X0000 0020, protected attr/function or opaque namespace
    isPlaceholder,
    // 6/, X0000 0040, an attribute defines a placeHolder
    isConst,
    // 7/128, X0000 0080,  // value of member variable must not be replaced
    thisCompoForm,
    // 8/256, X0000 0100, the internal interface of the component objects GUI
    isAbstract,
    // 9/512, X0000 0200, abstract function (always virtual abstract)
    //        or abstract pattern parameter or non-creatable interface
    forceOverride, //
    //10  1024, X0000 0400 override function if class is extended
    isPropGet,
    //11/2048, X0000 0800, get property function in a service implementation, the RefID is the ID of the property
    isPropSet,
    //12/4096, X0000 1000, set property function in a service implementation, the RefID is the ID of the property
    isAnyCatY,
    //13/8192, X0000 2000, //attribute or "self" of function may have any object category
    constituent,
    //14/16384, X0000 4000 constituent link
    acquaintance,
    //15/32768, X0000 8000 acquaintance link
    isInitializer,
    //16/, X0001 0000 initializer function
    isPersistent,
    //17/, X0002 0000 persistent component
    isFinalVT, //cannot be overridden,
    //18/, X0004 0000 the edit show function
    FREE_TYPEFLAG2, //oldIsGUI,
    //19, X0008 0000/ now free flag
    FREE_TYPEFLAG3,// sameAsSelf,
    //20, X0010 0000 IO parameter has same category as "self"
    isNative,
    //21, X0020 0000         Function or interface has native implementation
    defaultInitializer,
    //22, X0040 0000         Default initializer
    isTransaction,
    //23, X0080 0000 Function or initiator is transaction
    execConcurrent,
    //24, X0100 0000 Concurrent function or initiator
    execIndependent,
    //25, X0200 0000 Independent function or initiator
    copyOnAccess,
    //26, X0400 0000
    consumable,
    //27, X0800 0000
    isStateObjectY,  //attribute or "self" of function designates a state object
    //28, X1000 0000
    hasSetGet,    //attribute has set and get functions
    //29, X2000 0000
    definesObjCat, //pattern parameter defines object category
    //30, X4000 0000
    definiteCat    //attribute/IO-category is defined
    //31, X8000 0000
  };//TypeFlag


	enum SecondTFlag {
	  isLavaSignal,
      //0,X0000 0001, function is Signal
	  isHandler, 
    //X0000 0002, function is handler function
	  isGUI, //X0000 0004
    //2, X0000 0004/ form service, interface and implementation
    // and its (not removable/changeable) input and output
	  FREE_FLAG3, //X0000 0008
	  FREE_FLAG4, //X0000 0010
	  FREE_FLAG5, //X0000 0020
	  FREE_FLAG6, //X0000 0040
	  FREE_FLAG7, //X0000 0080
	  hasClosedInput,
    //8,X0000 0100, function has closed input
    enableName,
    //9,X0000 0200, 512, overriding function, virtual type or member does not inherit the name of the overridden
    overrides,
    //10,X0000 0400, 1024 member or function overrides member or function of extended interface
    funcImpl,
    //11,X0000 0800, 2048 function implementation
    inComponent,
    //12, X0000 1000, 4096 Used only in clipboard-data for drag/drop and paste:
    //    All Attr-, IAttr- and OAttr-types are structures or component interfaces
    isEnum,
    //13, X0000 2000, 8192 the LavaDECL defines an Enumeration or an interface which extends an Enumeration
    isSet,
    //14, X0000 4000, 16384 the LavaDECL defines a set or an interface which extends a set
    //    or is the set element virtual type
    isChain,
    //15, X0000 8000, 32768 the LavaDECL defines a chain or an interface which extends a chain
    //    or is the chain element virtual type
    isArray,
    //16, X0001 0000, the LavaDECL defines an array or an interface which extends a array
    //    or is the array element virtual type
    closed,
    //17, X0002 0000, object may be not yet fully initialized; therefore: member access forbidden
    isEventDesc,
    //18, X0004 0000 the decl is the event description virtual type of a callback pattern
    FREE_FLAG18,
    //19, X0008 0000 the decl is the event specification virtual type of a callback pattern
    isException,
	//20, X0010 0000, the LavaDECL defines an exception or an interface which extends an exception
    //    or is the exception enumeration virtual type
    FREE_FLAG21
	};

  enum WorkFlag {
    isPattern,
    //0, X0000 0001, the Package or Interface has pattern parameter
    isPartOfPattern,
    //1, X0000 0002, the Interface is part of a pattern
    runTimeOK,
    //2, X0000 0004, run time checked
    recalcVT,
    //3, X0000 0008, recalculate the vitual table after some changes
    flagDrag,
    //4, X0000 0010, used in drag and drop operations
    checkmark,
    //5, X0000 0020, temporary used for consistency checks
    selAfter,
    //6, X0000 0040, select the tree item of this decl after update
    selEnum,
    //7, X0000 0080, select the Enumeratio section node
    selPara,
    //8, X0000 0100, select the param section node
    selIn,
    //9, X0000 0200, select the input section node
    selOut,
    //10, X0000 0400, select the output section node
    selDefs,
    //11, X0000 0800, select the definitions section node
    selMems,
    //12, X0000 1000, select the features section node
    implRequired,
    //13, X0000 2000, interface must have an implementation
    hasDefaultIni,
    //14, X0000 4000, interface has a default initializer
    allowDEL,
    //15, X0000 8000, allow function implementation to be deleted
    nonEmptyInvariant,
    //16, X0001 0000, invariant was non-empty
    isReferenced,
    //17, X0002 0000, formal parameter is referenced in exec body
    skipOnCopy,
    //18, X0004 0000, used in refactoring: skipped copy actions if move from public to private
    skipOnDeleteID,
    //19, X0008 0000, used in refactoring: no delete of local id's if move from public to private
    fromPrivToPub,
    //20, X0010 0000, used in refactoring: private to public
    ImplFromBaseToEx,
    //21, X0020 0000, used in refactoring: base to extension
    SupportsReady,
    //22, X0040 0000, Supports are already from drop doc in clipEl
    poppedUp,
    //23, X0080 0000, used in LavaGUI: pop up after rebuilding the form
    formVTOK,
    //24, X0100 0000, used at runtime
	  newTreeNode,
	  //25, X0200 0000, used in updates
    fromPubToPriv,
    //26, X0400 0000, used in refactoring: public to private
    isIgnored,
    //27, X0800 0000, mandatory input parameter is ignored
    selRequire,
    //28, X0800 0001 
    selExec,
    //29, X0800 0010 
    selEnsure
    //30, X0800 0011 
  };

  enum BasicFlag {
    Atomic,
    /* 0, no tree representation, only string and internal rep. */
    useDefault,
    /* 1, atomic field value not copied from source */
    Text,
    /* 2, 2-dim text widget required for this VLString field */
    Vertical,
    /* 3, primary orientation of child widget arrangement;
       "horizontal" by default */
    PopUp,
    /* 4, This sub-tree is realized as a pop-up window if possible;
       used, e.g., for signatures/certificates */
    Invisible,
    /* 5, invisible tag field, popup button or popup variant parent */
    ButtonMenu,
    /* 6, This Enumeration field is realized as a button menu if possible */
    OptionMenu,
    /* 7, This Enumeration field is realized as a button menu if possible */
    PopUpMenu,
    /* 8, This Enumeration field is realized as a pop-up menu if possible */
    MenuText,
    /* 9, Marks pure text items in menus (no button must be generated! */
    MenuFirst,
    /* 10, menu precedes selection code field */
    BitmapLabel,
    /* 11, Widget contains a bitmap instead of a text label */
    BlankSelCode,
    /* 12, Marks menu lines with blank selection code */
    Toggle,
    /* 13, BOOLEAN or enumerated field which may be toggled;
       Annotation flag */
    DefaultMenu,
    /* 14,  Attribute type is enumeration interface (not form)*/
    Popdown,
    /* 15, radio/push buttons: pop down containing popup window */
    RadioButtons,
    /* 16, radio button menu*/
    Groupbox,
    /* 17, set Groupbox arround radio button menu or other frame*/
    hasPixmap,
    /* 18, String1 in Annotation used as pixmap file name (not a font string)*/
    beforeBaseType
    /* 19, use the literal before fields of base form type*/
  };

  enum IoSigFlag {
    UnprotectedUser,
    /* 0, the terminal user may modify this object */
    UnprotectedProg,
    /* 1, the appl. program may modify this object */
    Flag_INPUT,
    /* 2, nested indeterminate objects are UnprotectedUser by default */
    Flag_OUTPUT,
    /* 3, nested indeterminate objects are UnprotectedProg by default */
    DONTPUT,
    /* 4, pure placeholder atomic field, contents is irrelevant */
    Enter,
    /* 5, BOOLEAN/Enumeration fields: simulate an ENTER command if input
       has been accepted on this field and no CASE-variant remains
       to be shown */
    EnterAsIs,
    /* 6, like "Enter", but allow form to be incompletely filled in */
    Signature,
    /* 7, This sub-tree is an electronic signature */
    SigIsValid,
    /* 8, This signature is valid */
    DontSign,
    /* 9, Don't sign this field or sub-form*/
    trueValue,
    // 10, atomic field has true value
	setViewBorder,
	// 11
	firstUseOfFrame
	// 12
    };

  enum IterFlag {
    Optional,
    /* 0, optional syntactic component */
    IteratedItem,
    /* 1, used in FormNodes to mark items of iterations */
    FixedCount,
    /* 2, for iterations and optional elements only: the terminal user
       must not change the number of elements */
    NoEllipsis,
    /* 3, for iterations only: the ellipsis field is suppressed so that
       the terminal user can only duplicate existing elements
       but not create empty elements */
    Ignore,
    /* 4, ignore this node and all sub-nodes */
    Inserted,
    /* 5, this item of the containing "Iteration" has been inserted
       (compared to the last output of the application program;
       used by Windows.Response in TIP) */
    InsertAfter,
    /* 6, auxiliary flag, set and used by Windows.receiveField in TIP */
    Ellipsis
    /* 7, marks the "..." field of optional elements and iterations */

   };

}

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
           onGetAddress};

  struct CSecTabBase {
    virtual void Destroy() {}
  };

$TYPE +CDP {
  enum PrintFlag {
    inLineComment,
  trailingComment
  };

  struct TDECLComment {
    STRING Comment;
  SynFlags PrintFlags;
  };

  enum TBoundRel {
    GE,
    EQ,
    LE,
    LG};

  struct EnumSelId {
    STRING Id, SelectionCode;
    NESTED<TDECLComment> DECLComment;
  bool-CDP selItem;
  EnumSelId() {selItem = false;}
 };



  enum TDeclType {

   // Any change in TDeclType must take care of the following rules:
   // NoDef is always the first,
   // followed by all types which are structures (have StructDesc as DeclDescType), the last is FormDef,
   // followed by other types which are not structures,
   // but values which are realy used in LavaDECL and .lava files
   // after these there may be technical values used only in programs

    NoDef,
    Interface,
    Impl,
    Initiator,
    Function,
    Package,
    LavaComponent,
    CompObjSpec,
    CompObj,
    FormDef,
    Alias, //not used
    VirtualType,
    BasicDef,
    Attr,
    IAttr,
    OAttr,
    ExecDef,
    FormText,
    PatternDef,   // used as DeclType in chain-forms and as collection decl in override operations
    Require,
    Ensure,
    DragParam,    // only used as collaction decl in drag and drop- or copy operations
    DragDef,      // only used as collaction decl in drag and drop- or copy operations
    DragFeature,  // with attributes, only used as collaction decl in drag and drop- or copy operations
    DragFeatureF, // with functions, only used as collection decl in drag and drop- or copy operations
    DragIO,       // input- or output items in drag and drop or copy and paste
    DragEnum,     // enum items in drag and drop or copy and paste
    DragFText,     // FormText in drag and drop or copy and paste
    UnDef,   //only used as return value
    IsDef,   //only used as arg of GenHTML
    SetDef,  //only used as arg
    EnumDef  //only used as arg
  };

  enum TDeclDescType {
    BasicType,
    //CaseConstruct,
    //ImportedType,
    //Iteration,
    LiteralString,
    NamedType,  //RefID is the type or type parameter
    StructDesc,
    EnumType,
    Undefined,
  ExecDesc,
  //DefDesc,
    //SetType,
  UnknownDDT
  };



  typedef CHAINANY<TID> TSupports;


  struct TLength {
    unsigned Field;
    /* length of the entire field */
    unsigned Data;
    /* length bound for actual contents of atomic field */
    TBoundRel FieldBoundRel, DataBoundRel;
    /* number of digits following the decimal point,
     0 = no decimal point */
    unsigned DecPoint; //also used as default array length
  };

  enum AnnoExType {anno_Color, anno_Pixmap, anno_TextColor, anno_PBColor};

  struct TAnnoEx {
    CASE AnnoExType exType OF
     anno_Color, anno_TextColor, anno_PBColor:
        bool RgbBackValid;
        bool RgbForeValid;
        unsigned RgbBackColor;
        unsigned RgbForeColor;

    | anno_Pixmap:
        STRING xpmFile;

    ELSE

    END;

    TAnnoEx() {RgbBackValid = false; RgbForeValid = false;}
  };


  struct TAnnotation {
    LavaDECL--* myDECL;  //up-pointer

    NESTEDANY<TIteration> Iter;

    CHAINX /*LavaDECL*/ Prefixes, Suffixes;
    NESTEDANY<TAnnotation> FA;
    NESTEDANY<LavaDECL> PopupNode;
    NESTEDANY<LavaDECL> MenuDECL;
    NESTEDANY<TIteration> IterOrig;

    SynFlags BasicFlags, IoSigFlags, IterFlags;
    STRING WidgetName;
    TLength Length;
    TSigDef SigDef; /* used only in signatures */
    //CHAINANY<TSigRef> SigRefs;
    CHAINANY<TAnnoEx> AnnoEx;
    TEmphasis Emphasis;

    unsigned Space;
    int TabPosition; /* -1 = "undefined" */
    int FrameSpacing; /* -1 = "undefined" */
    TIndentation IndType;
    int Indentation;

    TJustification JustType;

    TAlignment Alignment;
    /* alignment of corresponding "Atomic" sub-components of the annotated
       component on successive lines */

    TBox Box;
    /* controls the insertion of horizontal/vertical lines arround the
       respective syntactic object */

    /* default values: */
    STRING StringValue;
    CASE TBasicType BType OF
    B_Bool:
        bool B;

    | Enumeration:
        unsigned D;

    | Integer:
        int I;

    ELSE

    END;
    STRING String1, String2; //String1: chain insert button text/label font string/push button pixmap file name,
                             //String2: delete button text/text font string

    TAnnotation ();
    CHETAnnoEx* GetAnnoEx(AnnoExType type, bool makeIt=false);
    void DelAnnoEx(AnnoExType type);
 };


  struct LavaDECL {
    STRING-CDP FullName; //the own qualified name
    STRING LocalName;
    CHAINX-CDP /*CLavaError*/ DECLError1;
    CHAINX-CDP /*CLavaError*/ DECLError2;
    NESTED<TDECLComment> DECLComment;
    SynFlags TreeFlags;  //used in TreeView
    SynFlags-CDP WorkFlags; //temporary used flags
    int OwnID;           // OwnID is the own TypeID
    int-CDP inINCL;      // inINCL is used for table management, it indicates the simplesyntax index where decl stands
    LavaDECL-- *ParentDECL; //the upper LavaDECL from which this is available
    TVElems-CDP VElems;        //Virtual table used in interfaces
    TBasicType-CDP fromBType;  //used in std.lava to indicate the coresponding basic type

    NESTED<TAnnotation> Annotation;

    SynFlags TypeFlags;
    SynFlags SecondTFlags;
    TID RefID;  // Forms (FormDef, GUI-Interface and GUI-Impl): ID of class
                // Attributes: ID of the type or type parameter
                // Virtual Type: ID of type
                // BasicTypes: ID of interface in std.lava
                // Set- Get function: id of property
    TDeclType DeclType;
    TOperator op;  // Function is operation
    int nInput;  //function: (1-based)  count of inputs
                 //IAttr and OAttr: (0-based) pos of the IAttr/OAttr in IAttrs and OAttrs
    int nOutput; //function: (1-based)  count of outputs
		             //component specification: case of component protocol
    TSupports Supports;  // extended/implemented interfaces,
                         // extended/overwritten/implemeted functions,
                         // implemented service property (the set and get function)
                         // overwritten member/overwritten in/out-parameter
    TSupports Inherits;    //  throwables
    CHAINANY<EnumSelId> Items; /* enum items, component IDs */
    CASE TDeclDescType DeclDescType OF
      BasicType:
         TBasicType BType;
    || EnumType:  //EnumType has also NestedDecls
        NESTEDANY<TEnumDescription> EnumDesc;

    || StructDesc:
        CHAINX /*LavaDECL*/ NestedDecls;

    | LiteralString:
        STRING LitStr;

    | ExecDesc:
      NESTEDANY<SynObjectBase> Exec;

     ELSE

      END;
    int GUISignaltype;   //0=value changed, 1=chain insert; 2=chain delete
    CHAINANY <TIDs> HandlerClients;
    TIDs --ImplIDs;         //  LavaPE only
    //run time infos
    LavaDECL--* RuntimeDECL;  // used at run time:
                                 // Interface, component specification: implementation decl,
                                 // Function: def and impl: the exec decl or the adapter function address
                                 // GUI-Impl: Interface decl
                                 // Attr, Property, VirtualType: TypeDECL
    LavaDECL--* RelatedDECL;  // used at run time:
                                 //Attr, VirtualType in FormDef: related Attr in Interface
                                 //GUI-Impl and FormDef: related Interface decl
                                 //Property: set-exec-decl
                                 //set exec decl: get-exec-decl
					                       //component specification
                                 // and component implementation: assembly interface
    address-- runTimeData;    // used at run time:
                                 // callback receiver list if sender is a class
    CSecTabBase-- *SectionTabPtr;     //used at run time in implementations and interfaces
    int-CDP nSection;    //used at run time only:
                  //   interfaces, namespaces: (1 based) count of sections of class/namespace
                  //   native functions: (0-based) pos in native function table
    int-CDP SectionInfo1;
                  //at LavaPE-time: holds the top of a topleft of a popupshell
                  //at run time:
                  // functions, attributes, pattern params: (0-based) pos in section
                  // IAttr and OAttr (0-based) pos in IAttr-section/OAttr-section
                  // implementations and interfaces: (1-based) count of functions in own section
    int-CDP SectionInfo2; //used
                  //at LavaPE-time:  holds the left of a topleft of a popupshell
                  // functions, attributes, virtual types: (0-based) index of section containing this function or attribute or virtual type
                  // implementations and interfaces: (1-based) count of attributes in own section
                  // -> length of section in object = SectionInfo2 +2
    int-CDP SectionInfo3;
                  //used at run time only:
                  // interfaces and namespaces: (1-based) count of virtual types in own section

    LavaDECL();
    ~LavaDECL();

    //void DeleteContents();

    bool isInSubTree(LavaDECL * ancestor);
    bool usableIn(LavaDECL * decl);
    bool HasDefaultInitializer();
//    bool usableInA(LavaDECL * decl);
    void CompleteFullName();
    void ResetCheckmarks();
    int GetAppendPos(TDeclType declType);
};


  struct TEnumDescription {
  LavaDECL EnumField, //BasicType, Enumeration
                 MenuTree;  // only in Forms used

  };

  struct TIteration {
    TBoundRel BoundType;
    unsigned Bound;
    NESTED<LavaDECL> IteratedExpr;
  };

  struct SimpleSyntax {
    int-CDP clipIncl;
    bool-CDP notFound;  //used in LavaPE
    bool-CDP selINCL;   //used in LavaPE
    bool-CDP inWork;
    int nINCL;
    STRING UsersName;
    STRING-CDP SyntaxName;
    STRING LocalTopName;
    NESTED<LavaDECL> TopDef;

    SimpleSyntax() {clipIncl = 0; notFound = false; inWork = false; selINCL = false;}
  };

  typedef CHAINANY<SimpleSyntax> SyntaxDefinition;

  struct ClipSimpleSyntax {
    int-CDP clipIncl;   //used in LavaPE
    bool-CDP notFound;   //used in LavaPE
    bool-CDP inWork;
    int nINCL;
    STRING-CDP UsersName;
    STRING SyntaxName;
    STRING-CDP LocalTopName;
    NESTED<LavaDECL>-CDP TopDef;

    ClipSimpleSyntax() {clipIncl = 0; notFound = false;}
  };

  typedef CHAINANY<ClipSimpleSyntax> ClipSyntaxDefinition;

  struct SynObjectBase {
    virtual void MakeTable(address /*table*/, int /*inINCL*/, SynObjectBase* /*parent*/, TTableUpdate /*update*/, address where=0, CHAINX *chx=0, address searchData=0) {}
    virtual void MakeExpression() {};
  };

/****************************************************************************/

// debug communication data:

enum DbgCommand {  Dbg_Nothing,
                   Dbg_Continue,     //LavaPE to Lava
                   Dbg_StopData,     //Lava to LavaPE
                   Dbg_MemberDataRq, //LavaPE to Lava
                   Dbg_MemberData,   //Lava to LavaPE
                   Dbg_StackRq,      //LavaPE to Lava
                   Dbg_Stack,         //Lava to LavaPE
                   Dbg_Exit          //LavaPE to Lava
                   };

enum StopReason {  Stop_BreakPoint,
                   Stop_NextStm,
                   Stop_NextFunc,
                   Stop_NextOp,
                   Stop_StepInto,
                   Stop_StepOut,
                   Stop_Exception,
                   Stop_Start
                   };

enum DbgContType {dbg_Cont, dbg_Step, dbg_StepFunc, dbg_StepOut, dbg_StepInto, dbg_RunTo};

    struct DDItemData  {    //Lava to LavaPE
      DDItemData() {}
      ~DDItemData() {}
      bool isPrivate;
//      bool Selected;
      bool HasChildren;
      STRING Column0;
      STRING Column1;
      STRING Column2;
      CHAINX Children; //DDItemData in LavaPE, DebugItem in Lava
    };

    struct StackData {         //Lava to LavaPE
      int SynObjID;       //crash point id
      TDeclType ExecType; //in exec type
      TID FuncID;         //of function
      CSecTabBase-- *** Stack;
      SynObjectBase-- *SynObj;
    };
    
    struct SynErr {
      TID ErrID;
      int SynObjID;
      TDeclType ExecType;
      SynErr() {SynObjID = -1;}
    };
    
    struct ProgPoint {          //LavaPE to Lava
      ProgPoint() {Skipped = false; SynObj=0;FuncDoc=0;}
      int SynObjID;       // point id
      TDeclType ExecType; // in exec type
      TID FuncID;         // of function
      bool Activate;
      bool Skipped;
      SynObjectBase-- *SynObj;
      DString-- FuncDocName;
      DString-- FuncDocDir;
      address-- FuncDoc;
    };

    struct DbgStopData { //from Lava to LavaPE
      StopReason stopReason;
      int ActStackLevel;
      CHAINANY <StackData> StackChain;
      CHAINX /*DDItemData*/ ObjectChain;
      CHAINX /*DDItemData*/ ParamChain;
      NESTED <SynErr> SynErrData;
      CSecTabBase-- *** CalleeStack;
      LavaDECL-- * CalleeFunc;
      DbgStopData() {ActStackLevel=0; CalleeStack =0; CalleeFunc =0;}
    };

    struct DbgContData { //from LavaPE to Lava
      bool ClearBrkPnts;
      CHAINANY <ProgPoint> BrkPnts;
      DbgContType ContType;
      NESTED <ProgPoint> RunToPnt;

      DbgContData() {ClearBrkPnts = false;}
    };

    typedef CHAINANY <int> ChObjRq;

    struct DbgMessage0 {
        CASE DbgCommand Command OF
        Dbg_StopData, Dbg_Stack:
            NESTEDANY <DbgStopData> DbgData; //Lava to LavaPE
        | Dbg_MemberData:
            NESTEDANY0 <DDItemData> ObjData; //Lava to LavaPE
        | Dbg_MemberDataRq:
            NESTED <ChObjRq> ObjNr;           //LavaPE to Lava
            bool fromParams;
        | Dbg_StackRq:
            int CallStackLevel;               //LavaPE to Lava
        | Dbg_Continue:
            NESTEDANY <DbgContData> ContData; //LavaPE to Lava
        ELSE
        END;

        void SetSendData(DbgCommand command, DbgStopData* data) //Lava to LavaPE
        {Command = command;
         DbgData.ptr = 0;
         DbgData.ptr = data;
         }

        void SetSendData(DDItemData* obj) //Lava to LavaPE
        { Command = Dbg_MemberData;
          ObjData.ptr = obj;
         }

        DbgMessage0(DbgCommand com) { Command = com; }
        DbgMessage0() { Command = Dbg_Nothing; }

        void Destroy() {
         DbgData.ptr = 0;
         ObjData.ptr=0; //it is also part of DbgData, therefor no Destroy()
         ObjNr.Destroy();
         ContData.Destroy();
        }
    };

    struct DbgMessage {
        CASE DbgCommand Command OF
        Dbg_StopData, Dbg_Stack:
            NESTEDANY <DbgStopData> DbgData; //Lava to LavaPE
        | Dbg_MemberData:
            NESTEDANY0 <DDItemData> ObjData; //Lava to LavaPE
        | Dbg_MemberDataRq:
            NESTED <ChObjRq> ObjNr;           //LavaPE to Lava
            bool fromParams;
        | Dbg_StackRq:
            int CallStackLevel;               //LavaPE to Lava
        | Dbg_Continue:
            NESTEDANY <DbgContData> ContData; //LavaPE to Lava
        ELSE
        END;


        DbgMessage(DbgCommand com) {Command = com;}
        DbgMessage() {}
    };

}

/*************************************************************************/
/* Concrete syntactic objects: */

struct CHEFormNode;

class LAVABASE_DLL CContext
{
public:
  SynFlags ContextFlags;
  LavaDECL* iContext;
  LavaDECL* oContext;
  CContext() {iContext = 0; oContext = 0;}
  CContext(LavaDECL* iC, LavaDECL* oC) {iContext = iC; oContext = oC;}
};

$TYPE {
  enum TFontCase {ownFont, parentFont, mainFont};

  struct Field {
    /*CWnd void*/ QWidget *widget, *frameWidget, *popupShell; //, *basePane;
    unsigned borderWidth, leng, visible; //, beg
    CHEFormNode *up; //, *fieldNode;
    bool poppedUp;   //, correct, modified;
  };

  typedef address SigNodePtr;
  
  struct HandlerInfo {
    CHEFormNode *HandlerNode;
    TID HandlerID;
  };
  
  typedef CHAINANY<HandlerInfo> HandlerInfos;

  struct FormNode {
    Field FIP;
    LavaDECL *FormSyntax;
    LavaDECL *EnumField;
    NESTED0<TAnnotation> Annotation;
    TFontCase ownLFont;
    QFont LFont;
    TFontCase ownTFont;
    QFont TFont;
    QPixmap* Pixmap;
    bool ColorBValid;
    bool ColorFValid;
    QColor ColorB;
    QColor ColorF;
    STRING WidgetName;
    bool allowOwnHandler;
    bool allowChainHandler;
    CHEFormNode *myHandlerNode;
    CHEFormNode *handlerShowNode; //only LavaPE
    bool handlerSearched;
    CHAINANY<HandlerInfo> myHandler;
    LavaDECL* HandlerDECL;
    CSecTabBase** GUIService;
    TIDs myName;
    CHAINANY0<SigNodePtr>-- SigNodes;
    SynFlags BasicFlags, IoSigFlags, IterFlags;
    STRING StringValue;  //String
    bool Atomic;
    TBasicType BType;
    unsigned D;  //Enumeration
    int I;       //Integer
    bool B;      //B_Bool
    float F;
    double Db;
    char Ch;     //Char
//    CContext nextContext;         //context for members of the object in ResultVarPtr
    CSecTabBase*** ResultVarPtr;  //LavaVariablePtr
    CSecTabBase** HandleObjPtr;   //LavaObjectPtr
    CHAINANY<FormNode> SubTree;
  FormNode () { ResultVarPtr = 0; HandleObjPtr = 0; EnumField = 0;
                ownLFont=mainFont; ownTFont=mainFont; Pixmap = 0;
                allowOwnHandler = false; GUIService = 0;
                allowChainHandler = false; HandlerDECL = 0;
                handlerSearched = false; myHandlerNode = 0; handlerShowNode = 0;
                ColorBValid = false; ColorFValid = false;}
  ~FormNode();
  unsigned GetLengthField();
  unsigned GetLengthDecPoint();
};
}
Q_DECLARE_METATYPE(CHEFormNode*)

extern LAVABASE_DLL LavaDECL *NewLavaDECL();
extern LAVABASE_DLL bool RemoveErrCode(CHAINX* errors, QString* ids);
                      //returns true if removes an error

extern LAVABASE_DLL TAnnotation *NewTAnnotation();
extern LAVABASE_DLL void CDPLavaDECL (PutGetFlag pgf, ASN1* cid, address varAddr, bool baseCDP);
#endif
