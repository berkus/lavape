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



/**********************************************************************

               #include file for class Tokens

 **********************************************************************/

#ifndef __Tokens
#define __Tokens

#include "SYSTEM.h"
#include "DString.h"
#include "qstring.h"

#ifdef WIN32
#ifdef LAVAEXECS_EXPORT
#define LAVAEXECS_DLL __declspec( dllexport )
#else
#define LAVAEXECS_DLL __declspec( dllimport )
#endif
#else
#define LAVAEXECS_DLL
#endif

extern const unsigned MaxLenToken;
extern const unsigned MaxString;


#include "SYSTEM.h"
#include "AnyType.h"

#include "ASN1pp.h"


enum TToken {
  Semicolon_T,
  or_T,
  xor_T,
  and_T,
  andThen_T,
  not_T,
  Equal_T,
  NotEqual_T,
  LessThan_T,
  GreaterThan_T,
  LessEqual_T,
  GreaterEqual_T,
  Plus_T,
  Minus_T,
  Mult_T,
  Slash_T,
  Percent_T,
  BitAnd_T,
  BitOr_T,
  BitXor_T,
  Invert_T,
  Lshift_T,
  Rshift_T,
  arrayAtIndex_T,
  Larrow_T,
  Rarrow_T,
  Period_T,
  Lparenth_T,
  Rparenth_T,
  Handle_T,
  Ord_T,
  FutureOP3,
  FutureOP4,
  FutureOP5,
  EndOfOperators,
  Colon_T,
  Comma_T,
  Ellipsis_T,
  EvalExpr_T,
  EvalStm_T,
  ExclMark_T,
  Lbrace_T,
  Rbrace_T,
  Lbracket_T,
  Rbracket_T,
  Tilde_T,
  Dollar_T,
  FutureSEP2,
  FutureSEP3,
  FutureSEP4,
  FutureSEP5,
  ENDassert_T,
  ENDrun_T,
  ENDcallback_T,
  ENDclone_T,
  ENDdeclare_T,
  ENDdelete_T,
  ENDexists_T,
  ENDforeach_T,
  ENDif_T,
  ENDifx_T,
  ENDnew_T,
  ENDselect_T,
  ENDswitch_T,
  ENDtransaction_T,
  ENDtry_T,
  ENDtypeof_T,
  FutureEND1,
  FutureEND2,
  FutureEND3,
  FutureEND4,
  FutureEND5,
  AVG_T,
  Boolean_T,
  Cardinal_T,
  CHAIN_T,
  Chain_T,
  Char_T,
  COUNT_T,
  Double_T,
  Float_T,
  Integer_T,
  MAX_T,
  MERGE_T,
  MIN_T,
  String_T,
  SUM_T,
  FutureBuiltin1,
  FutureBuiltin2,
  FutureBuiltin3,
  FutureBuiltin4,
  FutureBuiltin5,
  add_T,
  assert_T,
  assign_T,
  assignFS_T,
  assignFX_T,
  attach_T,
  as_T,
  base_T,
  but_T,
  by_T,
  run_T,
  callback_T,
  case_T,
  caseType_T,
  catch_T,
  clone_T,
  complete_T,
  concurrent_T,
  toBeCalledOn_T,
  const_T,
  constraint_T,
  copy_T,
  declare_T,
  delete_T,
  dftInitializer_T,
  div_T,
  do_T,
  else_T,
  elsif_T,
  enumConst_T,
  exec_T,
  exists_T,
  extract_T,
  fail_T,
  false_T,
  for_T,
  foreach_T,
  from_T,
  function_T,
  get_T,
  handle_T,
  holds_T,
  if_T,
  ifx_T,
  implementation_T,
  in_T,
  initializer_T,
  initializing_T,
  initiator_T,
  inputs_T,
  intIntv_T,
  itf_T,
  old_T,
  new_T,
  nil_T,
  of_T,
  oid_T,
  on_T,
  onto_T,
  outputs_T,
  parameter_T,
  qua_T,
  quant_T,
  select_T,
  service_T,
  succeed_T,
  switch_T,
  then_T,
  throw_T,
  to_T,
  transaction_T,
  true_T,
  try_T,
  type_T,
  unique_T,
  uuid_T,
  view_T,
  where_T,
  with_T,
  item_T,
  require_T,
  ensure_T,
  implies_T,
  FutureKWD5,
  CharConst_T,
  FloatConst_T,
  HexConst_T,
  Identifier_T,
  IntConst_T,
  OctalConst_T,
  StringConst_T,
  DoubleConst_T,
  BitConst_T,
  FutureCONST3,
  FutureCONST4,
  FutureCONST5,
  Callee_T,
  Comment_T,
  CompObj_T,
  FuturePLH1,
  Const_T,
  CrtblDisabled_T,
  CrtblPH_T,
  CrtblRef_T,
  Event_T,
  Exp_T,
  ExpDisabled_T,
  FormParm_T,
  FormParms_T,
  FuncPH_T,
  FuncDisabled_T,
  FuncRef_T,
  FuturePLH2,
  NewLineSym_T,
  ObjPH_T,
  ObjRef_T,
  ObjDisabled_T,
  SetAttribPH_T,
  SetPH_T,
  Stm_T,
  TDOD_T,
  TypePH_T,
  TypeRef_T,
  VarPH_T,
  VarName_T,
  TypePHopt_T,
  ExpOpt_T,
  ObjPHOpt_T,
  FuturePLH4,
  FuturePLH5,
  LarrowHTML,
  RarrowHTML,
  LtHTML,
  LeHTML,
  LshiftHTML,
  NoToken};

extern void CDPTToken (PutGetFlag pgf, ASN1* cid, address varAddr,
                       bool baseCDP=false);


//extern QChar TOKENSTR[NoToken+1][MaxLenToken];
extern QString TOKENSTR[NoToken+1];
extern unsigned priority[EndOfOperators];


typedef struct THTEntry *TEntryPtr;

struct THTEntry {
  TEntryPtr nextEntry;
  TToken index;
};

#define hashPrime 499

struct THashTable {
  TEntryPtr firstEntry, lastEntry;
};

extern THashTable hashTable[hashPrime];

extern unsigned HashCode (const char * const s);

extern LAVAEXECS_DLL void Tokens_INIT ();

#endif
