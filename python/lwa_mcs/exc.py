# -*- coding: utf-8 -*-

"""
Module for controlling MCS executive via UDP packets.
"""

import os
import time
import subprocess
from datetime import datetime, timedelta

from lwa_mcs.utils import mjdmpm_to_datetime
from lwa_mcs.sch import send_subsystem_command
from lwa_mcs._mcs import send_exec_command

__version__ = "0.3"
__all__ = ['get_pids', 'is_is_active', 'send_command', 'get_queue', 'cancel_observation']


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


def is_is_active():
    """
    Determine if MCS/exec should be considered operational.
    """
    
    pids = get_pids()
    return True if len(pids) == 3 else False


def send_command(cmd, data=""):
    """
    Use MCS/exec to execute the specified command.
    """
    
    # Send the command
    success = send_exec_command(cmd, data)
    
    # Wait a bit...
    time.sleep(0.2)
    
    return success


def get_queue():
    """
    Read in the current observation queue and return its contents as a 
    dictionary.  The dictionary is keyed using a two-element tuple of
    (project ID, session ID) and the values are three-element tuples of
    (beam, start datetime, stop datetime).
    """
    
    queue = {}
    
    # Open the 'mesq.dat' file for parsing
    try:
        fh = open(os.path.join(TP_PATH, 'mbox', 'mesq.dat'), 'r')
        for line in fh:
            if line.find('S PROJ     SESS B    MJD       MPM       DUR NOBS C') != -1:
                continue
                
            ## Parse the line
            fields = line.split(None, 10)
            if len(fields) != 11:
                continue
            pID = fields[2]
            sID = int(fields[3], 10)
            beam = int(fields[4], 10)
            mjd, mpm = int(fields[5], 10), int(fields[6], 10)
            dur = int(fields[7], 10)
            nobs = int(fields[8], 10)
            if nobs == 0:
                continue
                
            ## Manipulate the time
            dStart = mjdmpm_to_datetime(mjd, mpm)
            dStop  = dStart + timedelta(seconds=int(dur/1000), microseconds=int((dur*1e3) % 1000000))
            
            ## Pad to deal with session starts and stops
            dStart -= timedelta(seconds=6)
            dStop  += timedelta(seconds=6)
            
            ## Append
            queue[(pID,sID)] = (beam,dStart,dStop)
            
    except IOError as e:
        raise RuntimeError("Cannot parse the 'mesq.dat' file")
        
    return queue


def cancel_observation(project_id, session_id, stop_dr=True):
    """
    Cancel a scheduled observation.
    """
    
    # Get the exec queue and make sure we can even do this
    queue = get_queue()
    if (project_id, session_id) not in list(queue.keys()):
        raise RuntimeError("Project %s, sesison %i is scheduled" % (project_id, session_id))
        
    # It's at least scheduled.  Is it is_active?
    now = datetime.utcnow()
    beam, start, stop = queue[(project_id, session_id)]
    if now >= start and now < stop:
        is_active = True
    else:
        is_active = False
        
    # Cancel it
    cancelled = send_command("STP", "%s %i" % (project_id, session_id))
    if not cancelled:
        raise RuntimError("Cannot cancel observation")
    else:
        if is_active and stop_dr:
            ## Also stop the data recorder
            tag = send_subsystem_command("DR%i" % beam, "RPT" "OP-TAG")
            stopped = send_subsystem_command("DR%i" % beam, "STP", tag)
            
    return True
