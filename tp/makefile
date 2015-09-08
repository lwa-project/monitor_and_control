# assumes the following directories already exist
# ../common  (MCS-wide header files; specifically mcs.h)
# ../../tp (executables copied to here)

CFLAGS := -I../common
LDFLAGS := 

TARGETS := tpss tptc tpsdm tprs tpms tprcs

all: $(TARGETS)

tpss: tpss.c mt.h ../common/mcs.h tpss0.c tpss1.c tpss2.c tpss3.c tpss4.c tpss5.c
	$(CC) $(CFLAGS) -o tpss tpss.c $(LDFLAGS)

tptc: tptc.c mt.h ../common/mcs.h
	$(CC) $(CFLAGS) -o tptc tptc.c $(LDFLAGS)

tpsdm: tpsdm.c mt.h ../common/mcs.h
	$(CC) $(CFLAGS) -o tpsdm tpsdm.c $(LDFLAGS)

tprs: tprs.c mt.h tprs1.c ../common/mcs.h
	$(CC) $(CFLAGS) -o tprs tprs.c $(LDFLAGS)

tpms: tpms.c mt.h ../common/mcs.h
	$(CC) $(CFLAGS) -o tpms tpms.c $(LDFLAGS)

tprcs: tprcs.c mt.h ../common/mcs.h
	$(CC) $(CFLAGS) -o tprcs tprcs.c $(LDFLAGS)

.PHONY: install
install: all
	install -d ../../tp ../../tp/mbox ../../tp/test
	for target in $(TARGETS); do \
		install $$target ../../tp/.; \
	done
	install -m 644 test/* ../../tp/test/.

.PHONY: clean
clean:
	-rm -f $(TARGETS)
