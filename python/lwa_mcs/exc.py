# -*- coding: utf-8 -*-

"""
Module for controlling MCS executive via UDP packets.
"""

import time
import socket
import struct
import subprocess

from lwa_mcs.config import ADDRESSES, SOCKET_TIMEOUT, EXC_COMMANDS

__version__ = "0.1"
__revision__ = "$Rev$"
__all__ = ['COMMAND_STRUCT', 'get_pids', 'is_running', 'send_command']


COMMAND_STRUCT = struct.Struct('i256s')


def get_pids():
    """
    Return a list process IDs for all MCS/exec processes found.
    """
    
    p = subprocess.Popen(['ps', 'aux'], stdout=subprocess.PIPE)
    o, e = p.communicate()
    
    pids = []
    for line in o.split('\n'):
        fields = line.split(None, 10)
        if fields[-1].find('me_inproc') != -1 \
           or fields[-1].find('me_tpcom') != -1 \
           or fields[-1].find('ms_exec') != -1:
            pids.append(int(fields[1], 10))
            
    return pids


def is_running():
    """
    Determine if MCS/sch should be considered operational.
    """
    
    pids = get_pids()
    return True if len(pids) == 3 else False


def send_command(cmd, data=""):
    """
    Use MCS/exec to execute the specified command.
    """
    
    # Convert the command name to a MCS ID code
    try:
        cid = EXC_COMMANDS[cmd.upper()]
    except KeyError:
        raise ValueError("Unknown command: %s" % cmd)
        
    # Send the command
    try:    
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(ADDRESSES['MEE'])
        sock.settimeout(SOCKET_TIMEOUT)
        
        mcscmd = COMMAND_STRUCT.pack(cid, data)
        sock.sendall(mcscmd)
        response = sock.recv(COMMAND_STRUCT.size)
        response = COMMAND_STRUCT.unpack(response)
        
        sock.close()
    except Exception as e:
        print str(e)
        raise RuntimeError("MCS/exec - me_exec does not appear to be running")
        
    # Wait a bit...
    time.sleep(0.2)
    
    return False if response[0] < EXC_COMMANDS['NUL'] else True
