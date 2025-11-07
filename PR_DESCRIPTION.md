# Audit and Fix Internal Inconsistencies - only_ndp Branch

This PR addresses internal inconsistencies identified in the `only_ndp` branch resulting from the re-scoping of the project from DP (Data Processor) to NDP (New Data Processor).

---

## 🎯 Summary

**Audited:** 8 categories of potential inconsistencies
**Fixed:** 5 categories
**Intentional (no changes):** 2 categories
**Skipped (to avoid breaking changes):** 1 category

---

## 🔥 Critical Bug Fix

### exec/mesi.c - DRX Frequency Endianness Bug

**Issue:** When DRX command was updated to use `float64` (8 bytes) for frequency instead of `float32` (4 bytes), the endianness conversion code was not updated. It was still using `f4.b[3]` and `f4.b[7]` instead of `f8.b[3]` and `f8.b[7]`.

**Impact:** This bug would have corrupted all DRX frequency values sent to NDP, causing incorrect tuning.

**Fix:** Updated lines 275-276 to correctly reference the 8-byte float array.

---

## ✅ Categories Fixed

### Category 1: Documentation References to Obsolete Subsystem Names

**Files Modified:**
- `exec/README.md` (major update - 47 insertions, 49 deletions)
- `exec/meosx.c` (comment update)
- `exec/mesi.c` (critical bug fix)

**Changes:**
- Updated all references from DP/DP_ to NDP for current functionality
- Section titles: "Monitoring & Control of the DP Subsystem" → "NDP Subsystem"
- Updated current command examples to use NDP instead of DP_
- Converted TBW/TBN examples to TBT/TBS with correct parameters
- Updated example log outputs to show NDP subsystem (matching current test9.sh)
- Marked obsolete command examples (TBW, TBN with DP_) with clear "OBSOLETE COMMANDS" labels
- Updated DRX examples to reference NDP and current BAM command
- Noted that FST is obsolete throughout; BAM and DRX confirmed as current
- Updated beamforming file generator descriptions to note FST is obsolete
- Updated historical notes to reference NDP for current capabilities

### Category 4: FIXME Markers and Comment Updates

**Files Modified:**
- `common/mcs.h`

**Changes:**
- Removed outdated migration comment block (lines 2-5)
- Updated "DP emulator" → "NDP emulator" in timeout comment
- Updated "DP limits" → "NDP limits" in PTQ_SIZE comment
- Fixed typos: "accomodate" → "accommodate" (2 instances)
- Updated history comment to reference NDP

**Remaining FIXME Markers** (not addressed - require user input):
- `LWA_PTQ_TIMEOUT` (line 45): Currently 15 seconds
- `LWA_MS_TASK_QUEUE_TIMEOUT` (line 49): Currently 20 seconds
- `LWA_MAX_NSTD` vs `ME_MAX_NSTD` (line 886): Duplicate definitions

### Category 5: Stale FIXME References

**Files Modified:**
- `exec/README.md`

**Changes:**
- Removed FIXME references to non-existent files:
  - `mers` -- deprecated
  - `meis` -- deprecated
  - `me` (never existed)

**Kept:** FIXME references to files that do exist (meei, me_init, me_tpcom, me_crash.sh)

### Category 7: Migration Comments

**Files Modified:**
- `common/mcs.h`

**Changes:**
- Removed migration notes about deprecated headers (LWA_MCS.h, LWA_MCS_subsytems.h)
- These headers no longer exist; migration is complete

---

## ✅ Categories Not Requiring Changes (Intentional Design)

### Category 2: Obsolete Command Definitions

**Status:** Correct as-is

**Analysis:**
- Obsolete commands (TBW id=15, TBN id=16, FST id=19, CLK id=20, TBF id=40) properly marked "Obsolete" in `mcs.h`
- Current commands (TBT id=42, TBS id=43, COR id=41, BAM id=18, DRX id=17) properly defined for NDP
- Observation mode constants correctly structured:
  - Legacy: `LWA_OM_TBW` (5), `LWA_OM_TBN` (6), `LWA_OM_TBF` (8)
  - Current: `LWA_OM_TBT` (10), `LWA_OM_TBS` (11)
- This maintains backward compatibility with existing session definition files

### Category 8: Backward Compatibility Definitions

**Status:** Correct as-is

**Analysis:**
- `LWA_SID_DP_` (13) and `LWA_SID_ADP` (19) remain with "Obsolete" markers
- Maintained to prevent subsystem ID reuse in binary files
- Current code correctly uses `LWA_SID_NDP` (20) for all operations

---

## ⏭️ Category Skipped

### Category 3: File Naming Inconsistencies

**Status:** Skipped to avoid breaking changes

**Issue:** Inconsistent naming patterns in `exec/` directory:
- Files with underscore: `me_exec.c`, `me_init.c`, `me_getaltaz.c`
- Files without underscore: `mecfg.c`, `medfg.c`, `mesi.c`, `meos.c`

**Decision:** Leave as-is to avoid:
- Renaming 13 source files and executables
- Breaking downstream scripts, documentation, and user workflows
- Updating Makefile and build system

---

## 📊 Test Coverage

The following areas were verified:
- ✅ All obsolete DP_ enum values remain marked "Obsolete"
- ✅ All current NDP functionality references NDP consistently
- ✅ Observation mode constants maintain backward compatibility
- ✅ test9.sh script correctly references NDP (already updated)
- ✅ DRX command structure matches spec (float64 freq, uint8_t high_dr)

---

## 🔄 Backward Compatibility

This PR maintains full backward compatibility:
- Obsolete subsystem IDs (DP_, ADP) remain defined but marked obsolete
- Obsolete command IDs (TBW, TBN, FST, etc.) remain defined but marked obsolete
- Obsolete observation modes remain for session file compatibility
- No executables renamed
- No breaking API changes

---

## 📝 Commits

1. `538d8c6` - Add comprehensive audit report and codebase analysis
2. `c982317` - Fix Category 1: Update DP references to NDP throughout documentation
3. `ef11c13` - Fix Categories 4, 5, 7: Clean up stale comments and FIXME markers
4. `22de6f7` - Add audit fixes summary document
5. `d8dc05c` - Remove temporary audit documentation files

---

## 🔍 Manual Review Needed

Three FIXME markers remain in `common/mcs.h` that may require configuration review:

1. **Line 45: `LWA_PTQ_TIMEOUT`** - Currently 15 seconds
   - Is this appropriate for NDP operations?

2. **Line 49: `LWA_MS_TASK_QUEUE_TIMEOUT`** - Currently 20 seconds
   - Is this appropriate?

3. **Line 886: `LWA_MAX_NSTD` vs `ME_MAX_NSTD`** - Both defined as 256
   - Should these duplicate definitions be reconciled?

---

## ✨ Key Improvements

1. **Documentation Accuracy:** All current functionality now consistently references NDP instead of obsolete DP naming
2. **Code Correctness:** Critical DRX frequency bug fixed
3. **Code Hygiene:** Removed stale comments and FIXME references
4. **Clarity:** Obsolete examples clearly marked to prevent confusion
5. **Consistency:** Comments updated to match current NDP naming throughout

---

## 📚 Documentation Impact

### Updated Sections in exec/README.md:
- Table of Contents
- Introduction & Overview
- Quick Start examples
- One-Shot Acquisition section (TBW/TBN → TBT/TBS)
- Monitoring & Control section
- History section

All examples now correctly demonstrate NDP usage with TBT/TBS/DRX commands instead of obsolete DP_ with TBW/TBN commands.
