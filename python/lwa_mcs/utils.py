"""
Module with various utilties to help with working with MCS.
"""

import os
import re
import sys
import math
import shlex
import tempfile
import subprocess
from typing import Dict, Tuple, Union
from datetime import datetime, timedelta, timezone

from lwa_mcs._mcs import get_current_time
from lwa_mcs.config import STATION_TZ

__version__ = "0.3"
__all__ = ['get_uptime', 'get_current_mjdmpm', 'mjdmpm_to_datetime', 'datetime_to_mjdmpm',
           'get_at_queue', 'get_at_command', 'schedule_at_command']


def get_uptime() -> int:
    """
    Determine and return the current uptime in minutes.
    """
    
    # Create a regular expresion to help us parse the uptime command
    upre = re.compile('up ((?P<days>\d+) day(s)?,)?\s*((?P<hours>\d+)\:)?(?P<minutes>\d+)( min(ute(s)?)?)?,')
    
    # Run the command and see if we have something that looks right
    output = subprocess.check_output(['uptime'], stderr=subprocess.DEVNULL)
    output = output.decode('ascii', errors='backslashreplace')
    mtch = upre.search(output)
    if mtch is None:
        raise RuntimeError("Could not determine the current uptime")
    
    # Convert the uptime to minutes
    uptime = 0
    try:
        uptime += int(mtch.group('days'), 10)*24*60
    except (TypeError, ValueError):
        pass
    try:
        uptime += int(mtch.group('hours'), 10)*60
    except (TypeError, ValueError):
        pass
    try:
        uptime += int(mtch.group('minutes'), 10)
    except (TypeError, ValueError):
        pass
        
    # Done
    return uptime


def get_current_mjdmpm() -> Tuple[int, int]:
    return get_current_time()


def mjdmpm_to_datetime(mjd, mpm):
    """
    Convert a MJD, MPM pair to a UTC-aware datetime instance.
    
    From LSL.
    """
    
    unix = mjd*86400.0 + mpm/1000.0 - 3506716800.0
    return datetime.fromtimestamp(unix, tz=timezone.utc)


def datetime_to_mjdmpm(dt: datetime) -> Tuple[int, int]:
    """
    Convert a UTC datetime instance to a MJD, MPM pair (returned as a 
    two-element tuple).
    
    Based off: http://paste.lisp.org/display/73536
    
    From LSL.
    """
    
    year        = dt.year             
    month       = dt.month      
    day         = dt.day    

    hour        = dt.hour
    minute      = dt.minute
    second      = dt.second     
    millisecond = dt.microsecond / 1000
    
    if dt.tzinfo is not None:
        dt = dt.astimezone(timezone.utc)
        
    # compute MJD         
    # adapted from http://paste.lisp.org/display/73536
    # can check result using http://www.csgnetwork.com/julianmodifdateconv.html
    a = (14 - month) // 12
    y = year + 4800 - a          
    m = month + (12 * a) - 3                    
    p = day + (((153 * m) + 2) // 5) + (365 * y)   
    q = (y // 4) - (y // 100) + (y // 400) - 32045
    mjdi = int(math.floor( (p+q) - 2400000.5))
    mjd = mjdi

    # compute MPM
    mpmi = int(math.floor( (hour*3600 + minute*60 + second)*1000 + millisecond ))
    mpm = mpmi
    return (mjd, mpm)


def get_at_queue() -> Dict[int, datetime]:
    """
    Read in the current 'at' queue and return a dictionary of id, time pairs.
    The times that returned are timezone-aware datetime instances.
    """
    
    queue = {}
    
    # Run atq to get the current list of commands
    output = subprocess.check_output(['/usr/bin/atq',], stderr=subprocess.DEVNULL)
    output = output.decode('ascii', errors='backslashreplace')
    output = output.split('\n')
    
    # Loop over the output
    for line in output:
        if len(line) < 3:
            continue
            
        ## Parse the line
        fields = line.split()
        id = int(fields[0], 10)
        dt = datetime.strptime(' '.join(fields[1:6]), '%a %b %d %H:%M:%S %Y')
        
        ## The 'at' queue uses the system time zone which is currently set 
        ## to 'US/Mountain'
        dt = dt.replace(tzinfo=STATION_TZ)
        dt = dt.astimezone(timezone.utc)
        
        ## Append
        queue[id] = dt
        
    return queue


def get_at_command(id: int) -> str:
    """
    For the specified 'at' command ID, figure out what is happening.
    """
    
    # Run at to information about the specified command
    output = subprocess.check_output(['/usr/bin/at', '-c', str(id)], stderr=subprocess.DEVNULL)
    output = output.decode('ascii', errors='backslashreplace')
    output = output.split('\n')
    
    toExecute = ''
    inCommand = 2048
    for line in output:
        inCommand -= 1
        if inCommand <= 0:
            toExecute += line
            toExecute += '\n'
            
        if line.find("echo 'Execution directory inaccessible' >&2") != -1:
            inCommand = 3
            
    return toExecute.rstrip()


def schedule_at_command(execution_time: Union[int, float, datetime], command: str) -> int:
    """
    Simple function to schedule a command to run at the provided time using
    the 'at' command.  The job ID is returned as an integer.  This function 
    takes care of all of the time zone conversion problems (as handled via 
    the 'system_tz' keyword) and also works with Python .py commands.

    Supported execution time formats are:
      * float or integer - UNIX timestamp
      * naive datetime instance that is assumed to be in UTC
      * timezone-aware datetime instance

    Supported commands are:
      * anything that works with the '-f' option of 'at' without arguments
      * anything that is a Python script ending in .py
    """

    # Time conversion
    ## Does the the execution time look like a time stamp?
    if isinstance(execution_time, (int, float)):
        execution_time = datetime.utcfromtimestamp(execution_time)
    ## Has the execution time already had a time zone assigned to it?
    if execution_time.tzinfo is None:
        execution_time = execution_time.replace(tzinfo=timezone.utc)
    ## Convert to the systems's timezone	
    execution_time = execution_time.astimezone(STATION_TZ)

    # Command processing
    # Read the command and figure out the working directory to use
    cwd, name = os.path.split(command)
    if cwd == '':
        cwd = '.'
    ## Is the command to be executed a Python script?
    isPython = False
    if command.find('.py') != -1:
        isPython = True
        cwd, name = os.path.split(command.split('.py', 1)[0])
        name = "%s.py" % name
    else:
        name = shlex.split(command)[0]
        name = os.path.basename(name)
        
    # Schedule
    ## Build up the command sequence
    wrapper_body = f"""#!/bin/bash
# AT_METADATA: {shlex.quote(command)}
# AT_TYPE: {'python' if isPython else 'shell'}
# AT_CWD: {shlex.quote(cwd)}
# AT_NAME: {shlex.quote(name)}
cd {cwd}
if [ "{isPython}" == "True" ]; then
    {sys.executable} {command}
else
    {command}
fi
"""
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.sh', delete=False) as wrapper:
        wrapper.write(wrapper_body)
        wrapper_path = wrapper.name
    print(wrapper_path)
    with open(wrapper_path, 'r') as fh:
        print(fh.read())
    
    jobID = -1
    try:
        atc = subprocess.Popen(['/usr/bin/at', "%s" % execution_time.strftime("%H:%M %m/%d/%Y"), "-f", wrapper_path], 
                                cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        ## Execute
        atco, atce = atc.communicate()
        try:
            atco = atco.decode('ascii', errors='backslashreplace')
            atce = atce.decode('ascii', errors='backslashreplace')
        except AttributeError:
            pass  
        ## Interpret the results
        jobInfo = atce.split('\n')[-2]
        jobID = jobInfo.split(None, 2)[1]
        try:
            jobID = int(jobID)
        except ValueError:
            jobID = -1
    finally:
        os.unlink(wrapper_path)
        
    # Done
    return jobID
