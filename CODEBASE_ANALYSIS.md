# Monitor and Control (LWA) Codebase Structure Analysis

## Executive Summary

This is the **Monitor and Control (MCS)** system for the Long Wavelength Array (LWA), a radio astronomy facility. The project has been recently re-scoped on the `only_ndp` branch with significant naming and command structure changes from older DP (Data Processor) nomenclature to the newer NDP (New Data Processor) convention.

---

## 1. MAIN COMPONENTS AND MODULES

The codebase is organized into **4 major subsystems**:

### A. MCS/Executive (`exec/` directory) - 34 C source files
The executive layer that accepts commands from Task Processor and communicates with the Scheduler.

**Core Functions:**
- **me_exec.c / me_exec_1.c** - Main process; handles session management and orchestration
- **me_init.c** - Initialization of the executive environment
- **me_inproc.c** - In-process command handling (1,889 lines)
- **me_tpcom.c** - Task Processor communication interface

**Command Injection & Utilities:**
- **mesi.c / mesix.c** - Command injection to scheduler (mesi = library; mesix = CLI wrapper)
- **memdre.c / memdrex.c** - MIB entry queries
- **meos.c / meosx.c** - One-shot acquisition utility
- **meei.c / meeix.c** - Local session control interface
- **mecfg.c / medfg.c / megfg.c** - File generators (text→binary) for beamforming parameters
- **mefsdfg.c** - SDF (Session Definition File) generator
- **medrange.c** - Dynamic range calculations

**Astronomical Utilities:**
- **me_getaltaz[_x].c** - Altitude/azimuth calculations
- **me_getlst[_x].c** - Local Sidereal Time calculations
- **me_findsol[_x].c** - Solar position calculations
- **me_findlun[_x].c** - Lunar position calculations
- **me_findjov[_x].c** - Jupiter position calculations
- **me_point_corr[_x].c** - Pointing correction calculations
- **me_precess[_x].c** - Precession calculations
- **me_geteop.c** - EOP (Earth Orientation Parameters) data
- **me_read_ssmif.c** - Station System Model reading
- **me_mon.c** - Monitoring tool
- **me_astro.h** - Astronomical header

**Header Files:**
- **me.h** - Executive-specific definitions and includes (includes mesi.c and memdre.c)

### B. MCS/Scheduler (`sch/` directory) - 22 C source files
The central orchestrator managing subsystems and their MIBs.

**Core Processes:**
- **ms_init.c** - Initialization and startup management
- **ms_exec.c** (865 lines) - Main scheduler executive
- **ms_mcic.c** (855 lines) - Main subsystem interface controller
- **ms_mcic_SHL.c** - Shelter subsystem-specific handler
- **ms_mcic_ASP.c** - ASP subsystem-specific handler
- **ms_mcic_NDP.c** - NDP (New Data Processor) subsystem-specific handler
- **ms_mcic_mib.c** - MIB update dispatcher

**MIB Management:**
- **dat2dbm.c** - Data→DBM converter for MIB initialization
- **ms_makeMIB_SHL.c** - Generates SHL MIB structure
- **ms_makeMIB_ASP.c** - Generates ASP MIB structure
- **ms_makeMIB_NDP.c** - Generates NDP MIB structure
- **ms_makeMIB_DR.c** - Generates Data Recorder MIB structure

**MIB Access Tools:**
- **ms_mdre_ip.c** - IP-based MIB interface
- **ms_mdr.c** - MIB dump utility
- **ms_mdre.c** - MIB entry reader
- **ms_md2t.c** - MIB to text converter
- **ms_mb.c** - ncurses-based MIB browser
- **ms_mu.c** - MIB update utility

**Logging & Replay:**
- **ms_exec_log.c** - Logging system
- **ms_exec_replay.c** - Replay recorded sessions
- **ms_init_replay.c** - Replay initialization

**Monitoring & Configuration:**
- **ms_mon.c** - Log file monitor
- **msei.c** - Legacy command injector (for DP, now superseded by mesix)

**Test Utilities:**
- **mch_minimal_server.py** - Python subsystem emulator
- **test[1-9].sh** - Test scripts

**Configuration Files:**
- **NU[1-4]_MIB_init.dat** - Mock subsystem MIB data
- **ms_mb_*.cfg** - ncurses configuration files for MIB browser

### C. MCS/Task Processor (`tp/` directory) - 11 C source files
Interface between operators and the MCS system.

**Core Modules:**
- **tpss.c / tpss[0-5].c** - Task Processor Session Scheduler (modular design)
  - tpss0.c - Initialization/setup
  - tpss1.c - Configuration loading
  - tpss2.c - Session scheduling (500 lines)
  - tpss3.c - Observation management
  - tpss4.c - Session processing
  - tpss5.c - Finalization/cleanup
- **tptc.c** - Task Processor Time Check utility
- **tpsdm.c** (693 lines) - Station Design Model validator
- **tprs.c / tprs1.c** (2,641 lines) - Session Definition File (SDF) reader/parser
- **tpms.c** - MCS management utility
- **tprcs.c** - RCS (Revision Control System) integration

**Headers:**
- **mt.h** - Task Processor-specific definitions

**Test Data:**
- **test/sdf_test[1-5].txt** - Sample SDF files for testing

### D. Common Components (`common/` directory)
**Shared across all subsystems:**
- **mcs.h** (1,415 lines) - Central header with:
  - Data structure definitions
  - Subsystem/command ID definitions (LWA_SID_*, LWA_CMD_*)
  - Time conversion utilities
  - IPC definitions
  - MIB record structures
  - Platform-specific includes (Linux/OSX compatibility)

- **fileutils.h** - File operation utilities
- **config.h.in** - Build configuration template

### E. Utility Components
- **utils/** - Test and build utilities
  - test_sch_connection.c - Scheduler connectivity test
  - test_tp_connection.c - Task Processor connectivity test
  - build_station_start.c - Station startup builder

### F. Python Interface (`python/lwa_mcs/` directory)
- **__init__.py** - Package initialization
- **config.py** - Configuration management
- **exc.py** - Executive interface module
- **sch.py** - Scheduler interface module
- **tp.py** - Task Processor interface module
- **mib.py** - MIB management
- **utils.py** - Utility functions
- **_mcs.c** - C extension for high-performance operations

### G. Astronomical Library (`sofa/` directory)
SOFA (Standards of Fundamental Astronomy) library for precise astronomical calculations.

---

## 2. PROGRAMMING LANGUAGES

- **ANSI C** - ~332 source files (.c, .h) - Primary implementation language
- **Python** - ~13 files (.py) - High-level interface and emulation
- **Shell Script** - ~13 files (.sh) - Testing, deployment, and automation
- **Build System** - autotools (configure, Makefile.in, configure.ac)

**File Statistics:**
- C/Header files: 332
- Python files: 13
- Shell scripts: 13

---

## 3. KEY EXECUTABLE FILES AND SCRIPTS

### MCS/Executive Executables
| Executable | Type | Purpose |
|-----------|------|---------|
| me_exec | C | Main executive process |
| mesix | C | Command injection CLI |
| meosx | C | One-shot acquisition |
| memdrex | C | MIB query utility |
| meeix | C | Local session control |
| mecfg/medfg/megfg | C | Beamforming file generators |
| mefsdfg | C | SDF file generator |
| me_mon | C | Monitoring utility |
| meei.c (function) | C | In-process session API |

### MCS/Scheduler Executables
| Executable | Type | Purpose |
|-----------|------|---------|
| ms_init | C | Initialization |
| ms_exec | C | Scheduler executive |
| ms_mcic | C | Subsystem interface |
| ms_makeMIB_* | C | MIB generators (SHL/ASP/NDP/DR) |
| msei | C | Legacy command injector |
| ms_md* (mdr/mdre/md2t/mdre_ip) | C | MIB access tools |
| ms_mb | C | MIB browser (ncurses) |
| ms_mu | C | MIB update utility |
| ms_mon | C | Log monitor |
| dat2dbm | C | Data→DBM converter |

### MCS/Task Processor Executables
| Executable | Type | Purpose |
|-----------|------|---------|
| tpss | C | Session scheduler |
| tptc | C | Time check utility |
| tpsdm | C | Station Design Model validator |
| tprs | C | SDF reader |
| tpms | C | MCS management |
| tprcs | C | RCS integration |

### Test & Support
| Script | Type | Purpose |
|--------|------|---------|
| test[1-9].sh | Shell | Test suites |
| ms_shutdown.sh | Shell | Emergency shutdown |
| md.sh | Shell | MIB dump utility |
| me_crash.sh | Shell | Recovery utility |
| mch_minimal_server.py | Python | Subsystem emulator |

---

## 4. NAMING CONVENTIONS

### A. Subsystem Naming
**Three-letter codes:**
- `MCS` - Monitor and Control System (control subsystem)
- `SHL` - Shelter
- `ASP` - ASP subsystem
- `NDP` - New Data Processor (primary)
- `DP_` - Data Processor (OBSOLETE - marked in mcs.h)
- `ADP` - ADP (OBSOLETE)
- `DR1-DR5` - Data Recorders
- `NU1-NU9` - Null subsystems (for testing)

### B. Command Naming
**Three-letter codes (defined in mcs.h):**

**Common Commands:**
- `PNG` - Ping
- `RPT` - Report (read MIB values)
- `SHT` - Shutdown
- `INI` - Initialize

**NDP-Specific Commands (NEW):**
- `TBT` - Time-domain, Burst, wideband (Tuning) - LWA_CMD_TBT (id=42)
- `TBS` - Time-domain, Burst, narrowband (Sample) - LWA_CMD_TBS (id=43)
- `COR` - Correlation - LWA_CMD_COR (id=41)
- `DRX` - Beamforming (Direct Reception eXtended) - LWA_CMD_DRX (id=17)
- `BAM` - Beamforming (BEAM Assignment/Management) - LWA_CMD_BAM (id=18)
- `STP` - Stop (DR# and NDP) - LWA_CMD_STP (id=23)

**Legacy Commands (OBSOLETE):**
- `TBW` - Time-domain, Burst, Wideband - LWA_CMD_TBW (id=15) OBSOLETE
- `TBN` - Time-domain, Burst, Narrowband - LWA_CMD_TBN (id=16) OBSOLETE
- `FST` - Finite Impulse Response Setup - LWA_CMD_FST (id=19) OBSOLETE

**Data Recorder Commands:**
- `REC` - Record
- `DEL` - Delete
- `GET` - Get file
- `CPY` - Copy
- `DMP` - Dump
- `FMT` - Format
- etc.

### C. Source File Naming
**Pattern: `[prefix][function_name][_x].c`**

Where:
- `prefix` = subsystem identifier (me_=executive, ms_=scheduler, tp=task processor)
- `function_name` = what the function does
- `_x` suffix = standalone executable wrapper (CLI interface to a library function)

**Examples:**
- `mesi.c` = function library
- `mesix.c` = executable wrapper for mesi() function
- `meos.c` = function library
- `meosx.c` = executable wrapper for meos() function
- `meei.c` = function library
- `meeix.c` = executable wrapper for meei() function
- `me_exec.c` = main executable (includes me_exec_1.c)
- `me_init.c` = standalone initialization executable
- `ms_init.c` = standalone scheduler initialization executable

### D. Variable/Function Naming
**Prefixes in mcs.h and me.h:**
- `LWA_` - Global LWA definitions (IDs, constants, utilities)
- `ME_` - Monitor Executive specific
- `MS_` - Monitor Scheduler specific
- `MT_` - Task Processor specific (in mt.h)
- `MESI_ERR_` - mesi() error codes
- `MEOS_` - meos() error/mode codes
- `ME_MAX_` - Maximum values
- `LWA_SID_` - Subsystem IDs
- `LWA_CMD_` - Command IDs
- `MIB_` - MIB record types and field lengths
- `LWA_MSELOG_TP_` - Task progress codes

### E. Structure Naming
- `struct subsystem_status_struct` - Subsystem status
- `struct ndp_ch_struct` - NDP channel information
- `struct ndpo_struct` - NDP output information
- `struct dbm_record` - DBM (GDBM) record structure
- `struct LWA_mib_entry` - MIB entry for IP exchange
- `struct me_session_queue_struct` - Session queue (me_exec)
- `struct me_action_struct` - Action/command structure
- `struct sc_struct` - Station configuration structure

---

## 5. COMMAND STRUCTURES

### A. MCS/Executive → Scheduler Interface (via TCP/IP)
**Port:** 9734 (LWA_PORT_MSE)
**Function:** mesi()
**Protocol:** Custom TCP format with reference numbers

### B. MCS/Scheduler → Subsystems Interface
**Inter-Process Communication (IPC):** POSIX message queues
**Format:** Custom binary/ASCII hybrid (ICD-compliant)

### C. Task Processor → MCS/Executive Interface
**Communication:** Unknown - defined in me_tpcom.c

### D. Session Definition File (SDF)
Text-based configuration format read by Task Processor (tprs.c)
Specifies observations with parameters like:
- Observation mode (TBT, TBS, TRK_RADEC, etc.)
- Frequency, bandwidth
- Recording duration
- Target coordinates

### E. Command Line Arguments
Examples from README:
```bash
mesix <dest> <cmd> <data> <date> <time>
  <dest>  = Three-letter subsystem code
  <cmd>   = Three-letter command code
  <data>  = Command-specific parameters
  <date>  = MJD or "today"
  <time>  = MPM, +n (seconds), or "asap"

meosx <DR#> <ExtDev> <DestDir> <mode> <args>
  <mode>  = "TBT", "TBS", or "DRX"
  <args>  = Mode-specific parameters
```

### F. Configuration Hierarchy
1. **mcs.h** - Global defines and platform config
2. **me.h** - Executive configuration
3. **mt.h** - Task Processor configuration
4. **SDF files** - Session/observation specifications
5. **MIB files** (.dat) - Subsystem database initialization
6. **.cfg files** - ncurses display configuration

---

## 6. DOCUMENTATION

### README Files (by size):
1. **sch/README.md** (484 lines) - Comprehensive scheduler documentation
2. **exec/README.md** (318 lines) - Executive software guide
3. **tp/README.md** (221 lines) - Task Processor documentation
4. **README.md** (7 lines) - Root project description

### Key Documentation References:
- MCS0005 - MCS Overview
- MCS0030 - Observing documentation
- MCS0031 - Station MIB documentation
- MCS0012 - SHL emulator
- MCS0025 - MCS-DR ICD
- MCS0027 - DROS documentation
- MCS0029 - MIB browser usage

### Code Comments:
- Extensive banner comments at start of each file
- Version history tracked in file headers
- FIXME markers indicate incomplete/known issues
- Function-level documentation above major functions

---

## 7. DETECTED INCONSISTENCIES AND NAMING ISSUES

### A. CRITICAL: Subsystem Naming Transition
**Issue:** Code references both `DP_` (obsolete) and `NDP` (current)

**Evidence:**
```c
// In mcs.h:
#define LWA_SID_DP_  13  /* DP - Obsolete */
#define LWA_SID_ADP 19  /* ADP - Obsolete */
#define LWA_SID_NDP 20  /* NDP */
```

**Files Affected:**
- /home/user/monitor_and_control/common/mcs.h (defines both)
- /home/user/monitor_and_control/exec/mesi.c (references DP_/ADP/NDP)
- /home/user/monitor_and_control/sch/ms_mcic_NDP.c (primary handler for NDP)
- /home/user/monitor_and_control/sch/ms_makeMIB_NDP.c (generates NDP MIB)

**Status:** Appears intentional for backward compatibility, but note: DP_ is never instantiated as a subsystem in modern code.

### B. CRITICAL: Command Naming Transition
**Issue:** Obsolete and new commands coexist

**Old Commands (OBSOLETE):**
- TBW (id=15) - superseded by TBT/TBS
- TBN (id=16) - superseded by TBT/TBS
- FST (id=19) - Finite Impulse Response Setup
- BAM (id=18) - Still used for NDP beamforming
- CLK (id=20) - Clock command

**New Commands (NDP):**
- TBT (id=42) - Time-domain Burst Tuning
- TBS (id=43) - Time-domain Burst Sample
- COR (id=41) - Correlation command
- DRX (id=17) - Direct Reception eXtended beamforming

**Status:** Both sets coexist; documentation mentions TBW/TBN are obsolete but FST/BAM still used.

### C. MEDIUM: File Naming Pattern Inconsistencies

**Inconsistent patterns in exec/ directory:**

| Pattern | Count | Examples | Issue |
|---------|-------|----------|-------|
| me_* | 11 | me_exec, me_init, me_mon | Underscore separator |
| me* (direct) | 11 | mecfg, medfg, mesi, meos | No underscore |
| me*x | 9 | mecfg/medfg/mesi + x | Executable wrappers |
| me*_x | 9 | me_getaltaz_x, me_point_corr_x | Mixed patterns |

**Recommendation:** Standardize on one pattern (suggest `me_` for all)

**Examples of inconsistency:**
- `me_exec.c` (with underscore) but `mesi.c` (without)
- `me_init.c` (with underscore) but `mecfg.c` (without)
- `me_getaltaz_x.c` (mixed pattern) but `mefsdfg.c` (no underscore)

### D. MEDIUM: References to Deprecated Components in README

**In exec/README.md:**
```
FIXME mers -- deprecated
FIXME meis -- deprecated
FIXME me
FIXME meei(x)
```

**Status:** These are listed in FIXME section but no corresponding files exist
- No `mers.c` or `meis.c` in current codebase
- These were apparently removed but README not updated

### E. MEDIUM: Obsolete Code References

**In common/mcs.h (line 2-4):**
```c
// issues
// LWA_MCS.h deprecated; check all scheduler source code
// LWA_MCS_subsytems.h deprecated
// update readmes for sch, exec, tp
```

**Status:** These header files have been removed but migration notes remain

### F. MINOR: Timeout and Configuration FIXME Markers
**In common/mcs.h:**
```c
#define LWA_PTQ_TIMEOUT 15 /*FIXME*/
#define LWA_MS_TASK_QUEUE_TIMEOUT 20 /*FIXME*/
#define LWA_MAX_NSTD 256 /* FIXME should be reconciled with ME_MAX_NSTD */
```

**Status:** Timeout values marked as requiring review; duplicate #defines

### G. MINOR: File Header Comment Inconsistencies

**Different header styles across codebase:**
- Some use `//` comments (newer C99 style)
- Some use `/* */` (ANSI C style)
- Version history in files vs. not in others
- Date formats vary (e.g., "2014 Feb 10" vs "2015 Sep 11")

### H. MEDIUM: Python Module Naming
**Inconsistency:** Python modules use old command names in some docstrings
- Module `exc.py` interfaces with executor but doesn't clearly separate from `sch.py`
- No clear mapping between Python functions and C command codes

---

## 8. KEY INSIGHTS

### Architecture Philosophy
1. **Separation of Concerns:** Three-tier architecture (Task Processor → Executive → Scheduler)
2. **Message-Based Communication:** POSIX message queues and TCP/IP
3. **Library + Wrapper Pattern:** Core logic in `.c` files, CLI wrappers in `*x.c` files
4. **Include-Based Modularity:** Large files like me_exec.c include other .c files rather than linking
5. **Platform Abstraction:** Conditional compilation for Linux/OSX compatibility

### Code Maturity
- Extensive FIXME markers indicate ongoing development
- Deprecation comments show evolution of subsystem naming
- Test suites and emulators provided for development/validation
- Version history tracked in file headers (multiple authors/dates)

### Current Branch Status
- Branch: `claude/audit-only-ndp-branch-011CUti4e8AbFhdZPXswsGBM`
- Status: Clean working tree, all commits recent (Feb 10, 2014 - present)
- Focus: Only-NDP re-scope (removing DP_, adding TBT/TBS commands)

---

## 9. RECOMMENDATIONS

### For Inconsistency Resolution:
1. Standardize exec file naming to `me_*.c` pattern
2. Update README files to remove FIXME references to non-existent files
3. Update header comments to use consistent style (// vs /*)
4. Add deprecation warnings to obsolete command handling code
5. Reconcile ME_MAX_NSTD with LWA_MAX_NSTD
6. Document Python module mapping to C command codes

### For Code Quality:
1. Create a modern MCS API documentation
2. Add type hints to Python code
3. Update SOFA library to latest version if not already done
4. Add unit tests for command parsing
5. Create migration guide from DP to NDP naming

---

