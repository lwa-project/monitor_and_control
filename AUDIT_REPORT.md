# Audit Report: only_ndp Branch Inconsistencies
**Date:** 2025-11-07
**Branch:** only_ndp
**Auditor:** Claude (Automated Analysis)

---

## Executive Summary

This audit examines the `only_ndp` branch for internal inconsistencies resulting from the re-scoping of the project. The branch renamed the data processor subsystem from "DP" to "NDP" and transitioned from old commands (TBW, TBN, FST) to new commands (TBT, TBS, COR).

**Key Finding:** The C source code has been properly updated to use NDP and the new commands (TBT, TBS, COR), but the documentation (README files) extensively references the obsolete naming, creating confusion about current capabilities.

---

## Category 1: Documentation References to Obsolete Subsystem Names

### Issue: README files reference "DP" instead of "NDP"

**Status:** The actual code uses "NDP" correctly, but documentation is inconsistent.

#### exec/README.md
- **Line 15:** Section title "Monitoring & Control of the DP Subsystem" → should be "NDP Subsystem"
- **Line 57:** "DP FST, BAM, and DRX commands" → should be "NDP FST, BAM, and DRX commands"
- **Line 61:** "TBW, TBN, or DRX mode" → should be "TBT, TBS, or DRX mode"
- **Line 100:** "acquisition from DP, via DR" → should be "from NDP, via DR"
- **Line 103:** "DP FST command" → should be "NDP FST command"
- **Line 106, 109:** "DP BAM command" → should be "NDP BAM command"
- **Line 126:** "SHL, ASP, DP, and DR1 through DR5" → should be "NDP" not "DP"
- **Line 137, 151, 152, 164-166:** Example log entries show "DP_" → should show "NDP" in updated examples
- **Line 189:** "one-shot acquisition from DP" → should be "from NDP"
- **Line 199:** "TBW and TBN acquisitions" → should be "TBT and TBS acquisitions"
- **Line 206:** "DP-dictated TBW arguments" → should be "NDP-dictated TBT arguments"
- **Line 215:** "DP-dictated TBN arguments" → should be "NDP-dictated TBS arguments"
- **Line 220:** "DP output which is currently configured for DRX" → should be "NDP output"
- **Line 231:** Section title "Monitoring & Control of the DP Subsystem" → should be "NDP Subsystem"
- **Lines 234-237:** Multiple references to "DP subsystem" → should be "NDP subsystem"
- **Lines 241-263:** Multiple command examples with "DP_" → should use "NDP" (e.g., `./mesix DP_ TBW` → `./mesix NDP TBT`)
- **Line 278:** "A complete list of DP non-ICD MIB entries" → should be "NDP non-ICD MIB entries"

#### sch/README.md
- **Line 101:** "NDP beamforming commands FST, BAM, or DRX" - Inconsistent: should clarify if FST is for NDP or obsolete
- **Line 395:** "The DP commands FST, BAM, and DRX" → should be "NDP commands"
- **Line 397:** "parameters... sent as arguments of the FST, BAM, and DRX commands" → context suggests NDP
- **Line 399:** "cannot be used to send the DP FST, BAM, or DRX commands" → should be "NDP commands"
- **Line 450:** "DP is now partially supported" → should be "NDP"
- **Line 450:** "For DP, the MIB is fully supported as well as the commands TBW, TBN, CLK, and INI" → should reference TBT, TBS for NDP
- **Line 450:** "The DP commands DRX, BAM, and FST are not yet supported" → should be "NDP commands"

**User Input Required:**
1. Should all documentation examples be updated to use NDP instead of DP_?
2. Should historical examples (showing old test output) be preserved as-is with a note, or updated?

---

## Category 2: Documentation References to Obsolete Commands

### Issue: README files reference TBW and TBN extensively, but code implements TBT and TBS

**Evidence from Code:**
- `exec/meosx.c:9` - Modes are documented as "TBT", "TBS", "DRX" (not TBW/TBN)
- `exec/meos.c:17` - Error code says "TBT, TBS, or DRX" (not TBW/TBN)
- `exec/meos.c:23` - "NDP TBT, TBS, or DRX command"
- `exec/meos.c:109-128` - Code implements TBT and TBS parsing (not TBW/TBN)

**Status:** Code correctly uses TBT/TBS, but README describes TBW/TBN.

#### Common/mcs.h (Reference Only - Properly Documented)
The following are correctly marked as "Obsolete" in mcs.h:
- `LWA_CMD_TBW` (id=15) - marked "Obsolete"
- `LWA_CMD_TBN` (id=16) - marked "Obsolete"
- `LWA_CMD_FST` (id=19) - marked "Obsolete"
- `LWA_CMD_CLK` (id=20) - marked "Obsolete"
- `LWA_CMD_TBF` (id=40) - marked "Obsolete"

New commands are properly defined:
- `LWA_CMD_TBT` (id=42) - "TBT (NDP)"
- `LWA_CMD_TBS` (id=43) - "TBS (NDP)"
- `LWA_CMD_COR` (id=41) - "COR (NDP)"
- `LWA_CMD_DRX` (id=17) - "DRX (NDP)"
- `LWA_CMD_BAM` (id=18) - "BAM (NDP)"

**User Input Required:**
1. Should FST be marked as obsolete if it's still used for NDP beamforming configuration?
2. Should BAM continue to be supported for NDP, or is there a replacement?
3. Are DRX commands still current for NDP?

---

## Category 3: File Naming Inconsistencies in exec/ Directory

### Issue: Inconsistent use of underscore in file naming

**Pattern Analysis:**

Files **with** underscore after `me`:
- me_exec.c, me_exec_1.c
- me_init.c, me_inproc.c, me_mon.c, me_tpcom.c
- me_getaltaz.c, me_getaltaz_x.c
- me_getlst.c, me_getlst_x.c
- me_geteop.c, me_read_ssmif.c
- me_findjov.c, me_findjov_x.c
- me_findlun.c, me_findlun_x.c
- me_findsol.c, me_findsol_x.c
- me_point_corr.c, me_point_corr_x.c
- me_precess.c, me_precess_x.c

Files **without** underscore after `me`:
- mecfg.c, medfg.c, megfg.c
- meei.c, meeix.c
- mefsdfg.c
- memdre.c, memdrex.c
- meos.c, meosx.c
- mesi.c, mesix.c
- medrange.c

**Recommendation:** Standardize on `me_` prefix for all executive files.

**User Input Required:**
1. Should all files be renamed to use the `me_` prefix?
2. If renamed, this would affect:
   - Build system (Makefile)
   - Documentation references
   - Any scripts or tools that call these executables
   - Historical scripts/examples

**Proposed Renaming:**
```
mecfg.c      → me_cfg.c       (and mecfg executable → me_cfg)
medfg.c      → me_dfg.c       (and medfg executable → me_dfg)
megfg.c      → me_gfg.c       (and megfg executable → me_gfg)
meei.c       → me_ei.c        (and meei function name consideration)
meeix.c      → me_eix.c       (and meeix executable → me_eix)
mefsdfg.c    → me_fsdfg.c     (and mefsdfg executable → me_fsdfg)
memdre.c     → me_mdre.c      (and memdre function name consideration)
memdrex.c    → me_mdrex.c     (and memdrex executable → me_mdrex)
meos.c       → me_os.c        (and meos function name consideration)
meosx.c      → me_osx.c       (and meosx executable → me_osx)
mesi.c       → me_si.c        (and mesi function name consideration)
mesix.c      → me_six.c       (and mesix executable → me_six)
medrange.c   → me_drange.c    (and medrange executable → me_drange)
```

**Note:** Function names like `mesi()`, `meos()`, `meei()`, `memdre()` would also need consideration for consistency.

---

## Category 4: FIXME Markers Requiring Resolution

### Issue: mcs.h contains unresolved FIXME markers

**Location:** `/home/user/monitor_and_control/common/mcs.h`

#### FIXME 1: Timeout Values
- **Line 50:** `#define LWA_PTQ_TIMEOUT 15 /*FIXME*/`
  - Comment: "timeout in seconds (increased from 4 to 5 to accomodate DP emulator's INI)"
  - **Issue:** Comment mentions "DP emulator" instead of "NDP emulator"
  - **Question:** Is the timeout value of 15 appropriate for NDP?

- **Line 54:** `#define LWA_MS_TASK_QUEUE_TIMEOUT 20 /*FIXME*/`
  - **Question:** Is this timeout value appropriate?

#### FIXME 2: Duplicate Definitions
- **Line 891:** `#define LWA_MAX_NSTD 256 /* FIXME should be reconciled with ME_MAX_NSTD */`
  - **Issue:** Two different defines for the same concept
  - **Question:** Should these be unified into a single definition?

**User Input Required:**
1. What should the PTQ timeout be for NDP operations?
2. What should the task queue timeout be?
3. Should LWA_MAX_NSTD and ME_MAX_NSTD be reconciled? What should the value be?

---

## Category 5: README FIXME References to Non-Existent Files

### Issue: exec/README.md references deprecated files that no longer exist

**Location:** `exec/README.md:26-29`

```
FIXME mers -- deprecated
FIXME meis -- deprecated
FIXME me
FIXME meei(x)
```

**Status:** These files don't exist in the current codebase. The FIXME notes appear to be stale.

**Recommendation:** Remove these stale FIXME references from the README.

**User Input Required:**
1. Should these FIXME lines be removed entirely?
2. Should they be moved to a "Deprecated/Removed" section for historical context?

---

## Category 6: Observation Mode Constants Using Old Command Names

### Issue: mcs.h defines observation modes with old command names

**Location:** `common/mcs.h:671-672`

```c
#define LWA_OM_TBW       5
#define LWA_OM_TBN       6
```

**Status:** These constants reference obsolete commands TBW and TBN.

**User Input Required:**
1. Should these be renamed to `LWA_OM_TBT` and `LWA_OM_TBS`?
2. Are these observation modes still used? If so, do they map to the new TBT/TBS commands?
3. Would changing these break compatibility with existing session definition files or other code?

---

## Category 7: Header File Comments Referencing Migration

### Issue: mcs.h contains migration notes that may be outdated

**Location:** `common/mcs.h:2-4`

```c
// issues
// LWA_MCS.h deprecated; check all scheduler source code
// LWA_MCS_subsytems.h deprecated
// update readmes for sch, exec, tp
```

**Status:** These appear to be migration notes from an older version.

**Recommendation:** Either complete the migration tasks or remove the notes if migration is complete.

**User Input Required:**
1. Has the migration from LWA_MCS.h been completed?
2. Should these comment lines be removed?
3. Should the READMEs be updated per this note?

---

## Category 8: Code Maintains Backward Compatibility Definitions

### Status: INTENTIONAL - Not an inconsistency, but worth documenting

**Location:** `common/mcs.h:212, 218`

```c
#define LWA_SID_DP_  13  /* DP - Obsolete */
#define LWA_SID_ADP  19  /* ADP - Obsolete */
```

**Analysis:** These are properly marked as obsolete and maintained for backward compatibility. The code correctly uses `LWA_SID_NDP (20)` for actual operations.

**Action:** No change needed. This is intentional backward compatibility.

---

## Summary of Recommendations

### Can Fix Without User Input:
1. ✅ Remove stale FIXME references to non-existent files (mers, meis, me) from exec/README.md
2. ✅ Update mcs.h line 50 comment to reference "NDP emulator" instead of "DP emulator"
3. ✅ Remove migration comment from mcs.h lines 2-4 if migration is complete

### Requires User Clarification:

#### Priority 1: Command/Subsystem Naming (High Impact)
- [ ] Confirm that documentation should consistently use NDP instead of DP/DP_
- [ ] Confirm that documentation should use TBT/TBS instead of TBW/TBN
- [ ] Clarify status of FST, BAM, DRX commands for NDP
- [ ] Decide whether to update observation mode constants (LWA_OM_TBW → LWA_OM_TBT, etc.)

#### Priority 2: File Naming (Medium Impact)
- [ ] Decide whether to standardize exec/ filenames to use `me_` prefix
- [ ] If renaming, identify all dependent systems/scripts that need updating

#### Priority 3: Configuration Values (Low Impact)
- [ ] Resolve FIXME for LWA_PTQ_TIMEOUT - appropriate value for NDP?
- [ ] Resolve FIXME for LWA_MS_TASK_QUEUE_TIMEOUT
- [ ] Reconcile LWA_MAX_NSTD with ME_MAX_NSTD

#### Priority 4: Documentation Updates (Medium Impact)
- [ ] Update exec/README.md with current command examples (TBT/TBS, NDP)
- [ ] Update sch/README.md references to DP → NDP
- [ ] Update example command outputs in READMEs to show NDP instead of DP_

---

## Questions for User

Please provide guidance on the following:

1. **Documentation Update Scope:** Should all README examples be updated to reflect NDP and TBT/TBS, or should some historical examples be preserved with explanatory notes?

2. **Command Status:**
   - Is FST command still used with NDP, or is it obsolete?
   - Is BAM command still used with NDP for beamforming?
   - Is DRX command the current beamforming command for NDP?

3. **File Renaming:** Do you want to standardize exec/ filenames to the `me_` pattern? This is a larger change that affects:
   - 13 source files
   - Corresponding executables
   - Makefile
   - All documentation
   - Any external scripts

4. **Configuration Values:** Should I investigate the codebase to recommend appropriate timeout values, or do you have specific values in mind?

5. **Backward Compatibility:** Should the obsolete definitions (LWA_SID_DP_, LWA_CMD_TBW, etc.) remain in mcs.h for backward compatibility, or can they be removed?

---

## Next Steps

Once you provide guidance on the questions above, I can:
1. Make documentation updates (README files)
2. Update code comments and FIXME markers
3. Optionally perform file renaming if desired
4. Update configuration values
5. Create a comprehensive commit documenting all changes

Please let me know which inconsistencies you'd like me to address first.
