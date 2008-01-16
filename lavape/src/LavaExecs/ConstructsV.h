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


#ifndef __ConstructsV
#define __ConstructsV


#include "Constructs.h"
#undef IMPLEMENT_DYNAMIC_CLASS
// Single inheritance with one base class
#define IMPLEMENT_DYNAMIC_CLASS(name, basename) \
wxObject *wxConstructorFor##name(void) \
   { return new name##V; }\
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), wxConstructorFor##name);


/////////////////////////////////////////////////////////////////


enum InsertMode {
  InsNested,
  InsChain,
  InsMult,
  InsFlags,
  ToggleArrows,
  ChgComment,
  ChgOp,
  DelNested,
  ReDelNested,
  DelChain,
  DelMult,
  DelFlags,
  PlusMinus
};

class CProgText : public CProgTextBase {
public:
  CProgText () { INIT(); }

	bool insBlank;

  QWidget *redCtl;
	QScrollArea *sv;


  void Insert(TToken token,bool isPrimToken=false,bool isOpt=false);
  void SynObjNewLine();
  void Blank ();

  void NewSel (const QPoint *point);
  void Select (SynObject *selObj=0);
  CHETokenNode *FindNextPlaceholder();
  CHETokenNode *FindPrecedingPlaceholder();
};

class CProgHTML : public CProgTextBase {
public:
  unsigned selBeg, selEnd;

  CProgHTML () {
    INIT();
    selBeg = 0;
    selEnd = 0;
  }

  void Insert(TToken token,bool isPrimToken=false,bool isOpt=false);
  virtual void NewLine ();
  void SynObjNewLine();
  unsigned GetLineIndent ();
  void Blank ();
  void WriteIndent ();
};

class TCommentV : public TComment {
};

class SynObjectV : public SynObject {
public:
  SynObjectV();
  SynObjectV(TToken t);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

struct TDODV : public TDOD {
  TDODV(){}
  TDODV(bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QString(QObject::tr("<p>This is a <a href=\"../ObjectLifeCycle.htm\">variable</a> reference</p>")); }
};

class ExpressionV : public Expression {
};

class OperationV : public Operation {
};

class ReferenceV : public Reference {
public:
  ReferenceV (){}
  ReferenceV (TToken token,TID refID,const char *refName);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class EnumConstV : public EnumConst {
public:
  EnumConstV (){}
  EnumConstV (TToken token,TID &id,QString &name);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ObjReferenceV : public ObjReference {
public:
  ObjReferenceV (){}
  ObjReferenceV (TDODC &refIDs,const char *refName);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class VarNameV : public VarName {
public:
  VarNameV (){}
  VarNameV (const char *vn);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QString(QObject::tr("<p>This is a <a href=\"Declare.htm\">local variable</a></p>")); }
};

class FormParmV : public FormParm {
public:
  FormParmV (){}
  FormParmV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class FormParmsV : public FormParms {
public:
  FormParmsV (){}
  FormParmsV (SynObject *selfVar);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class BaseInitV : public BaseInit {
public:
  BaseInitV (){}
  BaseInitV (address idt,SelfVar *selfVar,LavaDECL *formBase);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class SelfVarV : public SelfVar {
public:
  SelfVarV (){}

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ConstantV : public Constant {
public:
  ConstantV (){}
  ConstantV (const char *c);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class BoolConstV : public BoolConst {
public:
  BoolConstV (){};
  BoolConstV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class NullConstV : public NullConst {
public:
  NullConstV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class SucceedStatementV : public SucceedStatement {
public:
  SucceedStatementV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"FailSucceed.htm\">Succeed</a>: immediate affirmative/successful return from an <a href=\"../EditExec.htm#exec\">exec</a></p>"); }
};

class FailStatementV : public FailStatement {
public:
  FailStatementV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"FailSucceed.htm\">Fail</a>: immediate negative/unsuccessful return from an <a href=\"../EditExec.htm#exec\">exec</a>,"
    " optionally throw an exception</p>"); }
};

class OldExpressionV : public OldExpression {
public:
  OldExpressionV ();

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Old.htm\">Old</a> value of a variable or expression (on entry to function)</p>");}
};

class UnaryOpV : public UnaryOp {
};

class InvertOpV : public InvertOp {
public:
  InvertOpV () {}
  InvertOpV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class HandleOpV : public HandleOp {
public:
  HandleOpV () {}
  HandleOpV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class OrdOpV : public OrdOp {
public:
  OrdOpV () {}
  OrdOpV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class MinusOpV : public MinusOp {
public:
  MinusOpV () {}
  MinusOpV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class LogicalNotV : public LogicalNot {
public:
  LogicalNotV () {}
  LogicalNotV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class EvalExpressionV : public EvalExpression {
public:
  EvalExpressionV () {}
  EvalExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class EvalStatementV : public EvalStatement {
public:
  EvalStatementV () {}
  EvalStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class InSetStatementV : public InSetStatement {
public:
  InSetStatementV () {}
  InSetStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
   return QObject::tr("<p><a href=\"InSet.htm\">Element in set</a> test</p>"); };
};

class BinaryOpV : public BinaryOp {
public:
  BinaryOpV () {}
  BinaryOpV (TToken primToken);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class MultipleOpV : public MultipleOp {
public:
  MultipleOpV (){}
  MultipleOpV (TToken primToken);
};


extern void Draw (MultipleOp *self,CProgTextBase &t,address where,CHAINX *chxp,bool ignored);

class SemicolonOpV : public SemicolonOp {
public:
  SemicolonOpV () { MultipleOpInit(Semicolon_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class AndOpV : public AndOp {
public:
  AndOpV () { MultipleOpInit(and_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class OrOpV : public OrOp {
public:
  OrOpV () { MultipleOpInit(or_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class XorOpV : public XorOp {
public:
  XorOpV () { MultipleOpInit(xor_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"LogOps.htm\">Logical conjunction</a></p>");}
};

class BitAndOpV : public BitAndOp {
public:
  BitAndOpV () { MultipleOpInit(BitAnd_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class BitOrOpV : public BitOrOp {
public:
  BitOrOpV () { MultipleOpInit(BitOr_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class BitXorOpV : public BitXorOp {
public:
  BitXorOpV () { MultipleOpInit(BitXor_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class DivideOpV : public DivideOp {
public:
  DivideOpV () { MultipleOpInit(Slash_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class ModulusOpV : public ModulusOp {
public:
  ModulusOpV () { MultipleOpInit(Percent_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class LshiftOpV : public LshiftOp {
public:
  LshiftOpV () { MultipleOpInit(Lshift_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class RshiftOpV : public RshiftOp {
public:
  RshiftOpV () { MultipleOpInit(Rshift_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class PlusOpV : public PlusOp {
public:
  PlusOpV () { MultipleOpInit(Plus_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class MultOpV : public MultOp {
public:
  MultOpV () { MultipleOpInit(Mult_T); }
  void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored) {
    ::Draw(this,text,where,chxp,ignored); }
};

class AssignmentV : public Assignment {
public:
  AssignmentV ();
  AssignmentV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Assign.htm\">Assignment statement</a></p>"); }
};

class ArrayAtIndexV : public ArrayAtIndex {
public:
  ArrayAtIndexV () {}
  ArrayAtIndexV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class ParameterV : public Parameter {
public:
  ParameterV () {}
  ParameterV (SynObject *param);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class FuncExpressionV : public FuncExpression {
public:
  FuncExpressionV () {}
  FuncExpressionV (bool funcDisabled, bool staticFunc=false);
  FuncExpressionV (Reference *ref);
  FuncExpressionV (ObjReference *ref);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class FuncStatementV : public FuncStatement {
public:
  FuncStatementV ();
  FuncStatementV (bool funcDisabled, bool out=false, bool staticFunc=false);
  FuncStatementV (Reference *ref);
  FuncStatementV (ObjReference *ref, bool out=false);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>This is a <a href=\"MemberFunctions.htm\">Function</a> call statement</p>"); }
};

class ConnectV : public Connect {
public:
  ConnectV (){};
  ConnectV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Use the <b>connect</b> statement to connect a "
    "<a href=\"../Callbacks.htm\">software signal</a> to a signal handler (\"callback\")</p>");}
};

class DisconnectV : public Disconnect {
public:
  DisconnectV (){};
  DisconnectV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Use the <b>disconnect</b> statement to disconnect a "
    "<a href=\"../Callbacks.htm\">software signal</a> from a signal handler (\"callback\")</p>");}
};

class SignalV : public Signal {
public:
  SignalV(){}
  SignalV(bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Use the <b>signal</b> statement to emit a "
    "<a href=\"../Callbacks.htm\">software signal</a>, i.e., to call the signal handlers"
    " (\"callbacks\") connected to the signal</p>");}
};

class AssertStatementV : public AssertStatement {
public:
  AssertStatementV () {}
  AssertStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>An <a href=\"Assert.htm\">embedded assertion</a> is embedded anywhwere in executable code"
    " (in contrast to <a href=\"../DBC.htm\">attached assertions</a>)"
    " and throws a specific exception in case of violation</p>"); }
};

class IgnoreStatementV : public IgnoreStatement {
public:
  IgnoreStatementV () {}
  IgnoreStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Ignore a mandatory input parameter</p>");}
};

class IfThenV : public IfThen {
public:
  IfThenV () {}
  IfThenV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class IfStatementV : public IfStatement {
public:
  IfStatementV () {}
  IfStatementV (bool);

  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"IfStm.htm\">if-then-elsif-else</a> statement with\noptional elsif and else branches</p>"); }
};

class IfxThenV : public IfxThen {
public:
  IfxThenV () {}
  IfxThenV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"IfExpr.htm\">if-then-elsif-else</a> conditional expression with optional\nelsif and else branches</p>");}
};

class IfdefStatementV : public IfdefStatement {
public:
  IfdefStatementV () {}
  IfdefStatementV (bool);

  virtual void Insert2Optionals (SynObject *&thenPart,SynObject *&elsePart);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"IfdefElse.htm\">ifdef</a> statement with optional else branch: if variable != null then ...</p>"); }
};

class CondExpressionV : public CondExpression {
public:
  CondExpressionV () {}
};

class IfExpressionV : public IfExpression {
public:
  IfExpressionV () {}
  IfExpressionV (bool);

  virtual void InsertBranch (SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"IfExpr.htm\">if-then-elsif-else</a> conditional expression with optional\nelsif and else branches</p>");}
};

class ElseExpressionV : public ElseExpression {
public:
  ElseExpressionV () {}
  ElseExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>\"x <a href=\"IfdefElse.htm\">else</a> y\" yields x if x != null, else y</p>");}
};

class BranchV : public Branch {
public:
  BranchV () {}
  BranchV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Switch.htm\">switch</a> statement with optional else branch</p>");}
};

class SwitchStatementV : public SwitchStatement {
public:
  SwitchStatementV () {}
  SwitchStatementV (bool);

  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Switch.htm\">switch</a> statement with optional else branch</p>");}
};

class CatchClauseV : public CatchClause {
public:
  CatchClauseV () {}
  CatchClauseV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class TryStatementV : public TryStatement {
public:
  TryStatementV () {}
  TryStatementV (bool);

  void InsertBranch (SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Try.htm\">Try</a> a statement, catch exceptions</p>"); }
};

class TypeBranchV : public TypeBranch {
public:
  TypeBranchV () {}
  TypeBranchV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"TypeSwitch.htm\">type switch</a> statement with optional else branch</p>");}
};

class TypeSwitchStatementV : public TypeSwitchStatement {
public:
  TypeSwitchStatementV () {}
  TypeSwitchStatementV (bool);

  virtual void Insert2Optionals (SynObject *&elsePart, SynObject *&branch, CHAINX *&chx);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"TypeSwitch.htm\">type switch</a> statement with optional else branch</p>");}
};

class AttachObjectV : public AttachObject {
public:
  AttachObjectV () {}
  AttachObjectV (bool);
  AttachObjectV (Reference *ref);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Attach.htm\">Attach</a> an existing <b><i><font color=\"red\">Lava</font></i></b> component object through one of its interfaces</p>"); }
};

class NewExpressionV : public NewExpression {
public:
  NewExpressionV () {}
  NewExpressionV (bool);
  NewExpressionV (Reference *ref,bool withItf,bool withLoc);
  NewExpressionV (FuncStatement *ref,bool withItf,bool withLoc);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Create a <a href=\"New.htm\">new</a> object</p>");}
};

class CloneExpressionV : public CloneExpression {
public:
  CloneExpressionV () {}
  CloneExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Clone.htm\">Clone</a> an existing <b><i><font color=\"red\">Lava</font></i></b> object</p>");}
};

class CopyStatementV : public CopyStatement {
public:
  CopyStatementV () {}
  CopyStatementV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Copy.htm\">Copy</a> an existing <b><i><font color=\"red\">Lava</font></i></b> object onto another object</p>");}
};

class EnumItemV : public EnumItem {
public:
  EnumItemV () {}
  EnumItemV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Get an enumeration <a href=\"EnumItem.htm\">item</a> from its index</p>");}
};

class ExtendExpressionV : public ExtendExpression {
public:
  ExtendExpressionV () {}
  ExtendExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p>Add a <a href=\"Scale.htm\">scale</a> (e.g. \"Meters\", derived from float/double) to a raw object (e.g. \"3.5\"): 3.5 Meters</p>"); }
};

class RunV : public Run {
public:
  RunV () {}
  RunV (bool);
  RunV (Reference *ref);
  RunV (FuncStatement *ref);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Run.htm\">Run</a> a nested <a href=\"../Packages.htm#initiator\">main program</a></p>");}
};

class QueryItfV : public QueryItf {
public:
  QueryItfV () {}
  QueryItfV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"QryItf.htm\">Query interface:</a> Make another interface of a component object accessible, starting from a known interface</p>");}
};

class GetUUIDV : public GetUUID {
public:
  GetUUIDV () {}
  GetUUIDV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class IntegerIntervalV : public IntegerInterval {
public:
  IntegerIntervalV () {}
  IntegerIntervalV (bool);
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class QuantifierV : public Quantifier {
public:
  QuantifierV () {}
  QuantifierV (bool withSet);

  virtual SynObject *InsertOptionals ();
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
};

class QuantStmOrExpV : public QuantStmOrExp {
public:
  QuantStmOrExpV () {}
};

class DeclareV : public Declare {
public:
  DeclareV () {}
  DeclareV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Declare.htm\">Declare</a> local variables</p>"); };
};

class ExistsV : public Exists {
public:
  ExistsV () {}
  ExistsV (bool);

  virtual SynObject *InsertOptionals ();
  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
   return QObject::tr("<p><a href=\"Exists.htm\">Existential quantifier</a> ranging \nover a finite set</p>"); };
};

class ForeachV : public Foreach {
public:
  ForeachV () {}
  ForeachV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
   return QObject::tr("<p><a href=\"Foreach.htm\">Universal quantifier</a> ranging \nover a finite set</p>"); };
};

class SelectExpressionV : public SelectExpression {
public:
  SelectExpressionV () {}
  SelectExpressionV (bool);

  virtual void Draw (CProgTextBase &text,address where,CHAINX *chxp,bool ignored);
  virtual QString whatsThisText() {
    return QObject::tr("<p><a href=\"Select.htm\">Select</a> quantifier ranging \nover a finite set</p>"); }
};


extern LAVAEXECS_DLL void DrawExec (DString file,bool singleFile,CLavaBaseDoc *doc,LavaDECL *myDECL,SynObject *newObj);
extern LAVAEXECS_DLL void UpdateParameters (CheckData &ckd);


#endif
