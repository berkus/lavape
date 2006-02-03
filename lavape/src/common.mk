SHELL=/usr/bin/env sh

DBG=-g

ph_files=$(wildcard *.ph)
h_ph_files=$(ph_files:.ph=.h)
cpp_ph_files=$(ph_files:.ph=G.cpp)
o_ph_files=$(cpp_ph_files:.cpp=.o)

ui_files=$(wildcard *.ui)
h_ui_files1=$(ui_files:.ui=.h)
h_ui_files=$(addprefix Generated/,$(h_ui_files1))
cpp_ui_files1=$(ui_files:.ui=.cpp)
cpp_ui_files=$(addprefix Generated/,$(cpp_ui_files1))
o_ui_files=$(cpp_ui_files:.cpp=.o)

moc_ui_files=$(addprefix Generated/moc_,$(cpp_ui_files1))
o_moc_ui_files=$(moc_ui_files:.cpp=.o)

cpp_files=$(wildcard *.cpp)
c_files=$(wildcard *.c)
o_files=$(cpp_files:.cpp=.o) $(c_files:.c=.o)

basenames_mocable=$(basename $(MOCABLES))
basenames_mocable_G=$(addprefix Generated/moc_,$(basenames_mocable))
moc_mocable_G=$(addsuffix .cpp,$(basenames_mocable_G))
o_mocable_G=$(moc_mocable_G:.cpp=.o)

gen_files=$(sort $(h_ph_files) $(h_ui_files) $(cpp_ph_files) $(cpp_ui_files) $(moc_ui_files) $(moc_mocable_G))
all_o_files=$(sort $(o_files) $(o_ph_files) $(o_ui_files) $(o_moc_ui_files) $(o_mocable_G))

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

ifeq ($(OPSYS),Darwin)
  DLLSUFFIX = .dylib
  OSDLLFLAGS = -undefined suppress -flat_namespace -dynamiclib -single_module -framework Carbon -framework QuickTime -lz -framework OpenGL -framework AGL
  OSCPPFLAGS = -D__$(OPSYS) $(DBG)
  ifeq ($(PRJ),SFLsockets)
    CC = cc
  else
    CC = c++
  endif
else
  DLLSUFFIX = .so
  OSDLLFLAGS = -shared $(SONAME)lib$(EXEC2) $(RPATH)$(LAVADIR)/lib $(RPATH)$(QTDIR)/lib
  OSEXECFLAGS = -fstack-check $(RPATH)$(LAVADIR)/lib $(RPATH)$(QTDIR)/lib
  ifeq ($(PRJ),SFLsockets)
    CC = gcc
  else
    CC = g++
  endif
  ifeq ($(OPSYS),SunOS)
    OSCPPFLAGS = -fPIC -D__$(OPSYS) $(DBG)
  else
    OSCPPFLAGS = -D__$(OPSYS) $(DBG) -DQT3_SUPPORT
  endif
endif

asscli=-lqassistantclient
ifeq ($(QTDIR),)
QTDIR=/usr/lib/qt3
endif

#all: this
rec_make: $(make_subpro) this

ifeq ($(suffix $(EXEC)),.so)
EXEC2 = $(addsuffix $(DLLSUFFIX),$(basename $(EXEC)))
this: ../../lib/lib$(EXEC2)
../../lib/lib$(EXEC2): $(LINKS) $(gen_files) $(PCH_TARGET) $(all_o_files)
	$(CC) -o ../../lib/lib$(EXEC2) $(OSDLLFLAGS) $(all_o_files) -L../../lib -L$(QTDIR)/lib -lqt-mt $(addprefix -l,$(SUBPRO)) $(OSLIBFLAGS) -lc
else
this: ../../bin/$(EXEC)
../../bin/$(EXEC): $(gen_files) $(PCH_TARGET) $(all_o_files) $(addprefix ../../lib/,$(addprefix lib,$(addsuffix $(DLLSUFFIX),$(SUBPRO))))
	$(CC) -o ../../bin/$(EXEC) $(OSEXECFLAGS) $(all_o_files) -L../../lib -L$(QTDIR)/lib $(addprefix -l,$(SUBPRO)) -lqassistantclient -lqt-mt $(OSLIBFLAGS) -lpthread -lc
endif

.cpp.o:
	$(CC) -c -pipe -MMD $(PCH_WARN) $(OSCPPFLAGS) -D$(OSCAT) -DQT_THREAD_SUPPORT $(CPP_FLAGS) $(PCH_INCL) $(CPP_INCLUDES) -o $@ $<
#	$(CC) -c -pipe -g -fPIC -MMD -H -Winvalid-pch -D_REENTRANT -D__UNIX__ -DQT_THREAD_SUPPORT $(CPP_FLAGS) -include PCH/$(PRJ)_all.h $(incl_subpro) $(CPP_INCLUDES) -o $@ $<

.c.o:
	$(CC) -c -pipe -MMD $(PCH_WARN) $(OSCPPFLAGS) -D$(OSCAT) $(CPP_FLAGS) $(PCH_INCL) $(CPP_INCLUDES) -o $@ $<
#	$(CC) -c -pipe -g -fPIC -MMD -Winvalid-pch -D_REENTRANT $(CPP_FLAGS) -include PCH/$(PRJ)_all.h $(CPP_INCLUDES) -o $@ $<

PCH/$(PRJ)_all.h.gch: $(PRJ)_all.h
	if [ ! -e PCH ] ; then mkdir PCH; fi; $(CC) -c -pipe -MMD -Winvalid-pch -D$(OSCAT) -DQT_THREAD_SUPPORT $(OSCPPFLAGS) $(CPP_FLAGS) $(CPP_INCLUDES) -o $@ $(PRJ)_all.h

# UIC rules; use "sed" to change minor version of ui files to "0":
# prevents error messages from older Qt3 UIC's
Generated/%.h: %.ui
	( grep -q -e 'UI version=\"[0-9]\+\.0\"' $< || \
	  sed -i -e 's/\(UI version=\"[0-9]\+\.\)[0-9]\+"/\10\"/' $<; ); \
  export LD_LIBRARY_PATH=/usr/X11R6/bin;$(QTDIR)/bin/uic $< -o $@
Generated/%.cpp: %.ui
	( grep -q -e 'UI version=\"[0-9]\+\.0\"' $< || \
	  sed -i -e 's/\(UI version=\"[0-9]\+\.\)[0-9]\+"/\10\"/' $< ); \
  $(QTDIR)/bin/uic -impl $*.h $< -o $@; \
  sed -i -e 's/static const unsigned char const/static const unsigned char/' $@

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
	../../bin/LPC -I. -I../disco $(impex) $<

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
