
MCS/Scheduler Software
Version 0.9.2 FIXME
S.W. Ellingson
March 19, 2011 FIXME


Table of Contents
=================
Notable Changes in this Version
Introduction
File Inventory
Quick Start
Defined Test Cases
Modifying Test Cases for Network Operation
Why dbm Files?
Why ncurses?
Troubleshooting / Known Bugs
Other Notes & Issues
History


Notable Changes in this Version  
===============================

1. Fixed message parsing memory leak (again) in me_mcic.c (vicinity line 550)

2. dat2dbm, ms_md2t: Discovered/fixed "close()" used in place of "fclose()"; didn't matter previously; evidently recent changes to mcs.h caused this to be important

FIXME: Below are included in directory structure
./sch/cfiles
./sch/dfiles
./sch/Emulator_SHL
./sch/gfiles

mcs.h -- select one of the LAN configurations 


3. See "History" for documentation of previous releases.


Introduction
============

This is software developed for the MCS "Scheduler" computer.  The Scheduler computer accepts commands from the MCS "Executive" software (or, for development/testing purposes, something emulating the Executive) and interfaces with subsystems (or software emulating subsystems).  Also provided are software and scripts which can be used to demonstrate and evaluate the software, including control programs and programs for emulating subsystems.  

This software supports the post-upgrade LWA-1 subsystems SHL, ASP, NDP, and MCS-DR PCs in accordance with their ICDs. "Mock" subsystems (NU1, NU2, etc.) are supported as a means for testing and development; the mock subsystems behave as actual LWA subsystems, but are limited to the commands PNG, RPT, and SHT and implement only the MCS-RESERVED branch of the MIB. 

Note that the software is designed such that everything can be run on a single computer without modification; the difference between "scheduler and subsystems on separate PCs" operation and "everything on a single PC" operation is simply whether interprocess communications are directed to separate computers using network IP addresses, or to local processes using the loopback (127.0.0.1) IP address, respectively.

The software is written entirely in ANSI C and was developed on Ubuntu Linux 10.10 (AMD64 Desktop).  Shell scripts (provided) are used for demonstration and testing.  Some of the subsystem emulators (provided for testing purposes; not part of MCS/Scheduler) are written in Python.

This software is considered "beta". Specifically, it is functional, includes all required features, and is released to facilitate review and comment; however it has not yet been extensively tested by other users or in "field" conditions.  Previous (alpha) versions of this software *have* been extensively tested by other users and in "field" conditions. 


File Inventory
==============

readme.txt
-- This file.

makefile
-- "$ make" compiles everything needed and places executibles in target directory.

ms_init.c
-- Using a script (specified in the command line), this process initializes locally-maintained MIB files, launches subsystem handling processes (ms_mcic's), sets up interprocess communications (using POSIX message queues), launches the scheduler's executive process (ms_exec), and terminates.

ms_exec.c
ms_exec_log.c
-- The scheduler's executive process.  On the user side, accepts commands via a TCP connection at port 9734 (can be changed; see LWA_PORT_MSE in mcs.h).  Communicates with subsystem handlers (ms_mcic's) via POSIX message queues. 

ms_mcic.c
ms_mcic_mib.c
ms_mcic_SHL.c
ms_mcic_ASP.c
ms_mcic_NDP.c
ms_mcic_DR_.c (handles MCS-DRs #1-#5)
-- Code for the subsystem handler process "ms_mcic".  One instance of ms_mcic is launched for each subsystem to be managed. 

ms_mdre_ip.c
-- This is source code for the ms_mdre_ip process, which is launched by ms_init and is used primarily as an interface for MCS/Executive into Scheduler-maintained subsystem MIB (dbm) files.

dat2dbm.c
-- This program converts initial subsystem MIBs from human-readable text format to the "dbm" format used by the scheduler software.  (See the section "Why dbm Files?" below for more information.) Called by ms_init once for each subsystem.

../common/mcs.h
-- Header files, including macro defines and some utility functions.

ms_makeMIB_SHL.c
ms_makeMIB_ASP.c
ms_makeMIB_NDP.c
ms_makeMIB_DR.c (handles MCS-DRs #1-#5)
-- Automates process of creating a text-format MIB init files for ASP and DR (required by ms_init), respectively.

****************************************************
*** That's it for actual MCS/Scheduler software. ***
****************************************************  
The following programs are *support* programs to facilitate testing, development, integration, and commissioning:

msei.c
-- Program that can be used to send commands to the Scheduler software (specifically, to ms_exec) in lieu of MCS/Executive.  Please note:  msei cannot be used to send the NDP beamforming commands FST, BAM, or DRX.  However, the MCS/Executive utility "mesix" can do this.  

ms_mu.c  
-- uses MCS/Scheduler to update every ICD-defined MIB value for a subsystem (periodically, if desired)

ms_mdr.c
ms_mdre.c
ms_md2t.c
-- Programs that can be used to read the dbm-format MIB files used by MCS/Scheduler software.  ms_mdr displays all content of a MIB file.  ms_mdre displays only a specified entry (handy especially for use in scripts).  ms_md2t converts a MIB dbm file to text; unlike ms_mdr, raw binary values also are converted to text, and the entries appear in order of MIB index.  All of these programs indicate the time of last update for each MIB entry.

ms_mb.c 
-- ms_mb is a user-configurable ncurses-based MIB monitor which also can be used to do periodic MIB updates.  ".cfg" files are used to select which MIB entries are displayed and how they are arranged on the screen (the "template"), and what other information ("annotations") appears on the screen.  See LWA Engineering Memo MCS0029 for additional information on the use of ms_mb.  The .cfg files included with this release are listed below:

ms_mb_NUx_template.cfg   <-- simple display showing only MCS-RESERVED MIB entries
ms_mb_NUx_annotate.cfg
ms_mb_SHL_template0.cfg  <-- SHL summary info
ms_mb_SHL_annotate0.cfg
ms_mb_SHL_template1.cfg  <-- Example showing status (ON/OFF) of 50 SHL power ports
ms_mb_SHL_annotate1.cfg
ms_mb_ASP_template0.cfg  <-- ASP summary demo
ms_mb_ASP_annotate0.cfg
ms_mb_NDP_template0.cfg  <-- NDP summary demo
ms_mb_NDP_annotate0.cfg
ms_mb_NDP_template1.cfg  <-- NDP TBT/TBS status demo
ms_mb_NDP_annotate1.cfg
ms_mb_DRn_template0.cfg  <-- DR# status demos (5 of these because there are a lot of MIB entries)
ms_mb_DRn_annotate0.cfg
ms_mb_DRn_template1.cfg  
ms_mb_DRn_annotate1.cfg
ms_mb_DRn_template2.cfg  
ms_mb_DRn_annotate2.cfg
ms_mb_DRn_template3.cfg  
ms_mb_DRn_annotate3.cfg
ms_mb_DRn_template4.cfg  
ms_mb_DRn_annotate4.cfg
-- These are configuration files for ms_mb; see above.

ms_mon.c 
-- This is an ncurses-based program which displaces the 20-or-so most recent lines from the Scheduler log file (mselog.txt), and also shows the current time in multiple formats.  This utility is useful for monitoring the interaction between Scheduler and subsystems, especially when timed execution of commands is used.

test1.sh
test2.sh
test3.sh
test4.sh
test5.sh
test6.sh
test7.sh
test8.sh
test9.sh
-- Shell scripts used to demonstrate and test the software.  See the "Quick Start" and "Defined Test Cases" sections below for more information.

ms_shutdown.sh
-- A simple shell script that kills ms_exec and any ms_mcic processes in the event that these processes have not (or cannot) be shut down in an orderly way.

NU1_MIB_init.dat
NU2_MIB_init.dat
NU3_MIB_init.dat
NU4_MIB_init.dat
SHL_MIB_init.dat
-- These are human-readable/editable text-format MIB files used by ms_init (via dat2dbm) to generate dbm-format MIB files for SHL and the mock subsystems (NU1, NU2, NU3, and NU4).  (The corresponding files for ASP, NDP, and DR1...DR5 are generated by ms_makeMIB_ASP.c,  ms_makeMIB_NDP.c, and ms_makeMIB_DR.c respectively.)  These files tell MCS/Scheduler (through dat2dbm) the necessary details about the structure of the subsystem's MIB.   

mch_minimal_server.py
mch_minimal_server_acceptall.py
-- Python scripts which can be used to emulate a minimally-functional MCS Common ICD-compliant subsystem.  One of the command line arguments is the subsystem three-letter name, so this script can be used to emulate any subsystem, although at a very limited level.  The difference between the two is that the "..._acceptall.py" responds to every command with an "A" (i.e., "accept"); that is, no command is rejected, regardless of how absurd it might be.  (The latter is occassionally useful for testing.)  

readme_SHL_EI.txt
-- How to use MCS/Scheduler as an Engineering Interface for SHL (applicable to ASP, NDP, etc. as well)


Quick Start
===========

In the procedure below, the software is compiled and a simple test is performed to demonstrate the functioning of the software.  

(0)  It is strongly recommended that you first run the MCS/TaskProcessor utility "tptc" on the computer on which MCS/Scheduler is to run, to ensure that time is set correctly and that the PC, OS, and LWA time-handling routines are working properly.

(1)  If not done already, place explode the directories/files from the distribution and cd into the directory src/sch/.

(2)  Make the software ($ make clean; make; make install).  Although very popular, not all computers will have the necessary libraries "gdbm" (used to handle the dbm-format MIB files) and "ncurses" (used to make text-based displays) installed. If make complains about either of these being unavailable, see the "Troubleshooting" section below.  The executables and various support files are placed in ~/sch; cd there before continuing. 

(3)  Ensure that ports 1738 and 1739 are not in use on your computer.  (On Ubuntu, "$ cat /etc/services" shows you a list of all committed ports.)  If these ports are in use, you will need to modify the test script (test1.sh) to change all instances of these ports to ports which are available.

(4)  (Optional)  In a separate terminal window, cd to ~/sch and run ms_mon ("$ ms_mon 1").  Initially, you will see nothing other than a screen which is blank except in the bottom line you will see date (yymmdd hh:mm:ss) followed by the current MJD and MPM, updating once per second.  As you run the various tests below, the most recent contents from the MCS/Scheduler log file, mselog.txt, will be shown and updated once per second. 

(5)  Run test1.sh ("$ sh ./test1.sh" should do it).  This script does the following things: 
-- Launches a subsystem emulator for a minimal (mock) subsystem called NU1.
-- Brings up MCS/Scheduler with NU1 as a defined subsystem.
-- Shows the current value of SUMMARY for NU1 ("UNK" for unknown) using ms_mdre.
-- Sends NU1 a PNG.
-- Shows the new value of SUMMARY for NU1 ("NORMAL") using ms_mdre
-- Shuts down MCS/Scheduler and the NU1 subystem emulator
The console output will look something like this ("**" indicates comments added here which do not actually appear in the output)

$ ./ms_init test1.dat
[1] I am ms_init (v.20101016.1)
[1] ms_init_filename: <test1.dat>
[1] mibinit NU1
[5] I am dat2dbm (v.20090816.2)
[5] exit(EXIT_SUCCESS)
[1] mcic NU1
[6/9224] I am ms_mcic (v.20101113.1) 
[6/9224] NU1 specified
[6/9224] IP_ADDRESS <127.0.0.1>
[6/9224] TX_PORT = 1738
[6/9224] RX_PORT = 1739
[1] From NU1's MQ: <I'm up and running>
[1] From NU1's MQ: <I saw a PNG>
[1] WARNING: ms_init_file command <> not recognized (ignored)
[1] Completed ms_init start-up script
[1] Launching ms_exec...
[1] Launching ms_mdre_ip...
[16/9226] I am ms_mdre_ip (v.20101016.1) 
[2] I am ms_exec (v.20110208.1)
[1] exit(EXIT_SUCCESS)

** ms_init ("[1]") runs, sets everything up, launches ms_exec ("[2]"), and quits. 
** "[6]" is the ms_mcic process talking.

$ ./ms_mdre NU1 SUMMARY
UNK
110215 13:07:17

** "UNK" means unknown.  The time (UT, yymmdd hh:mm:ss) of last update is shown in the next line.

$ ./msei NU1 PNG
[7] ref=2, bAccept=1, eSummary=0, data=<Task has been queued>

** msei ("[7]") instructs MCS/Scheduler (via ms_exec) to send PNG to NU1.  msei does not wait for, or provide, a response.  

$ ./ms_mdre NU1 SUMMARY
NORMAL
110215 13:07:26

** note MIB entry has now been updated to NORMAL (note also time).     

$ ./msei MCS SHT
[6/9224] Directed to shut down. I'm out...
[7] ref=0, bAccept=1, eSummary=5, data=<Starting shutdown>

** MCS can be sent PNG and SHT commands just like subsystems.  If you tell MCS to SHT, ms_exec will shut down the ms_mcic's and associated message queues in an orderly way, and then exits itself.  

Killed python(9216) with signal 15


(6)  Check out the file mselog.txt, which is a log file created by ms_exec (or, if you started ms_mon in step 4, you will already be seeing this...).  It should look something like this:

$ cat mselog.txt 
110215 13:07:18  55607  47238251 N I am ms_exec (v.20110208.1) [2]
110215 13:07:18  55607  47238251 N Command line: ms_exec 1
110215 13:07:18  55607  47238251 N Added subsystem MCS
110215 13:07:18  55607  47238251 N Added subsystem NU1
110215 13:07:26  55607  47246261 T         2 1 NU1 PNG |
110215 13:07:26  55607  47246262 T         2 2 NU1 PNG |
110215 13:07:26  55607  47246310 T         2 3 NU1 PNG |
110215 13:07:27  55607  47247266 N Starting shutdown...
110215 13:07:27  55607  47247266 T         3 1 NU1 SHT Request ms_mcic shutdown|
110215 13:07:27  55607  47247267 T         3 2 NU1 SHT Request ms_mcic shutdown|
110215 13:07:28  55607  47248267 N Deleting tx msg queue for NU1
110215 13:07:28  55607  47248267 N Killing the ms_mdre_ip process
110215 13:07:28  55607  47248280 N ms_exec shutdown complete

The first 5 columns are: yymmdd hh:mm:ss (UT), MJD, MPM, "N" (for "info" messages) or "T" (for "task progress" messages).  Info messages conclude with remarks.  Task progress ("T") messages have 5 more columns:  REFERENCE, task progress, subsystem, command, and remarks.  Task progress is indicated as a number (defined in mcs.h).  Here, 1 (LWA_MSELOG_TP_QUEUED) means the task has been queued by ms_exec, but not yet sent to the ms_mcic, 2 (LWA_MSELOG_TP_SENT) means the task has been sent to the ms_mcic, and 3 (LWA_MSELOG_TP_SUCCESS) means the task has completed successfully (i.e., ms_mcic reports that the subsystem responded, and that it has successfully updated the local MIB). The pipe ("|") symbols are used to denote the end of a data field.  Note that data which raw binary (typically, to/from NDP) may be excluded or may appear as one or more "@" characters.


(7)  Check out the current MIB for NU1 (as known to the scheduler) using ms_mdr ("$ ./ms_mdr NU1").  It should look something like this (This is a wide display so if you see line wrapping, increase the width of your display):

$ ./ms_mdr NU1
MCH_TX_PORT                      1 0.2          1738                             a5     NUL    |110215 13:07:17
INFO                             1 1.2          UNK                              a256   a256   |110215 13:07:17
SUBSYSTEM                        1 1.4          UNK                              a3     a3     |110215 13:07:17
MCS-RESERVED                     0 1            NUL                              NUL    NUL    |110215 13:07:17
SERIALNO                         1 1.5          UNK                              a5     a5     |110215 13:07:17
MCH_RX_PORT                      1 0.3          1739                             a5     NUL    |110215 13:07:17
LASTLOG                          1 1.3          UNK                              a256   a256   |110215 13:07:17
SUMMARY                          1 1.1          NORMAL                           a7     a7     |110215 13:07:26
MCH_IP_ADDRESS                   1 0.1          127.0.0.1                        a15    NUL    |110215 13:07:17
VERSION                          1 1.6          UNK                              a256   a256   |110215 13:07:17
[8/9748] exit(EXIT_SUCCESS)

The columns are: MIB label, "0" or "1" (indicating branch or value, respectively), MIB index, value, a format indicator (for internal use only), another format indicator (for internal use only), and the UT date/time.  Note most MIB entries are "UNK" (unknown) because we never asked for them.  Only SUMMARY has been updated (as a result of the PNG command). Several MIB entries ("MCH_IP_ADDRESS", "MCH_TX_PORT", and "MCH_RX_PORT") were not part of the initial MIB, but are added by ms_init (via dat2dbm).  The entries stored in dbm files are in no particular order, and ms_mdr makes no attempt to sort them.  The ms_md2t utility can also be used to obtain this information, in which case the list *is* sorted and also an attempt is made to display non-text (e.g., raw binary) entries in readable form. 


(8)  Other ways to use Scheduler include the ncurses-based utilities "ms_mb" (useful for monitoring subsystem MIBs) and "ms_mon" (useful for monitoring the Scheduler log file; see step 4 above).   


(9)  Scheduler can be used to *schedule* the timing of commands.  For example, it is possible to specify the MJD and MPM at which any given command message is to be sent to a subsystem.  However, this capability is supported only through MCS/Executive; for example, through the utility "mesix" (see the MCS/Executive readme for an intro, and the mesix source code for details).  msei cannot be used to schedule command messages.     


Defined Test Cases
==================

Shell scripts are provided to evaluate the software:

test1.sh
This is the script used in the "Quick Start" section, above. 

test2.sh
Similar to test1.sh, except the entire MIB (not just summary) is updated.  Thus, demonstrates the "RPT" command.  Witness the results using "$ ./ms_mdr NU1" (or ms_mb) to see the updated MIB, and "$ cat mselog.txt" (or ms_mon) to see the log.

test3.sh
Similar to test2.sh, except does this for a system of *four* mock subsystems.  These subsystems are called NU1, NU2, NU3, and NU4.  These mock subsystems come up with unique MIB values so as to allow the user to verify that MCS/Scheduler is accessing the correct subsystem; for example, VERSION for NU3 is "NU3-1".

test4.sh
Brings MCS/Scheduler with the four mock subsystems NU1, NU2, NU3, and NU4.  Then, sends each 120 "PNG" commands as quickly as possible.  This tests MCS/Scheduler's ability to juggle this without overflowing an internal task queue or experiencing some other load-related error.  To verify success, check mselog.txt and make sure that there are no "task progress" indicators greater than 3; i.e., that all tasks terminate with status "3" ("LWA_MSELOG_TP_SUCCESS").

test5.sh (SHL demo)
Brings up SHL (using the MCS0012 emulator, but easily modified to accomodate the actual SHL), updates a few MIB entries using RPT, and tests the SHL-specific commands.  This emulator is installed as part of MCS/Scheduler, but if for some reason it is missing you should install it (MCS0012) into the subdirectory "Emulator_SHL" first.  (Note that the MCS0012 SHL emulation software has the bug described in Note 3 of the Scheduler version 0.6 release notes, which results in an extra space at the beginning of each MIB entry.  Nevertheless, it should work OK for this test.)

test6.sh (Crude ASP demo)
Brings up ASP (using the generic python-based subsystem emulator, but easily modified to accomodate an ASP-savvy emulator or the actual ASP), updates a few MIB entries using RPT, and tests ASP-specific commands.  Also demonstrates use of ms_makeMIB_ASP.c to generate an initial MIB for ASP.  Note that the emulator won't recognize the ASP-specific MIB entries and commands, but at least you'll see that that MCS/Scheduler is recognizing the commands and handling the errors being returned by the emulator in a reasonable way.

test7.sh (Crude NDP demo) 
Brings up NDP (using the generic python-based subsystem emulator, but easily modified to accomodate an NDP-savvy emulator or the actual NDP), updates a few MIB entries using RPT, and tests NDP-specific commands.  Also demonstrates use of ms_makeMIB_NDP.c to generate an initial MIB for NDP.  Note that the emulator won't recognize the NDP-specific MIB entries and commands, but at least you'll see that that MCS/Scheduler is recognizing the commands and handling the errors being returned by the emulator in a reasonable way. 

test8.sh (Crude MCS-DR demo) 
Exercises MCS/Scheduler in a scenario with all 5 MCS-DR subsystems (DR1, DR2, ... DR5) using the generic python-based emulator software to emulate the MCS-DRs.  Note that the emulator won't recognize the MCS-DR-specific MIB entries and commands, but at least you'll see that that MCS/Scheduler is recognizing the subsystems and updating the correct MIBs.  Also demonstrates use of ms_makeMIB_DR.c to generate an initial MIB for each of the data recorders.   

test9.sh (Crude "full-shelter" multi-subsystem demo) 
This script brings up SHL, ASP, NDP, and DR1 through DR5 using generic python-based subsystem emulators.  Unlike test1.sh through test8.sh, this script simply brings up the mock subsystems and then exits.  The main purpose of this script is to facilitate testing of MCS/Executive software.  To discontinue the demo, issue a "SHT" command to MCS using "msei" (or the MCS/Executive equivalent, "mesix"; or just use "ms_shutdown.sh") and then do "$ killall -v python" to stop the subsystem emulators.  NOTE: Although the emulators won't recognize the subsystem-specific MIB entries and commands, the emulator used in this case (which is different from the emulator used in the above scripts) will ALWAYS accept commands.  The reason for this is that MCS/Scheduler will update non-ICD MIB entries only if the associated commands are accepted.  Thus, the "acceptall" behavior is useful for testing in lieu of the actual subsystem.


Modifying Test Cases for Network Operation
==========================================

As mentioned above and explained in the scripts, the difference between running everything on one computer (e.g., for development and test) and running processes on different computers (e.g., the actual operational condition, where ms_exec and the ms_mcic's are on one computer, and the subsystems are on other computers) is very simple -- just replace the loopback IP (127.0.0.1), whereever it appears in the test scripts (test#.sh), with the appropriate IP address.   


Why dbm Files?
==============

The reader may be curious as to why this software uses the "dbm" facility as opposed to some other method (e.g., ASCII files, packed/binary files, XML, SQL) to store MIB information.  The primary reason is that the dbm facility provides a C-friendly database capability that is lightweight, compact, fast, requires no separate server (in contrast to something like SQL), and is very popular and well-documented. Also, Python enthusiasts should note that Python can easily read/manipulate dbm files ("batteries included", as usual).      


Why ncurses?
==============

The library "ncurses" is used by ms_mb and ms_mon to create simple text-based status displays for showing subsystem MIB information.  Alternatively, this could be done using a graphical user interface (GUI), and this is in no way precluded by MCS/Scheduler software.  A primary motivation for using ncurses is that it provides a simple way to create displays which can be easily modified or customized using human-readable text-based configuration files.  A second reason for using this approach has to do with remote monitoring; that is, monitoring from a remote location over the internet.  Because the display is text-based, it can be viewed from pretty much anywhere the user can get an xterm with a command prompt.  That is, ms_mb can run in the shelter, and information sent between the shelter and the remote user is just xterm screen updates.  Although a similar method of operation is possible with a GUI, this approach is not as simple or robust, as it depends on either (1) the GUI being able to run on the 
remote PC, or (2) the GUI being able to run on a PC in the shelter, with graphical output being sent over the limited bandwidth to/from the shelter.      


Troubleshooting / Known Bugs
============================

-- For problems related to the dbm library, the book "Beginning Linux Programming" (N. Matthew and R. Stones, 4th Ed, Wrox Press, 2008) is recommended. The dbm library is not necessarily preinstalled on all *nix distributions, and sometimes goes by different names; the book will be helpful in figuring how to get dbm installed if you are using something other than Ubuntu. On Ubuntu, it's currently the package "libgdbm-dev".

-- If your computer complains that the ncurses library is not available, it should be very simple to find the suitable package to install regardless of your platform.  On Ubuntu, it's currently the packages "libncurses5" and "libncurses5-dev".  Also see the Matthew and Stones reference above for more information about ncurses generally.

-- User-side communication with the ms_exec process is via a TCP socket connection.  If ms_exec process is restarted within a few seconds of being killed, it is possible that the operating system will not yet have released the socket address. In this case, ms_exec will experience a fatal error during start up, including a console message reading something like "ms_exec: Address already in use", possibly also referring to an error in the "bind()" operation.  If this happens, simply kill all Scheduler processes (you can use ms_shutdown.sh for this) and wait a few seconds longer before beginning again.  Note that this is not a bug; it simply reflects the fact that the operating system requires several seconds to free socket addresses even when the associated sockets are properly and explicitly closed. 

-- If the software behaves strangely, then it could be because some leftover process(es) from a previous (aborted) test are getting in the way.  A simple way to make sure this is not the case is as follows:
$ sh ./ms_shutdown.sh
$ killall -v python 
Wait a minute or two, then try again.  This kills any ms_init, ms_exec, and ms_mcic processes, as well as anything that was started using 
"$ python ..." (e.g., the subsystem emulators).

-- The function LWA_time2tv() currently assumes that the number of milliseconds in a day is a constant.  This will eventually produce intermittent small errors since this is not exactly true.  

-- The message "[1] WARNING: ms_init_file command <> not recognized (ignored)" is of no consequence (just a wart I have yet to fix...).

-- Seeing an extra character (e.g., a space) at the beginning of MIB entries?  If this problem did not exist with MCS/Scheduler software versions 0.5 or earlier, this could be because of the comedy of errors described in note 3 of the "Notable changes in this release" section of the version 0.6 readme.  (This is included in the "History" section, below.)

-- If msei says something like "FATAL: cmd <???> not recognized as valid for NDP", it's probably because you are trying to send a FST, BAM, or DRX command.  These were never implemented in msei.  Use mesix (the MCS/Exec version of msei) instead.

-- ms_mdr and ms_mdre do not account for the big-endianness of NDP's responses to command messages.  ms_md2t and ms_mb do.  So an alternative to ms_mdr for reading the NDP subsytems MIB is something like "$ ms_md2t NDP | grep 'x'" where x is the MIB label.
 

Other Notes & Issues 
====================

-- MCS/Scheduler makes no attempt update MIB entries upon start up.  Further more, MCS/Scheduler may initialize MIB values to numbers which are "interesting" (i.e., not all zero or NUL) so that it can be verified that MIB readers (e.g., ms_mdr and ms_mdre) are able to read non-trivial values.  Thus, MIB entries should not be believed until they are explicitly updated. 


History
=======

Major Changes in Release 0.9.1:
-------------------------------

1.  The source code files "LWA_MCS.h" and "LWA_MCS_subsystems.h" are deprecated and removed from the distribution.  All source code now uses the file ../common/mcs.h, which is a common header file used also by MCS/Executive and MCS/TaskProcessor.  At the same time, bugs in the LWA_time2tv() and LWA_timeval() functions (causing MJDs to be off by 2 days in some cases) were corrected.

Major Changes in Release 0.9:
-----------------------------

1.  With this version, MCS/Scheduler is now "beta".  That is, all required functionality is implemented and has been lab-tested, but has not received extensive user/field testing.  Previous (alpha) versions of MCS/Scheduler *have* been extensively tested by other users and in field conditions.

2.  MCS/Scheduler now has the ability to accept commands for execution at a later scheduled time.  In previous versions, the software simply executed commands as quickly as possible, in the order received.  Starting in this version, commands specified to run in the future are accepted and queued until the specified time.  It should be noted that the "pending task queue", in which scheduled commands are stored, is of finite length.  When that length is exceeded, subsequent commands are rejected until more room becomes available in the queue.  To use this capability, refer to the MCS/Executive utility "mesix"; see step 9 in the Quick Start section.

3.  Add the utility "ms_mon", an ncurses-based program which displaces the 20-or-so most recent mselog lines, and also shows the current time in multiple formats.  This utility is useful for monitoring the interaction between Scheduler and subsystems, especially when timed execution of commands is used.  See the quick start (step 4) to get a feeling how this works.
   
4.  Significantly improved the function LWA_time2tv(), which should now be much more accurate.

Major Changes in Release 0.8:
-----------------------------

1.  The DP commands FST, BAM, and DRX are now fully supported.  

2.  Non-ICD MIB entries have been added to the DP MIB as a means to monitor parameters (e.g., beamforming delays) that are sent as arguments of the FST, BAM, and DRX commands.

3.  When the option exists, the MCS/Executive utility "mesix" should be used in lieu of the MCS/Scheduler utility "msei".  The utility "mesi" is still supported and still works, but cannot be used to send the DP FST, BAM, or DRX commands.

4.  Added two scripts to support testing of MCS/Executive software: test9.sh and mch_minimal_server_acceptall.py.  

Major Changes in Release 0.7:
-----------------------------

1.  This version is released primarily to facilitate integration with nascent MCS/Executive software.  The primary difference is that ms_init now launches a background process "ms_mdre_ip", which is functionally similar to ms_mdre but which can be accessed directly via TCP/IP in the same manner that ms_exec is accessed.     

2.  The directory structure and makefile behavior have been significantly changed.  The makefile now places executables and support files in a separate directory.

Major Changes in Release 0.6.1:
------------------------------

1.  This version is released to fix a bug in ms_mcic.h that caused problems when binary DATA fields containing more than 256 bytes were received in response messages from subsystems.  

Major Changes in Release 0.6:
-----------------------------

1.  Support for MCS-DR has been added.  That is, MCS-DR PCs can now be controlled and monitored via the MCS Tier-1 interface; e.g., using "msei" and "ms_mdr"/"ms_mu".   The subsystems are named DR1, DR2, ... DR5.  Should be fully-compliant with version 1.0 of the MCS-DR ICD (LWA Engineering Memo MCS0025), except in a half-dozen or so places where typographical errors were noted in the MCS-DR ICD (expected to be resolved in a near-future revision of the ICD).  Also, note that DROS version 1.0 (LWA Engineering Memo MCS0027) contains several bugs that may result in a few rejected commands or otherwise unexpected results; these are fixed in version 1.1 which is due to be released soon.  

2.  Fixed potential bug in parsing of responses from subsystems (in ms_mcic.c) in which lack of whitespace between certain fields would have messed up the parsing (discovered during integration with MCS-DR, which was zero-padding numeric fields in response messages).  

3.  Fixed bug in ms_mcic.c which may impact software written by others; see the end of this paragraph for a test to see if your subsystem is affected.  The bug was that R-COMMENT (the last field of the subsystem response message) was previously being read starting with the 48th byte (that is, message_string[47] as it appears in ms_mcic.c), as opposed to the *47th* byte, as is specified in the MCS Common ICD.  If your subsystem structured the R-COMMENT field correctly, then this bug resulted in the first character of the field being ignored by MCS/Scheduler.  This went unnoticed for a long time because in many cases the subsystem software includes a bug which inserts a extra space between the R-SUMMARY and R-COMMENT fields in response messages; thus the result of the two errors was a correct R-COMMENT value.  However, the MCS-DR software makes extensive use of the R-COMMENT field and starts the field at the correct byte; thus MCS/Scheduler software has been corrected so that MCS-DR may be properly supported.  
HOW TO KNOW IF YOUR SUBSYSTEM SOFTWARE IS AFFECTED: Try the following experiment: 
$ ./msei ABC RPT SUBSYSTEM (where "ABC" is the subsystem in question)
$ ./ms_mdre ABC SUBSYSYEM
If the result is a character (e.g., space) followed by "ABC", then you need to fix your R-COMMENT fields to start at the correct byte.  If the result is "ABC" (no leading characters), then you are compliant.  If there is a chance you handled different MIB entries differently, you should check all MIB entries, not just "SUBSYSTEM".

4.  Fixed bug in mch_minimal_server.py (generic subsystem emulator) in which extra space was inserted between R-SUMMARY and R-COMMENT (related to topic of #3 above!).

Major Changes in Release 0.5:
-----------------------------

1. Additional Tier-1 monitoring/control programs have been provided.  These include: ms_md2t, which converts a MIB dbm file to text (unlike ms_mdr, the output is sorted in order of MIB index, and many raw binary values also are converted to human-readable text);  ms_mu, which uses MCS/Scheduler to update every ICD-defined MIB value for a subsystem (periodically, if desired); and ms_mb, an easily-reconfigurable ncurses-based subsystem MIB monitor which also can be used to do periodic MIB updates.  See below for additional information about these new programs, and also see LWA Engineering Memo MCS0029 for examples of how they can used.

2. Enhancements to monitoring and control of DP.  MCS/Scheduler now adds MIB entries (numbered 0.4 and 0.5) to the DP MIB to allow easier monitoring of TBW and TBN status.  This is a work-around for the problem created by the fact that the MIB defined in the DP ICD does not include many parameters which need to be monitored.  For example, the DP ICD-defined MIB has no entry for TBN center frequency.  To accommodate this, MCS/Scheduler adds TBN center frequency (TBN_FREQ) to the locally-maintained MIB.  TBN_FREQ is updated by MCS/Scheduler whenever a sent TBN command (which includes TBN center frequency as an argument) is acknowledged.  Although these new MIB entries can be monitored in exactly the same way as ICD-defined MIB entries (that is, through the tier-1 interface using, for example, ms_md2t), DP will not recognize these entries, so they cannot be updated using the "RPT" command.   Most of the new entries are in the same raw binary form required by DP command message formats, however ms_md2t converts 
many (but not all) of these to human-readable text form.  To determine which entries MCS/Scheduler has added to the local MIB, use ms_md2t to look for entries with indices less than 1.  

3. The new MIB browser (ms_mb) uses the popular ncurses library to generate text-based xterm displays.  As a result, ncurses is now required for "make".  See comments below about how to deal with this if your computer complains about this library being missing.  Also see the new section "Why ncurses?" below.  

Major Changes in Release 0.4:
-----------------------------

1. Increased dimension of cmd[] and dest[] in msei.c to prevent the "[7] FATAL: subsystem <> not recognized" error reported on some systems.

2. Minor editing of this (readme) file in preparation for CDR review.

Major Changes in Release 0.3:
-----------------------------

1. DP is now partially supported, in addition to the previously-supported subsystems SHL, ASP, and the mock subsystems (NU1..NU4).  For DP, the MIB is fully supported as well as the commands TBW, TBN, CLK, and INI.  The DP commands DRX, BAM, and FST are not yet supported.

2. Although it should not be apparent to users of the previous version, many changes have been made in order to accommodate the raw binary parameters specified in the DP ICD.  Many changes have been made in order to make sure values are displayed in a reasonable, consistent way (e.g., as an integer when the data represents an integer, regardless of whether the data is being transported as characters or as raw binary) in log messages and ms_mdr(e) output.    

Major Changes in Release 0.2:
-----------------------------

1. SHL and ASP are now fully supported

2. Added test script (test5.sh) which tests support for SHL using the MCS0012 SHL emulator (easily modified to handle the actual subsystem)

3. Added a document "readme_SHL_EI.txt" which explains how to use this software to implement a simple engineering interface for SHL. (This can be easily adapted to work for ASP as well)

Release Notes from Release 0.1:
-------------------------------

This is software developed for the MCS "Scheduler" computer.  The Scheduler computer accepts commands from the MCS "Executive" computer (or, for development/testing purposes, something emulating the Executive) and interfaces with subsystems (or things emulating subsystems).  Also provided are software and scripts which can be used to demonstrate and evaluate the software, including control programs and a subsystem emulation program.  

Note that the software is designed such that everything can be run on a single computer without modification; the difference is simply whether interprocess communications are directed to separate computers using network IP addresses, or to local processes using the loopback (127.0.0.1) IP address.

The software is written entirely in ANSI C and was developed on Ubuntu Linux 9.04.  Shell scripts (provided) are used for demonstration and testing.  A subsystem emulator (also provided) is written in Python.

This software is considered "pre-alpha". Specifically, it is functional and released to facilitate review and comment; but not all required features are implemented, and thorough testing has not yet been done.  This software exists in the author's Subversion archive as revision 25.

Limitations of this release of the software include:

-- Only the commands PNG, RPT, and SHT are supported.

-- Only the MCS-RESERVED branch of the MIB is fully supported.

-- No subsystem-specific commands or MIB entries are explicitly supported.  (It might be possible to RPT MIB entries other than those in the MCS-RESERVED branch, but this has not been tested.)   

-- No specific subsystem is explicitly supported; instead, 4 mock subsystems (designated NU1 through NU4) are defined for the purposes of development and test of this software.  The mock subsystems behave as actual LWA subsystems, but are limited to commands and MIB entries identified above. 

-- The software currently sends commands to subsystems as quickly as possible, without regard for scheduling.  That is, the Scheduler (ironically) does not yet respect requests to queue tasks until a predefined future time.  (The task queue architecture for doing this exists and is used; it is simply that the Scheduler does not yet use scheduled time as a criterion for when to send commands to subsystems.)
