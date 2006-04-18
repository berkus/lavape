#SHELL=/usr/bin/env sh
SHELL=/bin/sh

DBG=-g

ph_files=$(wildcard *.ph)
h_ph_files=$(ph_files:.ph=.h)
cpp_ph_files=$(ph_files:.ph=G.cpp)
o_ph_files=$(cpp_ph_files:.cpp=.o)

ui_files=$(wildcard *.ui)
h_ui_files1=$(ui_files:.ui=.h)
h_ui_files=$(addprefix Generated/,$(h_ui_files1))
#cpp_ui_files1=$(ui_files:.ui=.cpp)
#cpp_ui_files=$(addprefix Generated/,$(cpp_ui_files1))
#o_ui_files=$(cpp_ui_files:.cpp=.o)

#moc_ui_files=$(addprefix Generated/moc_,$(cpp_ui_files1))
#o_moc_ui_files=$(moc_ui_files:.cpp=.o)

rc_file=$(QRC)
cpp_rc_file=$(rc_file:.qrc=.cpp)
o_rc_file=$(rc_file:.qrc=.o)

cpp_files0=$(wildcard *.cpp)
cpp_files=$(filter-out %_all.cpp,$(cpp_files0))
c_files=$(wildcard *.c)
o_files=$(cpp_files:.cpp=.o) $(c_files:.c=.o)

basenames_mocable=$(basename $(MOCABLES))
basenames_mocable_G=$(addprefix Generated/moc_,$(basenames_mocable))
moc_mocable_G=$(addsuffix .cpp,$(basenames_mocable_G))
o_mocable_G=$(moc_mocable_G:.cpp=.o)

gen_files=$(sort $(h_ph_files) $(h_ui_files) $(cpp_ph_files) $(moc_mocable_G) $(cpp_rc_file))
all_o_files=$(sort $(o_files) $(o_ph_files) $(o_mocable_G) $(o_rc_file))

gen_files=$(sort $(h_ph_files) $(h_ui_files) $(cpp_ph_files) $(moc_mocable_G) $(cpp_rc_file))
all_o_files=$(sort $(o_files) $(o_ph_files) $(o_mocable_G) $(o_rc_file))

make_subpro=$(addsuffix .rec,$(SUBPRO))
clean_subpro=$(addsuffix .cln,$(SUBPRO))
incl_subpro=$(addprefix -include ,$(addsuffix _all.h,$(SUBPRO)))

ifeq ($(LAVADIR),)
LAVADIR = $(shell cd ../..; pwd)
endif

ifeq ($(shell cd $(LAVADIR)/src; ./testPCHsupport.sh),yes)
PCH_INCL = -include PCH/$(PRJ)_all.h
PCH_TARGET = PCH/$(PRJ)_all.h.gch
PCH_WARN = -Winvalid-pch
endif

OPSYS = $(shell uname)

ifeq ($(OPSYS),SunOS)
  RPATH = -Wl,-R,
  SONAME = -h
  OSLIBFLAGS = -lsocket
else
  RPATH = -Wl,-rpath,
  SONAME = -Wl,-soname=
endif

ifeq ($(OPSYS),MINGW32_NT-5.1)
  OPSYS = MINGW32
  OSCAT = WIN32
  LN = cp
else
  OSCAT = __UNIX__
  LN = ln -s
endif

CC = g++

ifeq ($(OPSYS),Darwin)
  DLLSUFFIX = .dylib
  OSDLLFLAGS = -undefined suppress -flat_namespace -dynamiclib -single_module -framework Carbon -framework QuickTime -lz -framework OpenGL -framework AGL
  OSCPPFLAGS = -D__$(OPSYS) $(DBG)
  CC = c++
else
  DLLSUFFIX = .so
  ifeq ($(OPSYS),MINGW32)
    DLLNAME = $(addsuffix .dll,$(basename $(EXEC)))
    IMPLIB = -Wl,--out-implib,../../lib/lib$(addsuffix .a,$(basename $(EXEC)))
    OSDLLFLAGS = -shared
    OSEXECFLAGS = -fstack-check
    EXEC2 = $(EXEC).exe
    ifneq ($(DBG),)
	  QtS = d4
	  ACL = d
	else
	  QtS = 4
	  ACL =
	endif
  else
    DLLNAME = lib$(addsuffix .so,$(basename $(EXEC)))
    OSDLLFLAGS = -shared $(SONAME)lib$(EXEC) $(RPATH)$(LAVADIR)/lib $(RPATH)$(QTDIR)/lib
    OSEXECFLAGS = -fstack-check $(RPATH)$(LAVADIR)/lib $(RPATH)$(QTDIR)/lib
    EXEC2 = $(EXEC)
  	QtS = _debug
	ACL = _debug
  endif
  ifeq ($(OPSYS),SunOS)
    OSCPPFLAGS = -fPIC -D__$(OPSYS) $(DBG) -DQT3_SUPPORT
  else
    OSCPPFLAGS = -D__$(OPSYS) $(DBG) -DQT3_SUPPORT
  endif
endif

ALL_CPP_INCLUDES = $(CPP_INCLUDES) -I$(QTDIR)/include -I$(QTDIR)/include/Qt -I$(QTDIR)/include/QtCore -I$(QTDIR)/include/QtGui -I$(QTDIR)/include/QtNetwork -I$(QTDIR)/include/Qt3Support

asscli=-lqassistantclient
ifeq ($(QTDIR),)
QTDIR=/usr/lib/qt3
endif

#all: this
rec_make: $(make_subpro) this

ifeq ($(suffix $(EXEC)),.so)
this: ../../lib/$(DLLNAME)
../../lib/$(DLLNAME): $(LINKS) $(gen_files) $(PCH_TARGET) $(all_o_files)
	$(CC) -o ../../lib/$(DLLNAME) $(IMPLIB) $(OSDLLFLAGS) $(all_o_files) -L../../lib -L$(QTDIR)/lib  -lQtAssistantClient$(ACL) -lQt3Support$(QtS) -lQtCore$(QtS) -lQtGui$(QtS) -lQtNetwork$(QtS) -mt $(addprefix -l,$(SUBPRO)) $(OSLIBFLAGS)
else
  ifeq ($(OPSYS),MINGW32)
this: ../../bin/$(EXEC2)
../../bin/$(EXEC2): $(gen_files) $(PCH_TARGET) $(all_o_files) $(addprefix ../../lib/,$(addsuffix .dll,$(SUBPRO)))
	$(CC) -o ../../bin/$(EXEC2) $(OSEXECFLAGS) $(all_o_files) -L../../lib -L$(QTDIR)/lib $(addprefix -l,$(SUBPRO)) -lQtAssistantClient$(ACL) -lQt3Support$(QtS) -lQtCore$(QtS) -lQtGui$(QtS) -lQtNetwork$(QtS) $(OSLIBFLAGS)
  else
this: ../../bin/$(EXEC2)
../../bin/$(EXEC2): $(gen_files) $(PCH_TARGET) $(all_o_files) $(addprefix ../../lib/,$(addprefix lib,$(addsuffix $(DLLSUFFIX),$(SUBPRO))))
	$(CC) -o ../../bin/$(EXEC2) $(all_o_files) $(OSEXECFLAGS) -L../../lib $(addprefix -l,$(SUBPRO)) -L$(QTDIR)/lib -lQtAssistantClient$(ACL) -lQt3Support$(QtS) -lQtCore$(QtS) -lQtGui$(QtS) -lQtNetwork$(QtS) $(OSLIBFLAGS)
  endif
endif

.cpp.o:
	$(CC) -c -pipe -MMD $(PCH_WARN) $(OSCPPFLAGS) -D$(OSCAT) -DQT_THREAD_SUPPORT $(CPP_FLAGS) $(PCH_INCL) $(ALL_CPP_INCLUDES) -o $@ $<

.c.o:
	$(CC) -c -pipe -MMD $(PCH_WARN) $(OSCPPFLAGS) -D$(OSCAT) $(CPP_FLAGS) $(PCH_INCL) $(ALL_CPP_INCLUDES) -o $@ $<

PCH/$(PRJ)_all.h.gch: $(PRJ)_all.h
	if [ ! -e PCH ] ; then mkdir PCH; fi; $(CC) -c -pipe -MMD -Winvalid-pch -D$(OSCAT) -DQT_THREAD_SUPPORT $(OSCPPFLAGS) $(CPP_FLAGS) $(ALL_CPP_INCLUDES) -o $@ $(PRJ)_all.h

# UIC rules; use "sed" to change minor version of ui files to "0":
# prevents error messages from older Qt3 UIC's
Generated/%.h: %.ui
	$(QTDIR)/bin/uic $< -o $@
#	( grep -q -e 'UI version=\"[0-9]\+\.0\"' $< || \
#	  sed -i -e 's/\(UI version=\"[0-9]\+\.\)[0-9]\+"/\10\"/' $<; ); \
#  export LD_LIBRARY_PATH=/usr/X11R6/bin;
#Generated/%.cpp: %.ui
#	( grep -q -e 'UI version=\"[0-9]\+\.0\"' $< || \
#	  sed -i -e 's/\(UI version=\"[0-9]\+\.\)[0-9]\+"/\10\"/' $< ); \
#  $(QTDIR)/bin/uic -impl $*.h $< -o $@; \
#  sed -i -e 's/static const unsigned char const/static const unsigned char/' $@

#MOC rule
moc_%.cpp: %.h
	$(QTDIR)/bin/moc $< -o $@
Generated/moc_%.cpp: %.h
	$(QTDIR)/bin/moc $< -o $@

#LPC rule:
ifneq ($(DLL),)
impex = -X $(DLL)
endif

%.h %G.cpp: %.ph
	../../bin/LPC -I. -I../LavaBase $(impex) $<

%.cpp: %.qrc
	$(QTDIR)/bin/rcc -o $@ $<

ifeq ($(suffix $(EXEC)),)
run:
	$(EXEC)&
debug:
	/opt/insight-6.0/gdb/insight $(EXEC) &
endif

%.rec:
	cd $(LAVADIR)/src/$(basename $@) && $(MAKE) this

%.cln:
	cd $(LAVADIR)/src/$(basename $@) && $(MAKE) clean

clean:
	rm -rf *.o *.d PCH/*.gch PCH/*.d Generated/*.o Generated/*.d
#	rm -rf *.o *.d Generated/*.o Generated/*.d *.d Generated/*.cpp Generated/*.h $(gen_files)

cleanall: $(clean_subpro) clean


# Include the generated dependency files if they exist already
dependencies=$(wildcard *.d Generated/*.d PCH/*.d)

ifneq ($(dependencies),)
include $(dependencies)
endif
