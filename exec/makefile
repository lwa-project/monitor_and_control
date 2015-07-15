# assumes the following directories already exist
# ../common  (MCS-wide header files; specifically mcs.h)
# ../../exec (executables copied to here)

all: mesix meosx memdrex mecfg medfg megfg me_exec meeix me_init me_tpcom me_inproc mefsdfg me_getaltazx me_point_corr_x me_findsol_x me_findjov_x

mesix: mesix.c mesi.c me.h ../common/mcs.h
	gcc -o mesix mesix.c -I../common
	cp mesix ../../exec/.

meosx: meosx.c meos.c memdre.c mesi.c me.h ../common/mcs.h
	gcc -o meosx meosx.c -I../common
	cp meosx ../../exec/.

memdrex: memdrex.c memdre.c me.h ../common/mcs.h
	gcc -o memdrex memdrex.c -I../common
	cp memdrex ../../exec/.

mecfg: mecfg.c
	gcc -o mecfg mecfg.c -I../common
	cp mecfg ../../exec/.

medfg: medfg.c
	gcc -o medfg medfg.c -I../common
	cp medfg ../../exec/.

megfg: megfg.c
	gcc -o megfg megfg.c -I../common
	cp megfg ../../exec/.

me_exec: me_exec.c me_exec_1.c me.h ../common/mcs.h
	gcc -o me_exec me_exec.c -I../common
	cp me_exec ../../exec/.

meeix: meeix.c meei.c me.h ../common/mcs.h
	gcc -o meeix meeix.c -I../common
	cp meeix ../../exec/.

me_init: me_init.c me.h ../common/mcs.h
	gcc -o me_init me_init.c -I../common
	cp me_init ../../exec/.

me_tpcom: me_tpcom.c me.h ../common/mcs.h
	gcc -o me_tpcom me_tpcom.c -I../common
	cp me_tpcom ../../exec/.

me_inproc: me_inproc.c me_getaltaz.c me_point_corr.c me_findsol.c me_findjov.c ephem_vsop87.c ephem_vsop87_data.c ephem_astro.h ephem_vsop87.h me.h ../common/mcs.h
	gcc -o me_inproc me_inproc.c -I../common -lm
	cp me_inproc ../../exec/.

mefsdfg: mefsdfg.c me.h ../common/mcs.h
	gcc -o mefsdfg mefsdfg.c -I../common -lm
	cp mefsdfg ../../exec/.

me_getaltazx: me_getaltazx.c me_getaltaz.c me.h ../common/mcs.h
	gcc -o me_getaltazx me_getaltazx.c -I../common -lm
	cp me_getaltazx ../../exec/.

me_point_corr_x: me_point_corr_x.c me_point_corr.c me.h ../common/mcs.h
	gcc -o me_point_corr_x me_point_corr_x.c -I../common -lm
	cp me_point_corr_x ../../exec/.

me_findsol_x: me_findsol_x.c me_findsol.c
	gcc -o me_findsol_x me_findsol_x.c -lm
	cp me_findsol_x ../../exec/.

me_findjov_x: me_findjov_x.c me_findjov.c ephem_vsop87.c ephem_vsop87_data.c ephem_astro.h ephem_vsop87.h
	gcc -o me_findjov_x me_findjov_x.c -lm
	cp me_findjov_x ../../exec/.

install:
	cp *.sh ../../exec/.

clean:
	rm mesix meosx memdrex mecfg medfg megfg me_exec meeix me_init me_tpcom me_inproc mefsdfg me_getaltazx me_point_corr_x me_findsol_x me_findjov_x

