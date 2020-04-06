# -*- coding: utf-8 -*-
  
"""
Module for defining common MCS values.
"""

import os
import pytz

from _mcs import IS_ADP, MAX_NDR, SOCKET_TIMEOUT, SCH_PATH, MSE_ADDRESS, MSE_PORT, MSE2_ADDRESS, MSE2_PORT, MEE_ADDRESS, MEE_PORT

__version__ = '0.3'
__all__ = ['IS_ADP', 'MAX_NDR',
           'SOCKET_TIMEOUT', 'SCH_PATH', 'EXC_PATH', 'TP_PATH',
           'ADDRESSES', 'STATION_TZ']


# Paths
_BASE_PATH = os.path.dirname(SCH_PATH)
EXC_PATH = os.path.join(_BASE_PATH, 'exec')
TP_PATH = os.path.join_BASE_PATH, 'tp')


# IP address and ports
ADDRESSES = {
             'MSE':  (MSE_ADDRESS, MSE_PORT),
             'MSE2': (MSE2_ADDRESS, MSE2_PORT),
             'MEE':  (MEE_ADDRESS, MEE_PORT),
            }


# Station time zone for 'at' commands
try:
    ## Can be overridden with the value in ~/.lwa_mcs/timezone
    with open(os.path.join(os.path.expanduser('~'), '.lwa_mcs', 'timezone'), 'r') as fh:
        STATION_TZ = pytz.timezone(fh.read())
except (OSError, IOError):
    STATION_TZ = pytz.timezone('America/Denver')

