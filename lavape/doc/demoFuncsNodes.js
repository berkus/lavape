// You can find instructions for this file here:
// http://www.treeview.net

// Decide if the names are links or just the icons
USETEXTLINKS = 1  //replace 0 with 1 for hyperlinks

// Decide if the tree is to start all open or just showing the root folders
STARTALLOPEN = 0 //replace 0 with 1 to show the whole tree

HIGHLIGHT = 1

foldersTree = gFld("<i><b>LavaPE Manual</b></i>", "LavaOnlineDoc.htm")
  aux1 = insFld(foldersTree, gFld("Frequently asked questions", "html/FAQ.htm"))
      insDoc(aux1, gLnk("R", "Why yet another object-oriented programming language", "html/FAQWhyAnotherPL.htm"))
      insDoc(aux1, gLnk("R", "What's the principal orientation of the Lava development?", "html/FAQLavaOrientation.htm"))
      insDoc(aux1, gLnk("R", "Principal orientation in terms of spaghetti, tortellini and fractals", "html/FAQfromSpaghettiToFractals.htm"))
      insDoc(aux1, gLnk("R", "Who might be interested in Lava?", "html/FAQWhoTakeLook.htm"))
      insDoc(aux1, gLnk("R", "Will Lava provide the same wealth of class libraries as commercial languages?", "html/FAQClassLibraries.htm"))
      insDoc(aux1, gLnk("R", "Does Lava support component-based programming?", "html/Components.htm"))
      insDoc(aux1, gLnk("R", "Does Lava support design patterns?", "html/FAQDesignPatterns.htm"))
      insDoc(aux1, gLnk("R", "Does Lava support &quot;refactoring&quot;?", "html/Refactoring.htm"))
      insDoc(aux1, gLnk("R", "What about Lava and UML?", "html/FAQUML.htm"))
      insDoc(aux1, gLnk("R", "Next steps, future plans, desirable contributions", "html/FAQLavaFuture.htm"))
      insDoc(aux1, gLnk("R", "How can I contribute to the Lava development?", "html/FAQContributions.htm"))
      insDoc(aux1, gLnk("R", "Gibt es die Lava Dokumentation auch auf Deutsch?", "html/FAQGerman.htm"))
  aux1 = insFld(foldersTree, gFld("From Java and Visual Basic to Lava", "html/WhyLava.htm"))
	  insDoc(aux1, gLnk("R", "Advantages of Java", "html/AdvantJava.htm"))
	  insDoc(aux1, gLnk("R", "Advantages of Visual Basic", "html/AdvantVB.htm"))
	  insDoc(aux1, gLnk("R", "Urgent problems in current programming languages", "html/Unsolved.htm"))
  aux1 = insFld(foldersTree, gFld("Lava solutions and highlights", "html/LavaOverview.htm"))
	  insDoc(aux1, gLnk("R", "Lava is perfectly &quot;point-and-click&quot;", "html/PointAndClick.htm"))
	  insDoc(aux1, gLnk("R", "Advantages of structure editing", "html/StructEdAdvant.htm"))
	  insDoc(aux1, gLnk("R", "Lava refactoring support", "html/Refactoring.htm"))
	  insDoc(aux1, gLnk("R", "Improved comprehensibility of Lava programs", "html/LavaReadability.htm"))
	  insDoc(aux1, gLnk("R", "Object-oriented analysis and design", "html/OOanalysis.htm"))
	  insDoc(aux1, gLnk("R", "Object-Oriented Problem Separation through Many Irreducible Mini-Methods (OOPS:MIMM)", "html/OOPS-MIMM.htm"))
	  insDoc(aux1, gLnk("R", "Design By Contract (TM)", "html/DBC.htm"))
	  insDoc(aux1, gLnk("R", "Strict separation of interfaces and implementations", "html/SepItfImpl.htm"))
	  insDoc(aux1, gLnk("R", "Two object categories: state and value objects", "html/TwoObjectCategories.htm"))
	  insDoc(aux1, gLnk("R", "Lava as a component integration language", "html/Components.htm"))
	  insDoc(aux1, gLnk("R", "Lava supports design patterns/frameworks", "html/PatternsFrameworks.htm"))
	  insDoc(aux1, gLnk("R", "Lava is statically type-safe", "html/TypeSafety.htm"))
	  insDoc(aux1, gLnk("R", "Lava renders &quot;type casts&quot; superfluous", "html/TypeCasts.htm"))
	  insDoc(aux1, gLnk("R", "Single-assignment; imperative vs. logical semantics", "html/SingleAssigLogSem.htm"))
	  insDoc(aux1, gLnk("R", "Read-only Lava constructs and functions", "html/ReadOnly.htm"))
	  insDoc(aux1, gLnk("R", "Stages in the life of Lava objects", "html/ObjectLifeCycle.htm"))
	  insDoc(aux1, gLnk("R", "Comprehensive initialization checks", "html/IniChecks.htm"))
	  insDoc(aux1, gLnk("R", "Pointers, reference counts, storage management", "html/PointersRefCounts.htm"))
	  insDoc(aux1, gLnk("R", "Integrated database expressive means", "html/DatabaseIntegration.htm"))
	  insDoc(aux1, gLnk("R", "Multi-threading, synchronization, transactions", "html/Transactions.htm"))
	  insDoc(aux1, gLnk("R", "The Lava callback concept", "html/Callbacks.htm"))
	  insDoc(aux1, gLnk("R", "Lava projects, packages, programming in the large", "html/Packages.htm"))
	  insDoc(aux1, gLnk("R", "&quot;Private&quot; / &quot;protected&quot; / &quot;friend&quot; counterparts in Lava", "html/ProtectedFriend.htm"))
	  insDoc(aux1, gLnk("R", "Execution modes/environments of Lava applications", "html/ExecEnvironment.htm"))
	  insDoc(aux1, gLnk("R", "The o-o Lava interpreter", "html/Interpreter.htm"))
  aux1 = insFld(foldersTree, gFld("Learning Lava by samples", "html/LavaBySamples.htm"))
	  insDoc(aux1, gLnk("R", "&quot;Hello World&quot;", "html/HelloWorldSample.htm"))
	  insDoc(aux1, gLnk("R", "Forms-oriented user interface", "html/FormSample.htm"))
	  insDoc(aux1, gLnk("R", "Stages in the life of Lava objects", "html/ObjectLifeCycle.htm"))
	  insDoc(aux1, gLnk("R", "State and value objects", "html/StateValueSample.htm"))
	  insDoc(aux1, gLnk("R", "Interfaces and implementations", "html/ItfImplSample.htm"))
	  insDoc(aux1, gLnk("R", "Single-assignment", "html/SingleAssigSample.htm"))
	  insDoc(aux1, gLnk("R", "Private / protected / friend", "html/ProtectedFriendSample.htm"))
	  insDoc(aux1, gLnk("R", "Database access", "html/DatabaseSample.htm"))
  	  aux2 = insFld(aux1, gFld("Replacing loops by recursion", "html/RepetComputSamples.htm"))
	       insDoc(aux2, gLnk("R", "Recursion", "html/RecursionSamples.htm"))
	       insDoc(aux2, gLnk("R", "Cursor/iterator/visitor", "html/CursorVisitorSamples.htm"))
	       insDoc(aux2, gLnk("R", "Quantifiers and select", "html/QuantifierSamples.htm"))
  	  aux2 = insFld(aux1, gFld("Virtual type samples", "html/PatternSamples.htm"))
	       insDoc(aux2, gLnk("R", "Basic virtual type samples", "html/VTSamples.htm"))
	       insDoc(aux2, gLnk("R", "A type is either virtual or non-virtual", "html/IsSelfVirtualSample.htm"))
	       insDoc(aux2, gLnk("R", "Virtual base classes", "html/VirtualBaseSamples.htm"))
	       insDoc(aux2, gLnk("R", "Substitutable types", "html/SubstitutableSamples.htm"))
	       insDoc(aux2, gLnk("R", "Counter - counterexamples", "html/CounterSamples.htm"))
	  insDoc(aux1, gLnk("R", "Component objects", "html/ComponentSamples.htm"))
	  insDoc(aux1, gLnk("R", "Exception handling in Lava", "html/ExceptionSamples.htm"))
	  insDoc(aux1, gLnk("R", "Callback sample", "html/CallbackSample.htm"))
  aux1 = insFld(foldersTree, gFld("How to edit and run Lava programs", "html/BasicEditing.htm"))
	  insDoc(aux1, gLnk("R", "Editing the declaration tree", "html/EditTree.htm"))
	  insDoc(aux1, gLnk("R", "Using the override view", "html/OverrideView.htm"))
	  insDoc(aux1, gLnk("R", "Editing the include file view", "html/EditInclude.htm"))
	  insDoc(aux1, gLnk("R", "Editing executable code", "html/EditExec.htm"))
	  insDoc(aux1, gLnk("R", "Editing form specifications", "html/EditForm.htm"))
  aux1 = insFld(foldersTree, gFld("Property sheets and command help", "html/DialogAndCmdHelp.htm"))
  	  aux2 = insFld(aux1, gFld("Property sheets of Lava entities", "html/PropertySheets.htm"))
	       insDoc(aux2, gLnk("R", "Chain type in forms", "html/dialogs/ChainFormBox.htm"))
	       insDoc(aux2, gLnk("R", "Component object specification", "html/dialogs/CoSpecBox.htm"))
	       insDoc(aux2, gLnk("R", "Enumeration", "html/dialogs/EnumBox.htm"))
	       insDoc(aux2, gLnk("R", "Enumeration item", "html/dialogs/EnumItemBox.htm"))
	       insDoc(aux2, gLnk("R", "Field layout in forms", "html/dialogs/FieldLayoutBox.htm"))
	       insDoc(aux2, gLnk("R", "Function", "html/dialogs/FunctionBox.htm"))
	       insDoc(aux2, gLnk("R", "Function parameter", "html/dialogs/FuncParmBox.htm"))
	       insDoc(aux2, gLnk("R", "Implementation", "html/dialogs/ImplementationBox.htm"))
	       insDoc(aux2, gLnk("R", "Include file", "html/dialogs/IncludeBox.htm"))
	       insDoc(aux2, gLnk("R", "Individual explanatory text items in forms", "html/dialogs/ExplanTextBox.htm"))
	       insDoc(aux2, gLnk("R", "Initiator", "html/dialogs/InitiatorBox.htm"))
	       insDoc(aux2, gLnk("R", "Interface", "html/dialogs/InterfaceBox.htm"))
	       insDoc(aux2, gLnk("R", "Member variable", "html/dialogs/MemVarBox.htm"))
	       insDoc(aux2, gLnk("R", "Menu in form", "html/dialogs/MenuBox.htm"))
	       insDoc(aux2, gLnk("R", "Menu item in form", "html/dialogs/MenuItemBox.htm"))
	       insDoc(aux2, gLnk("R", "Package", "html/dialogs/PackageBox.htm"))
	       insDoc(aux2, gLnk("R", "Set", "html/dialogs/SetBox.htm"))
	       insDoc(aux2, gLnk("R", "Virtual type", "html/dialogs/VirtualTypeBox.htm"))
  	  aux2 = insFld(aux1, gFld("Command help", "html/CommandHelp.htm"))
	       insDoc(aux2, gLnk("R", "About (Help menu)", "html/commands/AboutCommand.htm"))
	       insDoc(aux2, gLnk("R", "Cascade (Window menu)", "html/commands/CascadeCommand.htm"))
	       insDoc(aux2, gLnk("R", "Check and run Lava program", "html/commands/CheckAndRun.htm"))
	       insDoc(aux2, gLnk("R", "Close (File menu)", "html/commands/CloseCommand.htm"))
	       insDoc(aux2, gLnk("R", "Comment", "html/commands/Comment.htm"))
	       insDoc(aux2, gLnk("R", "Context help", "html/commands/ContextHelpCommand.htm"))
	       insDoc(aux2, gLnk("R", "Edit menu commands", "html/commands/EditMenuCommands.htm"))
	       insDoc(aux2, gLnk("R", "File menu commands", "html/commands/FileMenuCommands.htm"))
	       insDoc(aux2, gLnk("R", "Go to declaration", "html/commands/GoToDeclaration.htm"))
	       insDoc(aux2, gLnk("R", "Insert text in form", "html/commands/InsertTextInForm.htm"))
	       insDoc(aux2, gLnk("R", "New component object implementation", "html/commands/NewCOImpl.htm"))
	       insDoc(aux2, gLnk("R", "New component object specification", "html/commands/NewCOSpec.htm"))
	       insDoc(aux2, gLnk("R", "New enumeration", "html/commands/NewEnumeration.htm"))
	       insDoc(aux2, gLnk("R", "New function", "html/commands/NewFunction.htm"))
	       insDoc(aux2, gLnk("R", "New implementation", "html/commands/NewImplementation.htm"))
	       insDoc(aux2, gLnk("R", "New include file", "html/commands/NewInclude.htm"))
	       insDoc(aux2, gLnk("R", "New initiator", "html/commands/NewInitiator.htm"))
	       insDoc(aux2, gLnk("R", "New interface", "html/commands/NewInterface.htm"))
	       insDoc(aux2, gLnk("R", "New member variable or function parameter", "html/commands/NewVariable.htm"))
	       insDoc(aux2, gLnk("R", "New package", "html/commands/NewPackage.htm"))
	       insDoc(aux2, gLnk("R", "New set", "html/commands/NewSet.htm"))
	       insDoc(aux2, gLnk("R", "New virtual type", "html/commands/NewVT.htm"))
	       insDoc(aux2, gLnk("R", "Open form view", "html/commands/OpenFormView.htm"))
	       insDoc(aux2, gLnk("R", "Override a virtual type or class member", "html/commands/Override.htm"))
	       insDoc(aux2, gLnk("R", "Return to reference", "html/commands/ReturnToRef.htm"))
	       insDoc(aux2, gLnk("R", "eyeglasses", "html/commands/Spectacles.htm"))
	       insDoc(aux2, gLnk("R", "Window menu commands", "html/commands/WindowMenuCommands.htm"))
  aux1 = insFld(foldersTree, gFld("Release notes, planned enhancements, desirable contributions", "html/ProjectState.htm"))
	  insDoc(aux1, gLnk("R", "Release notes", "html/ReleaseNotes.htm"))
	  insDoc(aux1, gLnk("R", "Next steps, future plans, desirable contributions", "html/FAQLavaFuture.htm"))
  aux1 = insFld(foldersTree, gFld("Appendix: The cited Morgenstern poems", "html/Poems.htm"))
      insDoc(aux1, gLnk("R", "Korf's Joke", "html/KorfsJoke.htm"))
      insDoc(aux1, gLnk("R", "The Aesthete", "html/Aesthetics.htm"))
      insDoc(aux1, gLnk("R", "The Picket Fence", "html/PicketFence.htm"))
      insDoc(aux1, gLnk("R", "The Snail's Monologue", "html/Snail.htm"))
      insDoc(aux1, gLnk("R", "The Spectacles", "html/KorfSpectacles.htm"))
  insDoc(foldersTree, gLnk("R", "GNU Free Documentation License", "Copyright.htm"))

  aux1 = insFld(foldersTree, gFld("<b><i>Index</i></b>", "javascript:parent.op()"))
      aux2 = insFld(aux1, gFld("Ada", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Advantages of Java</i>", "html/AdvantJava.htm"))
        insDoc(aux2, gLnk("R", "<i>Strict separation of interfaces and implementations</i>", "html/SepItfImpl.htm"))
      insDoc(aux1, gLnk("R", "advantages of java", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "attached assertions", "html/DBC.htm"))
      aux2 = insFld(aux1, gFld("C++", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Advantages of Java</i>", "html/AdvantJava.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm"))
      insDoc(aux1, gLnk("R", "categories of objects", "html/TwoObjectCategories.htm"))
      insDoc(aux1, gLnk("R", "categories: state, value", "html/TwoObjectCategories.htm"))
      insDoc(aux1, gLnk("R", "class notion", "html/SepItfImpl.htm#classNotion"))
      insDoc(aux1, gLnk("R", "code refactoring", "html/Refactoring.htm"))
      aux2 = insFld(aux1, gFld("COM", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Advantages of Java</i>", "html/AdvantJava.htm"))
        insDoc(aux2, gLnk("R", "<i>Advantages of Visual Basic</i>", "html/AdvantVB.htm"))
        insDoc(aux2, gLnk("R", "<i>Strict separation of interfaces and implementations</i>", "html/SepItfImpl.htm"))
      insDoc(aux1, gLnk("R", "combo boxes", "html/StructEdAdvant.htm"))
      aux2 = insFld(aux1, gFld("component objects", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Lava component support</i>", "html/Components.htm"))
        insDoc(aux2, gLnk("R", "<i>Two object categories</i>", "html/TwoObjectCategories.htm#component"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm#components"))
      insDoc(aux1, gLnk("R", "constraint", "html/PointAndClick.htm#constraint"))
      aux2 = insFld(aux1, gFld("CORBA", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Advantages of Java</i>", "html/AdvantJava.htm"))
        insDoc(aux2, gLnk("R", "<i>Strict separation of interfaces and implementations</i>", "html/SepItfImpl.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm#stateValue"))
      insDoc(aux1, gLnk("R", "creation of objects", "html/SepItfImpl.htm#objectCreation"))
      insDoc(aux1, gLnk("R", "DBC", "html/DBC.htm"))
      insDoc(aux1, gLnk("R", "DCE", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "DCOM", "html/AdvantVB.htm"))
      insDoc(aux1, gLnk("R", "declarations, nesting of", "html/LavaReadability.htm#declnesting"))
      insDoc(aux1, gLnk("R", "design by contract(TM)", "html/DBC.htm"))
      aux2 = insFld(aux1, gFld("design patterns", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm#patterns"))
        insDoc(aux2, gLnk("R", "<i>Genericity and Virtual Types</i>", "html/PatternsFrameworks.htm"))
      insDoc(aux1, gLnk("R", "documentation generator", "html/PointAndClick.htm#htmldoc"))
      insDoc(aux1, gLnk("R", "editing executable code", "html/PointAndClick.htm#exec"))
      aux2 = insFld(aux1, gFld("Eiffel", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Genericity and Virtual Types</i>", "html/PatternsFrameworks.htm"))
        insDoc(aux2, gLnk("R", "<i>Generic types in Eiffel</i>", "html/Unsolved.htm#Eiffel1"))
        insDoc(aux2, gLnk("R", "<i>Meyer, B.: Eiffel: The Language. Prentice Hall Europe</i>", "html/TypeSafety.htm#Eiffel"))
        insDoc(aux2, gLnk("R", "<i>Two object categories</i>", "html/TwoObjectCategories.htm#declaration"))
        insDoc(aux2, gLnk("R", "<i>Expanded classes in Eiffel</i>", "html/Unsolved.htm#Eiffel2"))
      insDoc(aux1, gLnk("R", "event handling", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "exec", "html/PointAndClick.htm#exec"))
      aux2 = insFld(aux1, gFld("Eiffel", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Genericity and Virtual Types</i>", "html/PatternsFrameworks.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm#patterns"))
      insDoc(aux1, gLnk("R", "garbage collection", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "IDE", "html/AdvantVB.htm"))
      insDoc(aux1, gLnk("R", "IDL", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "information hiding principle", "html/SepItfImpl.htm#infoHiding"))
      insDoc(aux1, gLnk("R", "inheritance", "html/SepItfImpl.htm"))
      insDoc(aux1, gLnk("R", "inheritance of implementations", "html/SepItfImpl.htm#implInheritance"))
      aux2 = insFld(aux1, gFld("interface", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Strict separation of interfaces and implementations</i>", "html/SepItfImpl.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm#sepItfImpl"))
      insDoc(aux1, gLnk("R", "Java", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "Java Beans", "html/Unsolved.htm#components"))
      insDoc(aux1, gLnk("R", "Java interfaces", "html/SepItfImpl.htm#JavaInterfaces"))
      insDoc(aux1, gLnk("R", "LISP", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "Many Irreducible Mini-Methods (MIMM)", "html/OOPS-MIMM.htm"))
      insDoc(aux1, gLnk("R", "members, private/public", "html/SepItfImpl.htm"))
      insDoc(aux1, gLnk("R", "mixins", "html/SepItfImpl.htm#mixins"))
      aux2 = insFld(aux1, gFld("Modula-2", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Advantages of Java</i>", "html/AdvantJava.htm"))
        insDoc(aux2, gLnk("R", "<i>Strict separation of interfaces and implementations</i>", "html/SepItfImpl.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm#sepItfImpl"))
      insDoc(aux1, gLnk("R", "multiple inheritance in Java", "html/SepItfImpl.htm#multipleInheritance"))
      insDoc(aux1, gLnk("R", "multiple inheritance in Lava", "html/SepItfImpl.htm"))
      insDoc(aux1, gLnk("R", "multiple inheritance versus code generation wizards", "html/LavaReadability.htm#codeGeneration"))
      insDoc(aux1, gLnk("R", "multiple inheritance with shared base classes", "html/SepItfImpl.htm#virtualBaseClasses"))
      insDoc(aux1, gLnk("R", "nesting of declarations", "html/LavaReadability.htm#declnesting"))
      insDoc(aux1, gLnk("R", "Oberon", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "object categories", "html/TwoObjectCategories.htm"))
      insDoc(aux1, gLnk("R", "object creation", "html/SepItfImpl.htm#objectCreation"))
      insDoc(aux1, gLnk("R", "objects, database", "html/TwoObjectCategories.htm#database"))
      aux2 = insFld(aux1, gFld("objects, two categories", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Two object categories</i>", "html/TwoObjectCategories.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm#stateValue"))
      insDoc(aux1, gLnk("R", "object-oriented analysis and design", "html/OOanalysis.htm"))
      insDoc(aux1, gLnk("R", "Object-Oriented Problem Separation through Many Irreducible Mini-Methods (OOPS:MIMM)", "html/OOPS-MIMM.htm"))
      insDoc(aux1, gLnk("R", "OLE", "html/AdvantVB.htm"))
      insDoc(aux1, gLnk("R", "OOPS:MIMM", "html/OOPS-MIMM.htm"))
      insDoc(aux1, gLnk("R", "Pascal", "html/AdvantJava.htm"))
      aux2 = insFld(aux1, gFld("patterns, frameworks", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Genericity and Virtual Types</i>", "html/PatternsFrameworks.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm#patterns"))
      insDoc(aux1, gLnk("R", "pattern language", "html/OOanalysis.htm#patternLanguage"))
      insDoc(aux1, gLnk("R", "pattern parameters", "html/PatternsFrameworks.htm#VT"))
      insDoc(aux1, gLnk("R", "persistent data", "html/TwoObjectCategories.htm#database"))
      insDoc(aux1, gLnk("R", "placeholder", "html/PointAndClick.htm#placeholder"))
      aux2 = insFld(aux1, gFld("point and click", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Lava is perfectly &quot;point-and-click&quot;</i>", "html/PointAndClick.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm"))
      insDoc(aux1, gLnk("R", "RAD", "html/AdvantVB.htm"))
      insDoc(aux1, gLnk("R", "semaphores", "html/Unsolved.htm#synch"))
      insDoc(aux1, gLnk("R", "Smalltalk", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "specialization, collective", "html/PatternsFrameworks.htm"))
      insDoc(aux1, gLnk("R", "SQL", "html/Unsolved.htm#SQL"))
      insDoc(aux1, gLnk("R", "state object (&quot;service&quot;)", "html/TwoObjectCategories.htm"))
      aux2 = insFld(aux1, gFld("textual programming", "javascript:parent.op()"))
        insDoc(aux2, gLnk("R", "<i>Lava is perfectly &quot;point-and-click&quot;</i>", "html/PointAndClick.htm"))
        insDoc(aux2, gLnk("R", "<i>Urgent problems in current programming languages</i>", "html/Unsolved.htm"))
      insDoc(aux1, gLnk("R", "textual syntax of Lava", "html/PointAndClick.htm#textualSyntax"))
      insDoc(aux1, gLnk("R", "threads", "html/AdvantJava.htm"))
      insDoc(aux1, gLnk("R", "transactions", "html/Unsolved.htm#synch"))
      insDoc(aux1, gLnk("R", "two categories of objects", "html/TwoObjectCategories.htm"))
      insDoc(aux1, gLnk("R", "type-safety of Lava", "html/TypeSafety.htm"))
      insDoc(aux1, gLnk("R", "Unsolved problems", "html/Unsolved.htm"))
      insDoc(aux1, gLnk("R", "value object (&quot;structure&quot;)", "html/TwoObjectCategories.htm"))
      insDoc(aux1, gLnk("R", "virtual base classes", "html/SepItfImpl.htm#virtualBaseClasses"))
      insDoc(aux1, gLnk("R", "virtual types", "html/PatternsFrameworks.htm"))
      insDoc(aux1, gLnk("R", "Visual Basic", "html/AdvantVB.htm"))
