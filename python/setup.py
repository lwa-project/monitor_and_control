
import os
import glob

from setuptools import setup, Extension, Distribution, find_packages

import config

include_dirs = ['../common/', '../exec/', '/usr/include/gdbm']
include_dirs.extend(config.include_dirs)

libraries = ['gdbm']
libraries.extend(config.libraries)

extra_link_args = []
extra_link_args.extend(['-L%s' % ldir for ldir in config.library_dirs])

ExtensionModules = [Extension('_mcs', ['lwa_mcs/_mcs.c',],
                              include_dirs=include_dirs, libraries=libraries,
                              extra_link_args=extra_link_args),]

setup(
    name                 = "lwa-mcs",
    version              = "0.4",
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
                            'Programming Language :: Python :: 3',
                            'Programming Language :: Python :: 3.6',
                            'Programming Language :: Python :: 3.7',
                            'Programming Language :: Python :: 3.8',
                            'Programming Language :: Python :: 3.9',
                            'Operating System :: POSIX :: Linux'],
    packages             = find_packages(exclude="tests"), 
    scripts              = glob.glob('scripts/*.py'), 
    python_requires      = '>=3.8',
    install_requires     = ['backports.zoneinfo; python_version<"3.9"'],
    include_package_data = True,
    ext_package          = 'lwa_mcs',
    ext_modules          = ExtensionModules,
    zip_safe             = False
)
