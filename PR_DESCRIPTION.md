# Fix NDP Command Handling Inconsistencies

## Summary

This PR fixes inconsistencies in NDP command parameter handling between `me_inproc.c` (which generates command strings) and `mesi.c` (which parses them into binary format). These mismatches could cause data corruption, incorrect command execution, or runtime failures.

## Issues Fixed

### 1. DRX Command - Format Specifier Mismatches (me_inproc.c lines 1171, 1199, 1461, 1490)

**Problem**: Parameters sent with incorrect types/format specifiers

**Changes**:
- `beam`: Changed from `%hd` (signed short) to `%hhu` (unsigned char)
- `bw`: Changed from `%hu` (unsigned short) to `%hhu` (unsigned char)
- `gain`: Changed from `%hd` (signed short) to `%hu` (unsigned short)
- `high_dr`: Changed from `%ld` (long) to `%hhu` (unsigned char)
- `subslot`: Changed from `%ld` (long) to `%hhu` (unsigned char)

**Impact**: Prevents potential buffer overflows and ensures values fit within the expected data types defined by the NDP ICD.

### 2. TBT Command - Trigger Time Type Mismatch (me_inproc.c line 1100)

**Problem**: First parameter sent as `%ld` (long) but parsed as `%u` (unsigned int)

**Changes**:
- Changed from `%ld` to `%u` for TBT_TRIG_TIME parameter

**Impact**: Ensures cross-platform compatibility (long vs int can differ on 32-bit vs 64-bit systems).

### 3. TBS Command - Bandwidth Type Mismatch (me_inproc.c line 1114)

**Problem**: Bandwidth sent as `%hu` (unsigned short) but parsed as `%hhu` (unsigned char)

**Changes**:
- Changed from `%hu` to `%hhu` for TBS_BW parameter

**Impact**: Prevents potential truncation and ensures values fit within uint8 range (0-8).

### 4. Documentation Corrections

**Array Size Documentation** (mesi.c, medfg.c, megfg.c, README.md):
- Fixed `BEAM_DELAY` array size: 520 → 512
- Fixed `BEAM_GAIN` array size: 260 → 256

**BAM Command Documentation** (mesi.c):
- Removed incorrect `drx_tuning` parameter from documentation (not part of BAM command)

**DRX Command Documentation** (mesi.c):
- Added missing `beam` parameter to documentation
- Added missing `subslot` parameter to documentation

## Testing Recommendations

1. Test DRX command with various beam configurations
2. Test TBT command with trigger times near slot boundaries
3. Test TBS command with all bandwidth settings (0-8)
4. Verify no buffer overflows with boundary values
5. Test on both 32-bit and 64-bit platforms if available

## Client Compatibility

All changes align with the NDP client implementation (`Ndp.py`) which expects:
- DRX: beam, tuning, frequency, filter, gain, high_dr, subslot
- TBT: trigger (int), samples (int), mask (long)
- TBS: frequency (double), filter (byte with range 0-8)
- BAM: beam, 512 delays, 256×2×2 gains, subslot

## Notes

- The COR command implementation is intentionally left as-is (not currently used in me_inproc.c)
- Frequency precision for DRX/TBS commands (%12.3f, %8.0f) is acceptable for expected use cases
