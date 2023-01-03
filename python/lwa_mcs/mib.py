"""
Module for reading data from the MCS MIB.
"""

import os
import time
import struct
from datetime import datetime, timedelta

from lwa_mcs._mcs import read_mib_ip, read_mib, MCS_TIMEOUT

__version__ = "0.4"
__all__ = ['read', 'read_from_disk']


def read(ss, label, trim_nulls=True):
    """
    Read the specified MIB label for the specified subsystem using the 
    ms_mdre_ip interface.  Returns a two-element tuple of value, UTC 
    timestamp.
    """
    
    # Send the command
    value, ts = read_mib_ip(ss, label)
    try:
        value = value.decode('ascii')
    except AttributeError:
        pass
    if trim_nulls:
        value = value.replace('\0', '').strip().rstrip()
        
    try:
        value = int(value, 10)
    except ValueError:
        try:
            value = float(value)
        except ValueError:
            pass
            
    return value, ts


def read_from_disk(ss, label, trim_nulls=True):
    """
    Read the specified MIB label for the specified subsystem using the 
    GDBM interface.  Returns a two-element tuple of value, UTC 
    timestamp.
    """
    
    t0 = time.time()
    while (time.time() - t0) < MCS_TIMEOUT:
        try:
            dtype, value, ts = read_mib(ss, label)
            
            d = datetime.utcfromtimestamp(ts)
            if datetime.utcnow() - d <= timedelta(seconds=3):
                # unsigned and signed char
                if dtype[:3] == 'i1u':
                    value, = struct.unpack('B', val[:1])
                elif dtype[:3] == 'i1s':
                    value, = struct.unpack('b', val[:1])
                    
                # unsigned and signed short int
                elif dtype[:3] == 'i2u':
                    if dtype[:4] == 'i2ur':
                        value, = struct.unpack('>H', val[:2])
                    else:
                        value, = struct.unpack('<H', val[:2])
                elif dtype[:3] == 'i2s':
                    if dtype[:4] == 'i2sr':
                        value, = struct.unpack('>h', val[:2])
                    else:
                        value, = struct.unpack('@h', val[:2])
                        
                # unsigned and signed int
                elif dtype[:3] == 'i4u':
                    if dtype[:4] == 'i4ur':
                        value, = struct.unpack('>I', val[:4])
                    else:
                        value, = struct.unpack('@I', val[:4])
                elif dtype[:3] == 'i4s':
                    if dtype[:4] == 'i4sr':
                        value, = struct.unpack('>i', val[:4])
                    else:
                        value, = struct.unpack('@i', val[:4])
                        
                # unsigned and signed long int
                elif dtype[:3] == 'i8u':
                    if dtype[:4] == 'i8ur':
                        value, = struct.unpack('>Q', val[:8])
                    else:
                        value, = struct.unpack('@Q', val[:8])
                elif dtype[:3] == 'i8s':
                    if dtype[:4] == 'i8sr':
                        value, = struct.unpack('>q', val[:8])
                    else:
                        value, = struct.unpack('@q', val[:8])
                        
                # float
                elif dtype[:2] == 'f4':
                    if dtype[:3] == 'f4r':
                        value, = struct.unpack('>f', val[:4])
                    else:
                        value, = struct.unpack('@f', val[:4])
                        
                # double
                elif dtype[:2] == 'f8':
                    if dtype[:3] == 'f8r':
                        value, = struct.unpack('>d', val[:8])
                    else:
                        value, = struct.unpack('@d', val[:8])
                        
                # everything else is a string to me
                else:
                    try:
                        val = val.decode('ascii')
                    except AttributeError:
                        pass
                    if trim_nulls:
                        value = val.replace('\0', '').strip().rstrip()
                break
                
            else:
                value = 'TIMEOUT'
                ts = 0.0
                time.sleep(0.2)
            fh.close()
            
        except Exception as e:
            print(str(e))
            value = None
            ts = 0.0
            
    return value, ts
