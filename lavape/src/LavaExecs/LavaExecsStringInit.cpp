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


#ifdef __GNUC__
#pragma implementation
#endif


#include "LavaExecsStringInit.h"
#include "qapplication.h"


/*
    ID_C_INSERT= QObject::tr("Insert an empty copy after or before the selection, depending on the current insert mode\nInsert empty copy");
    ID_SWITCH= QObject::tr("""switch"" statement\n""switch"" statement");
    ID_INNER = QObject::tr("""inner"" placeholder for function specialization\n""inner"" placeholder");
    ID_IF = QObject::tr("""if-then-elsif-else-#if"" statement\n""if"" statement");
    ID_FOREACH = QObject::tr("""for each"" quantifier statement\n""foreach"" statement");
    ID_DECLARE = QObject::tr("Declare local variables (but do not yet create corresponding objects)\nDeclare local variables");
    ID_INSERT_AFTER = QObject::tr("Insert another element of the same type after  or before this one\nInsert ");
    ID_INSERT_BEFORE = QObject::tr("Press to change insert mode  to ""insert before"" (default: ""insert after"")\nIf depressed: ""insert before""");
    ID_THROW = QObject::tr("Throw an error and terminate the current exec immediately\nThrow");
    ID_PLUS_MINUS = QObject::tr("Toggle sign of this expression; shortcut: -\nToggle sign");
    ID_TOGGLE_ARROWS = QObject::tr("Assignment arrows point to the right (default: to the left)\nIf depressed: right arrows");
    ID_NEW_LINE = QObject::tr("Put this syntactic element on a new line\nNew line");
    ID_MY_ITF = QObject::tr("Provide a handle for own interface\nOwn interface handle");
    ID_MODULUS = QObject::tr("Modulus: remainder of integer division; shortcut: %\nModulus");
    ID_IGNORE = QObject::tr("Skip this statement at run time\nSkip/comment out");
    ID_ATTACH = QObject::tr("Attach a running component object\nAttach component object");
    ID_YIELD = QObject::tr("""Query interface"": provide a handle for the specified interface given the specified old handle\nInterface navigation");
    ID_STATIC_CALL = QObject::tr("Call a static member function\nCall static function");
    ID_VFUNC_CALL = QObject::tr("Virtual or static invocation of a virtual function; shortcut: x\nFunction call");
    ID_ASSIGN = QObject::tr("Assign the value of an expression to a variable\nAssign value to variable");
    ID_FUNC_CALL_OBJ = QObject::tr("Build a ""canned function call"" for deferred execution (= prepare a callback)\nCallback");
    ID_TYPE_SWITCH = QObject::tr("Branch on type of expression\nbranch on type");
    ID_INTERVAL = QObject::tr("Integer interval or array element\nInterval / array element");
    ID_UUID = QObject::tr("Get universally unique interface ID of the specified interface\nGet UUID");
    ID_ATOMIC = QObject::tr("Toggle atomic execution as a transaction\nToggle atomic execution");
    ID_CALL = QObject::tr("Run an initiator or a service\nRun initiator or service");
    ID_EXISTS = QObject::tr("Find elements in set having certain properties\nFind in set");
    ID_CREATE = QObject::tr("Create, initialize, and  and customize a new object\nCreate new object");
    ID_QUERY_ITF = QObject::tr("Dynamic interface look-up for a given component object\nQuery interface");
    ID_AND_THEN = QObject::tr("Sequential execution/verification of statements\nSequential execution/verification");
    ID_SHOW_COMMENTS = QObject::tr("Show/hide comments in execs\nShow/hide comments in execs");
    ID_CONCURRENT = QObject::tr("Toggle concurrent or even autonomous execution in a separate thread\nToggle concurrent/autonomous execution");
    ID_ASSERT = QObject::tr("Check the following condition if in debug mode\nDebug-assertion");
    ID_CONFLICT = QObject::tr("Show conflicting previous assignment\nShow conflicting assignment");
    ID_TOGGLE = QObject::tr("Switch between ""state object/value object/same as self""\nState/value object/same as self");
    ID_REFRESH = QObject::tr("Refresh the entire exec view\nRefresh");
    ID_CLONE = QObject::tr("Clone an existing object and modify the clone\nClone and change");
    ID_COPY = QObject::tr("Copy an existing source object onto an existing target object and modify the copy\nCopy and change");
    ID_IF_EXPR = QObject::tr("Conditional expression\n""if"" expression");
    ID_LT = QObject::tr("""less than"" relation between expressions; shortcut: <\nLess than");
    ID_GE = QObject::tr("""greater than or equal"" relation between expressions; shortcut: Alt->\nGreater or equal");
    ID_EQ = QObject::tr("""equal"" relation between expressions; shortcut: =\nEqual");
    ID_NE = QObject::tr("""not equal"" relation between expressions; shortcut: #\nNot equal");
    ID_GT = QObject::tr("""greater"" relation between expressions; shortcut: >\nGreater than");
    ID_SELECT = QObject::tr("""Select"" expression: set-oriented database query (corresponds roughly to SQL ""select""\nSet-oriented DB query");
    ID_PLUS = QObject::tr("Addition; shortcut: +\nAddition");
    ID_MULT = QObject::tr("Multiplication; shortcut: *\nMultiplication");
    ID_BIT_OR = QObject::tr("Bitwise ""or"" operation on a BitSet; shortcut: |\nBitwise ""or""");
    ID_LE = QObject::tr("""less than or equal"" relation between expressions; shortcut: Alt-<\nLess than or equal");
    ID_AND = QObject::tr("Logical ""and"" conjunction between statements\n""and"" conjunction");
    ID_BIT_XOR = QObject::tr("Bitwise ""exclusive or"" operation on a BitSet\n Bitwise ""xor""");
    ID_BIT_AND = QObject::tr("Bitwise ""and"" operation on a BitSet; shortcut: &\nBitwise ""and""");
    ID_LSHIFT = QObject::tr("<number> left shift by <# bits>\nLeft shift");
    ID_RSHIFT = QObject::tr("<number> right shift by <# bits>\nRight shift");
    ID_IN = QObject::tr("<element> in <set> relation\nElement in set");
    ID_OR = QObject::tr("Logical ""or"" conjunction between statements\n""or"" conjunction");
    ID_REPLACE = QObject::tr("Reuse an existing data object and change it ""in place""\nReuse and change object");
    ID_XOR = QObject::tr("""exclusive or"" conjunction between statements\n""exclusive or"" conjunction");
    ID_OR_ELSE = QObject::tr("<stm1> ""or else"" <stm2>:  <stm2> is verified only if <stm1> fails\nor else");
    ID_TRUE = QObject::tr("Successful termination of current function/initializer/constraint\nSuccessful termination");
    ID_FALSE = QObject::tr("Throw exception, terminate current function/initializer/constraint\nThrow exception");
    ID_NULL = QObject::tr("Constant ""nothing/null/nil""; shortcut: 0\nNothing/null/nil");
    ID_CLASS_CALL = QObject::tr("Call a function via the virtual function table of its first input argument\nVirtual function call");
    ID_INVERT = QObject::tr("Invert bits of a BitSet / logical not of a Boolean; shortcut: !\nNot / invert bits");
    ID_DIVIDE = QObject::tr("Division; shortcut: /\nDivision");
    ID_NOT = QObject::tr("Logical negation; shortcut: !\nLogical ""not""");
    ID_EVALUATE = QObject::tr("""<expr> = true""  /  ifx <stm> then true else false #if; shortcut: ?\nEvaluate <expr>/<stm>");
    ID_INPUT_ARROW = QObject::tr("Put an arrow between function and input parameter(s)\nPut an arrow between function and input parameter(s)");
    ID_TOGGLE_FCOMP = QObject::tr("Toggle ""substitutable type""\nToggle ""substitutable type""");
    ID_HANDLE = QObject::tr("Handle of a set element; shortcut: @\nHandle of a set element");
    ID_STATIC_FUNC = QObject::tr("Call a static member function\nCall static function");
    ID_OPT_LOCAL_VAR = QObject::tr("Toggle ""optional local variable "" (= may attain value ""0"")\nToggle ""optional local variable""");
    ID_ITEM = QObject::tr("Item <int> of <enum>\nItem <int> of <enum>");
    ID_QUA = QObject::tr("Make an extension object (of a more derived class)\nExtend object");
    ID_FAIL = QObject::tr("Current function/initializer/constraint returns ""false"" and possibly throws an exception\nFailure return with(out) exception");
    ID_SUCCEED = QObject::tr("Successful termination of current function/initializer/constraint\nSuccessful termination");
    ID_ORD = QObject::tr("Ordinal (=Integer) value of enumerated item\nOrdinal of enum. item");
    ID_OR_EXCEPTION = QObject::tr("Exception handling ""or"" branch\nException handling");
*/

QString ID_ENTER_CONST = QObject::tr("Click once or click spectacles or press ""cursor down"" key to enter or change constant");
QString ID_ENTER_VARNAME = QObject::tr("Click once or click spectacles or press ""cursor down"" key to enter or change variable name");
QString ID_F1_HELP = QObject::tr("For Help, press F1");
QString ID_TOGGLE_CATEGORY = QObject::tr("Use the ""Tilde"" toolbutton to select ""state object"" or ""value object""");
QString ID_INSERT_STM = QObject::tr("Click a statement toolbutton to insert the associated statement template after or before the selected statement");
QString ID_OPT_CLAUSE = QObject::tr("Press DEL key to delete this optional clause");
QString ID_EDIT_COMMENT = QObject::tr("Click once or click spectacles or press ""cursor down"" key to edit comment");
QString ID_REPLACE_EXP = QObject::tr("Click an expression toolbutton to replace the highlighted expression");

