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
  OP_high
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

  class CLavaError {
  public:
    QString* IDS;
    STRING textParam;
    bool showAutoCorrBox;
    CLavaError() {showAutoCorrBox=false;}
    CLavaError(CHAINX* errors, QString* ids, DString *text=0, bool autoC = false);

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
		B_LavaException,
    B_Callback,
    Identifier //this must be the last
  };

  enum TreeFlag {
    isExpanded,    
    //0, 1
    ItemsExpanded, 
    //1, 2
    ParaExpanded,  
    //2, 4
    InExpanded,    
    //3, 8
    OutExpanded,   
    //4, 16
    DefsExpanded,  
    //5, 32
    MemsExpanded,  
    //6, 64
    hasEmptyOpt,    
    //7, 128
    leftArrows,
    //8, 256 in exec: assignment arrows from right to left

    FREE_FLAG9,
    FREE_FLAG10,
    FREE_FLAG11,
    FREE_FLAG12,
    FREE_FLAG13,
    FREE_FLAG14,
    FREE_FLAG15,
    FREE_FLAG16,
    FREE_FLAG17,
    dragOverrides,  //overrides on clipboard
    //18,  
    mixedDrag,
    //19
    dragVT,   //VirtualType on clipboard 
    //20
    dragInt,  //Interface on clipboard 
    //21
    dragImpl,  //Implementation on clipboard 
    //22
    dragIni,      //Initiator on clipboard 
    //23
    dragFuncDef,  //Function on clipboard 
    //24
    dragFuncImpl, //Function on clipboard 
    //25
    dragPack, //Package on clipboard 
    //26
    dragCompo,//Component on clipboard 
    //27
    dragCOS,  //CompObjSpec on clipboard 
    //28
    dragCO,   //CompObj on clipboard 
    //29
    dragInput, //input (from initiator) on clipboard
    //30
    ShowExecComments
    //31
  };  


  enum TypeFlag {
    isOptional,
    // 0/1, optional input or output or attribute
    isStatic,
    // 1 , static function
		thisComponent,
		//2/4, the internal interface of the component object
    isComponent,
    // 3/8, Component object interface
    substitutable,
    // 4/16 substitutable type (C- and P-derivations) of Attr, IAttr or OAttr
    isProtected,
    // 5/32, protected attr/function or opaque namespace
    isPlaceholder,
    // 6/64, an attribute defines a placeHolder
    isConst, 
    // 7/128,  //const function or property
    thisCompoForm, 
    // 8/256, the internal interface of the component objects GUI
    isAbstract,
    // 9/512, abstract function (always virtual abstract) 
    //        or abstract pattern parameter or non-creatable interface
    forceOverride, //
    //10  1024 override function if class is extended
    isPropGet,
    //11/2048, get property function in a service implementation, the RefID is the ID of the property
    isPropSet,
    //12/4096, set property function in a service implementation, the RefID is the ID of the property
    Free_FLAG13,
    //13/8192
    constituent, 
    //14/16384 an attribute defines a down link
    acquaintance,
    //15/32768 an attribute defines a  'follow on copy link'
    isInitializer,
    //16/ initializer function
    isPersistent,
    //17/ persistent component
    isGUIEdit,
    //18/ the edit show function
    isGUI,
    //19/ form service, interface and implementation
    // and its (not removable/changeable) input and output
    sameAsSelf, 
    //20 IO parameter has same category as "self"
    isNative,
    //21         Function or interface has native implementation
    defaultInitializer,
    //22         Default initializer
    isTransaction, 
    //23 Function or initiator is transaction
    execConcurrent,
    //24 Concurrent function or initiator
    execIndependent,
    //25 Independent function or initiator
    copyOnAccess,
    //26 
    consumable,
    //27
    stateObject,  //attribute references a state object
    //28 
    hasSetGet,    //attribute has set and get functions
    //29
    definesObjCat, //pattern parameter defines object category
    //30
    trueObjCat    //attribute/IO-category is defined
    //31
  };//TypeFlag


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
    //9, 512, overriding function, virtual type or member does not inherit the name of the overridden
    overrides,
    //10, 1024 member or function overrides member or function of extended interface
    funcImpl,
    //11, 2048 function implementation
    inComponent,
    //12 4096 Used only in clipboard-data for drag/drop and paste:
    //    All Attr-, IAttr- and OAttr-types are structures or component interfaces
    isEnum, 
    //13, 8192 the LavaDECL defines an Enumeration or an interface which extends an Enumeration 
    isSet,
    //14, 16384 the LavaDECL defines a set or an interface which extends a set
    //    or is the set element virtual type
    isChain,
    //15, 32768 the LavaDECL defines a chain or an interface which extends a chain
    //    or is the chain element virtual type
    isArray,
    //16, the LavaDECL defines an array or an interface which extends a array
    //    or is the array element virtual type
    isCallback,
    //17  the decl extends Callback or the callback virtual type
    isEventDesc,
    //18 the decl is the event descrption virtual type of a callback pattern
    isEventSpec,
    //19 the decl is the event specification virtual type of a callback pattern
	  isException
		//20, the LavaDECL defines an exception or an interface which extends an exception
    //    or is the exception enumeration virtual type
	};
	  
  enum WorkFlag {
    isPattern,
    //0, the Package or Interface has pattern parameter
    isPartOfPattern,
    //1, the Interface is part of a pattern
    runTimeOK,
    //2, run time checked
    recalcVT,
    //3, recalculate the vitual table after some changes
    flagDrag,
    //4, used in drag and drop operations
    checkmark,
    //5, temporary used for consistency checks
    selAfter,
    //6, select the tree item of this decl after update
    selEnum,
    //7, select the Enumeratio section node
    selPara,
    //8, select the param section node
    selIn,
    //9, select the input section node
    selOut,
    //10, select the output section node
    selDefs,
    //11, select the definitions section node
    selMems,
    //12, select the features section node
    implRequired,
    //13, interface must have an implementation
    hasDefaultIni,
    //14, interface has a default initializer
    allowDEL,
    //15, allow function implementation to be deleted
    nonEmptyInvariant,
    //16, invariant was non-empty
    isReferenced,
    //17, formal parameter is referenced in exec body
    skipOnCopy,
    //18, used in refactoring: skipped copy actions if move from public to private
    skipOnDeleteID,
    //19, used in refactoring: no delete of local id's if move from public to private
    fromPrivToPub,
    //20, used in refactoring: private to public
    ImplFromBaseToEx,
    //21, used in refactoring: base to extension
    SupportsReady,
    //22, Supports are already from drop doc in clipEl
    poppedUp,
    //23, used in LavaGUI: pop up after rebuilding the form
    formVTOK,
    //24, used at runtime
	newTreeNode,
	//25, used in updates
    fromPubToPriv
    //26, used in refactoring: public to private
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
    Component,
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
                // PatternParam: ID of type
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
    || EnumType:  //EnumType erhält NestedDecls
        NESTEDANY<TEnumDescription> EnumDesc;

    || StructDesc:
        CHAINX /*LavaDECL*/ NestedDecls;

    | LiteralString:
        STRING LitStr;
    
    | ExecDesc:
      NESTEDANY<SynObjectBase> Exec;

     ELSE

      END;
    TIDs-CDP ImplIDs;         //  LavaPE only
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
    bool-CDP notFound;   //used in LavaPE
    bool-CDP inWork;
    int nINCL;
    STRING UsersName;
    STRING-CDP SyntaxName;
    STRING LocalTopName;
    NESTED<LavaDECL> TopDef;

    SimpleSyntax() {clipIncl = 0; notFound = false; inWork = false;}
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
                   Stop_Exception
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

    struct DbgMessage {
        CASE DbgCommand Command OF
        Dbg_StopData, Dbg_Stack:
            NESTEDANY0 <DbgStopData> DbgData; //Lava to LavaPE
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

        DbgMessage(DbgCommand com) {Command = com;}
        DbgMessage() {}

        void Destroy() {
         DbgData.ptr = 0;
         ObjData.ptr=0; //it is also part of DbgData, therefor no Destroy()
         ObjNr.Destroy();
         ContData.Destroy();
        }
    };

}

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

$TYPE {
  enum TFontCase {ownFont, parentFont, mainFont};

  struct Field {
    /*CWnd void*/ QWidget *widget, *frameWidget, *popupShell; //, *basePane;
    unsigned borderWidth, leng, visible; //, beg
    CHEFormNode *up; //, *fieldNode;
    bool poppedUp;   //, correct, modified;
  };
 
  typedef address SigNodePtr;

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
                ownLFont=mainFont; ownTFont=mainFont; Pixmap = 0; ColorBValid = false; ColorFValid = false;}
  ~FormNode();
  unsigned GetLengthField();
  unsigned GetLengthDecPoint();
};
}

extern DISCO_DLL LavaDECL *NewLavaDECL();
extern DISCO_DLL bool RemoveErrCode(CHAINX* errors, QString* ids);
                      //returns true if removes an error

extern DISCO_DLL TAnnotation *NewTAnnotation();

#endif
