#
# Standard Project Makefile

# ------------------------------------------------------------------------------
# Including the generated make support files
#
#     Include the file 'macros.incl'
#     which defines all project dependent SNIFF_* macros.
include .sniffdir/macros.incl
#
# ------------------------------------------------------------------------------
# The SNIFF_* macros are defined in the Make View of the Project Attributes
#
#     Include directive(s)
#SNIFF_INCLUDE       += <Additional Includes>
#
#     Executable target
LINK_TARGET       = $(SNIFF_LINK_TARGET)
#
#     Relinkable object target
RELINK_TARGET     = $(SNIFF_RELINK_TARGET)
#
#     Library target
LIB_TARGET        = $(SNIFF_LIB_TARGET)
#
#     Shared Library target
SHAREDLIB_TARGET        = $(SNIFF_SHAREDLIB_TARGET)
#
#     Libraries received from subprojects. 
#     Linked to the project's executable and relinkable object targets.
SUB_LIBS          = $(SNIFF_SUB_LIBS)
#
#     Relinkable objects received from subprojects. 
#     Linked to the project's executable, relinkable object and library targets.
SUB_RELINK_OFILES = $(SNIFF_SUB_RELINK_OFILES)
#
#     Other libraries linked to the project's executable target
OTHER_LIBS        = $(SNIFF_OTHER_LIBS)
#     Other libraries linked to the project's relinkable object target
RELINK_OTHER_LIBS = $(SNIFF_RELINK_OTHER_LIBS)
#
#     Recursive make directories
SUBDIRS           = $(SNIFF_SUBDIRS)

# ------------------------------------------------------------------------------
# You can define some additional files which will also be deleted using the
# clean help target.
#     The clean help target deletes the core $(SNIFF_TARGETS) $(OFILES) $(OTHER_OFILES) 
#     $(OTHER_CLEANUP) files
#OTHER_CLEANUP = 
# You can define some other object files which are not in the project,
#     but should be also linked to the project's targets.
#OTHER_OFILES =

# Macros to activate the purify and quantify rules in the general.link.mk
#PURIFY_TARGET = $(LINK_TARGET).purify
#QUANTIFY_TARGET = $(LINK_TARGET).quantify
# You can define the following project dependent additional flags. These
# flags do not override any other flags defined by SNiFF+'s Make Support.
#
#OTHER_CFLAGS        = <project dependent additional C compiler flags>
#OTHER_CXXFLAGS      = <project dependent additional C++ compiler flags>
#OTHER_YACCFLAGS     = <project dependent additional yacc compiler flags>
#OTHER_LEXFLAGS      = <project dependent additional lex flags>
#OTHER_IDLFLAGS      = <project dependent additional IDL compiler flags>
#OTHER_FFLAGS        = <project dependent additional fortran compiler flags>
#OTHER_LDFLAGS       = <project dependent additional linker flags>
#OTHER_PURIFYFLAGS   = <project dependent additional purify flags>
#OTHER_QUANTIFYFLAGS = <project dependent additional quantify flags>
#OTHER_PROCFLAGS     = <project dependent additional proc flags>
#OTHER_JFLAGS        = <project dependent additional java compiler flags>
#OTHER_JARFLAGS      = <project dependent additional java archive flags>
#OTHER_UXCGFLAGS     = <project dependent additional UIM/X code generator flags>
#OTHER_ILBFLAGS      = <project dependent additional additional Ilog Broker preprocessor flags>
#OTHER_WINRCFLAGS	 = <project dependent additional Windows Resource compiler flags>

# PRE_TARGETS is currently used only for IDL make support. 
#     The PRE_TARGETS macro is needed, since it is not possible to create a
#     simple implicit rule to compile the IDL files
#PRE_TARGETS     = $(SNIFF_PRE_TARGETS)

# IDL-specific macro
#     "S" stands for server and "C" for client
#IDL_CFILE_TYPE_SPEC = <S|C>

# Common make macro and rule definitions
#     The general.mk is split into several files. This files are:
#       $(SNIFF_DIR)/make_support/general.link.mk      # rules for executable, relinkable object and library
#       $(SNIFF_DIR)/make_support/general.c.mk         # rules for c, c++ and pro*c/c++ compilers
#       $(SNIFF_DIR)/make_support/general.fortran.mk   # rule for fortran compiler
#       $(SNIFF_DIR)/make_support/general.idl.mk       # rule for idl compiler
#       $(SNIFF_DIR)/make_support/general.yl.mk        # rules for yacc and lex
#     Sniff generates the necessary includes in the Make Support File 'include.incl'
#     based on the file types in the project.
#     Therefore only the required Language Makefiles are included by the
#     include.incl file.
include $(SNIFF_DIR)/make_support/general.mk
# Include the generated Make Support File 'dependencies.incl'.
include $(SNIFF_MAKEDIR)/$(SNIFF_DEPEND_INCL)

# ------------------------------------------------------------------------------
#     The following macros can only be set after including general.mk.
#     These macros override identically-named macros in general.mk
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Configuration macros for YACC
#
#     The generated c and h files (y.tab.*) will be renamed to basename.* after
#     the yacc compilation.
#     You should set the macro to 0, if you use the generated header file in the
#     format of 'y.tab.h'.
#     Default 1, renaming the generated y.tab.h to basename.h
#YACC_MOVE_HEADER  = 0

#     This macro defines which compiler (C or C++) should compile the
#     generated yacc file.
#     The default suffix is defined in the platform makefile.
#YACC_CFILE_SUFFIX = <c|C>

# Configuration macro for LEX
#
#     This macro defines which compiler (C or C++) should compile the
#     generated lex file.
#     The default suffix is defined in the platform makefile.
#LEX_CFILE_SUFFIX  = <c|C>

# You should set this macro if you use another project makefile name as
#     'Makefile'. (For example MAKEFILE=Makefile.client)
#MAKEFILE = <name of this project makefile>

# ------------------------------------------------------------------------------
# You can create and use a working enviroment dependent makefile. The makefile
#     can defines some special rules and macros for all projects in this working
#     environment.
#
# If you use working enviroment dependent makefile, disactivate the include
# statement from above in which general.mk was included. Then activate the
# following include statement, which includes the working enviroment dependent makefile
# (which we call common.mk). Note that common.mk must be stored in the working environment
# root directory.
# NOTE:	You have to include general.mk in your common.mk 
#		to ensure Common make macro and rule definitions are done properly.
#include $(SNIFF_RELATIVE_ROOT_DIR)/common.mk
#
# Here is a example of a working enviroment dependent makefile called common.mk.
# 
#------ BEGIN of common.mk ------
#
# You can define the following working environment dependent additional flags:
# These flags do not override any other flags defined by SNiFF+'s Make Support.
#
#COMMON_CFLAGS        = <working environment dependent additional C compiler flags>
#COMMON_CXXFLAGS      = <working environment dependent additional C++ compiler flags>
#COMMON_YACCFLAGS     = <working environment dependent additional yacc compiler flags>
#COMMON_LEXFLAGS      = <working environment dependent additional lex flags>
#COMMON_IDLFLAGS      = <working environment dependent additional IDL flags>
#COMMON_FFLAGS        = <working environment dependent additional fortran flags>
#COMMON_LDFLAGS       = <working environment dependent additional linker flags>
#COMMON_PURIFYFLAGS   = <working environment dependent additional purify flags>
#COMMON_QUANTIFYFLAGS = <working environment dependent additional quantify flags>
#COMMON_PROCFLAGS     = <working environment dependent additional proc flags>
#COMMON_JFLAGS        = <working environment dependent additional java compiler flags>
#COMMON_JARFLAGS      = <working environment dependent additional java archive flags>
#COMMON_UXCGFLAGS     = <working environment dependent additional UIM/X code generator flags>
#COMMON_ILBFLAGS      = <working environment dependent additional Ilog Broker preprocessor flags>
#COMMON_WINRCFLAGS      = <working environment dependent additional Windows Resource compiler flags>
#
#include $(SNIFF_DIR)/make_support/general.mk
#------ END of common.mk ------

# ------------------------------------------------------------------------------
# The following macros are platform dependent macros defined in the platform
#     makefiles.
#     The macros can be redefined in the project makefile, but this is not
#     recommended.
#     The project makefile should always remain platform independent.
#SYSTEM    = <Unix|Windows>
#SHELL     = <shell command>
#RM        = <remove command>
#RM_INTACT = <interactive remove command>
#MV        = <move command>
#MK_DIR    = <create dir command>
#MK_SYMLINK= <symbolic link command>
#CP        = <copy command>
#CLASSPATH_DELIMITER = <path delimiter character in CLASSVPATH macro>
#D         = <path delimiter character in SNIFF_Sh... macros>
#DEVNULL   = <null device>
#SYSTYPE          = <platform dependent general flags>
#OS_CFLAGS        = <platform dependent OS C compiler flags>
#OS_CXXFLAGS      = <platform dependent OS C++ compiler flags>
#OS_YACCFLAGS     = <platform dependent OS yacc compiler flags>
#OS_LEXFLAGS      = <platform dependent OS lex flags>
#OS_IDLFLAGS      = <platform dependent OS IDL flags>
#OS_FFLAGS        = <platform dependent OS fortran flags>
#OS_LDFLAGS       = <platform dependent OS linker flags>
#OS_PURIFYFLAGS   = <platform dependent OS purify flags>
#OS_QUANTIFYFLAGS = <platform dependent OS quantify flags>
#OS_PROCFLAGS     = <platform dependent OS proc flags>
#OS_JFLAGS        = <platform dependent OS java flags>
#OS_JARFLAGS      = <platform dependent OS java archive flags>
#OS_UXCGFLAGS     = <platform dependent OS UIM/X code generator flags>
#OS_ILBFLAGS      = <platform dependent OS Ilog Broker preprocessor flags>
#OS_LIBS          = <platform dependent OS libraries>
#OS_WINRCFLAGS	  = <platform dependent OS WINRC flags>
#
#CXX       = <C++ compiler>
#CXXFLAGS  = <C++ compiler flags>
#OVERALL_OPTION_CXX        = 
#CC        = <C compiler>
#CFLAGS    = <C compiler flags>
#OVERALL_OPTION_CC         =
#LINK      = <linker>
#LD        = <linker for relocatable object files>
#LDFLAGS   = <linker flags>
#AR        = <ar program>
#ARFLAGS   = <ar flags>
#LD_SHARED  = <ar program for shared libs>
#LD_SHAREDFLAGS    = <ar flags for shared libs>
#RANLIB    = <ranlib program>
#YACC              = <yacc compiler>
#YACCFLAGS         = <yacc compiler flags>
#LEX               = <lex program>
#LEXFLAGS          = <lex flags>
#IDL               = <IDL compiler>
#IDLFLAGS          = <IDL compiler flags>
#IDL_CFILE_SUFFIX  = <suffix of the generated file of IDL>
#FC        = <fortran compiler>
#FFLAGS    = <fortran compiler flags>
#OVERALL_OPTION_FC         =
#JAVAC              = <java compiler>
#JAVA_DEPEND_FLAG   = 
#JFLAGS             = <java flags>
#JAR              = <java archive>
#JARFLAGS           = <java archive flags>
#JAVA_INTERPRETER   = <java interpreter>
#JAVA_APPLET_VIEWER = <applet viewer>
#PURIFY        = <purify executable>
#PURIFYFLAGS   = <purify program>
#QUANTIFY      = <quantify program>
#QUANTIFYFLAGS = <quantify flags>
#ILB       = <ilog broker preprocessor>
#ILBFLAGS  = <ilog broker flags>
#ILV_LIBS  = <ilog view libraries>
#UXCG      = <UIM/X code generator>
#UXCGFLAGS = <UIM/X code generator flags>
#PROC      = <Oracle ProC preprocessor>
#PROCFLAGS = <Oracle ProC preprocessor flags>

# ------------------------------------------------------------------------------
# The following macros are defined in the general.mk makefile or Language Makefiles. 
#     The macros should be used as command line parameters of the make command.
#     For example: make CHECK_UPDATE=0 DEBUG=1
#CHECK_UPDATE        = <Default 1, Command line flag to check the up-to-date status of the working environment>
#DEBUG               = <Default 0, Command line flag to print debug messages>
#FORCE               = <Default 0, Command line forcement flag>

# ------------------------------------------------------------------------------
# The following macros are common macros defined in the Language Makefiles
#     The macros can be redefined in the project makefile, but this is not
#     recommended.
#     The macros are used internally by rules in the Language Makefiles. 
#
#     Macros defined in the general.mk
#CLEANUP             = <Files affected by cleanup>
#SNIFF_SYMBOL_DIR    = <Directory of the symbol information files>
#
#     Macros defined in the general.c.mk
#ALL_CXXFLAGS        = <All C++ compiler flags>
#CXX_CMD             = <C++ compiler command>
#ALL_CFLAGS          = <All C compiler flags>
#CC_CMD              = <C compiler command>
#ALL_PROCFLAGS       = <All proc flags>
#PROC_CMD            = <Oracle ProC command>

#     Macros defined in the general.fortran.mk
#ALL_FFLAGS          = <All fortran compiler flags>
#FORTRAN_CMD         = <fortran compiler command>

#     Macros defined in the general.idl.mk
#ALL_IDLFLAGS        = <All IDL compiler flags>
#IDL_CMD             = <IDL compiler command>
#IDL_CXX_CMD         = <C++ compiler command for generated files of IDL compiler>

#     Macros defined in the general.ilog.mk
#ALL_ILBFLAGS        = <All Ilog Broker preprocessor flags>
#ILB_CMD             = <Ilog Broker preprocessor command>

#     Macros defined in the general.java.mk
#ALL_JFLAGS              = <All java compiler flags>
#ALL_JARFLAGS            = <All java archive flags>
#JAVA_CMD                = <Java compiler command with using of generate dir parameter>
#JAVA_CMD_WITHOUT_GENDIR = <Java compiler command without using of generate dir parameter>
#SNIFF_CLASSPATH         = <Java CLASSPATH variable>

#     Macros defined in the general.link.mk
#ALL_LDFLAGS         = <All linker flags>
#LINK_CMD            = <command to link an executable>
#RELINK_CMD          = <command to make a relocatable object>
#LIB_CMD             = <command to make library>
#SHAREDLIB_CMD       = <command to make shared library>
#ALL_PURIFYFLAGS     = <All purify flags>
#PURIFY_CMD          = <Purify command>
#ALL_QUANTIFYFLAGS   = <All quantify flags>
#QUANTIFY_CMD        = <Quantify command>

#     Macros defined in the general.obsolete.mk
#UPDATE_FILE_SH      = <Shell script to update file with respect to the repository and the shared workspaces>

#     Macros defined in the general.uimx.mk
#ALL_UXCGFLAGS       = <All UIM/X code generator flags>
#UXCG_CMD            = <UIM/X code generator command>
#UXCG_CXX_CMD        = <C++ command to compile generated files>

#     Macros defined in the general.yl.mk
#ALL_YACCFLAGS       = <All yacc compiler flags>
#YACC_CMD            = <yacc compiler command>
#YACC_CXX_CMD        = <C++ compiler command to compile yacc generated file>
#YACC_CC_CMD         = <C compiler command to compile yacc generated file>
#ALL_LEXFLAGS        = <All lex flags>
#LEX_CMD             = <lex command>
#LEX_CXX_CMD         = <C++ compiler command to compile lex generated file>
