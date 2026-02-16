# MCS Programs and Utilities

This document lists the programs and utilities built by the MCS software
package, organized by subsystem.  The naming convention is:

- **`ms_`** = MCS/Scheduler
- **`me`** / **`me_`** = MCS/Executive
- **`tp`** = Task Processor
- **`x`** suffix = standalone command-line wrapper for a library function

## MCS/Scheduler (`sch/`)

| Program | Likely Expansion | Description |
|---------|-----------------|-------------|
| `ms_init` | MCS/Scheduler -- Initialize | Initializes MCS/Scheduler from a configuration file and launches `ms_exec`. |
| `ms_exec` | MCS/Scheduler -- Execute | Main Scheduler execution loop; manages subsystem communication and session scheduling. |
| `msei` | MCS/Scheduler -- Executive Inject | Command-line tool to inject commands (e.g., PNG, RPT, SHT) into the Scheduler's `ms_exec` process. |
| `ms_mcic` | MCS/Scheduler -- MCS Common ICD (Communication) | Handles subsystem message protocol per the MCS Common ICD; includes per-subsystem modules for ASP, NDP, DR, and SHL. |
| `ms_mon` | MCS/Scheduler -- Monitor | ncurses-based Scheduler monitor; shows recent log entries and current time. |
| `ms_mb` | MCS/Scheduler -- MIB Browser | ncurses-based MIB browser; displays and optionally updates MIB entries using template files. |
| `ms_mu` | MCS/Scheduler -- MIB Update | Periodically requests RPT updates from a subsystem to refresh its MIB. |
| `ms_mdr` | MCS/Scheduler -- MIB Dump (Read) | Dumps the entire MIB dbm database for a subsystem. |
| `ms_mdre` | MCS/Scheduler -- MIB Dump Read Entry | Reads a single MIB entry from a subsystem's dbm database. |
| `ms_mdre_ip` | MCS/Scheduler -- MIB Dump Read Entry (IP) | Socket-based server providing Executive access to Scheduler MIB dbm files. |
| `ms_md2t` | MCS/Scheduler -- MIB Dump to Text | Dumps a subsystem's MIB dbm database to a text file. |
| `ms_makeMIB_*` | MCS/Scheduler -- Make MIB | Generates initial MIB dbm database files for each subsystem (ASP, NDP, DR, SHL). |
| `dat2dbm` | DAT to DBM | Converts a subsystem MIB initialization `.dat` file into a gdbm database. |

### Replay tools

| Program | Description |
|---------|-------------|
| `ms_init_replay` | Replay version of `ms_init` for offline testing. |
| `ms_exec_replay` | Replay version of `ms_exec` for offline testing. |
| `ms_mdre_replay` | Replay version of `ms_mdre` for offline testing. |

## MCS/Executive (`exec/`)

### Core

| Program | Likely Expansion | Description |
|---------|-----------------|-------------|
| `me_exec` | MCS/Executive -- Execute | Main Executive process; manages observation execution. |
| `me_init` | MCS/Executive -- Initialize | Initialization for MCS/Executive. |
| `me_inproc` | MCS/Executive -- In-Processing | Processes observations during execution; handles stepped mode transitions, DRX commands, etc. |
| `me_tpcom` | MCS/Executive -- Task Processor Communication | Executive's interface to the task processor layer; handles session setup and tarball assembly. |
| `me_mon` | MCS/Executive -- Monitor | ncurses-based Executive monitor; shows recent `meelog.txt` entries and current time. |

### Command injection

| Program | Likely Expansion | Description |
|---------|-----------------|-------------|
| `meeix` | MCS/Executive -- Executive Inject (eXecutable) | Command-line tool to inject commands (SHT, STP) into the Executive. |
| `mesix` | MCS/Executive -- Scheduler Inject (eXecutable) | Command-line tool to inject commands into the Scheduler's `ms_exec` via sockets. |
| `memdrex` | MCS/Executive -- MIB Dump Read Entry (eXecutable) | Command-line tool to query the Scheduler's MIB via `ms_mdre_ip`. |
| `meosx` | MCS/Executive -- One-Shot (eXecutable) | Command-line tool for one-shot data acquisitions (TBT, TBS, DRX). |

### Beamforming file generators

| Program | Likely Expansion | Description |
|---------|-----------------|-------------|
| `mecfg` | MCS/Executive -- Coefficient File Generator | Converts `.cft` (text) to `.cf` (binary) delay filter coefficient files. |
| `medfg` | MCS/Executive -- Delay File Generator | Converts `.dft` (text) to `.df` (binary) beam delay files. |
| `megfg` | MCS/Executive -- Gains File Generator | Converts `.gft` (text) to `.gf` (binary) beam gain files. |
| `mefsdfg` | MCS/Executive -- Full SDF Delay File Generator | Generates beam delay files from SSMIF data for use in SDFs. |
| `medrange` | MCS/Executive -- Delay Range | Computes minimum and maximum beam delays for a station from its SSMIF. |

### Astrometry (standalone wrappers)

These `_x` programs are standalone wrappers around library functions used by
`me_exec` and `me_inproc`.  They are useful for testing and debugging.

| Program | Description |
|---------|-------------|
| `me_precess_x` | Precesses J2000 RA/Dec to current epoch (wraps `me_precess`). |
| `me_getaltaz_x` | Converts apparent RA/Dec to topocentric alt/az (wraps `me_getaltaz`). |
| `me_findsol_x` | Computes geocentric apparent position of the Sun (wraps `me_findsol`). |
| `me_findjov_x` | Computes geocentric apparent position of Jupiter (wraps `me_findjov`). |
| `me_findlun_x` | Computes geocentric apparent position of the Moon (wraps `me_findlun`). |
| `me_getlst_x` | Computes local apparent sidereal time (wraps `me_getlst`). |
| `me_point_corr_x` | Applies pointing corrections (wraps `me_point_corr`). |

## Task Processor (`tp/`)

| Program | Likely Expansion | Description |
|---------|-----------------|-------------|
| `tpss` | Task Processor -- Submit Session | Parses a session definition file (SDF), validates it, and generates the session specification (`.ses`) and observation specification (`.obs`) binary files. |
| `tprs` | Task Processor -- Read SSMIF | Reads a Station Static MIB Initialization File (SSMIF) and produces the packed binary `ssmif.dat`. |
| `tpsdm` | Task Processor -- Station Dynamic MIB | Reads/generates Station Dynamic MIB (SDM) data. |
| `tpms` | Task Processor -- Make Session | Generates a quick session from command-line arguments (mode, duration, frequency, etc.) without writing an SDF by hand. |
| `tprcs` | Task Processor -- Read Command Script | Reads and displays MCS/Executive command script (`.cs`) files. |
| `tptc` | Task Processor -- Time Check | Checks time-keeping on the local system and exercises the time calculation and conversion routines in `mcs.h`. |

## Python Package (`python/lwa_mcs/`)

| Module | Description |
|--------|-------------|
| `sch` | Interface to MCS/Scheduler. |
| `tp` | Interface to task processor functions. |
| `mib` | MIB access utilities. |
| `utils` | General MCS utilities. |
| `exc` | MCS exception classes. |
| `_mcs` | C extension module for low-level MCS operations. |

## Utilities (`utils/`)

| Program | Description |
|---------|-------------|
| `build_station_start` | Builds station startup configuration. |
| `test_sch_connection` | Tests connectivity to the Scheduler. |
| `test_tp_connection` | Tests connectivity to the task processor. |
