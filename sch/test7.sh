#!/bin/bash

# test7.sh: S.W. Ellingson, Virginia Tech, 2009 Aug 25
#
# This script tests MCS/Scheduler's handling of DP
# using the Python-based generic emulator (although VERY easy to modify
# to use with actual DP subsystem).
# 
# 1. Launches generic subsystem emulator
# 2. Creates a MIB initialization file 
# 3. Brings up MCS/Scheduler with DP as a defined subsystem.
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

# Exit on any error
set -e 

# Cleanup function to ensure we always kill the server and remove temp files
cleanup() {
    local exit_code=$?
    echo "Cleaning up..."
    
    # Kill Python server if running
    if [ ! -z "$SERVER_PID" ]; then
        echo "Killing Python server (PID: $SERVER_PID)"
        kill $SERVER_PID 2>/dev/null || true
    fi

    # Remove temp files
    rm -f test7.dat

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

# Fire up an emulator to play the role of DP/ADP
python3 mch_minimal_server.py ${dpName} 127.0.0.1 1739 1738 &
SERVER_PID=$!
sleep 1

# Create a DP/ADP MIB initialization file 
if [ "${dpName}" == "DP_" ]; then
	# DP
	./ms_makeMIB_DP 
else
	# ADP
	./ms_makeMIB_ADP
fi

# Create an ms_init initialization file called "test7.dat" 
echo \
"mibinit ${dpName} 127.0.0.1 1738 1739
mcic    ${dpName}" > test7.dat

# MCS/Scheduler start (allow a few seconds to get everything running)
./ms_init test7.dat
sleep 5

# Get MCS-RESERVED MIB entry values
# Note SUMMARY gets updated with every response, so no need to 
# explicitly get that
./msei ${dpName} RPT INFO
./msei ${dpName} RPT LASTLOG
./msei ${dpName} RPT SUBSYSTEM
./msei ${dpName} RPT SERIALNO
./msei ${dpName} RPT VERSION

# Here, it would be good to try to RPT some DP-specific MIB entries
# However, there is no point since these will fail with the generic emulator described above.
# Expect garbage unless a DP-capable emulator or the actual subsystem is used.


# Testing DP-specific commands
# Note these will be handled correctly by the MCS software, but will not be
# recognized by the limited subsystem emulator used here.  
./msei ${dpName} TBW 1 2 3
./msei ${dpName} TBN 38.0 7 15 99
./msei ${dpName} INI

# Time for responses to be received/processed before shutting down
sleep 5

# Send MCS/Scheduler shutdown command 
./msei MCS SHT
