# Network Code Cleanup Plan

## Objective
Remove all mTCP and network-related code to reduce memory usage and simplify the codebase.

## Files to Delete

### Source Code
1. `src/network/socket.cpp` - mTCP socket implementation
2. `src/network/socket.h` - mTCP socket header
3. `src/runtime/socket.c` - Runtime socket wrapper
4. `src/runtime/socket.h` - Runtime socket header
5. `tools/sockhelp.c` - Socket helper v1
6. `tools/sockhelp_v2.c` - Socket helper v2

### Build Scripts
1. `build_mtcp_test.bat`
2. `build_netdiag.bat`
3. `build_sockhelp.bat`
4. `build_sockhelp_v2.bat`
5. `build_sockrt.bat`
6. `build_sendrecv.bat`
7. `build_memprof.bat`

### Documentation
1. `COMPILER_SOCKET_FIX.md`
2. `COMPILER_SOCKET_SUPPORT.md`
3. `PHASE4_1_COMPLETION_SUMMARY.md`
4. `PHASE4_2_DAY15_17_SUMMARY.md`
5. `PHASE4_2_DAY22_24_LARGE_MODEL_MIGRATION.md`
6. `PHASE4_2_DAY22_24_SUMMARY.md`
7. `PHASE4_2_SOCKET_HELPER_USAGE.md`
8. `PHASE4_2_SOCKET_STATUS.md`
9. `PHASE4_2_SOCKET_WORKAROUND_PLAN.md`
10. `PHASE4_CONFIGURATION_GUIDE.md`
11. `PHASE4_MEMORY_OPTIMIZATION.md`
12. `PHASE4_MTCP_PLAN.md`
13. `PHASE4_MTCP_QUICKSTART.md`
14. `PHASE4_MTCP_TASKS.md`
15. `PHASE4_NETWORK_PLAN.md`
16. `PHASE4_NETWORK_QUICKSTART.md`
17. `PHASE4_NETWORK_TASKS.md`
18. `PHASE4_PREPROCESSOR.md`
19. `PHASE4_SOCKET_API.md`
20. `PHASE4_USAGE_GUIDE.md`
21. `SOCKET_INIT_DEBUG_PLAN.md`
22. `SOCKET_INIT_DESCRIPTOR_FIX.md`
23. `SOCKET_INIT_PARAM_FIX.md`
24. `SOCKHLP2_TEST_PLAN.md`
25. `VM_PHASE3_PHASE4_PLAN.md`
26. `VM_PHASE3_PHASE4_REVIEW.md`
27. `VM_QUICKSTART_PHASE3_PHASE4.md`

### Keep (Debug cleanup documentation)
- `PHASE4_2_DEBUG_CLEANUP.md` - Documents debug output cleanup (not network-specific)

### Configuration Files
1. `dosjava.cfg` - mTCP configuration (if exists)
2. `*.map` files - sockhelp map files

### Test Files
1. `tests/sockinit.jav` (if exists)
2. `tests/sockconn.jav` (if exists)
3. `tests/socksend.jav` (if exists)
4. `tests/network/` directory (if exists)

## Makefile Changes

Remove from Makefile:
1. NETWORK_OBJS variable
2. MTCP_OBJS variable
3. RUNTIME_SOCKET_OBJS variable
4. sockhelp.exe target
5. sockhlp2.exe target
6. Any socket-related build rules

## Code Changes

### native.c
- Remove Socket.* native method implementations
- Keep File.* and other non-network methods

### Makefile
- Remove network/socket object files from all targets
- Remove mTCP library references
- Remove socket-related targets

## Verification

After cleanup:
1. Build with `build_all.bat`
2. Test basic functionality (arrays.jav, exc2.jav, exc3.jav, exc4.jav)
3. Verify memory usage reduction

## Expected Benefits

1. **Reduced Memory Usage**: No mTCP library overhead
2. **Simpler Codebase**: Easier to maintain
3. **Faster Builds**: Fewer files to compile
4. **Cleaner Documentation**: Focus on core functionality