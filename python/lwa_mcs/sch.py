"""
Module for controlling MCS scheduler via UDP packets.
"""

import time
import subprocess
from typing import List, Union

from lwa_mcs import mib
from lwa_mcs._mcs import send_sch_command, MCS_TIMEOUT

__version__ = "0.5"
__all__ = ['get_pids', 'is_replay', 'is_running', 'get_active_subsystems',
           'send_subsystem_command']


def get_pids() -> List[int]:
    """
    Return a list process IDs for all MCS/sch processes found.
    """
    
    output = subprocess.check_output(['ps', 'aux'], stderr=subprocess.DEVNULL)
    output = output.decode('ascii', errors='backslashreplace')
    output = output.split('\n')
        
    pids = []
    for line in output:
        fields = line.split(None, 10)
        if len(fields) != 11:
            continue
        if fields[-1].find('ms_mcic') != -1 \
           or fields[-1].find('ms_exec') != -1 \
           or fields[-1].find('ms_mdre_ip') != -1 \
           or fields[-1].find('ms_mdre_replay') != -1:
            pids.append(int(fields[1], 10))
            
    return pids


def is_replay() -> bool:
    """
    Determine if MCS/sch is running in "replay" mode.
    """
    
    output = subprocess.check_output(['ps', 'aux'], stderr=subprocess.DEVNULL)
    output = output.decode('ascii', errors='backslashreplace')
    output = output.split('\n')
    
    pids = []
    for line in output:
        fields = line.split(None, 10)
        if fields[-1].find('ms_exec_replay') != -1 \
           or fields[-1].find('ms_mdre_replay') != -1:
            pids.append(int(fields[1], 10))
            
    return (len(pids) == 2)


def is_running() -> bool:
    """
    Determine if MCS/sch should be considered operational.
    """
    
    pids = get_pids()
    min_active = 3
    if is_replay():
        min_active = 2
        
    return True if len(pids) >= min_active else False


def get_active_subsystems() -> List[str]:
    """
    Return a list of subsystems with active 'ms_mcic' processes.
    """
    
    output = subprocess.check_output(['ps', 'aux'], stderr=subprocess.DEVNULL)
    output = output.decode('ascii', errors='backslashreplace')
    output = output.split('\n')
    
    ss = []
    for line in output:
        fields = line.split(None, 10)
        if len(fields) != 11:
            continue
        if fields[-1].find('ms_mcic') != -1:
            ss.append(fields[-1].split(None, 1)[-1])
            
    return ss


def send_subsystem_command(ss: str, cmd: str="RPT", data: str="SUMMARY") -> Union[str,int,float,bytes,None]:
    """
    Use MCS/sch to send the given command to the specified subsystem.  For 
    'RPT' commands the value for the MIB is returned, otherwise the refernce
    ID is returned.
    """
    
    # Send the command
    ref, success = send_sch_command(ss, cmd, data)
    
    # Wait a bit...
    time.sleep(0.2)
    
    if cmd == "RPT":
        # Parse the response if this is a RPT command
        t0 = time.time()
        while (time.time() - t0) < MCS_TIMEOUT:
            try:
                val, ts = mib.read(ss, data)
                if (time.time() - ts) <= MCS_TIMEOUT:
                    value = val
                    break
                else:
                    value = 'TIMEOUT'
                    time.sleep(0.2)
                    
            except Exception as e:
                #print(str(e))
                value = None
    else:
        # Otherwise return the reference ID
        value = ref
        
    return value
