#!/bin/bash

# test4.sh: S.W. Ellingson, Virginia Tech, 2009 Aug 04
# 
# 1. Launches subsystem emulators for minimal subsystems called NU1, NU2, NU3, and NU4.
# 2. Brings up MCS/Scheduler with NU1-NU4 as defined subsystems.
# 3. Fires off 120 PNGs to each subsystem and waits for the smoke to clear.
# 4. Shuts down MCS/Scheduler and the subystem emulator
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
    rm -f test4.dat

    # Report exit status
    if [ $exit_code -ne 0 ]; then
        echo "Test failed with exit code $exit_code"
    fi
    exit $exit_code
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Fire up subsystem emulators for NU1 - NU4
python3 mch_minimal_server.py NU1 127.0.0.1 1739 1738 &
SERVER_PID1=$!
python3 mch_minimal_server.py NU2 127.0.0.1 1741 1740 &
SERVER_PID2=$!
python3 mch_minimal_server.py NU3 127.0.0.1 1743 1742 &
SERVER_PID3=$!
python3 mch_minimal_server.py NU4 127.0.0.1 1745 1744 &
SERVER_PID4=$!
sleep 1

# Create an ms_init initialization file called "test3.dat" 
echo \
'mibinit NU1 127.0.0.1 1738 1739
mcic    NU1
mibinit NU2 127.0.0.1 1740 1741
mcic    NU2
mibinit NU3 127.0.0.1 1742 1743
mcic    NU3
mibinit NU4 127.0.0.1 1744 1745
mcic    NU4' > test4.dat

# MCS/Scheduler start (allow a few seconds to get everything running)
./ms_init test4.dat
sleep 5

# Send 120 PNGs to each subsystem
for i in `seq 120`
  do
  ./msei NU1 PNG
  ./msei NU2 PNG
  ./msei NU3 PNG
  ./msei NU4 PNG  
  done
sleep 10

# Send MCS/Scheduler shutdown command 
./msei MCS SHT
