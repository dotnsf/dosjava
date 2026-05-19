# Socket.init() Descriptor Generation Fix

## Problem
After fixing the native method registration, djvm.exe still failed with:
```
[DEBUG] Method: init, Descriptor: (I)V, Expected args: 1, Actual args: 0
ERROR: Method descriptor argument mismatch for init
```

The compiler (djc.exe) was generating an incorrect descriptor `"(I)V"` (one int parameter) instead of `"()V"` (no parameters) for `Socket.init()`.

## Root Cause
In `tools/compiler/codegen.c`, the method descriptor generation code (Lines 2464-2509) had no specific case for `Socket.init()`, causing it to fall through to the default `else` block:

```c
} else if (strcmp(method_name, "close") == 0) {
    strcpy(descriptor, "()V");
} else if (arg_node_type == NODE_LITERAL_STRING || first_arg_is_string) {
    strcpy(descriptor, "(Ljava/lang/String;)V");
} else {
    strcpy(descriptor, "(I)V");  // ← DEFAULT: Assumes int parameter!
}
```

Since `Socket.init()` has no arguments:
- `arg_node_type` is not `NODE_LITERAL_STRING`
- `first_arg_is_string` is false
- Falls through to `else` → generates `"(I)V"`

## Solution
Added explicit case for `Socket.init()` before the default cases:

```c
} else if (strcmp(method_name, "close") == 0) {
    strcpy(descriptor, "()V");
} else if (strcmp(method_name, "init") == 0) {
    /* Socket.init() has no parameters */
    strcpy(descriptor, "()V");
} else if (arg_node_type == NODE_LITERAL_STRING || first_arg_is_string) {
    strcpy(descriptor, "(Ljava/lang/String;)V");
} else {
    strcpy(descriptor, "(I)V");
}
```

## Files Modified

### tools/compiler/codegen.c
- **Line 2503-2504**: Added condition for `Socket.init()` to generate `"()V"` descriptor
- **Location**: In `generate_call()` function, within the native method descriptor generation block

## Testing Results

### Before Fix
```
> djc sockinit.jav
Compiled: sockinit.jav -> sockinit.djc

> djvm -d sockinit.djc
[DEBUG] Method: init, Descriptor: (I)V, Expected args: 1, Actual args: 0
ERROR: Method descriptor argument mismatch for init
```

### After Fix
Expected output:
```
> djc sockinit.jav
Compiled: sockinit.jav -> sockinit.djc

> djvm -d sockinit.djc
[DEBUG] Method: init, Descriptor: ()V, Expected args: 0, Actual args: 0
Socket Init Test
Socket initialized
```

## Related Issues Fixed

This fix completes the Socket.init() implementation chain:

1. **Semantic Analysis** (semantic.c): ✓ Correctly registers `Socket.init()` with 0 parameters
2. **Code Generation** (codegen.c): ✓ Now generates correct descriptor `"()V"`
3. **Native Registration** (native.c): ✓ Registers with `param_count=0`, `param_types=NULL`
4. **VM Execution** (interpreter.c): ✓ Validates descriptor matches argument count

## Descriptor Generation Logic

The descriptor generation code handles various native methods:

### Zero-Parameter Methods
```c
if (strcmp(method_name, "readLine") == 0) {
    strcpy(descriptor, "()Ljava/lang/String;");  // Returns String
} else if (strcmp(method_name, "close") == 0) {
    strcpy(descriptor, "()V");                    // Returns void
} else if (strcmp(method_name, "init") == 0) {
    strcpy(descriptor, "()V");                    // Returns void (NEW)
}
```

### One-Parameter Methods
```c
if (strcmp(method_name, "writeLine") == 0) {
    strcpy(descriptor, "(Ljava/lang/String;)V");  // String parameter
} else if (arg_node_type == NODE_LITERAL_STRING || first_arg_is_string) {
    strcpy(descriptor, "(Ljava/lang/String;)V");  // Generic String parameter
} else {
    strcpy(descriptor, "(I)V");                    // Default: int parameter
}
```

### Multi-Parameter Methods
```c
if (strcmp(method_name, "open") == 0) {
    if (arg_count == 1) {
        strcpy(descriptor, "(Ljava/lang/String;)V");
    } else {
        strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)V");
    }
}
```

## Why This Matters

The descriptor is critical for:
1. **Method Lookup**: VM uses descriptor to find the correct native method
2. **Argument Validation**: VM checks descriptor matches actual arguments on stack
3. **Type Safety**: Ensures correct parameter types are passed

Without the correct descriptor, the VM cannot:
- Find the registered native method
- Validate the call is correct
- Execute the method safely

## Build Verification

```
C:\Users\dotns\src\dosjava> .\build_all.bat
...
Compiling codegen.c...
Linking djc.exe...
...
Exit code: 0
```

All executables rebuilt successfully:
- `build/bin/djc.exe` - Compiler with fixed descriptor generation
- `build/bin/djvm.exe` - VM (unchanged, but will now work with correct bytecode)

## Next Steps

1. **Copy to DOSBox-X**: Copy updated `djc.exe` to DOSBox-X environment
2. **Recompile Test**: `djc sockinit.jav` to generate correct bytecode
3. **Run Test**: `djvm -d sockinit.djc` to verify execution
4. **Verify Output**: Should see "Socket initialized" without errors

## Lessons Learned

### Problem: Default Fallback Assumptions
The code assumed all unhandled native methods take an int parameter. This is dangerous because:
- New methods with different signatures fail silently
- Error only appears at runtime, not compile time
- Debugging requires understanding the entire call chain

### Solution: Explicit Cases
Always add explicit cases for new native methods:
```c
} else if (strcmp(method_name, "newMethod") == 0) {
    strcpy(descriptor, "appropriate_descriptor");
}
```

### Better Approach: Descriptor from Symbol Table
Future improvement: Generate descriptor from method symbol instead of hardcoding:
```c
Symbol* method_sym = find_method_symbol(codegen, method_name);
if (method_sym) {
    generate_descriptor_from_symbol(method_sym, descriptor);
}
```

This would:
- Eliminate hardcoded descriptors
- Automatically handle new methods
- Reduce maintenance burden
- Prevent this class of bugs

## Conclusion

The fix adds a single condition to handle `Socket.init()`'s zero-parameter signature. This completes the implementation chain from semantic analysis through code generation to VM execution, allowing Socket functionality to work correctly.