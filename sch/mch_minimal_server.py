#!/usr/bin/env python3

# mch_minimal_server.py - S. Ellingson, VT - 2010 Jun 21
# usage:
#   $ python mch_minimal_server.py <subsystem> <ip_address> <tx_port> <rx_port>
#     <subsystem> = three letter subsystem designator; e.g., "NU1", "SHL"
#     <ip_address> = IP address of the *client* as a dotted-quad; e.g., "127.0.0.1" 
#     <tx_port> = port address for transmit; e.g., 1739
#     <rx_port> = port address for receive; e.g., 1738
# This is the "server" side; the other end is MCS ("client") 
# Minimum implementation of the MCS Common ICD for controlled subsystems:
# -- Minimum MIB: MCS-RESERVED section only
# -- PNG supported
# -- RPT supported, but is limited to 1 index at a time (no branches)
# -- SHT will simply change STATUS to SHTDOWN.  But, it will return correctly.
# This code runs forever -- use CTRL-C or "kill <pid>" to crash out when done.
# The MIB is implemented completely in memory.  See code for initial values.
# Some notes:
# -- Intended to be compliant with MCS Common ICD v.1.0, except as noted above
# -- This is not production code.  It is not really even alpha code.  Don't worry.  Be happy.
# -- Example command line:
# -- $ python mch_minimal_server.py NU1 127.0.0.1 1739 1738

import socket
import time
import datetime
import math
import string
import struct   # for packing of binary to/from strings
import sys
import argparse
import threading

# Below are things that shouldn't be changed
B = 8192                    # [bytes] Max message size


# ------------------------------
# Reading command line arguments
# ------------------------------

# Check for required command line argument <CMD> 
parser = argparse.ArgumentParser(
                    description='Minimum implementation of the MCS Common ICD for controlled subsystems')
parser.add_argument('subsystem', type=str,
                    help='three letter subsystem designator; e.g., "NU1", "SHL"')
parser.add_argument('ip_address', type=str,
                    help='IP address of the *client* as a dotted-quad; e.g., "127.0.0.1"')
parser.add_argument('tx_port', type=int,
                    help='port address for transmit; e.g., 1739')
parser.add_argument('rx_port', type=int,
                    help='port address for receive; e.g., 1738')
parser.add_argument('-a', '--accept-all', action='store_true',
                    help='respond to all incoming commands whether they are valid or not')
parser.add_argument('-v', '--verbose', action='store_true',
                    help='be verbose about what is happening')
args = parser.parse_args()

# Global flag for shutdown
shutdown_flag = False

# --------------------------
# Set up the MIB
# --------------------------

if args.verbose:
    print('Setting up the MIB...')

ml = [] # this becomes a list of MIB labels
me = [] # this becomes a list of MIB entries (data)
ml.append('SUMMARY');   me.append('NORMAL')
ml.append('INFO');      me.append('This is mock INFO from '+args.subsystem)
ml.append('LASTLOG');   me.append('This is mock LASTLOG from '+args.subsystem)
ml.append('SUBSYSTEM'); me.append(args.subsystem)
ml.append('SERIALNO');  me.append(args.subsystem+'-1')
ml.append('VERSION');   me.append('mch_minimal_server.py_'+args.subsystem)

if args.verbose:
    print(ml[0]+' '+me[0])
    print(ml[1]+' '+me[1])
    print(ml[2]+' '+me[2])
    print(ml[3]+' '+me[3])
    print(ml[4]+' '+me[4])
    print(ml[5]+' '+me[5])
    print('I am '+me[3]+'.')

def get_mjd_mpm():
    """Calculate current MJD and MPM"""
    dt = datetime.datetime.utcnow()
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
    mjd = str(mjdi).rjust(6)

    # compute MPM
    mpmi = int(math.floor( (hour*3600 + minute*60 + second)*1000 + millisecond ))
    mpm = str(mpmi).rjust(9)
    
    return mjd, mpm

def send_unsolicited_update(t):
    """Send periodic unsolicited RPT updates for SUMMARY"""
    global shutdown_flag
    sequence = 0
    states = ['NORMAL', 'WARNING', 'ERROR', 'NORMAL', 'BOOTING']  # Cycle through these states
    
    while not shutdown_flag:
        try:
            # Update SUMMARY in MIB
            me[0] = states[sequence % len(states)]
            sequence += 1
            
            # Build unsolicited RPT message
            mjd, mpm = get_mjd_mpm()
            
            # Format matches MCS Common ICD:
            # DESTINATION(3) SOURCE(3) TYPE(3) REFERENCE(9) DATALEN(4) MJD(6) MPM(9) ' ' R-RESPONSE(1) R-SUMMARY(7) R-COMMENT
            response = f'A{me[0].rjust(7)}{me[0]}'
            payload = f'MCS{me[3]}RPT999999999{str(len(response)).rjust(4)}{mjd}{mpm} {response}'
            
            if args.verbose:
                print(f'Sending unsolicited update: {payload}')
            
            t.send(payload.encode())
            
        except Exception as e:
            if args.verbose:
                print(f"Error in update thread: {e}")
        
        time.sleep(5)  # Send update every 5 seconds

# --------------------------
# Set up comms
# --------------------------

# Set up the receive socket for UDP
r = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
r.bind(('',args.rx_port)) # Accept connections from anywhere
r.setblocking(1)   # Blocking on this sock

# Set up the transmit socket for UDP
t = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t.connect((args.ip_address,args.tx_port)) 

if args.verbose:
    print('Starting update thread...')

# Start update thread
update_thread = threading.Thread(target=send_unsolicited_update, args=(t,))
update_thread.daemon = True  # Thread will exit when main thread exits
update_thread.start()

if args.verbose:
    print('Running...')

while not shutdown_flag:
    try:
        payload = r.recv(B)  # wait for something to appear

        if args.verbose:
            # Say what was received
            print(b'rcvd> '+payload+b'|')

        # --------------------------
        # Analyzing received command
        # --------------------------

        # The default is that a response is not necessary:
        bRespond = False  

        # Now the possibilities are:
        # (1) The message is not for us; then no response should be made
        # (2) The message is for us but is not a PNG, RPT, or SHT message. In this case, send "reject" response
        # (3) The message is for us and is a PNG, RPT, or SHT message. In this case, respond appropriately  
                    
        destination = payload[:3].decode()
        sender      = payload[3:6].decode()
        command     = payload[6:9].decode()
        reference   = int(payload[9:18])
        datalen     = int(payload[18:22]) 
        mjd         = int(payload[22:28]) 
        mpm         = int(payload[28:37]) 
        data        = payload[38:38+datalen]
        
        if args.verbose:
            print('DESTINATION: |'+destination+'|')
            print('SENDER:      |'+sender+'|')
            print('TYPE:        |'+command+'|')
            print('REFERENCE: ', reference)
            print('DATALEN:   ', datalen)
            print('MJD:       ', mjd)
            print('MPM:       ', mpm)
            print(b'DATA: |'+data+b'|')

        if (destination==me[3]) or (destination=='ALL'): # comparing to MIB entry 1.4, "SUBSYSTEM"              

            bRespond = True
            if args.accept_all:
                response = 'A'+me[0].rjust(7)+'Command not recognized' # use this until we find otherwise
            else:
                response = 'R'+me[0].rjust(7)+'Command not recognized' # use this until we find otherwise

            if command=='PNG':
                response = 'A'+me[0].rjust(7)

            if command=='RPT':
                response = 'R'+me[0].rjust(7)+'Invalid MIB label' # use this until we find otherwise
                mib_label = data.decode().strip()
                #print('|'+mib_label+'|')
                #print(b'|'+data+b'|')
                # find in mib  
                response = 'R'+me[0].rjust(7)+'MIB label not recognized'  
                for i in range(len(ml)):
                    if ml[i]==mib_label:
                        response = 'A'+me[0].rjust(7)+me[i]

            if command=='SHT':
                response = 'A'+me[0].rjust(7)  # use this until we find otherwise
                arg = data.decode().strip()
                me[0] = "SHTDOWN"
                shutdown_flag = True  # Signal threads to exit
                # verify arguments
                while len(arg)>0:
                   args = arg.split(' ',1)
                   args[0] = args[0].strip()
                   #print('>'+args[0]+'|')
                   if (not(args[0]=='SCRAM') and not(args[0]=='RESTART')):
                       response = 'R'+me[0].rjust(7)+' Invalid extra arguments' 
                   if len(args)>1:
                       arg = args[1]
                   else:
                       arg = '' 
                    
        # -------------------
        # Message Preparation
        # -------------------

        payload = '(nothing)' # default payload
        if bRespond:
            # Get current MJD/MPM
            mjd, mpm = get_mjd_mpm()

            # Build the payload
            # Note we are just using a single, non-updating REFERENCE number in this case
            payload = 'MCS'+me[3]+command+str(reference).rjust(9)
            payload = payload + str(len(response)).rjust(4)+mjd+mpm+' '
            payload = payload + response

            t.send(payload.encode())      # send it 

        if args.verbose:
            print('sent> '+payload+'|') # say what was sent

    except (KeyboardInterrupt, SystemExit):
        shutdown_flag = True
        break

# Cleanup
update_thread.join(timeout=1)  # Wait for update thread to finish
r.close()
t.close()

#==================================================================================
#=== HISTORY ======================================================================
#==================================================================================
# mch_minimal_server.py: J. Dowell, UNM, 2023 Feb 22
#   .1: Convert to Python3
# mch_minimal_server.py: S.W. Ellingson, Virginia Tech, 2010 Jun 21
#   .1: Fixing bug in which extra space was inserted between R-SUMMARY and R-COMMENT
# mch_minimal_server.py: S.W. Ellingson, Virginia Tech, 2009 Jul 22
#   .1: Adding command line args <subsystem> <ip_address> <tx_port> <rx_port>
#       Also, makes it's own MIB file upon start-up (svn rev 7)
# mch_minimal_server.py: S.W. Ellingson, Virginia Tech, 2009 Jul 20
#   .1: Porting into project
# mch_mins_3.py - S. Ellingson, VT - 2009 Jun 30
