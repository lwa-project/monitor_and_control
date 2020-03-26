
MCS / Task Processor Software
Version 0.2 FIXME
S.W. Ellingson
March 08, 2011 FIXME


Table of Contents 
=================
Notable Changes in this Version 
Introduction & Overview
File Inventory / Directory Structure
Quick Start
Troubleshooting / Known Bugs
Other Notes & Issues
History


Notable Changes in This Version  
===============================

FIXME: DIAG1 mode
FIXME: tpsdm
FIXME: tprs
FIXME: tpms
FIXME: tprcs

mcs.h -- select one of the LAN configurations 

FIXME: X. See "History" for documentation of previous releases.


Introduction & Overview
=======================

This is software developed for the MCS "Task Processor" computer.  Task Processor consists of a set of utilities that are the primary interface between station operators and MCS/Executive.  In this release, utilities include "tpss" (Task Processor Session Scheduler) and "tptc" (Task Processor Time Check).

This software is functional and released to facilitate review and comment; but not all TP required features are implemented.    

The software is written entirely in ANSI C and was developed on Ubuntu Linux 10.10.  


File Inventory / Directory Structure
====================================

MCS/Task Processor source code is completely contained in the subdirectory src/tp, except for the header file mcs.h which is found in the subdirectory src/common.  When compiled using "make", executables and support files are copied to the subdirectory "tp" (on the same level as "src"; distinct from "src/tp"), from where they should be used.  In other words, "~src/tp" is the source code, and "~tp" is the sandbox.

src/tp/readme.txt
-- This file.

src/tp/makefile
-- "$ make clean; make; make install" compiles everything needed and places executables and necessary support files in the sandbox directory "tp" (on the same level as "src"; distinct from "src/tp").

src/common/mcs.h
-- Header file & common utility functions.  (This file is also used by MCS/Executive & MCS/Scheduler.)  

src/tp/mt.h
-- Header file specific to MCS/Task Processor software.

src/tp/tpss.c  -- includes main() for tpss
src/tp/tpss0.c -- these files are #include'd (separated to improve readability of main())
src/tp/tpss1.c 
src/tp/tpss2.c 
src/tp/tpss3.c 
src/tp/tpss4.c 
src/tp/tpss5.c 
-- Source code for tpss.

src/tp/tptc.c
-- Source code for tptc.  This utility program checks the PC's and OS's time handling functions, as well as LWA-specific time handling and time format conversion functions.  

****************************************************
*** That's it for actual MCS/Task Processor software. ***
****************************************************  
The following programs are *support* programs to facilitate testing, development, integration, and commissioning:
 
src/test/sdf_test1.txt -- Test SDF consisting of 2 short TRK_RADEC observations
src/test/sdf_test2.txt -- Test SDF consisting of 2 short TBN observations
src/test/sdf_test3.txt -- Test SDF consisting of 2 TBW observations
src/test/sdf_test4.txt -- Test SDF consisting of a STEPPED mode observation
src/test/sdf_test5.txt -- Same as sdf_test1.txt, but observing mode is changed to DIAG1
-- These are sample session definition files (SDFs) which can be used to experiment with tpss

src/test/mess_test1.dat 
src/test/mess_test2.dat
src/test/mess_test3.dat
-- These are sample MCS Executive Sessions Scheduled (MESS) files which can be used to test the scheduling function of tpss 


Quick Start
===========

In the procedure below, the software is compiled and a simple test is performed to demonstrate the functioning of the software.  These procedure does NOT require that MCS/Executive be operational.

(1)  If not done already, explode the directories/files from the distribution and cd into the directory ~src/tp/.

(2)  Make the software (~src/tp$ make; then make install).  The executables and various support files are placed in ~/tp. cd to ~/tp.

(3)  Run tptc ("$ ./tptc").  This utility program checks the PC's and OS's time handling functions, as well as LWA-specific time handling and time format conversion functions.  The output is self-explainatory.  

(4)  In this Quick Start, it is assumed the folder through which MCS/TP and MCS/Exec exchange files is ~src/tp/mbox.  When running, MCS/Executive maintains a MESS file in this directory.  Here, a mock MESS file is used instead.  Examine it using "$ cat mess.dat".  You see: 

1
0 3 3 3 3 
55615 86379999 2 15000 9 PROJ1 0001
55616    10000 3 20000 9 PROJ1 0002
55616    10000 4 20000 9 PROJ1 0003
55616    21000 4 20000 9 PROJ1 0003

-- The number in the first line is the station's configuration request authority (CRA) policy, which will be either 0 or 1.  
-- The five numbers in the second line are the status of the DP output beams 1-4 followed by the TBW/TBN output.  See MCS0031 for an explanation of the 0-3 codes used to indicate status.  In this case, DP output beam 1 is indicated as "not available" and the other outputs are "OK"
-- Each subsequent line refers to one scheduled session.  The columns are: (1) Start MJD, (2) Start MPM, (3) DP output (1-5), (4) Duration of the session in milliseconds, (5) Session CRA priority, (6) PROJECT_ID, (7) SESSION_ID.

(5)  Look at the sample SDF "sdf_test1.txt".  The format of this file is explained in MCS0030.  This SDF seeks to schedule a session requiring a DP beam output (i.e., one of 1-4), but does not specify which beam output is to be used (since no SESSION_DRX_BEAM keyword appears).  

(6)  Use tpss to check the SDF and schedule the specified session.  The command line in general is:

tpss <sdf> <max_phase> <bIgnoreActualTime> <mbox> 

tpss consists of 5 phases: (1) SDF file parsing, (2) Consistency & additional error checking, (3) Check resource availability (i.e., scheduling), (4) Generate output SDF, SSF, and OSFs, (5) Push files to tp outbox and notify MCS/Executive.  The command line argument <max_phase> specifies the highest-numbered phase to be executed; this is to allow processing (including error checking) of SDFs without actually scheduling a session.  If Phase 5 completes, the session is scheduled.  The command line argument <bIgnoreActualTime> is used to suppress checking to see if observation start times are in the past, which is useful for testing.  The command line argument <mbox> is the path (no trailing slash) to the local directory through which tpss and MCS/Executive communicate.

Try this:
$ ./tpss sdf_test1.txt 4 1 mbox

The terminal output should look like this:

[201/24541] Input: sdf='sdf_test1.txt'
[201/24541] Input: max_phase=4
[201/24541] Input: bIgnoreActualTime=1
[201/24541] Input: mbox='mbox'
[201/24541] *********************************************************
[201/24541] *** Phase 1: Reading/Parsing Session Defintion File *****
[201/24541] *********************************************************
[201/24541] PI_ID='1'

(Here there will be a lot of lines indicating the parsing of individual SDF keywords)

[201/24541] Phase 1: OK
[201/24541] *********************************************************
[201/24541] *** Phase 2: Second Pass Consistency/Error Checking *****
[201/24541] *********************************************************
[201/24541] Phase 2: OK
[201/24541] *********************************************************
[201/24541] *** Phase 3: Checking Resource Availability *************
[201/24541] *********************************************************
[201/24541] Duration of session is 20000 ms = 0.333333 min = 0.005556 h
[201/24541] For DP output 1 of 4, LWA_dpoavail says: UNAVAILABLE (status = 0)
[201/24541] For DP output 2 of 4, LWA_dpoavail says: available with status = 3
[201/24541] For DP output 3 of 4, LWA_dpoavail says: conflict with project 'PROJ1' session 'SESS2'
[201/24541] For DP output 4 of 4, LWA_dpoavail says: conflict with project 'PROJ1' session 'SESS3'
[201/24541] Selected DP output 2 of 4
[201/24541] Phase 3: OK
[201/24541] *************************************************
[201/24541] *** Phase 4: Generate Full-SDF, SSF, and OSFs ***
[201/24541] *************************************************
[201/24541] SDF 'TPSS0001_0001.txt' successfully written
[201/24541] SSF 'TPSS0001_0001.dat' successfully written
[201/24541] OSF 'TPSS0001_0001_0001.dat' successfully written
[201/24541] OSF 'TPSS0001_0001_0002.dat' successfully written
[201/24541] Phase 4: OK

Note that tpss has indicated that the SDF is correctly, reasonably, and consistently constructed (Phases 1 and 2).  In Phase 3, tpss has identified that DP output 1 is unavailable, and that DP outputs 3 and 4 are already assigned sessions which conflict with the desired session, and so has assigned this session to DP output 2.  Phase 4 has completed with the writing out of files to be sent to MCS/Executive. 

(7)  For the purposes of demonstration, we can also see what happens in Phase 5.  WARNING: MCS/Executive scheduler should NOT be running if you try this!

Execute the following command line:
$ ./tpss sdf_test1.txt 5 -1 mbox

(bIgnoreActualTime=-1 allows Phase 5 -- including submission of the session to MCS/Executive -- to run even if start times are specified to be in the past.)

The following additional lines should appear in the terminal output this time:

[201/24634] ******************************************************
[201/24634] *** Phase 5: Push to tp outbox and notify MCS/Exec ***
[201/24634] ******************************************************
[201/24634] Moved SDF, SSF, and OSFs to directory 'mbox'
[201/24634] Updated mbox/manifest.dat
[201/24634] Phase 5: OK

This time, the output files are transferred and the file "mbox/manifest.dat" is updated to include a list of these files.  The next time MCS/Executive checks, it will see from manifest.dat that the new files defining a session are available and will act on them.  It is important that you do not alter or otherwise manipulate the contents of this directory (mbox, in this example) when MCS/Executive is running.

If MCS/Executive were running, it would (1) transfer the files listed in manifest.dat, (2) process the session files, and assuming it did not find reason to reject them (3) update mess.dat and clear the associated lines of manifest.dat.

(8)  tpss can detect when the operator is attempting (probably by accident) to schedule the same session multiple times.  For example, execute the following command line (again):

$ ./tpss sdf_test1.txt 5 -1 mbox

The terminal output this time concludes with:

[201/24673] ******************************************************
[201/24673] *** Phase 5: Push to tp outbox and notify MCS/Exec ***
[201/24673] ******************************************************
[201/24673] FATAL: Session 'TPSS0001_0001' already appears in manifest

Here, tpss has found that a session with the same PROJECT_ID and SESSION_ID already appears in manifest.dat, and has refused to execute Phase 5.  tpss also checks mess.dat for the same issue.    


Troubleshooting / Known Bugs
============================

-- None at this time.


Other Notes & Issues 
====================

-- None at this time.


History
=======

Ver 0.2 (Mar 08, 2011)
----------------------

1.  Added the utility program "tptc" (Task Processor Time Check). 

Ver 0.1
-------

This is the initial version.
