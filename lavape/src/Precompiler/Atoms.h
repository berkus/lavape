#ifdef __GNUC__
#pragma interface
#endif


/**********************************************************************

               #include file for class AtomsCLASS

 **********************************************************************/

#ifndef __Atoms
#define __Atoms


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"

const unsigned MaxLenAtom = 14;
const unsigned MaxAtomStore = 200000;

enum AtomType {
  StartAtom,
  ApplicationClass,
  ArbitraryA,
  Available,
  BitmapA,
  BitstringA,
  Buttonmenu,
  BytesA,
  Call,
  Callsuspend,
  Case,
  Cdp,
  Centeraligned,
  CenteredA,
  ChainA,
  Chain0A,
  ChainAnyA,
  ChainAny0A,
  ChainX,
  CheckA,
  Checkpoint,
  ContextClass,
  Copy,
  Cursor,
  DateA,
  Default,
  Done,
  Drop,
  ELSE,
  End,
  EnterA,
  ErrorText,
  Explicit,
  False,
  Fio,
  Fixedlength,
  FOR,
  Form,
  Forsys,
  From,
  Fullbox,
  Get,
  Getprivate,
  Getsuspend,
  Highemphasis,
  Horiz,
  Horizontal,
  Hrules,
  Implicit,
  Import,
  Include,
  Indentation,
  Inoid,
  Insysoid,
  Inval,
  InvalEnc,
  InvisibleA,
  Leftaligned,
  Leftjustified,
  Like,
  Local,
  Lowemphasis,
  Lzcardinal,
  Menu,
  Menufirst,
  MoneyStringA,
  Name,
  NestedA,
  Nested0A,
  NestedAnyA,
  NestedAny0A,
  NestedX,
  Newframe,
  Newline,
  Nf,
  Nf0,
  Nf1,
  Nf2,
  Nf3,
  Nl,
  Nl0,
  Nl1,
  Nl2,
  Nl3,
  Noclass,
  Noecho,
  Noellipsis,
  Norules,
  Of,
  Oid,
  Optionmenu,
  Orgunit,
  Orgval,
  Orgvar,
  Outoid,
  Outsysoid,
  Outval,
  Overlay,
  Owner,
  PopDownA,
  Popup,
  PopupCase,
  Popupmenu,
  PrivateClass,
  Proc,
  Procedure,
  Put,
  Putprivate,
  Realize,
  Return,
  ReverseA,
  Rightaligned,
  Rightjustified,
  Set,
  Show,
  ShowEnc,
  Sig,
  SignatureA,
  Space,
  Start,
  StopA,
  STRINGA,
  STRING0A,
  Sysoid,
  Sysproc,
  Tab,
  TextA,
  TimeA,
  ToggleA,
  True,
  Type,
  Unaligned,
  UniversalClass,
  Val,
  ValEnc,
  Variant,
  Verify,
  Version,
  Vert,
  VerticalA,
  Vrules,
  YearA,
  
// small initial letter:
  Address,
  Asm,
  Auto,
  BooleanA,
  Break,
  ByteA,
  Catch,
  CharA,
  Class,
  ConstA,
  Continue,
  SwitchDefault,
  Delete,
  Do,
  DoubleA,
  Else,
  Enum,
  Extern,
  FloatA,
  For,
  Friend,
  Goto,
  If,
  Inline,
  Int,
  Long,
//  New,
  Operator,
  Protected,
  Public,
  PrivateA,
  Register,
  Short,
  Signed,
  Sizeof,
  Static,
  Struct,
  Switch,
  Template,
//  This,
  Throw,
  Try,
  Typedef,
  Union,
  Unsigned,
  Virtual,
  Void,
  Volatile,
  While,
  Word,
  EndOfReservedWords,

  IdentifierA,
  CharConst,
  StringConst,
  IntConst,
  OctalConst,
  HexConst,
  FloatConst,

  LogNot,
  Modulus,
  ExclOr,
  And,
  Mult,
  Lparenth,
  Rparenth,
  Minus,
  Plus,
  Assign,
  Lbrace,
  Rbrace,
  Or,
  Not,
  Lbracket,
  Rbracket,
  Semicolon,
  Colon,
  LessThan,
  GreaterThan,
  QuestionMark,
  Comma,
  Period,
  Slash,

  PointerTo,
  PlusPlus,
  MinusMinus,
  MemberByOffset,
  MemberByPointer,
  Lshift,
  Rshift,
  LessEqual,
  GreaterEqual,
  EqualA,
  NotEqual,
  LogAnd,
  LogOr,
  MultAssign,
  DivAssign,
  ModulusAssign,
  PlusAssign,
  MinusAssign,
  LshiftAssign,
  RshiftAssign,
  AndAssign,
  ExclOrAssign,
  OrAssign,
  ClassMember,
  ClassMemberPtr,
  
  PPescape,
  NumberSign,
  Eof,
  NoAtom};

typedef unsigned TStringBufferPos;

typedef char *AtomPtr;

/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/

#include "DString.h"


/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/
void Atoms_INIT ();

extern AtomType Atom;
extern char StringBuffer[MaxAtomStore];
extern TStringBufferPos StringBufferPos;
extern char ATOMSTR[NoAtom+1][MaxLenAtom];
extern unsigned AtomLine, AtomCol, CommentLine, CommentCol;


extern bool IsReservedWord ();

#endif
