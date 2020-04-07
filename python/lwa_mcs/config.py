# -*- coding: utf-8 -*-
  
"""
Module for defining common MCS values.
"""

import os
import pytz

from lwa_mcs._mcs import IS_ADP, MAX_NDR, SCH_PATH

__version__ = '0.4'
__all__ = ['IS_ADP', 'MAX_NDR', 'SCH_PATH', 'EXC_PATH', 'TP_PATH', 'STATION_TZ']


# Paths
_BASE_PATH = os.path.dirname(SCH_PATH)
EXC_PATH = os.path.join(_BASE_PATH, 'exec')
TP_PATH = os.path.join(_BASE_PATH, 'tp')


# Station time zone for 'at' commands
## Default for New Mexico
tzname = 'America/Denver'
if os.path.exists('/etc/timezone'):
    ## Debian/Ubuntu systems
    with open('/etc/timezone', 'r') as fh:
        tzname = fh.read().rstrip()
elif os.path.exists('/etc/sysconfig/clock'):
    ## Redhat systems
    with open('/etc/sysconfig/clock', 'r') as fh:
        for line in fh:
            if line.find('ZONE') != -1:
                _, tzname = line.rstrip().split('=', 1)
                tzname = tzname.replace('"', '').replace("'", '')
                break
STATION_TZ = pytz.timezone(tzname)

