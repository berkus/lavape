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
o_files=$(cpp_files:.cpp=.o)

basenames_mocable=$(basename $(MOCABLES))
basenames_mocable_G=$(addprefix Generated/moc_,$(basenames_mocable))
moc_mocable_G=$(addsuffix .cpp,$(basenames_mocable_G))
o_mocable_G=$(moc_mocable_G:.cpp=.o)

gen_files=$(sort $(h_ph_files) $(h_ui_files) $(cpp_ph_files) $(cpp_ui_files) $(moc_ui_files) $(moc_mocable_G))
all_o_files=$(sort $(o_files) $(o_ph_files) $(o_ui_files) $(o_moc_ui_files) $(o_mocable_G))

make_subpro=$(addsuffix .rec,$(SUBPRO))
clean_subpro=$(addsuffix .cln,$(SUBPRO))

ifeq ($(LAVADIR),)
LAVADIR = $(shell cd ../..; pwd)
endif

ifeq ($(shell uname -o),Cygwin)
QTDIR=/opt/qt/3.2
LD_LIBRARY_PATH=/usr/X11R6/bin
export LD_LIBRARY_PATH
else
asscli=-lqassistantclient
ifeq ($(QTDIR),)
QTDIR=/usr/lib/qt3
endif
endif

rec_make: $(make_subpro) this
	
ifeq ($(shell uname -o),Cygwin)
ifeq ($(suffix $(EXEC)),.so)
base=$(basename $(EXEC))
EXEC2=cyg$(addsuffix .dll,$(base))
imports=$(addprefix -l,$(SUBPRO))
this: ../../bin/$(EXEC2)
../../bin/$(EXEC2): $(gen_files) $(all_o_files)
	g++ -o ../../bin/$(EXEC2) -shared -fstack-check -Wl,--out-implib=../../bin/lib${base}.dll.a -Wl,--export-all-symbols -Wl,--enable-auto-import -Wl,--whole-archive $(all_o_files) -Wl,--no-whole-archive -L../../bin $(imports) -L$(QTDIR)/lib -lqt-mt
else
EXEC2=$(addsuffix .exe,$(EXEC))
this: ../../bin/$(EXEC2)
../../bin/$(EXEC2): $(gen_files) $(all_o_files) $(addprefix ../../bin/,$(addprefix cyg,$(addsuffix .dll,$(SUBPRO))))
	g++ -o ../../bin/$(EXEC2) $(all_o_files) -fstack-check -L../../bin $(addprefix -l,$(SUBPRO)) -L$(QTDIR)/lib $(asscli) -lqt-mt
endif
else
ifeq ($(suffix $(EXEC)),.so)
this: ../../lib/lib$(EXEC)
../../lib/lib$(EXEC): $(gen_files) $(all_o_files)
	g++ -o ../../lib/lib$(EXEC) -shared -fstack-check -Wl,-soname=lib$(EXEC) -L../../lib -Wl,-rpath,$(LAVADIR)/lib -L$(QTDIR)/lib -Wl,-rpath,$(QTDIR)/lib -lqt-mt $(all_o_files)
else
this: ../../bin/$(EXEC) ../LavaPE/Debug/std.lava
../../bin/$(EXEC): $(gen_files) $(all_o_files) $(addprefix ../../lib/,$(addprefix lib,$(addsuffix .so,$(SUBPRO))))
	g++ -o ../../bin/$(EXEC) $(all_o_files) -fstack-check -L../../lib -Wl,-rpath,$(LAVADIR)/lib $(addprefix -l,$(SUBPRO)) -L$(QTDIR)/lib -Wl,-rpath,$(QTDIR)/lib -lqassistantclient -lqt-mt
../LavaPE/Debug/std.lava: ../../bin/std.lava
	cp -p ../../bin/std.lava $@
endif
endif

.cpp.o:
	g++ -c -ggdb -MMD -D__UNIX__ -D_REENTRANT -DQT_THREAD_SUPPORT $(CPP_FLAGS) $(CPP_INCLUDES) -o $@ $<
		
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
	cd ../$(basename $@) && $(MAKE) this

%.cln:
	cd ../$(basename $@) && $(MAKE) clean

clean:
	rm -rf *.o *.d Generated/*.o Generated/*.d $(gen_files)

cleanall: $(clean_subpro) clean


# Include the generated dependency files if they exist already
dependencies=$(wildcard *.d)
ifneq ($(dependencies),)
include $(dependencies)
endif

