# assumes the following directories already exist
# ../common  (MCS-wide header files; specifically mcs.h)
# ../../tp (executables copied to here)

all: tpss tptc tpsdm tprs tpms tprcs

tpss: tpss.c mt.h ../common/mcs.h tpss0.c tpss1.c tpss2.c tpss3.c tpss4.c tpss5.c
	gcc -o tpss tpss.c -I../common
	cp tpss ../../tp/.

tptc: tptc.c mt.h ../common/mcs.h
	gcc -o tptc tptc.c -I../common
	cp tptc ../../tp/.

tpsdm: tpsdm.c mt.h ../common/mcs.h
	gcc -o tpsdm tpsdm.c -I../common
	cp tpsdm ../../tp/.

tprs: tprs.c mt.h tprs1.c ../common/mcs.h
	gcc -o tprs tprs.c -I../common
	cp tprs ../../tp/.

tpms: tpms.c mt.h ../common/mcs.h
	gcc -o tpms tpms.c -I../common
	cp tpms ../../tp/.

tprcs: tprcs.c mt.h ../common/mcs.h
	gcc -o tprcs tprcs.c -I../common
	cp tprcs ../../tp/.

install: 
	cp test/* ../../tp/test/.

clean:
	rm tpss tptc tpsdm tprs tpms tprcs

