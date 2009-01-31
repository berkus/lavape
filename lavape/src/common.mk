#to build a release version set REL
ifeq ($(REL),)
ifeq ($(OPSYS),Darwin)
DBG=-gdwarf-2
else
DBG=-gstabs+
endif
endif


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

gen_files=$(sort $(h_ph_files) $(cpp_ph_files) $(cpp_rc_file) $(moc_mocable_G))
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

ifeq ($(OPSYS),SunOS)
  RPATH = -Wl,-R,
  SONAME = -h
  OSLIBFLAGS = -lsocket
else
  RPATH = -Wl,-rpath,
  SONAME = -Wl,-soname=
endif

CC = g++

QTOOLS = $(QBIN)

ifeq ($(OPSYS),Darwin)
  DLLPREFIX = lib
  DLLSUFFIX = .dylib
  DLLNAME = lib$(addsuffix .dylib,$(basename $(EXEC)))
#  OSDLLFLAGS = -dynamiclib -Wl,-headerpad_max_install_names 
  OSDLLFLAGS = -dynamiclib -Wl,-headerpad_max_install_names -Wl,-install_name,@executable_path/../lib/$(DLLNAME)
  OSEXECFLAGS = -fstack-check -Wl,-headerpad_max_install_names
  OSCPPFLAGS = -D__$(OPSYS)
#  OSLIBFLAGS = -F/System/Library/Frameworks -framework AppKit -framework Carbon
  EXEC2 = $(EXEC)
#  CC = c++
	ifeq ($(QLIB),/Library/Frameworks)
	  QFRW = .framework/Headers
#	  QTOOLS = /usr/bin
#	else
#	  QTOOLS = $(QBIN)
	endif
else
  ifeq ($(OPSYS),MINGW32)
    DLLSUFFIX = .dll
    DLLNAME = $(addsuffix .dll,$(basename $(EXEC)))
    IMPLIB = -mthreads -Wl,--out-implib,../../lib/lib$(addsuffix .a,$(basename $(EXEC)))
#    IMPLIB = -mthreads -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc -Wl,--out-implib,../../bin/lib$(addsuffix .a,$(basename $(EXEC)))
	  OSCPPFLAGS = -D__$(OPSYS) -frtti -fexceptions
    OSDLLFLAGS = -shared
    OSEXECFLAGS = -fstack-check
    EXEC2 = $(EXEC).exe
    ifneq ($(DBG),)
      QtS = d4
      ACL = d4
    else
      QtS = d4
      ACL = d4
    endif
  else
    DLLPREFIX = lib
    ifeq ($(OPSYS),SunOS)
      OSCPPFLAGS = -fPIC -D__$(OPSYS) -fno-strict-aliasing
    else
#      OSCPPFLAGS = -D__$(OPSYS) -ffriend-injection
      OSCPPFLAGS = -D__$(OPSYS)
    endif
    DLLNAME = lib$(addsuffix .so,$(basename $(EXEC)))
    DLLSUFFIX = .so
    OSDLLFLAGS = -shared -z nodefaultlib $(SONAME)lib$(EXEC) $(RPATH)$(LAVADIR)/lib:$(QLIB):/usr/lib:/lib -L../../lib
    OSEXECFLAGS = -fstack-check -z nodefaultlib -L../../lib $(RPATH)$(LAVADIR)/lib:$(QLIB):/usr/lib:/lib
    EXEC2 = $(EXEC)
    ifneq ($(DBG),)
      QtS =
      ACL =
    else
      QtS =
      ACL =
    endif
  endif
endif

ALL_CPP_INCLUDES = $(CPP_INCLUDES) -I$(QINCL) -I$(QINCL)/QtCore$(QFRW) -I$(QINCL)/QtNetwork$(QFRW) -I$(QINCL)/QtGui$(QFRW) -I$(QINCL)/QtAssistant$(QFRW) -I$(QINCL)/QtTest$(QFRW)

#shared libraries:
ifeq ($(suffix $(EXEC)),.so)
  ifeq ($(OPSYS),MINGW32)
this: ../../bin/$(DLLNAME)
../../bin/$(DLLNAME): $(LINKS) $(gen_files) $(PCH_TARGET) $(all_o_files)
	$(CC) -o ../../bin/$(DLLNAME) $(IMPLIB) $(OSDLLFLAGS) $(all_o_files) $(addprefix -l,$(SUBPRO)) -lQtAssistantClient$(ACL) -lQtCore$(QtS) -lQtGui$(QtS) -lQtNetwork$(QtS) -mt $(OSLIBFLAGS)
  else
  ifeq ($(OPSYS),Darwin)
this: ../../lib/$(DLLNAME)
../../lib/$(DLLNAME): $(LINKS) $(gen_files) $(PCH_TARGET) $(all_o_files)
	$(CC) $(DBG) -o ../../lib/$(DLLNAME) $(OSDLLFLAGS) -F$(QLIB) -L../../lib $(all_o_files) $(addprefix -l,$(SUBPRO)) -framework QtAssistant -framework QtCore -framework QtGui -framework QtNetwork $(OSLIBFLAGS)
  else
this: ../../lib/$(DLLNAME)
../../lib/$(DLLNAME): $(LINKS) $(gen_files) $(PCH_TARGET) $(all_o_files)
	$(CC) $(DBG) -o ../../lib/$(DLLNAME) $(OSDLLFLAGS) $(all_o_files) -L$(QLIB) -L../../lib $(addprefix -l,$(SUBPRO)) -lQtAssistantClient$(QtS) -lQtCore$(QtS) -lQtGui$(QtS) -lQtNetwork$(QtS) -mt $(OSLIBFLAGS)
  endif
  endif

#executables:
else 
  ifeq ($(OPSYS),MINGW32)
this: ../../bin/$(EXEC2)
../../bin/$(EXEC2): $(gen_files) $(PCH_TARGET) $(all_o_files)
	$(CC) $(DBG) -o ../../bin/$(EXEC2) $(OSEXECFLAGS) -mwindows  $(all_o_files) $(addprefix -l,$(SUBPRO)) -lmingw32 -lQtAssistantClient$(QtS) -lQtCore$(QtS) -lQtGui$(QtS) -lQtNetwork$(QtS) $(OSLIBFLAGS)
#$(CC) $(DBG) -o ../../bin/$(EXEC2) $(OSEXECFLAGS) -mthreads -mwindows -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc -Wl,-s -Wl,-subsystem,windows $(all_o_files) -L../../lib $(addprefix -l,$(SUBPRO)) -L$(QLIB) -lmingw32 -lqtmain -lQtAssistantClient$(QtS) -lQtCore$(QtS) -lQtGui$(QtS) -lQtNetwork$(QtS) $(OSLIBFLAGS)
  else
  ifeq ($(OPSYS),Darwin)
this: ../../bin/$(EXEC2)
../../bin/$(EXEC2): $(gen_files) $(PCH_TARGET) $(all_o_files)
	$(CC) $(DBG) -o ../../bin/$(EXEC2) -F$(QLIB) -L../../lib $(all_o_files) $(OSEXECFLAGS) $(addprefix -l,$(SUBPRO)) -framework QtAssistant -framework QtCore -framework QtGui -framework QtNetwork $(OSLIBFLAGS)
  else
this: ../../bin/$(EXEC2)
../../bin/$(EXEC2): $(gen_files) $(PCH_TARGET) $(all_o_files)
	$(CC) $(DBG) -o ../../bin/$(EXEC2) $(all_o_files) $(OSEXECFLAGS) -L$(QLIB) $(addprefix -l,$(SUBPRO)) -lQtAssistantClient$(QtS) -lQtCore$(QtS) -lQtGui$(QtS) -lQtNetwork$(QtS) $(OSLIBFLAGS)
  endif
  endif
endif

.cpp.o:
	$(CC) -c -fPIC $(DBG) -pipe -MMD $(PCH_WARN) -D$(OSCAT) -DQASSISTANT=$(QASSCALL) $(OSCPPFLAGS) $(CPP_FLAGS) -DQT_THREAD_SUPPORT $(PCH_INCL) $(ALL_CPP_INCLUDES) -o $@ $<

.c.o:
	$(CC) -c -fPIC $(DBG) -pipe -MMD $(PCH_WARN) -D$(OSCAT) $(OSCPPFLAGS) $(CPP_FLAGS) $(PCH_INCL) $(ALL_CPP_INCLUDES) -o $@ $<

PCH/$(PRJ)_all.h.gch: $(PRJ)_all.h $(h_ui_files) $(h_ph_files)
	if [ ! -d PCH ] ; then mkdir PCH; fi; $(CC) -c -fPIC $(DBG) -pipe -MMD -Winvalid-pch -D$(OSCAT) -DQASSISTANT=$(QASSCALL) $(OSCPPFLAGS) $(CPP_FLAGS) -DQT_THREAD_SUPPORT $(ALL_CPP_INCLUDES) -o $@ $(PRJ)_all.h

# UIC rules; use "sed" to change minor version of ui files to "0":
# prevents error messages from older Qt3 UIC's
Generated/%.h: %.ui
	$(QTOOLS)/uic$(QSUFF) $< -o $@
#	( grep -q -e 'UI version=\"[0-9]\+\.0\"' $< || \
#	  sed -i -e 's/\(UI version=\"[0-9]\+\.\)[0-9]\+"/\10\"/' $<; ); \
#  export LD_LIBRARY_PATH=/usr/X11R6/bin;
#Generated/%.cpp: %.ui
#	( grep -q -e 'UI version=\"[0-9]\+\.0\"' $< || \
#	  sed -i -e 's/\(UI version=\"[0-9]\+\.\)[0-9]\+"/\10\"/' $< ); \
#  $(QBIN)/bin/uic -impl $*.h $< -o $@; \
#  sed -i -e 's/static const unsigned char const/static const unsigned char/' $@

#MOC rule
moc_%.cpp: %.h
	$(QTOOLS)/moc$(QSUFF) $< -o $@
Generated/moc_%.cpp: %.h
	$(QTOOLS)/moc$(QSUFF) $< -o $@

#LPC rule:
ifneq ($(DLL),)
impex = -X $(DLL)
endif

%.h %G.cpp: %.ph
	../../bin/LPC $(impex) -I. -I../LavaBase $<

%.cpp: %.qrc
	$(QTOOLS)/rcc -o $@ $<

ifeq ($(suffix $(EXEC)),)
run:
	$(EXEC)&
debug:
	/opt/insight-6.0/gdb/insight $(EXEC) &
endif

%.rec:
	cd $(LAVADIR)/src/$(basename $@) && $(MAKE) this

%.cln:
	cd $(LAVADIR)/src/$(basename $@) && $(MAKE) dirclean

dirclean:	
	rm -rf *.o *.d PCH/*.gch PCH/*.d $(gen_files) Generated/*.cpp Generated/*.h Generated/*.o Generated/*.d


cleanall: dirclean $(clean_subpro)


# Include the generated dependency files if they exist already
dependencies=$(wildcard *.d Generated/*.d PCH/*.d)

ifneq ($(dependencies),)
include $(dependencies)
endif
