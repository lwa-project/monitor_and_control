# assumes the following directories already exist
# ../common  (MCS-wide header files; specifically mcs.h)
# ../../exec (executables copied to here)

CFLAGS := -I../common
LDFLAGS := -lm -lncurses

TARGETS :=  mesix meosx memdrex mecfg medfg megfg me_exec meeix me_init me_tpcom me_inproc me_mon mefsdfg medrange me_getaltaz_x me_point_corr_x me_findsol_x me_findjov_x me_findlun_x me_precess_x me_getlst_x

all: $(TARGETS)

mesix: mesix.c mesi.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o mesix mesix.c $(LDFLAGS)

meosx: meosx.c meos.c memdre.c mesi.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o meosx meosx.c $(LDFLAGS)

memdrex: memdrex.c memdre.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o memdrex memdrex.c $(LDFLAGS)

mecfg: mecfg.c
	$(CC) $(CFLAGS) -o mecfg mecfg.c $(LDFLAGS)

medfg: medfg.c
	$(CC) $(CFLAGS) -o medfg medfg.c $(LDFLAGS)

megfg: megfg.c
	$(CC) $(CFLAGS) -o megfg megfg.c $(LDFLAGS)

me_exec: me_exec.c me_exec_1.c meos.c memdre.c mesi.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o me_exec me_exec.c $(LDFLAGS)

meeix: meeix.c meei.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o meeix meeix.c $(LDFLAGS)

me_init: me_init.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o me_init me_init.c $(LDFLAGS)

me_tpcom: me_tpcom.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o me_tpcom me_tpcom.c $(LDFLAGS)

me_mon: me_mon.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o me_mon me_mon.c $(LDFLAGS)

mefsdfg: mefsdfg.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o mefsdfg mefsdfg.c $(LDFLAGS)

medrange: medrange.c me.h ../common/mcs.h
	$(CC) $(CFLAGS) -o medrange medrange.c $(LDFLAGS)

me_point_corr_x: me_point_corr_x.o me_point_corr.o
	$(CC) $(CFLAGS) -o me_point_corr_x me_point_corr_x.o me_point_corr.o $(LDFLAGS) 

#
# Programs that rely on code from SOFA
#

SOFA_CFLAGS := -I../sofa/src
SOFA_LDFLAGS := -L. -lsofa_c -lm

me_inproc: me_inproc.o me_getaltaz.o me_point_corr.o me_findsol.o me_findjov.o me_findlun.o me_precess.o me.h ../common/mcs.h
	$(CC) -o me_inproc me_inproc.o  me_getaltaz.o me_point_corr.o me_findsol.o me_findjov.o me_findlun.o me_precess.o $(LDFLAGS) $(SOFA_LDFLAGS)

me_getaltaz.o: me_getaltaz.c
	$(CC) $(CFLAGS) $(SOFA_CFLAGS) -c -o me_getaltaz.o me_getaltaz.c

me_getaltaz_x: me_getaltaz_x.o me_getaltaz.o
	$(CC) -o me_getaltaz_x me_getaltaz_x.o me_getaltaz.o $(LDFLAGS) $(SOFA_LDFLAGS)

me_getlst.o: me_getlst.c
	$(CC) $(CFLAGS) $(SOFA_CFLAGS) -c -o me_getlst.o me_getlst.c

me_getlst_x: me_getlst_x.o me_getlst.o
	$(CC) -o me_getlst_x me_getlst_x.o me_getlst.o $(LDFLAGS) $(SOFA_LDFLAGS)

me_findsol.o: me_findsol.c
	$(CC) $(CFLAGS) $(SOFA_CFLAGS) -c -o me_findsol.o me_findsol.c
	
me_findsol_x: me_findsol_x.o me_findsol.o
	$(CC) -o me_findsol_x me_findsol_x.o me_findsol.o $(LDFLAGS) $(SOFA_LDFLAGS)

me_findjov.o: me_findjov.c
	$(CC) $(CFLAGS) $(SOFA_CFLAGS) -c -o me_findjov.o me_findjov.c
	
me_findjov_x: me_findjov_x.o me_findjov.o
	$(CC) -o me_findjov_x me_findjov_x.o me_findjov.o $(LDFLAGS) $(SOFA_LDFLAGS)

me_findlun.o: me_findlun.c
	$(CC) $(CFLAGS) $(SOFA_CFLAGS) -c -o me_findlun.o me_findlun.c

me_findlun_x: me_findlun_x.o me_findlun.o
	$(CC) -o me_findlun_x me_findlun_x.o me_findlun.o $(LDFLAGS) $(SOFA_LDFLAGS)

me_precess.o: me_precess.c
	$(CC) $(CFLAGS) $(SOFA_CFLAGS) -c -o me_precess.o me_precess.c
	
me_precess_x: me_precess_x.o me_precess.o
	$(CC) -o me_precess_x me_precess_x.o me_precess.o $(LDFLAGS) $(SOFA_LDFLAGS)

#
# End Programs that rely on code from SOFA
#

.PHONY: install
install:
	install -d ../../exec
	for target in $(TARGETS); do \
		install $$target ../../exec/.; \
	done
	install *.sh ../../exec/.

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f $(TARGETS)
