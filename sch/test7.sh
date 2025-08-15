#!/bin/bash

# test7.sh: S.W. Ellingson, Virginia Tech, 2009 Aug 25
#
# This script tests MCS/Scheduler's handling of NDP
# using the Python-based generic emulator (although VERY easy to modify
# to use with actual NDP subsystem).
# 
# 1. Launches generic subsystem emulator
# 2. Creates a MIB initialization file 
# 3. Brings up MCS/Scheduler with DP as a defined subsystem.
# 4. Updates local MIB, one entry at a time, using the RPT command
#    Note: the emulator supports only the MCS-RESERVED branch.
# 5. Exercises NDP commands
#    Note: the emulator won't recognize NDP-specific commands
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

# Fire up an emulator to play the role of NDP
python3 mch_minimal_server.py NDP 127.0.0.1 1739 1738 &
SERVER_PID=$!
sleep 1

# Create a NDP MIB initialization file 
./ms_makeMIB_NDP

# Create an ms_init initialization file called "test7.dat" 
echo \
"mibinit NDP 127.0.0.1 1738 1739
mcic    NDP" > test7.dat

# MCS/Scheduler start (allow a few seconds to get everything running)
./ms_init test7.dat
sleep 5

# Get MCS-RESERVED MIB entry values
# Note SUMMARY gets updated with every response, so no need to 
# explicitly get that
./msei NDP RPT INFO
./msei NDP RPT LASTLOG
./msei NDP RPT SUBSYSTEM
./msei NDP RPT SERIALNO
./msei NDP RPT VERSION

# Here, it would be good to try to RPT some NDP-specific MIB entries
# However, there is no point since these will fail with the generic emulator described above.
# Expect garbage unless a NDP-capable emulator or the actual subsystem is used.


# Testing NDP-specific commands
# Note these will be handled correctly by the MCS software, but will not be
# recognized by the limited subsystem emulator used here.  
./msei NDP TBT 1 2 3
./msei NDP TBS 38.0 7 15 99
./msei NDP INI

# Time for responses to be received/processed before shutting down
sleep 5

# Send MCS/Scheduler shutdown command 
./msei MCS SHT
