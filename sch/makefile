# assumes the following directories already exist
# ../common  (MCS-wide header files; specifically mcs.h)
# ../../sch  (executables copied to here)

all: ms_init dat2dbm ms_mcic ms_exec msei ms_mdr ms_mdre ms_makeMIB_ASP ms_makeMIB_DP ms_makeMIB_DR ms_md2t ms_mb ms_mu ms_mdre_ip ms_mon

ms_init: ms_init.c ../common/mcs.h
	gcc -o ms_init ms_init.c -I../common
	cp ms_init ../../sch/.

dat2dbm: dat2dbm.c ../common/mcs.h
	gcc -o dat2dbm -I/usr/include/gdbm dat2dbm.c -lgdbm_compat -lgdbm -I../common 
	cp dat2dbm ../../sch/.

ms_mcic: ms_mcic.c ../common/mcs.h ms_mcic_mib.c ms_mcic_SHL.c ms_mcic_ASP.c ms_mcic_DP_.c ms_mcic_DR_.c 
	gcc -o ms_mcic -I/usr/include/gdbm ms_mcic.c -lgdbm_compat -lgdbm -I../common  
	cp ms_mcic ../../sch/.

ms_exec: ms_exec.c  ../common/mcs.h ms_exec_log.c 
	gcc -o ms_exec ms_exec.c -I../common 
	cp ms_exec ../../sch/.

msei: msei.c  ../common/mcs.h 
	gcc -o msei msei.c -I../common 
	cp msei ../../sch/.

ms_mdr: ms_mdr.c  ../common/mcs.h
	gcc -o ms_mdr -I/usr/include/gdbm ms_mdr.c -lgdbm_compat -lgdbm -I../common 
	cp ms_mdr ../../sch/.

ms_mdre: ms_mdre.c  ../common/mcs.h
	gcc -o ms_mdre -I/usr/include/gdbm ms_mdre.c -lgdbm_compat -lgdbm -I../common 
	cp ms_mdre ../../sch/.

ms_makeMIB_ASP: ms_makeMIB_ASP.c 
	gcc -o ms_makeMIB_ASP ms_makeMIB_ASP.c
	cp ms_makeMIB_ASP ../../sch/.

ms_makeMIB_DP: ms_makeMIB_DP.c 
	gcc -o ms_makeMIB_DP ms_makeMIB_DP.c
	cp ms_makeMIB_DP ../../sch/.

ms_makeMIB_DR: ms_makeMIB_DR.c 
	gcc -o ms_makeMIB_DR ms_makeMIB_DR.c
	cp ms_makeMIB_DR ../../sch/.

ms_md2t: ms_md2t.c ../common/mcs.h
	gcc -o ms_md2t -I/usr/include/gdbm ms_md2t.c -lgdbm_compat -lgdbm -I../common 
	cp ms_md2t ../../sch/.

ms_mb: ms_mb.c 
	gcc -o ms_mb ms_mb.c -lncurses
	cp ms_mb ../../sch/.

ms_mu: ms_mu.c ../common/mcs.h
	gcc -o ms_mu -I/usr/include/gdbm ms_mu.c -lgdbm_compat -lgdbm -I../common
	cp ms_mu ../../sch/.

ms_mdre_ip: ms_mdre_ip.c ../common/mcs.h
	gcc -o ms_mdre_ip -I/usr/include/gdbm ms_mdre_ip.c -lgdbm_compat -lgdbm -I../common 
	cp ms_mdre_ip ../../sch/.

ms_mon: ms_mon.c ../common/mcs.h 
	gcc -o ms_mon ms_mon.c -lncurses -I../common
	cp ms_mon ../../sch/.

install:
	cp mch_minimal_server.py ../../sch/.
	cp mch_minimal_server_acceptall.py ../../sch/.
	cp ms_mb_* ../../sch/.
	cp *.sh ../../sch/.
	cp readme.txt ../../sch/.
	cp readme_SHL_EI.txt ../../sch/.
	cp SHL_MIB_init.dat ../../sch/.
	cp NU?_MIB_init.dat ../../sch/.
	
clean:
	rm ms_init dat2dbm ms_mcic ms_exec msei ms_mdr ms_mdre ms_makeMIB_ASP ms_makeMIB_DP ms_makeMIB_DR ms_md2t ms_mb ms_mu ms_mdre_ip ms_mon

