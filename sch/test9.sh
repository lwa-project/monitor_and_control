# test9.sh: S.W. Ellingson, Virginia Tech, 2010 Nov 16
#
# This script is provided to emulate a complete system consisting of 
# SHL, ASP, DP_, and DR1-DR5; primarily to facilitate development and
# test of MCS/Executive.  Here, the script:
# 
# 1. Launches a generic subsystem emulator to play the role of each subsystem.
# 2. Creates a MIB initialization file 
# 3. Brings up MCS/Scheduler.
# 4. Exits, leaving MCS/Scheduler and the subsystem emulators running.
#
# To discontinue the demo, issue a "SHT" command to MCS using "msei" 
# (or the MCS/Executive equivalent, "mesix"; or just use "ms_shutdown.sh")
# and then do "$ killall -v python" to stop the subsystem emulators.  
#
# NOTE: Although the emulators won't recognize the subsystem-specific MIB
# entries and commands, the emulator used in this case (which are different
# from the emulator used in the above scripts) will ALWAYS accept commands
# nevertheless.  
#
# Note this script assumes all software running on the same computer
# (Otherwise, change 127.0.0.1 to appropriate IPs)

# Fire up an emulator to play the role of subsystems
python mch_minimal_server_acceptall.py SHL 127.0.0.1 1739 1738 &
python mch_minimal_server_acceptall.py ASP 127.0.0.1 1741 1740 &
python mch_minimal_server_acceptall.py DP_ 127.0.0.1 1743 1742 &
python mch_minimal_server_acceptall.py DR1 127.0.0.1 1745 1744 &
python mch_minimal_server_acceptall.py DR2 127.0.0.1 1747 1746 &
python mch_minimal_server_acceptall.py DR3 127.0.0.1 1749 1748 &
python mch_minimal_server_acceptall.py DR4 127.0.0.1 1751 1750 &
python mch_minimal_server_acceptall.py DR5 127.0.0.1 1753 1752 &

# Create ASP MIB initialization file for an ASP with
# 3 ARX power supplies, 2 FEE power supplies, and 10 temperature sensors
./ms_makeMIB_ASP 3 2 10

# Create a DP MIB initialization file 
./ms_makeMIB_DP 

# Create DP MIB initialization files 
./ms_makeMIB_DR 1 5 5 2 8 5 3 10 2
./ms_makeMIB_DR 2 5 5 2 8 5 3 10 2
./ms_makeMIB_DR 3 5 5 2 8 5 3 10 2
./ms_makeMIB_DR 4 5 5 2 8 5 3 10 2
./ms_makeMIB_DR 5 5 5 2 8 5 3 10 2

# Create an ms_init initialization file called "test9.dat" 
echo \
'mibinit SHL 127.0.0.1 1738 1739
mcic    SHL
mibinit ASP 127.0.0.1 1740 1741
mcic    ASP
mibinit DP_ 127.0.0.1 1742 1743
mcic    DP_
mibinit DR1 127.0.0.1 1744 1745
mcic    DR1
mibinit DR2 127.0.0.1 1746 1747
mcic    DR2
mibinit DR3 127.0.0.1 1748 1749
mcic    DR3
mibinit DR4 127.0.0.1 1750 1751
mcic    DR4
mibinit DR5 127.0.0.1 1752 1753
mcic    DR5' > test9.dat

# MCS/Scheduler start (allow a few seconds to get everything running)
./ms_init test9.dat
sleep 5




