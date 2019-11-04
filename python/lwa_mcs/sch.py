# -*- coding: utf-8 -*-

"""
Module for controlling MCS scheduler and executive via UDP packets.
"""

import time
import socket
import struct
import subprocess

from lwa_mcs.config import ADDRESSES, SOCKET_TIMEOUT, SUBSYSTEMS, COMMANDS
from lwa_mcs import mib

__version__ = "0.1"
__revision__ = "$Rev$"
__all__ = ['COMMAND_STRUCT', 'get_pids', 'is_running', 'get_active_subsystems',
           'send_subsystem_command']


COMMAND_STRUCT = struct.Struct('lliilliii256si')
#  1 sid: subsystem ID
#  2 ref: reference number
#  3 cid: command ID
#  4 scheduled? 1 = "do as close as possible to time in tv
#  5 tv.tv_sec: epoch seconds
#  6 tv.tv_usec: fractional remainder in microseconds
#  7 response: see LWA_MSELOG_TP_*
#  8 eSummary: see LWA_SIDSUM_*
#  9 eMIBerror: > 0 on error; see LWA_MIBERR_*
# 10 DATA on way out, R-COMMENT on way back
# 11 datalen: -1 for (printable) string; 0 for zero-len;
#    otherwise number of significant bytes


def get_pids():
    """
    Return a list process IDs for all MCS/sch processes found.
    """
    
    p = subprocess.Popen(['ps', 'aux'], stdout=subprocess.PIPE)
    o, e = p.communicate()
    
    pids = []
    for line in o.split('\n'):
        fields = line.split(None, 10)
        if fields[-1].find('ms_mcic') != -1 \
           or fields[-1].find('ms_exec') != -1 \
           or fields[-1].find('ms_mdre_ip') != -1:
            pids.append(int(fields[1], 10))
            
    return pids


def is_running():
    """
    Determine if MCS/sch should be considered operational.
    """
    
    pids = get_pids()
    return True if len(pids) >= 3 else False


def get_active_subsystems():
    """
    Return a list of subsystems with active 'ms_mcic' processes.
    """
    
    p = subprocess.Popen(['ps', 'aux'], stdout=subprocess.PIPE)
    o, e = p.communicate()
    
    ss = []
    for line in o.split('\n'):
        fields = line.split(None, 10)
        if fields[-1].find('ms_mcic') != -1:
            ss.append(fields[-1].split(None, 1)[-1])
            
    return ss


def send_subsystem_command(ss, cmd="RPT", data="SUMMARY"):
    """
    Use MCS/sch to send the given command to the specified subsystem.  For 
    'RPT' commands the value for the MIB is returned, otherwise the refernce
    ID is returned.
    """
    
    # Convert the subsystem name to an MCS ID code
    try:
        sid = SUBSYSTEMS[ss.upper()]
    except KeyError:
        raise ValueError("Unknown subsystem ID: %s" % ss)
        
    # Convert the command name to a MCS ID code
    try:
        cid = COMMANDS[cmd.upper()]
    except KeyError:
        raise ValueError("Unknown command: %s" % cmd)
        
    # Send the command
    try:    
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(ADDRESSES['MSE'])
        sock.settimeout(SOCKET_TIMEOUT)
        
        t = time.time()
        tv_sec, tv_usec = long(t), long((t % 1) * 1e6)
        mcscmd = COMMAND_STRUCT.pack(sid, 0, cid, 0, tv_sec, tv_usec, 0, 0, 0, data, -1)
        sock.sendall(mcscmd)
        response = sock.recv(COMMAND_STRUCT.size)
        response = COMMAND_STRUCT.unpack(response)
        
        sock.close()
    except Exception as e:
        raise RuntimeError("MCS/sch - ms_mdre_ip does not appear to be running")
        
    # Wait a bit...
    time.sleep(0.2)
    
    if cmd == "RPT":
        # Parse the response if this is a RPT command
        t0 = time.time()
        while (time.time() - t0) < 3.0:
            try:
                val, ts = mib.read(ss, data)
                if (time.time() - ts) <= 3.0:
                    value = val
                    break
                else:
                    value = 'TIMEOUT'
                    time.sleep(0.2)
                    
            except Exception as e:
                #print str(e)
                value = None
    else:
        # Otherwise return the reference ID
        value = response[1]
        
    return value
