# -*- coding: utf-8 -*-

"""
Module for intefacing with MCS task processor.
"""

import time
import subprocess

from lwa_mcs.config import TP_PATH
from lwa_mcs.exc import parse_queue

__version__ = "0.1"
__all__ = ['schedule_sdfs']


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


def schedule_sdfs(filenames, max_retries=10):
    # Figure out the input
    if isinstance(fileanmes, str):
        filenames = [filenames,]
        
    # Submit the files
    ids = {}
    for filename in filenames:
        ids[_get_sdf_id(filename)] = filename
        tpss = subprocess.Popen(['./tpss', filename, '5', '0', 'mbox'], 
                                cwd=TP_PATH, stdin=None, stdout=None)
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
                                        cwd=TP_PATH, stdin=None, stdout=None)
                tpss.wait()
                time.sleep(0.5)
                
            ### Update the retry counter, giving up as needed
            counter += 1
            if counter > max_retries:
                raise RuntimeError("Cannot schedule all SDFs after %i attempts" % max_retries)
                
    return True
