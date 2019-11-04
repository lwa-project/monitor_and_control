"""
Module with various utilties to help with working with MCS.
"""

import math
from datetime import datetime

__version__ = "0.1"
__revision__ = "$Rev$"
__all__ = ['mjdmpm_to_datetime', 'datetime_to_mjdmpm']


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