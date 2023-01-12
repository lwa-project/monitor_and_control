"""
Module for intefacing with MCS task processor.
"""

import os
import glob
import time
import warnings
import subprocess

try:
    from io import IOBase
except ImportError:
    IOBase = file

from lwa_mcs.config import TP_PATH
from lwa_mcs.exc import get_queue as get_exec_queue

__version__ = "0.2"
__all__ = ['schedule_sdfs', 'get_completed_metadata']


def _get_sdf_id(filename):
    pid, sid = None, None
    with open(filename, 'r') as fh:
        for line in fh:
            line = line.strip().rstrip()
            if len(line) < 3:
                continue
            if line[0] == '#':
                continue
                
            fields = line.split(None, 1)
            if fields[0] == 'PROJECT_ID':
                pid = fields[1].split('#')[0].strip().rstrip()
            elif fields[0] == 'SESSION_ID':
                sid = fields[1].split('#')[0].strip().rstrip()
                sid = int(sid, 10)
                
    return pid, sid


def schedule_sdfs(filenames, max_retries=5, fast_submit=False, logfile=None, errorfile=None):
    # Figure out the input
    if not isinstance(filenames, list):
        filenames = [filenames,]
        
    # Fast submit or not
    t_sub_wait= 10
    if fast_submit:
        t_sub_wait = 5
        
    # Deal with the logging
    ## stdout
    log_is_string = False
    if isinstance(logfile, str):
        try:
            fh = open(logfile, 'a')
            logfile = fh
            log_is_string = True
        except IOError as e:
            warnings.warn("Could not open logfile '%s' for appending: %s" % (logfile, str(e)), 
                          RuntimeWarning)
            logfile = None
    elif isinstance(logfile, IOBase):
        pass
    elif logfile is None:
        pass
    else:
        warnings.warn("logfile is of unknown type '%s'" % str(type(logfile)), 
                      RuntimeWarning)
        logfile = None
    ## stderr
    err_is_string = False
    if isinstance(errorfile, str):
        try:
            fh = open(errorfile, 'a')
            errorfile = fh
            err_is_string = True
        except IOError as e:
            warnings.warn("Could not open errorfile '%s' for appending: %s" % (logfile, str(e)), 
                          RuntimeWarning)
            errorfile = None
    elif isinstance(errorfile, IOBase):
        pass
    elif errorfile is None:
        pass
    else:
        warnings.warn("errorfile is of unknown type '%s'" % str(type(logfile)), 
                      RuntimeWarning)
        errorfile = None
        
    # Submit the files
    ids = {}
    for filename in filenames:
        filename = os.path.abspath(filename)
        psID = _get_sdf_id(filename)
        if logfile is not None:
            logfile.write("Submitting SDF for %s, session %i\n" % psID)
        ids[psID] = filename
        tpss = subprocess.Popen(['./tpss', filename, '5', '0', 'mbox'], 
                                cwd=TP_PATH, stdout=logfile, stderr=errorfile)
        tpss.wait()
        time.sleep(0.5)
        
    # Verify that the SDFs made it into the queue
    scheduled = False
    counter = 0
    while not scheduled:
        time.sleep(max([t_sub_wait, t_sub_wait+1*(len(filenames)-2)]))
        ## Get the exec queue
        queue = get_exec_queue()
        
        ## Find out what is missing
        missing_files = []
        for id,filename in ids.items():
            if id not in list(queue.keys()):
                missing_files.append((id,filename))
                
        if not missing_files:
            ### Nothing.  Good, we are done
            scheduled = True
        else:
            ### Resubmit what is missing
            for psID,filename in missing_files:
                if errorfile is not None:
                    errorfile.write("Resubmitting SDF for %s, session %i\n" % psID)
                tpss = subprocess.Popen(['./tpss', filename, '5', '0', 'mbox'], 
                                        cwd=TP_PATH, stdout=logfile, stderr=errorfile)
                tpss.wait()
                time.sleep(0.5)
                
            ### Update the retry counter, giving up as needed
            counter += 1
            if counter > max_retries:
                if log_is_string:
                    logfile.close()
                if err_is_string:
                    errorfile.close()
                raise RuntimeError("Cannot schedule all SDFs after %i attempts" % max_retries)
                
    if log_is_string:
        logfile.close()
    if err_is_string:
        errorfile.close()
        
    return True


def get_completed_metadata():
    """
    Return a list of metadata tarballs for completed observations.
    """
    
    tarballs = glob.glob(os.path.join(TP_PATH, 'mbox', '*.tgz'))
    # Make sure the files are actually ready by being more than 10 seconds old
    tarballs = list(filter(lambda x: (time.time() - os.path.getmtime(x)) > 10, tarballs))
    tarballs.sort()
    return tarballs
