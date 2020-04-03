"""
Module with various utilties to help with working with MCS.
"""

import math
import pytz
from datetime import datetime timedelta

from lwa_mcs._mcs import get_current_time

__version__ = "0.2"
__all__ = ['get_current_mjdmpm', 'mjdmpm_to_datetime', 'datetime_to_mjdmpm', 'schedule_at_command']


UTC = pytz.utc
MST = pytz.timezone('America/Denver')


def get_current_mjdmpm():
    return get_current_time()


def mjdmpm_to_datetime(mjd, mpm):
    """
    Convert a MJD, MPM pair to a UTC-aware datetime instance.
    
    From LSL.
    """
    
    unix = mjd*86400.0 + mpm/1000.0 - 3506716800.0
    return datetime.utcfromtimestamp(unix)


def datetime_to_mjdmpm(dt):
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


def schedule_at_command(execution_time, command, system_tz=MST):
    """
    Simple function to schedule a command to run at the provided time using
    the 'at' command.  The job ID is returned as an integer.  This function 
    takes care of all of the time zone conversion problems (as handled via 
    the 'system_tz' keyword) and also works with Python .py commands.

    Supported execution time formats are:
      * float or integer - UNIX timestamp
      * naive datetime instance - assumed to be in UTC
      * aware datetime instance

    Supported commands are:
      * anything that works with the '-f' option of 'at' without arguments
      * anything that is a Python script ending in .py
    """

    # Time conversion
    ## Does the the execution time look like a time stamp?
    if isinstance(execution_time), (float, int)):
        execution_time = datetime.utcfromtimestamp(execution_time)
    ## Has the execution time already had a time zone assigned to it?
    elif execution_time.tzinfo is None:
        execution_time = UTC.localize(execution_time)
    ## Convert to the systems's timezone	
    execution_time = execution_time.astimezone(system_tz)

    # Command processing
    # Read the command and figure out the working directory to use
    cwd, name = os.path.split(command)
    ## Is the command to be executed a Python script?
    isPython = False
    if command.find('.py') != -1:
        isPython = True
        cwd, name = os.path.split(command.split('.py', 1)[0])
        name = "%s.py" % name
        
    # Schedule
    ## Build up the command sequence
    if isPython:
        echoc = subprocess.Popen(['/bin/echo', 'python %s' % (command,)], stdout=subprocess.PIPE)
        echoc.wait()
        atc = subprocess.Popen(['/usr/bin/at',  "%s" % execution_time.strftime("%H:%M %m/%d/%Y")], 
                                cwd=cwd, stdin=echoc.stdout, stderr=subprocess.PIPE)
    else:
        atc = subprocess.Popen(['/usr/bin/at', "%s" % execution_time.strftime("%H:%M %m/%d/%Y"), "-f", name], 
                                cwd=cwd, stderr=subprocess.PIPE)
    ## Execute						
    atco, atce = atc.communicate()
    ## Interpret the results
    jobInfo = atce.split('\n')[-2]
    jobID = jobInfo.split(None, 2)[1]
    try:
        jobID = int(jobID)
    except ValueError:
        jobID = -1
        
    # Done
    return jobID
