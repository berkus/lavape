
#ifdef __GNUC__
#pragma interface
#endif


#ifndef __LavaBaseStringInit
#define __LavaBaseStringInit

#include "SynIDTable.h"
#include "qstring.h"


extern LAVABASE_DLL QString IDS_LAVACOM_FILE;
extern LAVABASE_DLL QString ERR_One_must_remain;
extern LAVABASE_DLL QString ERR_No_input;
extern LAVABASE_DLL QString ERR_const_expected;
extern LAVABASE_DLL QString ERR_Odd_char;
extern LAVABASE_DLL QString ERR_Var_expected;
extern LAVABASE_DLL QString ERR_Wrong_spcl_char;
extern LAVABASE_DLL QString ERR_Apostr_missing;
extern LAVABASE_DLL QString ERR_QMark_missing;
extern LAVABASE_DLL QString ERR_Wrong_hex;
extern LAVABASE_DLL QString ERR_HexTooLong;
extern LAVABASE_DLL QString ERR_OverOrUnderflow;
extern LAVABASE_DLL QString ERR_Wrong_oct;
extern LAVABASE_DLL QString ERR_Wrong_exp;
extern LAVABASE_DLL QString ERR_AssignInQuery;
extern LAVABASE_DLL QString ERR_Broken_ref;
extern LAVABASE_DLL QString ERR_IsntSet;
extern LAVABASE_DLL QString ERR_MissingParms;
extern LAVABASE_DLL QString ERR_RedundantParms;
extern LAVABASE_DLL QString ERR_IncompatibleType;
extern LAVABASE_DLL QString ERR_NotSingleOutput;
extern LAVABASE_DLL QString ERR_ExInLogNot;
extern LAVABASE_DLL QString ERR_NoOutputsAllowed;
extern LAVABASE_DLL QString ERR_UndefRef;
extern LAVABASE_DLL QString ERR_MissingViewAttr;
extern LAVABASE_DLL QString ERR_RedundantExtracts;
extern LAVABASE_DLL QString ERR_MissingExtracts;
extern LAVABASE_DLL QString ERR_SelfIsReserved;
extern LAVABASE_DLL QString ERR_NoViewImpl;
extern LAVABASE_DLL QString ERR_OperatorUndefined;
extern LAVABASE_DLL QString ERR_UndefinedSetElemType;
extern LAVABASE_DLL QString ERR_InadmissibleThrow;
extern LAVABASE_DLL QString ERR_ArrOperatorUndefined;
extern LAVABASE_DLL QString ERR_TempUnfinished;
extern LAVABASE_DLL QString ERR_SelfUnfinished;
extern LAVABASE_DLL QString ERR_ExInIfCond;
extern LAVABASE_DLL QString ERR_NotYetImplemented;
extern LAVABASE_DLL QString ERR_AssigToInput;
extern LAVABASE_DLL QString ERR_AssigToSelf;
extern LAVABASE_DLL QString ERR_EventInputRequired;
extern LAVABASE_DLL QString ERR_Placeholder;
extern LAVABASE_DLL QString ERR_ExecutionFailed;
extern LAVABASE_DLL QString ERR_EmptyExec;
extern LAVABASE_DLL QString ERR_AssertionViolation;
extern LAVABASE_DLL QString ERR_NoErrors;
extern LAVABASE_DLL QString ERR_ExecError;
extern LAVABASE_DLL QString ERR_DLLError;
extern LAVABASE_DLL QString ERR_AssigToFrozen;
extern LAVABASE_DLL QString ERR_NoMemberFunction;
extern LAVABASE_DLL QString ERR_SingleAssViol;
extern LAVABASE_DLL QString ERR_PrevDescAssig;
extern LAVABASE_DLL QString ERR_NotOptional;
extern LAVABASE_DLL QString ERR_AssigToLocal;
extern LAVABASE_DLL QString ERR_NameInUse;
extern LAVABASE_DLL QString ERR_OutOfScope;
extern LAVABASE_DLL QString ERR_NoAbstract;
extern LAVABASE_DLL QString ERR_SingleAssignment;
extern LAVABASE_DLL QString ERR_FuncParmOutOfScope;
extern LAVABASE_DLL QString ERR_AssigInLogNot;
extern LAVABASE_DLL QString ERR_SelfInStatic;
extern LAVABASE_DLL QString ERR_UndefType;
extern LAVABASE_DLL QString ERR_CallExprUndefType;
extern LAVABASE_DLL QString ERR_NoCompObjItf;
extern LAVABASE_DLL QString ERR_ReservedName;
extern LAVABASE_DLL QString ERR_Parameter;
extern LAVABASE_DLL QString ERR_AssignInIf;
extern LAVABASE_DLL QString ERR_NullException;
extern LAVABASE_DLL QString ERR_NotYetInitialized;
extern LAVABASE_DLL QString ERR_AssigToRdOnly;
extern LAVABASE_DLL QString ERR_AssigToMemberOfFrozen;
extern LAVABASE_DLL QString ERR_RdOnlyFunc;
extern LAVABASE_DLL QString ERR_OverriddenDiffs;
extern LAVABASE_DLL QString ERR_UndefBaseItf;
extern LAVABASE_DLL QString ERR_ImplUndefItf;
extern LAVABASE_DLL QString ERR_BaseInit;
extern LAVABASE_DLL QString ERR_MissingBaseInits;
extern LAVABASE_DLL QString ERR_FuncImplToLocal;
extern LAVABASE_DLL QString ERR_EnterValue;
extern LAVABASE_DLL QString ERR_SelectValue;
extern LAVABASE_DLL QString ERR_WrongCopyTarget;
extern LAVABASE_DLL QString ERR_NoExecBody;
extern LAVABASE_DLL QString ERR_AssigInForeach;
extern LAVABASE_DLL QString ERR_NestedVTs;
extern LAVABASE_DLL QString ERR_VTOutOfScope;
extern LAVABASE_DLL QString ERR_StdNotRemovable;
extern LAVABASE_DLL QString ERR_CallbackHasNoInput;
extern LAVABASE_DLL QString ERR_CallbackWrongEvtArg;
extern LAVABASE_DLL QString ERR_Protected;
extern LAVABASE_DLL QString ERR_InvisibleType;
extern LAVABASE_DLL QString ERR_InvisibleBasictype;
extern LAVABASE_DLL QString ERR_VirtualCircle;
extern LAVABASE_DLL QString ERR_InvalidValOfVT;
extern LAVABASE_DLL QString ERR_SelfUnfinishedCallObj;
extern LAVABASE_DLL QString ERR_NoInitializer;
extern LAVABASE_DLL QString ERR_ExplicitInitializerCall;
extern LAVABASE_DLL QString ERR_MissingFuncDecl;
extern LAVABASE_DLL QString ERR_MissingInitialization;
extern LAVABASE_DLL QString ERR_UnaryOpUndefined;
extern LAVABASE_DLL QString ERR_HandleOpParm;
extern LAVABASE_DLL QString ERR_UseVT;
extern LAVABASE_DLL QString ERR_WrongContext;
extern LAVABASE_DLL QString ERR_ImmutableCallObj;
extern LAVABASE_DLL QString ERR_NonROCallInROClause;
extern LAVABASE_DLL QString ERR_SuccFailNotLast;
extern LAVABASE_DLL QString ERR_NoNativeImpl;
extern LAVABASE_DLL QString ERR_NoFuncImpl;
extern LAVABASE_DLL QString ERR_NoClassFunc;
extern LAVABASE_DLL QString ERR_CorruptForm;
extern LAVABASE_DLL QString ERR_CorruptForm2;
extern LAVABASE_DLL QString ERR_NotBaseIF;
extern LAVABASE_DLL QString ERR_NoOverridden;
extern LAVABASE_DLL QString ERR_NoClassImpl;
extern LAVABASE_DLL QString ERR_IOParams;
extern LAVABASE_DLL QString ERR_SetGetError;
extern LAVABASE_DLL QString ERR_funcImpl;
extern LAVABASE_DLL QString ERR_NoImplIF;
extern LAVABASE_DLL QString ERR_NoSetElemType;
extern LAVABASE_DLL QString ERR_CommonContext;
extern LAVABASE_DLL QString ERR_OverriddenStatic;
extern LAVABASE_DLL QString ERR_SubstInContext;
extern LAVABASE_DLL QString ERR_VBaseWithoutDefaultIni;
extern LAVABASE_DLL QString ERR_NoImplClass;
extern LAVABASE_DLL QString ERR_ImplOfAbstract;
extern LAVABASE_DLL QString ERR_CleanSupports;
extern LAVABASE_DLL QString ERR_OverInOtherCon;
extern LAVABASE_DLL QString ERR_NoDefaultIni;
extern LAVABASE_DLL QString ERR_CopyToSelf;
extern LAVABASE_DLL QString ERR_NoIFforForm;
extern LAVABASE_DLL QString ERR_NoBaseFormIF;
extern LAVABASE_DLL QString ERR_ClassCircle;
extern LAVABASE_DLL QString ERR_Unused;
extern LAVABASE_DLL QString ERR_DoubleVal;
extern LAVABASE_DLL QString ERR_OverriddenIOType;
extern LAVABASE_DLL QString ERR_SecondImpl;
extern LAVABASE_DLL QString ERR_inStd;
extern LAVABASE_DLL QString ERR_SecondDefaultIni;
extern LAVABASE_DLL QString ERR_NoCallbackType;
extern LAVABASE_DLL QString ERR_IncompWithTargetVT;
extern LAVABASE_DLL QString ERR_WrongEvSpecType;
extern LAVABASE_DLL QString ERR_IncompatibleCategory;
extern LAVABASE_DLL QString ERR_SetIsFrozenValObj;
extern LAVABASE_DLL QString ERR_EnumIntvType;
extern LAVABASE_DLL QString ERR_AbstractInherited;
extern LAVABASE_DLL QString ERR_IncompAbstractVT;
extern LAVABASE_DLL QString ERR_CallExprRequired;
extern LAVABASE_DLL QString ERR_CorruptForm3;
extern LAVABASE_DLL QString ERR_CorruptObject;
extern LAVABASE_DLL QString ERR_AbstrMemType;
extern LAVABASE_DLL QString ERR_NoCallbackFunc;
extern LAVABASE_DLL QString ERR_CallbackHasOutput;
extern LAVABASE_DLL QString ERR_IfxForbidden;
extern LAVABASE_DLL QString ERR_MissingExec;
extern LAVABASE_DLL QString ERR_NoAbstractForm;
extern LAVABASE_DLL QString ERR_NoPicture;
extern LAVABASE_DLL QString ERR_ItfNotInCOS;
extern LAVABASE_DLL QString ERR_NoCompoSpec;
extern LAVABASE_DLL QString ERR_noCompoInterf;
extern LAVABASE_DLL QString ERR_ldocIncompatible;
extern LAVABASE_DLL QString ERR_ldocNotOpened;
extern LAVABASE_DLL QString ERR_ldocNotStored;
extern LAVABASE_DLL QString ERR_ExactlyOneLcom;
extern LAVABASE_DLL QString ERR_AllocObjectFailed;
extern LAVABASE_DLL QString ERR_ImmutableArray;
extern LAVABASE_DLL QString ERR_ThrowInInitiator;
extern LAVABASE_DLL QString ERR_ForceOver;
extern LAVABASE_DLL QString ERR_IsntEnum;
extern LAVABASE_DLL QString ERR_AssigToMultiArray;
extern LAVABASE_DLL QString ERR_FaultyArrayObj;
extern LAVABASE_DLL QString ERR_HasNoDftIni;
extern LAVABASE_DLL QString ERR_CompoNotFound;
extern LAVABASE_DLL QString ERR_MustThrowExc;
extern LAVABASE_DLL QString ERR_OIDrequired;
extern LAVABASE_DLL QString ERR_URLrequired;
extern LAVABASE_DLL QString ERR_NullCallObject;
extern LAVABASE_DLL QString ERR_NullMandatory;
extern LAVABASE_DLL QString ERR_IntegerRange;
extern LAVABASE_DLL QString ERR_EnumOrdLow;
extern LAVABASE_DLL QString ERR_EnumOrdHigh;
extern LAVABASE_DLL QString ERR_RunTimeException;
extern LAVABASE_DLL QString ERR_OneEnum;
extern LAVABASE_DLL QString ERR_MissingExceptionDef;
extern LAVABASE_DLL QString ERR_NoException;
extern LAVABASE_DLL QString ERR_OverThrow;
extern LAVABASE_DLL QString ERR_SuccFailNotNot;
extern LAVABASE_DLL QString ERR_FailNoException;
extern LAVABASE_DLL QString ERR_InadmissibleCategory;
extern LAVABASE_DLL QString ERR_ElemNotInSet;
extern LAVABASE_DLL QString ERR_ArrayXOutOfRange;
extern LAVABASE_DLL QString ERR_SwitchMissingElse;
extern LAVABASE_DLL QString ERR_CanceledForm;
extern LAVABASE_DLL QString ERR_NoSetGetMember;
extern LAVABASE_DLL QString ERR_SucceedNeedless;
extern LAVABASE_DLL QString ERR_CopyNullToNonNull;
extern LAVABASE_DLL QString ERR_ZombieAccess;
extern LAVABASE_DLL QString ERR_CallObjInStatic;
extern LAVABASE_DLL QString ERR_UnfinishedObject;
extern LAVABASE_DLL QString ERR_NullParent;
extern LAVABASE_DLL QString ERR_NotReferenced;
extern LAVABASE_DLL QString ERR_URLmustBeString;
extern LAVABASE_DLL QString ERR_lcomNotOpened;
extern LAVABASE_DLL QString ERR_OutFunctionFailed;
extern LAVABASE_DLL QString ERR_Private;
extern LAVABASE_DLL QString ERR_NoRefType;
extern LAVABASE_DLL QString ERR_NoBaseType;
extern LAVABASE_DLL QString ERR_NoElType;
extern LAVABASE_DLL QString ERR_NoBaseIF;
extern LAVABASE_DLL QString ERR_NoFiredIF;
extern LAVABASE_DLL QString ERR_NoExtForm;
extern LAVABASE_DLL QString ERR_InVTofBaseIF;
extern LAVABASE_DLL QString ERR_InVT;
extern LAVABASE_DLL QString ERR_ParIncompatible;
extern LAVABASE_DLL QString ERR_MemIncompatible;
extern LAVABASE_DLL QString ERR_AmbgInVT;
extern LAVABASE_DLL QString ERR_NameCollision;
extern LAVABASE_DLL QString ERR_ErrInForm;
extern LAVABASE_DLL QString ERR_MissingBrowserPath;
extern LAVABASE_DLL QString ERR_BrowserStartFailed;
extern LAVABASE_DLL QString ERR_ResolvingShortcutFailed;
extern LAVABASE_DLL QString ERR_LavaStartFailed;
extern LAVABASE_DLL QString ERR_SaveFailed;
extern LAVABASE_DLL QString ERR_LookForCompoObj;
extern LAVABASE_DLL QString ERR_LookForCompoObjF;
extern LAVABASE_DLL QString ERR_NoImplForAbstract;
extern LAVABASE_DLL QString ERR_LocalOrOutputVarInOld;

extern void LavaBaseStringInit();

#endif
