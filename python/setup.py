# -*- coding: utf-8 -*-

# Python3 compatibility
from __future__ import print_function, division, absolute_import

import os
import glob

from setuptools import setup, Extension, Distribution, find_packages


def _parse_header(filename):
    defines = {}
    
    with open(filename, 'r') as fh:
        inside = False
        valid = False
        for line in fh:
            line = line.strip()
            if line.startswith('#define'):
                try:
                    _, name, value = line.split(None, 2)
                except ValueError:
                    _, name = line.split(None, 1)
                    value = 'True'
                value = value.split('//', 1)[0]
                value = value.split('/*', 1)[0]
                if (not inside) or (inside and valid):
                    defines[name] = value
            elif line.startswith('#ifdef'):
                _, name = line.split(None, 1)
                inside = True
                if name in defines:
                    valid = True
            elif line.startswith('#ifndef'):
                _, name = line.split(None, 1)
                inside = True
                if name not in defines:
                    valid = True
            elif line.startswith('#endif'):
                inside = False
                valid = False
    return defines
                

def _convert(value):
    value = value.strip().rstrip()
    value = value.replace('"', '')
    try:
        value = int(value, 10)
    except ValueError:
        try:
            value = float(value)
        except ValueError:
            pass
    return value


def write_config_info():
    """
    Write the MCS configuration info to the lwa_mcs.config module.
    """
    
    # Parse
    defines_lwa  = _parse_header('../common/mcs.h')
    defines_exec = _parse_header('../exec/me.h')
    
    # Group
    ## mcs.h
    commands = {}
    subsystems = {}
    addresses = {}
    for name in defines_lwa:
        value = _convert(defines_lwa[name])
        if name.startswith('LWA_CMD_'):
            name = name.split('_', 2)[2]
            if name.find('_') != -1 or name == 'MCSSHT':
                continue
            commands[name] = value
        elif name.startswith('LWA_SID_'):
            name = name.split('_', 2)[2]
            if name.startswith('NU'):
                continue
            subsystems[name] = value
        elif name.startswith('LWA_IP_'):
            name = name.split('_', 2)[2]
            try:
                addresses[name][0] = value
            except KeyError:
                addresses[name] = [value, 0]
        elif name.startswith('LWA_PORT_'):
            name = name.split('_', 2)[2]
            try:
                addresses[name][1] = value
            except KeyError:
                addresses[name] = ['', value]
    addresses['MSE2'][0] = addresses['MSE'][0]
    
    ## me.h
    exec_commands = {}
    for name in defines_exec:
        value = _convert(defines_exec[name])
        if name.startswith('ME_CMD_'):
            name = name.split('_', 2)[2]
            if name.find('_') != -1:
                continue
            exec_commands[name] = value
            
    # Write        
    with open('lwa_mcs/config.py', 'w') as fh:
        fh.write("__all__ = ['SOCKET_TIMEOUT', 'SCH_PATH', 'EXC_PATH', 'TP_PATH',\n")
        fh.write("           'ADDRESSES',\n")
        fh.write("           'SUBSYSTEMS', 'SUBSYSTEM_IDS',\n")
        fh.write("           'COMMANDS', 'COMMAND_IDS',\n")
        fh.write("           'EXC_COMMANDS', 'EXC_COMMAND_IDS']\n")
        
        fh.write("\n\n")
        
        fh.write("SOCKET_TIMEOUT = 5.0\n")
        
        fh.write("\n\n")
        
        prefix_path = os.path.dirname(_convert(defines_lwa['LWA_SCH_SCP_DIR']))
        fh.write("SCH_PATH = '%s'\n" % os.path.join(prefix_path, 'sch'))
        fh.write("EXC_PATH = '%s'\n" % os.path.join(prefix_path, 'exec'))
        fh.write("TP_PATH = '%s'\n" % os.path.join(prefix_path, 'tp'))
        
        fh.write("\n\n")
        
        fh.write("ADDRESSES = {\n")
        for name in addresses:
            fh.write("             '%s': %s,\n" % (name, tuple(addresses[name])))
        fh.write("}\n")
        
        fh.write("\n\n")
        
        fh.write("SUBSYSTEMS = {\n")
        for name,value in sorted(subsystems.items(), key=lambda x: x[1]):
            fh.write("              '%s': %s,\n" % (name, value))
        fh.write("}\n")
        fh.write("SUBSYSTEM_IDS = {\n")
        for name,value in sorted(subsystems.items(), key=lambda x: x[1]):
            fh.write("                 %s: '%s',\n" % (value, name))
        fh.write("}\n")
        
        fh.write("\n\n")
        
        fh.write("COMMANDS = {\n")
        for name,value in sorted(commands.items(), key=lambda x: x[1]):
            fh.write("            '%s': %s,\n" % (name, value))
        fh.write("}\n")
        fh.write("COMMAND_IDS = {\n")
        for name,value in sorted(commands.items(), key=lambda x: x[1]):
            fh.write("                %s: '%s',\n" % (value, name))
        fh.write("}\n")
        
        fh.write("EXC_COMMANDS = {\n")
        for name,value in sorted(exec_commands.items(), key=lambda x: x[1]):
            fh.write("                 '%s': %s,\n" % (name, value))
        fh.write("}\n")
        fh.write("EXC_COMMAND_IDS = {\n")
        for name,value in sorted(exec_commands.items(), key=lambda x: x[1]):
            fh.write("                     %s: '%s',\n" % (value, name))
        fh.write("}\n")


write_config_info()
            

setup(
    name                 = "lwa-mcs",
    version              = "0.1",
    description          = "LWA MCS interface module", 
    long_description     = "Module for interacting with the LWA MCS softare",
    url                  = "https://github.com/lwa-project/monitor_and_control/", 
    author               = "Jayce Dowell",
    author_email         = "jdowell@unm.edu",
    license              = 'GPL',
    classifiers          = ['Development Status :: 5 - Production/Stable',
                            'Intended Audience :: Developers',
                            'Intended Audience :: Science/Research',
                            'License :: OSI Approved :: GNU General Public License (GPL)',
                            'Topic :: Scientific/Engineering :: Astronomy',
                            'Programming Language :: Python :: 2',
                            'Programming Language :: Python :: 2.6',
                            'Programming Language :: Python :: 2.7',
                            'Operating System :: POSIX :: Linux'],
    packages             = find_packages(exclude="tests"), 
    scripts              = glob.glob('scripts/*.py'), 
    python_requires      = '>=2.6, <3', 
    include_package_data = True,  
    zip_safe             = False
)
