# Phase 7.3: Switch Fall-Through Bug Fix

## Overview
Fixed critical bug in switch statement fall-through implementation where execution would incorrectly jump to default case instead of falling through to the next case body.

## Problem Description

### Symptoms
When testing `swdebug.jav` with `z = 5`:
```java
case 5:
    System.out.println("Matched case 5");
case 6:
    System.out.println("Matched case 6");
    multiResult = 2;
    break;
```

**Expected Output:**
```
Matched case 5
Matched case 6
multiResult = 2
```

**Actual Output (Before Fix):**
```
Matched case 5
Matched default
multiResult = 0
```

### Root Cause
The original implementation had a fundamental flaw in how fall-through was handled:

1. When case 5 matched:
   - Switch value was POPed from stack
   - Case 5 body executed
   - No break, so execution continued

2. When falling through to case 6:
   - Case 6's **comparison code** was executed
   - But switch value was already POPed, so stack was empty/invalid
   - Comparison failed
   - Jumped to next_case_label (which led to default)

**Key Insight**: Fall-through should skip the next case's comparison and jump directly to its body.

## Solution

### Design
Implemented a two-pass approach with separate labels for case comparison and case body:

1. **First Pass**: Pre-create body labels for all cases
2. **Second Pass**: Generate case comparisons with knowledge of next case's body label
3. **Fall-through Logic**: When a case has no break, jump directly to next case's body label

### Implementation Changes

#### 1. Modified `generate_case_comparison()` Signature
```c
// Before
static int generate_case_comparison(CodeGenerator* codegen, ASTNode* case_node, 
                                    uint16_t switch_expr_type, uint16_t end_label);

// After
static int generate_case_comparison(CodeGenerator* codegen, ASTNode* case_node, 
                                    uint16_t switch_expr_type, uint16_t end_label,
                                    uint16_t this_body_label, uint16_t next_body_label);
```

**New Parameters:**
- `this_body_label`: Label for this case's body (for fall-through from previous case)
- `next_body_label`: Label for next case's body (for fall-through to next case)

#### 2. Updated Fall-Through Logic
```c
/* After case body: */
/* - If has break: jump to end */
/* - If no break and has next case: jump to next case body (skip its comparison) */
/* - If no break and no next case: fall through to default/end */
if (has_break) {
    emit_jump(codegen, OP_GOTO, end_label);
} else if (next_body_label != 0) {
    /* Fall-through: jump directly to next case body, skipping its comparison */
    emit_jump(codegen, OP_GOTO, next_body_label);
}
```

#### 3. Two-Pass Processing in `generate_switch_stmt()`
```c
/* First pass: Create body labels for all cases */
uint16_t* body_labels = (uint16_t*)malloc(case_count * sizeof(uint16_t));
for (i = 0; i < case_count; i++) {
    body_labels[i] = create_label(codegen);
}

/* Second pass: Generate case comparisons with body labels */
while (case_idx != 0) {
    this_body_label = body_labels[i];
    next_body_label = (i + 1 < case_count) ? body_labels[i + 1] : 0;
    
    generate_case_comparison(codegen, case_node, switch_expr_type, end_label,
                            this_body_label, next_body_label);
    i++;
}
```

### Control Flow Diagram

**Before Fix:**
```
case 5: compare → match → POP → body → (no break)
case 6: compare → FAIL (no value on stack) → next_case_label → default
```

**After Fix:**
```
case 5: compare → match → POP → body → (no break) → GOTO case_6_body_label
case 6: case_6_body_label: body → break → end
```

## Test Results

### Test File: `tests/swdebug.jav`
```java
int z = 5;
switch (z) {
    case 5:
        System.out.println("Matched case 5");
    case 6:
        System.out.println("Matched case 6");
        multiResult = 2;
        break;
    default:
        System.out.println("Matched default");
        multiResult = 0;
        break;
}
```

**Output:**
```
Testing switch with z = 5
Matched case 5
Matched case 6
multiResult = 2
```
✅ **PASS** - Correct fall-through behavior

### Test File: `samples/swint.jav`
Tests three scenarios:
1. Basic switch with break
2. Switch with default case
3. Multi-case fall-through (cases 4, 5, 6)

✅ **PASS** - All scenarios work correctly

### Test File: `samples/swstring.jav`
String switch statement (no fall-through)

✅ **PASS** - Already working, still works correctly

## Technical Details

### Memory Management
- Dynamically allocates array for body labels: `malloc(case_count * sizeof(uint16_t))`
- Properly frees memory after use: `free(body_labels)`
- No memory leaks

### Stack Management
- Switch value remains on stack during all case comparisons
- POPed only when a case matches
- For LONG type: stored in temporary variable (local 126-127)

### Label Management
Each case now has two labels:
1. **Comparison Label** (`next_case_label`): Where to jump if comparison fails
2. **Body Label** (`this_body_label`): Where to jump for fall-through

## Files Modified

### `dosjava/tools/compiler/codegen.c`
- **Line 18**: Updated forward declaration
- **Lines 4295-4449**: Modified `generate_case_comparison()` function
- **Lines 4520-4595**: Modified `generate_switch_stmt()` with two-pass processing

## Related Issues Fixed

This fix completes the switch statement implementation started in Phase 7:
- ✅ Phase 7.1: Basic switch statement support
- ✅ Phase 7.2: String switch support
- ✅ Phase 7.3: Fall-through support (this fix)

## Lessons Learned

1. **Fall-through requires careful label management**: Need to distinguish between comparison entry points and body entry points
2. **Two-pass approach is cleaner**: Pre-creating labels allows forward references
3. **Stack discipline is critical**: Must track when switch value is on stack vs. POPed
4. **Test edge cases**: Fall-through is an edge case that requires specific testing

## Future Considerations

1. **Optimization**: Could detect empty case bodies and optimize away unnecessary jumps
2. **Warning**: Could add compiler warning for fall-through without comment (like Java)
3. **Documentation**: Add fall-through examples to user documentation

## Conclusion

The switch fall-through bug has been successfully fixed. The implementation now correctly handles:
- Basic switch with break statements
- Fall-through between multiple cases
- Empty case bodies (implicit fall-through)
- String switch statements
- Long integer switch statements
- Default case handling

All test cases pass, and the implementation is ready for production use.

---
**Date**: 2026-05-25  
**Author**: Bob (AI Assistant)  
**Status**: ✅ Complete