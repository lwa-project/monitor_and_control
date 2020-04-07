# -*- coding: utf-8 -*-

"""
Module for intefacing with MCS task processor.
"""

import os
import glob
import time
import warnings
import subprocess

from lwa_mcs.config import TP_PATH
from lwa_mcs.exc import parse_queue

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


def schedule_sdfs(filenames, max_retries=5, logfile=None, errorfile=None):
    # Figure out the input
    if not isinstance(filenames, list):
        filenames = [filenames,]
        
    # Deal with the logging
    ## stdout
    log_is_string = False
    if type(logfile) == str:
        try:
            fh = open(logfile, 'a')
            logfile = fh
            log_is_string = True
        except IOError as e:
            warnings.warn("Could not open logfile '%s' for appending: %s" % (logfile, str(e)), 
                          warnings.RuntimeWarning)
            logfile = None
    elif type(logfile) == file:
        pass
    elif logfile is None:
        pass
    else:
        warnings.warn("logfile is of unknown type '%s'" % str(type(logfile))), 
                      warings.RuntimeWarning)
        logfile = None
    ## stderr
    err_is_string = False
    if type(errorfile) == str:
        try:
            fh = open(errorfile, 'a')
            errorfile = fh
            err_is_string = True
        except IOError as e:
            warnings.warn("Could not open errorfile '%s' for appending: %s" % (logfile, str(e)), 
                          warnings.RuntimeWarning)
            errorfile = None
    elif type(errorfile) == file:
        pass
    elif errorfile is None:
        pass
    else:
        warnings.warn("errorfile is of unknown type '%s'" % str(type(logfile))), 
                      warings.RuntimeWarning)
        errorfile = None
        
    # Submit the files
    ids = {}
    for filename in filenames:
        ids[_get_sdf_id(filename)] = filename
        tpss = subprocess.Popen(['./tpss', filename, '5', '0', 'mbox'], 
                                cwd=TP_PATH, stdin=logfile, stdout=errorfile)
        tpss.wait()
        time.sleep(0.5)
        
    # Verify that the SDFs made it into the queue
    scheduled = False
    counter = 0
    while not scheduled:
        time.sleep(30)
        ## Get the exec queue
        queue = get_queue()
        
        ## Find out what is missing
        missing_files = []
        for id in ids.keys():
            if id not in list(queue.keys()):
                missing_files.append(filename)
                
        if not missing_files:
            ### Nothing.  Good, we are done
            scheduled = True
        else:
            ### Resubmit what is missing
            for filename in missing_files:
                tpss = subprocess.Popen(['./tpss', filename, '5', '0', 'mbox'], 
                                        cwd=TP_PATH, stdin=logfile, stdout=errorfile)
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
