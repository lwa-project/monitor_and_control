# Python2 compatibility
from __future__ import print_function, division, absolute_import

import os
import glob

from setuptools import setup, Extension, Distribution, find_packages

ExtensionModules = [Extension('_mcs', ['lwa_mcs/_mcs.c',], include_dirs=['../common/', '../exec/', '/usr/include/gdbm'], extra_link_args=['-lgdbm',])]

setup(
    name                 = "lwa-mcs",
    version              = "0.3",
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
                            'Programming Language :: Python :: 2.7',
                            'Programming Language :: Python :: 3',
                            'Programming Language :: Python :: 3.6',
                            'Operating System :: POSIX :: Linux'],
    packages             = find_packages(exclude="tests"), 
    scripts              = glob.glob('scripts/*.py'), 
    python_requires      = '>=2.7', 
    include_package_data = True,
    ext_package          = 'lwa_mcs',
    ext_modules          = ExtensionModules,
    zip_safe             = False
)
