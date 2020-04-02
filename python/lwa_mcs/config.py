# -*- coding: utf-8 -*-
  
"""
Module for defining common MCS values.
"""

import os

from _mcs import IS_ADP, MAX_NDR, SOCKET_TIMEOUT, SCH_PATH, MSE_ADDRESS, MSE_PORT, MSE2_ADDRESS, MSE2_PORT, MEE_ADDRESS, MEE_PORT

__version__ = '0.2'
__all__ = ['IS_ADP', 'MAX_NDR',
           'SOCKET_TIMEOUT', 'SCH_PATH', 'EXC_PATH', 'TP_PATH',
           'ADDRESSES']

_BASE_PATH = os.path.dirname(SCH_PATH)
EXC_PATH = os.path.join(_BASE_PATH, 'exec')
TP_PATH = os.path.join_BASE_PATH, 'tp')

ADDRESSES = {
             'MSE':  (MSE_ADDRESS, MSE_PORT),
             'MSE2': (MSE2_ADDRESS, MSE2_PORT),
             'MEE':  (MEE_ADDRESS, MEE_PORT),
            }
