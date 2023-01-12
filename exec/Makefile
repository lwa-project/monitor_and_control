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
# Programs that rely on code from PyEphem/XEphem
#

EPHEM_OBJECTS := ephem_aberration.o ephem_deflect.o ephem_deltat.o ephem_eq_ecl.o ephem_mjd.o ephem_moon.o ephem_nutation.o ephem_obliq.o ephem_parallax.o ephem_precess.o ephem_sphcart.o ephem_sun.o ephem_utc_gst.o ephem_vsop87.o ephem_vsop87_data.o

me_inproc: me_inproc.o me_getaltaz.o me_point_corr.o me_findsol.o me_findjov.o me_findlun.o me_precess.o  $(EPHEM_OBJECTS) me.h ../common/mcs.h
	$(CC) -o me_inproc me_inproc.o  me_getaltaz.o me_point_corr.o me_findsol.o me_findjov.o me_findlun.o me_precess.o  $(EPHEM_OBJECTS) $(LDFLAGS)

me_getaltaz_x: me_getaltaz_x.o me_getaltaz.o $(EPHEM_OBJECTS)
	$(CC) -o me_getaltaz_x me_getaltaz_x.o me_getaltaz.o $(EPHEM_OBJECTS) $(LDFLAGS)

me_getlst_x: me_getlst_x.o me_getlst.o $(EPHEM_OBJECTS)
	$(CC) -o me_getlst_x me_getlst_x.o me_getlst.o $(EPHEM_OBJECTS) $(LDFLAGS) 

me_findsol_x: me_findsol_x.o me_findsol.o $(EPHEM_OBJECTS)
	$(CC) -o me_findsol_x me_findsol_x.o me_findsol.o $(EPHEM_OBJECTS) $(LDFLAGS) 

me_findjov_x: me_findjov_x.o me_findjov.o $(EPHEM_OBJECTS)
	$(CC) -o me_findjov_x me_findjov_x.o me_findjov.o $(EPHEM_OBJECTS) $(LDFLAGS)

me_findlun_x: me_findlun_x.o me_findlun.o $(EPHEM_OBJECTS)
	$(CC) -o me_findlun_x me_findlun_x.o me_findlun.o $(EPHEM_OBJECTS) $(LDFLAGS)
	
me_precess_x: me_precess_x.o me_precess.o $(EPHEM_OBJECTS)
	$(CC) -o me_precess_x me_precess_x.o me_precess.o $(EPHEM_OBJECTS) $(LDFLAGS)

ephem_aberration.o: ephem_aberration.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_aberration.o ephem_aberration.c

ephem_deflect.o: ephem_deflect.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_deflect.o ephem_deflect.c

ephem_deltat.o: ephem_deltat.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_deltat.o ephem_deltat.c

ephem_eq_ecl.o: ephem_eq_ecl.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_eq_ecl.o ephem_eq_ecl.c

ephem_mjd.o: ephem_mjd.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_mjd.o ephem_mjd.c

ephem_moon.o: ephem_moon.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_moon.o ephem_moon.c

ephem_nutation.o: ephem_nutation.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_nutation.o ephem_nutation.c

ephem_obliq.o: ephem_obliq.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_obliq.o ephem_obliq.c

ephem_parallax.o: ephem_parallax.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_parallax.o ephem_parallax.c

ephem_precess.o: ephem_precess.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_precess.o ephem_precess.c

ephem_sphcart.o: ephem_sphcart.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_sphcart.o ephem_sphcart.c

ephem_sun.o: ephem_sun.c ephem_astro.h ephem_vsop87.c ephem_vsop87_data.c ephem_vsop87.h
	$(CC) $(CFLAGS) -c -o ephem_sun.o ephem_sun.c

ephem_utc_gst.o: ephem_utc_gst.c ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_utc_gst.o ephem_utc_gst.c

ephem_vsop87.o: ephem_vsop87.c ephem_vsop87_data.c ephem_vsop87.h ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_vsop87.o ephem_vsop87.c

ephem_vsop87_data.o: ephem_vsop87_data.c ephem_vsop87.h ephem_astro.h
	$(CC) $(CFLAGS) -c -o ephem_vsop87_data.o ephem_vsop87_data.c

#
# End Programs that rely on code from PyEphem/XEphem
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
