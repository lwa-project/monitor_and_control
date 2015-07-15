# test8.sh: S.W. Ellingson, Virginia Tech, 2010 Jun 07
#
# This script tests MCS/Scheduler's handling of MCS-DRs
# using the Python-based generic emulator (although VERY easy to modify
# to use with actual DP subsystem).
# 
# 1. Launches generic subsystem emulator
# 2. Creates a MIB initialization file 
# 3. Brings up MCS/Scheduler with DR1 as a defined subsystem.
# 4. Updates local MIB, one entry at a time, using the RPT command
#    Note: the emulator supports only the MCS-RESERVED branch.
# 5. Exercises DP commands
#    Note: the emulator won't recognize DP-specific commands
# 6. Shuts down MCS/Scheduler and the subystem emulator
# When done, consider using "$ ms_mdr" to see entire MIB,
# and "$ cat mselog.txt" to see the log.
#
# Note this script assumes all software running on the same computer
# (Otherwise, change 127.0.0.1 to appropriate IPs)

# Fire up emulators to play the role of DR1, DR2, DR3, DR4, DR5
python mch_minimal_server.py DR1 127.0.0.1 1739 1738 &
python mch_minimal_server.py DR2 127.0.0.1 1741 1740 &
python mch_minimal_server.py DR3 127.0.0.1 1743 1742 &
python mch_minimal_server.py DR4 127.0.0.1 1745 1744 &
python mch_minimal_server.py DR5 127.0.0.1 1747 1746 &

# Create DP MIB initialization files 
./ms_makeMIB_DR 1 5 5 2 8 5 3 10 2
./ms_makeMIB_DR 2 5 5 2 8 5 3 10 2
./ms_makeMIB_DR 3 5 5 2 8 5 3 10 2
./ms_makeMIB_DR 4 5 5 2 8 5 3 10 2
./ms_makeMIB_DR 5 5 5 2 8 5 3 10 2

# Create an ms_init initialization file called "test7.dat" 
echo \
'mibinit DR1 127.0.0.1 1738 1739
mcic    DR1
mibinit DR2 127.0.0.1 1740 1741
mcic    DR2
mibinit DR3 127.0.0.1 1742 1743
mcic    DR3
mibinit DR4 127.0.0.1 1744 1745
mcic    DR4
mibinit DR5 127.0.0.1 1746 1747
mcic    DR5' > test8.dat

# MCS/Scheduler start (allow a few seconds to get everything running)
./ms_init test8.dat
sleep 5

# Get MCS-RESERVED MIB entry values
# Note SUMMARY gets updated with every response, so no need to 
# explicitly get that
./msei DR1 RPT INFO
./msei DR2 RPT INFO
./msei DR3 RPT INFO
./msei DR4 RPT INFO
./msei DR5 RPT INFO
./msei DR1 RPT LASTLOG
./msei DR2 RPT LASTLOG
./msei DR3 RPT LASTLOG
./msei DR4 RPT LASTLOG
./msei DR5 RPT LASTLOG
./msei DR1 RPT SUBSYSTEM
./msei DR2 RPT SUBSYSTEM
./msei DR3 RPT SUBSYSTEM
./msei DR4 RPT SUBSYSTEM
./msei DR5 RPT SUBSYSTEM
./msei DR1 RPT SERIALNO
./msei DR2 RPT SERIALNO
./msei DR3 RPT SERIALNO
./msei DR4 RPT SERIALNO
./msei DR5 RPT SERIALNO
./msei DR1 RPT VERSION
./msei DR2 RPT VERSION
./msei DR3 RPT VERSION
./msei DR4 RPT VERSION
./msei DR5 RPT VERSION

# Here, it would be good to try to RPT some DP-specific MIB entries
# However, there is no point since these will fail with the generic emulator described above.
# Expect garbage unless a DP-capable emulator or the actual subsystem is used.



## Testing DP-specific commands
## Note these will be handled correctly by the MCS software, but will not be
## recognized by the limited subsystem emulator used here.  
#./msei DP_ TBW 1 2 3
#./msei DP_ TBN 38.0 7 15 99
#./msei DP_ CLK 1
#./msei DP_ INI

# Time for responses to be received/processed before shutting down
sleep 5

# Send MCS/Scheduler shutdown command 
./msei MCS SHT

# Shut down the subsystem emulator
killall -v python



