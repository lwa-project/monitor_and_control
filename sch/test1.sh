#!/bin/bash

# test1.sh: S.W. Ellingson, Virginia Tech, 2009 Aug 14
# 
# This is more or less a "hello world" for MCS/Scheduler
# 1. Launches a subsystem emulator for a minimal subsystem called NU1.
# 2. Brings up MCS/Scheduler with NU1 as a defined subsystem.
# 3. Shows the current value of SUMMARY for NU1 ("UNK" for unknown) using ms_mdre.
# 4. Sends NU1 a PNG.
# 5. Shows the new value of SUMMARY for NU1 ("NORMAL") using ms_mdre
# 6. Shuts down MCS/Scheduler and the NU1 subystem emulator
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
    rm -f test1.dat

    # Report exit status
    if [ $exit_code -ne 0 ]; then
        echo "Test failed with exit code $exit_code"
    fi
    exit $exit_code
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Fire up a subsystem emulator for NU1
python3 mch_minimal_server.py NU1 127.0.0.1 1739 1738 &
SERVER_PID=$!
sleep 1

# Create an ms_init initialization file called "test1.dat" 
# The first line converts "NU1_MIB_init.dat" into a dbm-format MIB file
#   and embeds information about where to find NU1 (the IP and ports used)
# The second line launches a subsystem handler process (an "mcic") for NU1
echo \
'mibinit NU1 127.0.0.1 1738 1739
mcic    NU1' > test1.dat

# MCS/Scheduler start (allow a few seconds to get everything running)
echo
echo '$ ./ms_init test1.dat'
./ms_init test1.dat
sleep 5

# Take a look at SUMMARY in the NU1 MIB 
echo
echo '$ ./ms_mdre NU1 SUMMARY'
./ms_mdre NU1 SUMMARY

# Send a PNG to NU1, and allow a moment for this to be processed
echo
echo '$ ./msei NU1 PNG'
./msei NU1 PNG
sleep 1

# Send a RPT SUMMARY to NU1, and allow a moment for this to be processed
echo
echo '$ ./msei NU1 RPT SUMMARY'
./msei NU1 RPT SUMMARY
sleep 1

# Take a look at SUMMARY in the NU1 MIB 
echo
echo '$ ./ms_mdre NU1 SUMMARY'
./ms_mdre NU1 SUMMARY
echo

# Wait and check for unsolicited updates (checking every 2 seconds for 12 seconds total)
for i in {1..6}; do
    sleep 2
    echo
    echo "Check $i for unsolicited updates:"
    echo '$ ./ms_mdre NU1 SUMMARY'
    ./ms_mdre NU1 SUMMARY
done

# Send MCS/Scheduler shutdown command 
echo
echo '$ ./msei MCS SHT'
./msei MCS SHT
echo
