/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
	 (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "LavaBase_all.h"
#include "BASEMACROS.h"

#include "qobject.h"
#include "qstring.h"
#include "LavaBaseStringInit.h"

#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////
//
// String Table
//
QString IDS_LAVACOM_FILE;
QString ERR_One_must_remain;
QString ERR_No_input;
QString ERR_const_expected;
QString ERR_Odd_char;
QString ERR_Var_expected;
QString ERR_Wrong_spcl_char;
QString ERR_Apostr_missing;
QString ERR_QMark_missing;
QString ERR_Wrong_hex;
QString ERR_HexTooLong;
QString ERR_OverOrUnderflow;
QString ERR_Wrong_oct;
QString ERR_Wrong_exp;
QString ERR_AssignInQuery;
QString ERR_AssignInROClause;
QString ERR_AssignInPrePostInv;
QString ERR_Broken_ref;
QString ERR_IsntSet;
QString ERR_MissingParms;
QString ERR_RedundantParms;
QString ERR_IncompatibleType;
QString ERR_IncompatibleTypeTargetIsVT;
QString ERR_NotSingleOutput;
QString ERR_ExInLogNot;
QString ERR_NoOutputsAllowed;
QString ERR_UndefRef;
QString ERR_MissingViewAttr;
QString ERR_RedundantExtracts;
QString ERR_MissingExtracts;
QString ERR_SelfIsReserved;
QString ERR_NoViewImpl;
QString ERR_OperatorUndefined;
QString ERR_UndefinedSetElemType;
QString ERR_InadmissibleThrow;
QString ERR_ArrOperatorUndefined;
QString ERR_ExInIfCond;
QString ERR_NotYetImplemented;
QString ERR_AssigToInput;
QString ERR_AssigToSelf;
QString ERR_EventInputRequired;
QString ERR_Placeholder;
QString ERR_ExecutionFailed;
QString ERR_EmptyExec;
QString ERR_AssertionViolation;
QString ERR_InvariantViolation;
QString ERR_PreconditionViolation;
QString ERR_PreconditionConsistency;
QString ERR_PostconditionViolation;
QString ERR_PostconditionConsistency;
QString ERR_NoErrors;
QString ERR_ExecError;
QString ERR_DLLError;
QString ERR_AssigToFrozen;
QString ERR_NoMemberFunction;
QString ERR_SingleAssViol;
QString ERR_PrevDescAssig;
QString ERR_PrevAncestAssig;
QString ERR_Optional;
QString ERR_IfdefMissing;
QString ERR_Closed;
QString ERR_ShouldBeClosed;
QString ERR_ClosedVar;
QString ERR_CallObjClosed;
QString ERR_SelfUnfinished;
QString ERR_SelfNotClosed;
QString ERR_SelfVirtual;
QString ERR_NotSelfVT;
QString ERR_TempUnfinished;
QString ERR_ElseExprObsolete;
QString ERR_SuperfluousMandatory;
QString ERR_AssigToLocal;
QString ERR_NameInUse;
QString ERR_OutOfScope;
QString ERR_NoAbstract;
QString ERR_SingleAssignment;
QString ERR_FuncParmOutOfScope;
QString ERR_SelfInStatic;
QString ERR_UndefType;
QString ERR_CallExprUndefType;
QString ERR_NoCompObjItf;
QString ERR_ReservedName;
QString ERR_Parameter;
QString ERR_NullException;
QString ERR_MissingInitialization;
QString ERR_AssigToRdOnly;
QString ERR_AssigToMemberOfFrozen;
QString ERR_RdOnlyFunc;
QString ERR_OverriddenDiffs;
QString ERR_UndefBaseItf;
QString ERR_ImplUndefItf;
QString ERR_BaseInit;
QString ERR_MissingBaseInits;
QString ERR_FuncImplToLocal;
QString ERR_EnterValue;
QString ERR_SelectValue;
QString ERR_WrongCopyTarget;
QString ERR_NoExecBody;
QString ERR_AssigInForeach;
QString ERR_NestedVTs;
QString ERR_VTOutOfScope;
QString ERR_StdNotRemovable;
QString ERR_Protected;
QString ERR_InvisibleType;
QString ERR_InvisibleBasictype;
QString ERR_VirtualCircle;
QString ERR_InvalidValOfVT;
QString ERR_SelfUnfinishedCallObj;
QString ERR_SelfUnfinishedParm;
QString ERR_NoInitializer;
QString ERR_InitializerForbidden;
QString ERR_ExplicitInitializerCall;
QString ERR_NoSignal;
QString ERR_SignalNoOutput;
QString ERR_MissingFuncDecl;
QString ERR_MissingItfFuncDecl;
QString ERR_UnaryOpUndefined;
QString ERR_HandleOpParm;
QString ERR_UseVT;
QString ERR_WrongContext;
QString ERR_ImmutableCallObj;
QString ERR_NonROCallInROClause;
QString ERR_SuccFailNotLast;
QString ERR_NoNativeImpl;
QString ERR_NoFuncImpl;
QString ERR_NoClassFunc;
QString ERR_CorruptForm;
QString ERR_CorruptForm2;
QString ERR_NotBaseIF;
QString ERR_NoOverridden;
QString ERR_NoClassImpl;
QString ERR_IOParams;
QString ERR_SetGetError;
QString ERR_funcImpl;
QString ERR_NoImplIF;
QString ERR_NoSetElemType;
QString ERR_CommonContext;
QString ERR_OverriddenStatic;
QString ERR_SubstInContext;
QString ERR_VBaseWithoutDefaultIni;
QString ERR_NoImplClass;
QString ERR_ImplOfAbstract;
QString ERR_CleanSupports;
QString ERR_OverInOtherCon;
QString ERR_NoDefaultIni;
QString ERR_CopyToSelf;
QString ERR_NoIFforForm;
QString ERR_NoBaseFormIF;
QString ERR_ClassCircle;
QString ERR_Unused;
QString ERR_DoubleVal;
QString ERR_OverriddenIOType;
QString ERR_SecondImpl;
QString ERR_inStd;
QString ERR_SecondDefaultIni;
QString ERR_IncompWithTargetVT;
QString ERR_SlotHasOutput;
QString ERR_SlotWrongArg;
QString ERR_SlotTooManyArgs;
QString ERR_SlotTooFewArgs;
QString ERR_IncompatibleCategory;
QString ERR_SetIsFrozenValObj;
QString ERR_EnumIntvType;
QString ERR_NonCreatable;
QString ERR_AbstractInherited;
QString ERR_IncompAbstractVT;
QString ERR_CallExprRequired;
QString ERR_CorruptForm3;
QString ERR_CorruptObject;
QString ERR_AbstrMemType;
QString ERR_IfxForbidden;
QString ERR_MissingExec;
QString ERR_NoAbstractForm;
QString ERR_NoPicture;
QString ERR_ItfNotInCOS;
QString ERR_NoCompoSpec;
QString ERR_noCompoInterf;
QString ERR_ldocIncompatible;
QString ERR_ldocNotOpened;
QString ERR_ldocNotStored;
QString ERR_ExactlyOneLcom;
QString ERR_AllocObjectFailed;
QString ERR_ImmutableArray;
QString ERR_ForceOver;
QString ERR_ForceOverInValOfVT;
QString ERR_ForceOverCalled;
QString ERR_IsntEnum;
QString ERR_AssigToMultiArray;
QString ERR_FaultyArrayObj;
QString ERR_HasNoDftIni;
QString ERR_CompoNotFound;
QString ERR_MustThrowExc;
QString ERR_OIDrequired;
QString ERR_URLrequired;
QString ERR_NullCallObject;
QString ERR_NullMandatory;
QString ERR_IntegerRange;
QString ERR_EnumOrdLow;
QString ERR_EnumOrdHigh;
QString ERR_RunTimeException;
QString ERR_OneEnum;
QString ERR_MissingExceptionDef;
QString ERR_NoException;
QString ERR_OverThrow;
QString ERR_SuccFailNotNot;
QString ERR_FailNoException;
QString ERR_InadmissibleCategory;
QString ERR_ElemNotInSet;
QString ERR_ArrayXOutOfRange;
QString ERR_SwitchMissingElse;
QString ERR_CanceledForm;
QString ERR_NoSetGetMember;
QString ERR_SucceedNeedless;
QString ERR_CopyNullToNonNull;
QString ERR_ZombieAccess;
QString ERR_CallObjInStatic;
QString ERR_UnfinishedObject;
QString ERR_NullParent;
QString ERR_NotReferenced;
QString ERR_URLmustBeString;
QString ERR_lcomNotOpened;
QString ERR_OutFunctionFailed;
QString ERR_Private;
QString ERR_NoRefType;
QString ERR_NoBaseType;
QString ERR_NoElType;
QString ERR_NoBaseIF;
QString ERR_NoFiredIF;
QString ERR_NoExtForm;
QString ERR_InVTofBaseIF;
QString ERR_InVT;
QString ERR_ParIncompatible;
QString ERR_MemIncompatible;
QString ERR_AmbgInVT;
//QString ERR_NameCollision;
QString ERR_ErrInForm;
QString ERR_MissingBrowserPath;
QString ERR_BrowserStartFailed;
QString ERR_ResolvingShortcutFailed;
QString ERR_LavaStartFailed;
QString ERR_LavaPEStartFailed;
QString ERR_SaveFailed;
QString ERR_LookForCompoObj;
QString ERR_LookForCompoObjF;
QString ERR_NoImplForAbstract;
QString ERR_LocalOrOutputVarInOld;
QString ERR_OneLibName;
QString ERR_CompObjIntfNotFound;
QString ERR_CallCheck_NYI;
QString ERR_clone_copy_inp_closed;
QString ERR_Broken_ref_in_HC;
QString ERR_NoHandlerIO;
QString ERR_IgnoreInputsOnly;
QString ERR_IgnoreMandatoryOnly;
QString ERR_RefToIgnored;
QString ERR_AlreadyIgnored;
QString ERR_SetLavaExceptionFailed;
QString ERR_AbortQuestion;
QString ERR_ExecutionAborted;
QString ERR_NotSingleContainer;
QString ERR_FinalVTisOverridden;

void LavaBaseStringInit() {
  IDS_LAVACOM_FILE  = QObject::tr("Lava component file (*.lcom) defining the type of a lava component object");
  ERR_const_expected = QObject::tr("Constant expected");
  ERR_Odd_char  = QObject::tr("Odd character");
  ERR_Var_expected  = QObject::tr("Variable name is expected");
  ERR_Wrong_spcl_char  = QObject::tr("Wrong special character");
  ERR_Apostr_missing  = QObject::tr("Closing ' expected");
  ERR_QMark_missing  = QObject::tr("Closing "" expected");
  ERR_Wrong_hex  = QObject::tr("Error in hexadecimal constant");
  ERR_HexTooLong  = QObject::tr("Hexadecimal constant too long");
  ERR_OverOrUnderflow = QObject::tr("Over- or underflow: Exponent to big");
  ERR_Wrong_oct  = QObject::tr("Error in octal constant");
  ERR_Wrong_exp  = QObject::tr("Wrong or incomplete exponent");
  ERR_AssignInQuery  = QObject::tr("\"Read-only\" exec: assignments are allowed only to variables that do not yet count as conclusively initialized");
  ERR_AssignInROClause  = QObject::tr("Forbidden assignment in read-only clause");
  ERR_AssignInPrePostInv  = QObject::tr("Pre/postcondition or invariant: assignments are allowed only to declare variables");
  ERR_Broken_ref  = QObject::tr("Broken reference");
  ERR_IsntSet  = QObject::tr("This should be a set");
  ERR_MissingParms  = QObject::tr("Missing parameter(s)");
  ERR_RedundantParms  = QObject::tr("Redundant parameter(s)");
  ERR_IfxForbidden  = QObject::tr("If-expression not allowed here");
  ERR_MissingExec  = QObject::tr("Missing function body");
  ERR_NoAbstractForm  = QObject::tr("Do not use a GUI service of a non creatable type ");
  ERR_NoPicture  = QObject::tr("Creation of the picture of the lava object failed");
  ERR_ItfNotInCOS  = QObject::tr("Interface not contained in component object specification");
  ERR_NoCompoSpec  = QObject::tr("The component specification to be implemented by this component object is missing");
  ERR_noCompoInterf  = QObject::tr("The corresponding internal interface of this component object is missing");
  ERR_ldocIncompatible  = QObject::tr("Cannot attach lava object: type and object incompatible");
  ERR_ldocNotOpened  = QObject::tr("Can't read Lava document (*.ldoc) file");
  ERR_ldocNotStored  = QObject::tr("Cannot store the lava object");
  ERR_ExactlyOneLcom  = QObject::tr("Exactly one name of a  *.lcom file or link to a *.lcom file expected");
  ERR_AllocObjectFailed  = QObject::tr("Object allocation failed");
  ERR_ImmutableArray  = QObject::tr("This array is a frozen, immutable value object");
//  ERR_ThrowInInitiator  = QObject::tr("In main programs, ""fail"" must not throw a non-null exception ");
  ERR_SecondImpl  = QObject::tr("Interface with more than one implementation");
  ERR_inStd  = QObject::tr("std.lava corrupted");
  ERR_SecondDefaultIni  = QObject::tr("Second default initializer");
  ERR_IncompWithTargetVT  = QObject::tr("Incompatible with virtual type of assignment target or set elements (""force override"" at function or ""final"" at VT missing?)");
  ERR_SlotHasOutput  = QObject::tr("Callback functions must not have output parameters");
  ERR_SlotTooManyArgs  = QObject::tr("Callback function has more parameters than signal function");
  ERR_SlotTooFewArgs  = QObject::tr("Callback function has fewer parameters than signal function");
  ERR_SlotWrongArg  = QObject::tr("Argument #%1 of callback function incompatible to corresponding argument of signal function");

  ERR_IncompatibleCategory  = QObject::tr("Incompatible object categories: target cat. must be \"any\" (*), or else target cat. = source cat.");
  ERR_SetIsFrozenValObj  = QObject::tr("Forbidden insertion: result set is a frozen value object");
  ERR_EnumIntvType  = QObject::tr("Type restriction not allowed for enumerated and interval sets");
  ERR_NonCreatable  = QObject::tr("\"declare\" with \"initialize\" clause requires a \"creatable\" class here");
  ERR_AbstractInherited  = QObject::tr(": Creatable class inherits abstract feature or virtual type ");
  ERR_IncompAbstractVT  = QObject::tr("Assignment source has incompatible abstract virtual type");
  ERR_CallExprRequired  = QObject::tr("This function is no longer static; a call expression is required");
  ERR_CorruptForm3  = QObject::tr("Member type in form is not a form of member type in class");
  ERR_CorruptObject  = QObject::tr("Cannot read lava object with this lava component program");
  ERR_AbstrMemType  = QObject::tr("Do not use an abstract type for a member or formal parameter in a creatable class");
  ERR_PrevDescAssig  = QObject::tr("Single assignment violation: replacement of an ancestor; press the ""conflict button"" (red and blue left arrows) to see the conflicting previous assignment");
  ERR_PrevAncestAssig  = QObject::tr("Single assignment violation: overwrites a descendant; press the ""conflict button"" (red and blue left arrows) to see the conflicting previous assignment");
  ERR_IfdefMissing  = QObject::tr("Access to member function or variable of optional variable/expression isn't secured by \"ifdef\" statement or \"else\" expression");
  ERR_Optional  = QObject::tr("An expression whose value is mandatory (= always non-null) is expected here. \"ifdef\" statement or \"else\" expression missing?");
  ERR_Closed  = QObject::tr("A (possibly) not yet fully initialized object may be assigned only to a \"closed\" variable or formal parameter");
  ERR_ShouldBeClosed  = QObject::tr(" Variable to be initialized must be declared \"closed\" since initializer has \"closed\" actual input parameter(s)");
  ERR_CallObjClosed  = QObject::tr("This call object is \"closed\", so the function must have the \"self is closed\" attribute");
  ERR_ClosedVar  = QObject::tr("\"Closed\" variable: access to member variables forbidden");
  ERR_SelfUnfinished  = QObject::tr("\"self\" may be still unfinished here: member access forbidden");
  ERR_SelfNotClosed  = QObject::tr("Call object still incomplete here, so the called function must have the \"self is closed\" attribute");
  ERR_SelfVirtual  = QObject::tr("The class of the called function is the value of a virtual type. Therefore the call expression must be of this same virtual type");
  ERR_NotSelfVT  = QObject::tr("Virtual type of call expression and VT of \"self\" must be the same");
  ERR_SelfUnfinishedCallObj  = QObject::tr("""self"" must be completely initialized before its methods may be called, or the method must have the \"self is closed\" attribute");
  ERR_SelfUnfinishedParm  = QObject::tr("\"self\" is not yet completely initialized here, so the receiving formal parameter must be \"closed\"");
  ERR_ElseExprObsolete  = QObject::tr("Else-expression superfluous: first operand is already mandatory");
  ERR_SuperfluousMandatory  = QObject::tr("Superfluous mandatory variable in ifdef condition");
  ERR_AssigToLocal  = QObject::tr("Single-assignment violation: forbidden assignment to initialized local variable");
  ERR_NameInUse  = QObject::tr("This name is already in use within the current scope; please choose a different name");
  ERR_OutOfScope  = QObject::tr("Variable is referenced outside its scope");
  ERR_NoAbstract  = QObject::tr("An abstract virtual type or function cannot occur in a creatable class");
  ERR_FuncParmOutOfScope  = QObject::tr("This function parameter belongs to a different function");
  ERR_SelfInStatic  = QObject::tr("""self"" is used in an main program or static function");
  ERR_UndefType  = QObject::tr("Undefined type");
  ERR_CallExprUndefType = QObject::tr("Call expression has undefined type");
  ERR_NoCompObjItf  = QObject::tr("Must be a component object interface");
  ERR_ReservedName  = QObject::tr("Reserved name");
  ERR_Parameter  = QObject::tr("Formal/actual parameters don't match");
  ERR_NullException  = QObject::tr("Fail statement: exception parameter is null");
  ERR_MissingInitialization  = QObject::tr(": a value must be assigned in all preceding program branches (check also for missing \"else\" branches!)");
  ERR_AssigToRdOnly  = QObject::tr("Forbidden assignment to read-only member variable");
  ERR_IncompatibleType  = QObject::tr("Incompatible type");
  ERR_IncompatibleTypeTargetIsVT  = QObject::tr("Source type/actual parameter could become incompatible with a derivation of the (virtual!) target type/formal parameter");
  ERR_NotSingleOutput  = QObject::tr("Function must have exactly one output parameter");
  ERR_NoOutputsAllowed  = QObject::tr("No output parameters allowed here");
  ERR_UndefRef  = QObject::tr("Dangling (=undefined) reference");
  ERR_MissingViewAttr  = QObject::tr("View element type must have at least one attribute");
  ERR_RedundantExtracts  = QObject::tr("There are more extract objects than attributes in the view element type");
  ERR_MissingExtracts  = QObject::tr("There are fewer extract objects than attributes in the view element type");
  ERR_NoViewImpl  = QObject::tr("Invalid implementation of the select view");
  ERR_OperatorUndefined  = QObject::tr("First operand has no such operator");
  ERR_UndefinedSetElemType  = QObject::tr("Undefined set element type");
  ERR_InadmissibleThrow  = QObject::tr("The exception specification of the current function is empty or doesn\'t contain this exception type");
  ERR_ArrOperatorUndefined  = QObject::tr("No array access operator defined");
  ERR_TempUnfinished  = QObject::tr("""temp"" must not be passed as a parameter (unless to initializers) since it counts as ""still unfinished""");
  ERR_One_must_remain  = QObject::tr("At least one such item must remain!");
  ERR_No_input  = QObject::tr("An empty input is not allowed here");
  ERR_ForceOver  = QObject::tr(": Function of a base class has the force override on derivation flag and must therefore be overridden in the derived class");
  ERR_ForceOverInValOfVT  = QObject::tr(": The value-class of this virtual type in a base pattern has a function with the \"force override on derivation\" flag and must therefore be overridden in the derived pattern");
  ERR_ForceOverCalled  = QObject::tr("This function has the \"force override on derivation\" flag and must not be invoked by a static call");
  ERR_IsntEnum  = QObject::tr("Must be an enumeration type");
  ERR_AssigToMultiArray  = QObject::tr("Multi-dimensional (""nested"") arrays must be constructed layer by layer");
  ERR_FaultyArrayObj  = QObject::tr("Only variables and array refs allowed");
  ERR_HasNoDftIni  = QObject::tr("Target class of extend/copy must have a default initializer");
  ERR_CompoNotFound  = QObject::tr("Cannot attach lava object: loaded component object is not an implementation of the requested component specification");
  ERR_MustThrowExc  = QObject::tr("Initializers and functions with output parameters must succeed or throw an exception");
  ERR_OIDrequired  = QObject::tr("Object ID required");
  ERR_URLrequired  = QObject::tr("URL required for this kind of components");
  ERR_NullCallObject  = QObject::tr("Function call: Call object is null");
  ERR_NullMandatory  = QObject::tr("Null assignment to mandatory variable");
  ERR_IntegerRange  = QObject::tr("Integer range error (number too big!)");
  ERR_EnumOrdLow  = QObject::tr("Ord of enumeration item <0 forbidden");
  ERR_EnumOrdHigh  = QObject::tr("Given integer value is higher than maximum Ord of enumeration");
  ERR_RunTimeException  = QObject::tr("Lava run time system: internal exception");
  ERR_OneEnum  = QObject::tr("More then one set of enumerated items is declared in this class and/or in its base interfaces");
  ERR_NotYetImplemented  = QObject::tr("Not yet implemented");
  ERR_AssigToInput  = QObject::tr("Forbidden assignment to input variable");
  ERR_AssigToSelf  = QObject::tr("Forbidden assignment to ""self""");
  ERR_EventInputRequired  = QObject::tr("At least one input parameter required (""event descriptor"")");
  ERR_Placeholder  = QObject::tr("Incomplete program: exec/invariant contains a placeholder");
  ERR_ExecutionFailed  = QObject::tr("Execution failed");
  ERR_EmptyExec  = QObject::tr("Missing or empty exec");
  ERR_AssertionViolation  = QObject::tr("Assertion violation");
  ERR_InvariantViolation  = QObject::tr("Invariant violation");
  ERR_PreconditionViolation  = QObject::tr("Precondition violation");
  ERR_PreconditionConsistency  = QObject::tr("Precondition violation");
  ERR_PostconditionViolation  = QObject::tr("Postcondition violation");
  ERR_PostconditionConsistency  = QObject::tr("Postcondition of overridden method should be true");
  ERR_NoErrors  = QObject::tr("No errors or placeholders found in this exec");
  ERR_ExecError  = QObject::tr("Error or placeholder in executable code");
  ERR_DLLError  = QObject::tr("Native implementation DLL error");
  ERR_AssigToFrozen  = QObject::tr("Forbidden assignment to a constituent of a frozen value object");
  ERR_SingleAssViol  = QObject::tr("Single-assignment violation; press the ""conflict button"" (red and blue left arrows) to show the last conflicting previous assignment");
  ERR_NullParent  = QObject::tr("Attempt to access a member of a null object");
  ERR_NotReferenced  = QObject::tr(": Unused mandatory input parameter");
  ERR_URLmustBeString  = QObject::tr("URL must be String");
  ERR_lcomNotOpened  = QObject::tr("Cannot open the component object specification");
  ERR_OutFunctionFailed  = QObject::tr("Function with output parameters failed");
  ERR_Private  = QObject::tr("Inadmissible reference to a private class member");
  ERR_NoRefType  = QObject::tr("Missing declaration of referenced type");
  ERR_NoBaseType  = QObject::tr("Missing declaration of base class");
  ERR_NoElType  = QObject::tr("Missing declaration of element type");
  ERR_NoBaseIF  = QObject::tr("Missing declaration of a  base class");
  ERR_NoFiredIF  = QObject::tr("Missing declaration of exception or signal type");
  ERR_NoExtForm  = QObject::tr("Missing declaration of base form");
  ERR_InVTofBaseIF  = QObject::tr("Error in virtual table of a base class");
  ERR_InVT  = QObject::tr("Error in virtual table");
  ERR_ParIncompatible  = QObject::tr("Virtual type is incompatible with its original, overridden declaration");
  ERR_MemIncompatible  = QObject::tr("Incompatible type");
  ERR_AmbgInVT  = QObject::tr("Ambiguity in the virtual table");
  ERR_ErrInForm  = QObject::tr("Error in form");
  ERR_RdOnlyFunc  = QObject::tr("Read-only functions may assign only to output and declare variables (initializers also to self.member)");
  ERR_OverriddenDiffs  = QObject::tr("Overridden attribut/function/input/output/virtual type has inconsistent properties");
  ERR_ImplUndefItf  = QObject::tr("Implementation implements an undefined class");
  ERR_BaseInit  = QObject::tr("Erroneous base initializer call");
  ERR_MissingBaseInits  = QObject::tr("Missing base initializer call");
  ERR_EnterValue  = QObject::tr("Please enter value");
  ERR_SelectValue  = QObject::tr("Please select value");
  ERR_WrongCopyTarget  = QObject::tr("The target of a copy operation must be a state object, the ""self"" of an initializer, or ""temp""");
  ERR_NoExecBody  = QObject::tr("Function or main program body still empty");
  ERR_AssigInForeach  = QObject::tr("Potential multiple assignment in ""foreach"" construct");
  ERR_NestedVTs  = QObject::tr("Nested virtual types not allowed here");
  ERR_VTOutOfScope  = QObject::tr("Out of scope reference to virtual type");
  ERR_StdNotRemovable  = QObject::tr("You cannot remove std.lava");
  ERR_Protected  = QObject::tr("Inadmissible reference to a protected class member");
  ERR_InvisibleType  = QObject::tr("Forbidden reference to an invisible type (that is hidden in an implementation or opaque package)");
  ERR_InvisibleBasictype  = QObject::tr("Overridden type is invisible (= hidden in an implementation or opaque package)");
  ERR_VirtualCircle  = QObject::tr("Circular self-reference of virtual type");
  ERR_InvalidValOfVT  = QObject::tr("Virtual type has invalid value");
  ERR_NoInitializer  = QObject::tr("This isn't an initializer");
  ERR_InitializerForbidden = QObject::tr("Initializer not allowed here");
  ERR_ExplicitInitializerCall  = QObject::tr("Explicit initializer calls are not allowed");
  ERR_NoSignal  = QObject::tr("This isn't a signal function");
  ERR_SignalNoOutput = QObject::tr("Signal functions must not have output parameters");
  ERR_MissingFuncDecl  = QObject::tr("No such member function or variable:");
  ERR_MissingItfFuncDecl  = QObject::tr("No such member function in class interface:");
  ERR_UnaryOpUndefined  = QObject::tr("Operator undefined for this type of operand");
  ERR_HandleOpParm  = QObject::tr("Parameter of handle operator # must be a quantified variable");
  ERR_UseVT  = QObject::tr("Error: This or a base type is virtual; => ref. to concrete class forbidden");
  ERR_WrongContext  = QObject::tr("Type cannot be referenced from the current pattern context");
  ERR_ImmutableCallObj  = QObject::tr("This (immutable!) value object is used to call a non-read-only member function (= whose \"self\" is variable)");
  ERR_NonROCallInROClause = "Forbidden call of non-read-only function in read-only clause or exec";
  ERR_SuccFailNotLast  = QObject::tr("Succeed/throw returns control; therefore there must not follow any further statement");
  ERR_NoNativeImpl  = QObject::tr("Implementation of native function not found/no adapter table");
  ERR_NoFuncImpl  = QObject::tr("Implementation of this function not found");
  ERR_NoClassFunc  = QObject::tr("No corresponding function in class declaration");
  ERR_CorruptForm  = QObject::tr("Corrupt declaration of GUI service: missing form declaration");
  ERR_CorruptForm2  = QObject::tr("Declaration of GUI service and declaration of corresponding class do not match");
  ERR_NotBaseIF  = QObject::tr("is not base class");
  ERR_NoOverridden  = QObject::tr("Overridden feature or virtual type not found in base class ");
  ERR_NoClassImpl  = QObject::tr("Implementation of this class not found");
  ERR_IOParams  = QObject::tr("Inconsistency of function parameter (class interface versus implementation)");
  ERR_SetGetError  = QObject::tr("Set or get function of attribute not found");
  ERR_funcImpl  = QObject::tr("Inconsistent function properties (implementation versus class interface)");
  ERR_NoSetElemType  = QObject::tr("Set element type not found");
  ERR_CommonContext  = QObject::tr("Common base context of inner and outer context not allowed");
  ERR_OverriddenStatic  = QObject::tr("A static function cannot be overridden");
  ERR_SubstInContext  = QObject::tr("Substitutable type must not belong to the current context or a context that has a base context in common with the current context");
  ERR_VBaseWithoutDefaultIni  = QObject::tr("A virtual base class needs a default initializer");
  ERR_NoImplClass  = QObject::tr("The class to be implemented by this implementation is missing");
  ERR_ImplOfAbstract  = QObject::tr("An abstract function cannot have an implementation");
  ERR_CleanSupports  = QObject::tr("Redundant entry in the list of base types");
  ERR_OverInOtherCon  = QObject::tr("You cannot extend a class that has been declared in another context");
  ERR_NoDefaultIni  = QObject::tr("Default initializer missing");
  ERR_CopyToSelf  = QObject::tr("Copy onto self permitted only from a source expression of the same or a more derived type");
  ERR_NoIFforForm  = QObject::tr("The associated class of this GUI service is missing");
  ERR_NoBaseFormIF  = QObject::tr("The associated class of a base GUI service is missing");
  ERR_ClassCircle  = QObject::tr("Circular reference of interfaces as type of nested members: ");
  ERR_Unused  = QObject::tr("Missing initialization");
  ERR_DoubleVal  = QObject::tr("Second virtual type with the same value (or a base class as value) found");
  ERR_OverriddenIOType  = QObject::tr("Overridden input/output has inconsistent type");
  ERR_MissingExceptionDef  = QObject::tr("The exception type is missing");
  ERR_NoException  = QObject::tr("One of the throw types isn't derived from ""Exception""");
  ERR_OverThrow  = QObject::tr("Only exceptions already thrown by the overridden function should be thrown");
  ERR_SuccFailNotNot  = QObject::tr("Succeed/throw must not be negated");
  ERR_FailNoException  = QObject::tr("Type isn't derived from Exception");
  ERR_InadmissibleCategory  = QObject::tr("Inadmissible category: should be value object");
  ERR_ElemNotInSet  = QObject::tr("Element is not member of set");
  ERR_ArrayXOutOfRange  = QObject::tr("Array index is out of range");
  ERR_SwitchMissingElse  = QObject::tr("Switch statement: No case applies and missing ""else""");
  ERR_CanceledForm  = QObject::tr("Form fill-out/edit canceled");
  ERR_NoSetGetMember  = QObject::tr("No attribute in class for set/get access function");
  ERR_SucceedNeedless  = QObject::tr("Succeed is superfluous here");
  ERR_CopyNullToNonNull  = QObject::tr("Cannot copy a null source to a non null target");
  ERR_ZombieAccess  = QObject::tr("Inadmissible access to zombified object");
  ERR_CallObjInStatic  = QObject::tr("Inadmissible invocation of a static function with a call object");
  ERR_UnfinishedObject  = QObject::tr("Inadmissible use of an unfinished object");
	ERR_MissingBrowserPath = QObject::tr("Plase select a web browser first");
	ERR_BrowserStartFailed = QObject::tr("Browser couldn't be started, errno=%1");
	ERR_ResolvingShortcutFailed = QObject::tr("Resolving shortcut %1 failed");
	ERR_LavaStartFailed = QObject::tr("The Lava interpreter couldn't be started, errno=%1");
	ERR_LavaPEStartFailed = QObject::tr("LavaPE couldn't be started");
	ERR_SaveFailed = QObject::tr("Changes couldn't be saved to read-only Lava file.\nRun unchanged program?");
	ERR_LookForCompoObj = QObject::tr("Implementation of this component object interface or specification not found. \n(Expected in a corresponding *.lcom file)");
	ERR_LookForCompoObjF = QObject::tr("Implementation of the attribute or function of the component object interface not found. \n(Expected in a corresponding *.lcom file");
  ERR_NoImplForAbstract	= QObject::tr("Abstract or native function doesn't have a Lava implementation");
  ERR_LocalOrOutputVarInOld = QObject::tr("Local and output variables not allowed in \"old\" expression");
  ERR_OneLibName = QObject::tr("Only set the name of the library implementing the component");
  ERR_CompObjIntfNotFound = QObject::tr("Supported component object interface not found");
  ERR_CallCheck_NYI = QObject::tr("Call check not yet implemented");
  ERR_clone_copy_inp_closed = QObject::tr("\"Closed\" objects cannot be cloned/copied");
  ERR_Broken_ref_in_HC = QObject::tr("Broken reference in handler clients");
  ERR_NoHandlerIO = QObject::tr("Inconsistency in function parameter versus handler type or versus handler clients");
  ERR_IgnoreMandatoryOnly = QObject::tr("Only *mandatory* inputs may be ignored (and must be ignored if not used)");
  ERR_IgnoreInputsOnly = QObject::tr("Only input parameters may be ignored (and must be ignored if mandatory but not used)");
  ERR_RefToIgnored = QObject::tr("Expressly ignored mandatory input parameters must not be referenced");
  ERR_AlreadyIgnored = QObject::tr("Is already ignored!");
  ERR_SetLavaExceptionFailed = QObject::tr("SetLavaException failed");
  ERR_AbortQuestion = QObject::tr("Do you really want to abort this Lava program?");
  ERR_ExecutionAborted = QObject::tr("Lava program: Execution aborted");
  ERR_NotSingleContainer = QObject::tr("Interface extends more then one container classes (set, chain or array)");
  ERR_FinalVTisOverridden = QObject::tr("This VT illegally overrides a final (=non-overridable) VT");
}
