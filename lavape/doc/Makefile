doc: LavaPE.qhc LavaPE.qch

include ../src/common.mk

helpfiles=$(wildcard html/*.htm) $(wildcard html/*.css) $(wildcard images/*.png) $(wildcard images/*.gif) $(wildcard images/*.jpg) \
$(wildcard html/commands/*.htm) $(wildcard html/dialogs/*.htm) $(wildcard html/whatNext/*.htm) $(wildcard html/whatsThis/*.htm)

LavaPE.qhc LavaPE.qch: LavaPE.qhcp LavaPE.qhp $(helpfiles)
	echo $(helpfiles); $(QBIN)/qcollectiongenerator LavaPE.qhcp
