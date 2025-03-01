#!/bin/bash

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

# Exit on any error
set -e 

# Cleanup function to ensure we always kill the server and remove temp files
cleanup() {
    local exit_code=$?
    echo "Cleaning up..."
    
    # Kill Python server(s) if running
    i=1
    while true; do
        pidname="SERVER_PID${i}"
        if [ -z "${!pidname+x}" ]; then
            break
        fi
        
        SERVER_PID="${!pidname}"
        echo "Killing Python server (PID: $SERVER_PID)"
        kill $SERVER_PID 2>/dev/null || true
        
        i=$((i+1))
    done

    # Remove temp files
    rm -f test9.dat

    # Report exit status
    if [ $exit_code -ne 0 ]; then
        echo "Test failed with exit code $exit_code"
    fi
    exit $exit_code
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Figure out if we are DP, ADP, or NDP-compatible
usingADP=`strings msei | grep ADP `
usingNDP=`strings msei | grep NDP `
if [ "${usingADP}" != "" ]; then
	# ADP
	dpName="ADP"
else
	if [ "${usingNDP}" != "" ]; then
		# NDP
		dpName="NDP"
	else
		# DP
		dpName="DP_"
	fi
fi

# Fire up an emulator to play the role of subsystems
python3 mch_minimal_server.py --accept-all SHL 127.0.0.1 1739 1738 &
python3 mch_minimal_server.py --accept-all ASP 127.0.0.1 1741 1740 &
python3 mch_minimal_server.py --accept-all ${dpName} 127.0.0.1 1743 1742 &
python3 mch_minimal_server.py --accept-all DR1 127.0.0.1 1745 1744 &
if [ "${dpName}" == "DP_" ]; then
	python3 mch_minimal_server.py --accept-all DR2 127.0.0.1 1747 1746 &
	python3 mch_minimal_server.py --accept-all DR3 127.0.0.1 1749 1748 &
	python3 mch_minimal_server.py --accept-all DR4 127.0.0.1 1751 1750 &
	python3 mch_minimal_server.py --accept-all DR5 127.0.0.1 1753 1752 &
fi

# Create ASP MIB initialization file for an ASP with
# 3 ARX power supplies, 2 FEE power supplies, and 10 temperature sensors
./ms_makeMIB_ASP 3 2 10

# Creat SHL MIB initialization file for a SHL with
# 8 racks and 8 ports per rack
./ms_makeMIB_SHL 8 8

# Create a DP/ADP MIB initialization file 
if [ "${dpName}" == "DP_" ]; then
	# DP
	./ms_makeMIB_DP 
else
	# ADP
	./ms_makeMIB_ADP
fi

# Create DP MIB initialization files 
./ms_makeMIB_DR 1 5 5 2 8 5 3 10 2
if [ "${dpName}" == "DP_" ]; then
	./ms_makeMIB_DR 2 5 5 2 8 5 3 10 2
	./ms_makeMIB_DR 3 5 5 2 8 5 3 10 2
	./ms_makeMIB_DR 4 5 5 2 8 5 3 10 2
	./ms_makeMIB_DR 5 5 5 2 8 5 3 10 2
fi

# Create an ms_init initialization file called "test9.dat" 
echo \
"mibinit SHL 127.0.0.1 1738 1739
mcic    SHL
mibinit ASP 127.0.0.1 1740 1741
mcic    ASP
mibinit ${dpName} 127.0.0.1 1742 1743
mcic    ${dpName}
mibinit DR1 127.0.0.1 1744 1745
mcic    DR1" > test9.dat
if [ "${dpName}" == "DP_" ]; then
	echo \
"mibinit DR2 127.0.0.1 1746 1747
mcic    DR2
mibinit DR3 127.0.0.1 1748 1749
mcic    DR3
mibinit DR4 127.0.0.1 1750 1751
mcic    DR4
mibinit DR5 127.0.0.1 1752 1753
mcic    DR5" >> test9.dat
fi

# MCS/Scheduler start (allow a few seconds to get everything running)
./ms_init test9.dat
sleep 5

# Time for responses to be received/processed before shutting down
sleep 5

# Send MCS/Scheduler shutdown command 
./msei MCS SHT
