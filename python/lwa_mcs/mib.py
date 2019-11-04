# -*- coding: utf-8 -*-

"""
Module for controlling MCS scheduler and executive via UDP packets.
"""

import os
import time
import socket
import struct
try:
    import anydbm as dbm
except ImportError:
    import dbm
from datetime import datetime, timedelta

from lwa_mcs.config import ADDRESSES, SOCKET_TIMEOUT, SCH_PATH

__version__ = "0.1"
__revision__ = "$Rev$"
__all__ = ['MIB_STRUCT', 'read', 'read_from_disk']


MIB_STRUCT = struct.Struct('4s32s8192sll')
#  1 ss: subsystem name
#  2 label: MIB label to interogate
#  3 val: Value for the MIB label, padded with NULLs
#  4 tv.tv_sec: epoch seconds
#  5 tv.tv_usec: fractional remainder in microseconds
    

def read(ss, label, trim_nulls=True):
    """
    Read the specified MIB label for the specified subsystem using the 
    ms_mdre_ip interface.  Returns a two-element tuple of value, UTC 
    timestamp.
    """
    
    
    # Send the command
    try:    
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(ADDRESSES['MSE2'])
        sock.settimeout(SOCKET_TIMEOUT)
        
        mcscmd = MIB_STRUCT.pack(ss.upper(), label.upper(), '', 0, 0)
        sock.sendall(mcscmd)
        response = sock.recv(MIB_STRUCT.size)
        
        sock.close()
    except Exception as e:
        raise RuntimeError("MCS/sch - ms_mdre_ip does not appear to be running")
        
    response = MIB_STRUCT.unpack(response)
    value = response[2]
    if trim_nulls:
        value = value.replace('\x00', '').strip().rstrip()
    ts = response[3] + response[4]/1e6
    
    return value, ts


def read_from_disk(ss, label, trim_nulls=True):
    """
    Read the specified MIB label for the specified subsystem using the 
    GDBM interface.  Returns a two-element tuple of value, UTC 
    timestamp.
    """
    
    t0 = time.time()
    while (time.time() - t0) < 3:
        try:
            fh = dbm.open(os.path.join(SCH_PATH, "%s.gdb" % ss.upper()), 'ru')
            mibEntry = label.upper()
            
            eType, index, val, typeDBM, typeICD, tv_sec, tv_usec = MIB_STRUCT.unpack(fh[mibEntry])
            ts = tv_sec + tv_usec/1e6
            d = datetime.utcfromtimestamp(ts)
            if datetime.utcnow() - d <= timedelta(seconds=3):
                # unsigned and signed char
                if typeDBM[:3] == 'i1u':
                    value, = struct.unpack('B', val[:1])
                elif typeDBM[:3] == 'i1s':
                    value, = struct.unpack('b', val[:1])
                    
                # unsigned and signed short int
                elif typeDBM[:3] == 'i2u':
                    if typeDBM[:4] == 'i2ur':
                        value, = struct.unpack('>H', val[:2])
                    else:
                        value, = struct.unpack('<H', val[:2])
                elif typeDBM[:3] == 'i2s':
                    if typeDBM[:4] == 'i2sr':
                        value, = struct.unpack('>h', val[:2])
                    else:
                        value, = struct.unpack('@h', val[:2])
                        
                # unsigned and signed int
                elif typeDBM[:3] == 'i4u':
                    if typeDBM[:4] == 'i4ur':
                        value, = struct.unpack('>I', val[:4])
                    else:
                        value, = struct.unpack('@I', val[:4])
                elif typeDBM[:3] == 'i4s':
                    if typeDBM[:4] == 'i4sr':
                        value, = struct.unpack('>i', val[:4])
                    else:
                        value, = struct.unpack('@i', val[:4])
                        
                # unsigned and signed long int
                elif typeDBM[:3] == 'i8u':
                    if typeDBM[:4] == 'i8ur':
                        value, = struct.unpack('>Q', val[:8])
                    else:
                        value, = struct.unpack('@Q', val[:8])
                elif typeDBM[:3] == 'i8s':
                    if typeDBM[:4] == 'i8sr':
                        value, = struct.unpack('>q', val[:8])
                    else:
                        value, = struct.unpack('@q', val[:8])
                        
                # float
                elif typeDBM[:2] == 'f4':
                    if typeDBM[:3] == 'f4r':
                        value, = struct.unpack('>f', val[:4])
                    else:
                        value, = struct.unpack('@f', val[:4])
                        
                # double
                elif typeDBM[:2] == 'f8':
                    if typeDBM[:3] == 'f8r':
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
