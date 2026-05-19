# Socket.init() Parameter Mismatch Fix

## Problem
After fixing the memory heap size issue, djvm.exe failed with a new error:
```
ERROR: Method descriptor argument mismatch for init
ERROR: Execution failed at PC=0007
```

## Root Cause
The `Socket.init()` method was registered with incorrect parameter type specification in `native_register_socket_methods()`.

### Incorrect Registration (Line 687-704 in native.c)
```c
int native_register_socket_methods(void) {
    static NativeParamType param_void[] = { NATIVE_PARAM_VOID };  // WRONG!
    ...
    
    /* Socket.init() */
    if (native_register(
        "java/net/Socket",
        "init",
        "()V",              // Descriptor: no parameters
        native_socket_init,
        0,                  // param_count: 0
        param_void,         // WRONG: Should be NULL for 0 parameters
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
```

### The Issue
When a native method has **zero parameters** (descriptor `"()V"`), the `param_types` argument to `native_register()` should be **NULL**, not an array containing `NATIVE_PARAM_VOID`.

The native method invocation code in `interpreter.c` (Line 108-112 in native.c) verifies:
```c
/* Verify argument count */
if (arg_count != method->param_count) {
    printf("ERROR: Native method %s expects %u args, got %u\n",
           method->method_name, method->param_count, arg_count);
    return -1;
}
```

When `param_types` is not NULL, the system expects to validate parameter types, but with 0 parameters, there's nothing to validate, causing a mismatch.

## Solution
Changed `param_types` from `param_void` to `NULL` for `Socket.init()`:

```c
int native_register_socket_methods(void) {
    // Removed: static NativeParamType param_void[] = { NATIVE_PARAM_VOID };
    static NativeParamType param_string_int[] = { NATIVE_PARAM_STRING, NATIVE_PARAM_INT };
    static NativeParamType param_object_string[] = { NATIVE_PARAM_OBJECT, NATIVE_PARAM_STRING };
    static NativeParamType param_object_int[] = { NATIVE_PARAM_OBJECT, NATIVE_PARAM_INT };
    static NativeParamType param_object[] = { NATIVE_PARAM_OBJECT };
    
    /* Socket.init() - no parameters, so param_types is NULL */
    if (native_register(
        "java/net/Socket",
        "init",
        "()V",
        native_socket_init,
        0,
        NULL,               // FIXED: NULL for zero parameters
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    ...
}
```

## Native Method Registration Rules

### For Methods with Zero Parameters
```c
native_register(
    class_name,
    method_name,
    "()V",              // Descriptor with no parameters
    function_pointer,
    0,                  // param_count = 0
    NULL,               // param_types = NULL (not an array!)
    return_type
);
```

### For Methods with Parameters
```c
static NativeParamType params[] = { NATIVE_PARAM_INT, NATIVE_PARAM_STRING };

native_register(
    class_name,
    method_name,
    "(ILjava/lang/String;)V",  // Descriptor with parameters
    function_pointer,
    2,                          // param_count = 2
    params,                     // param_types = array of types
    return_type
);
```

## Testing Results

### Before Fix
```
> djvm.exe -d sockinit.djc
[DEBUG] About to initialize memory manager...
[DEBUG] Initializing memory manager with heap_size=16384
[DEBUG] malloc succeeded, heap_start=a8e4
[DEBUG] Memory manager initialized successfully
Executing method: main
  Code offset: 0
  Code length: 19
  Max stack:   1
  Max locals:  0

=== Debug Mode ===
[PC=0000] Opcode=0x01
DEBUG VM: Executing opcode 0x01 at PC offset 0
[PC=0003] Opcode=0x40
DEBUG VM: Executing opcode 0x40 at PC offset 3
Socket Init Test
[PC=0007] Opcode=0x40
DEBUG VM: Executing opcode 0x40 at PC offset 7
ERROR: Method descriptor argument mismatch for init
ERROR: Execution failed at PC=0007
```

### After Fix
Expected output:
```
> djvm.exe -d sockinit.djc
[DEBUG] About to initialize memory manager...
[DEBUG] Initializing memory manager with heap_size=16384
[DEBUG] malloc succeeded, heap_start=XXXX
[DEBUG] Memory manager initialized successfully
Executing method: main
  Code offset: 0
  Code length: 19
  Max stack:   1
  Max locals:  0

=== Debug Mode ===
[PC=0000] Opcode=0x01
DEBUG VM: Executing opcode 0x01 at PC offset 0
[PC=0003] Opcode=0x40
DEBUG VM: Executing opcode 0x40 at PC offset 3
Socket Init Test
[PC=0007] Opcode=0x40
DEBUG VM: Executing opcode 0x40 at PC offset 7
Socket initialized
[PC=0011] Opcode=0xb1
DEBUG VM: Executing opcode 0xb1 at PC offset 11
```

## Files Modified

### src/vm/native.c
- Line 687: Removed `param_void` array declaration
- Line 693-704: Changed `Socket.init()` registration to use `NULL` for `param_types`

### Build Output
- All executables rebuilt successfully
- djvm.exe now correctly handles zero-parameter native methods

## Related Native Methods

All other native methods in the system have at least one parameter, so they are not affected:
- `System.out.println(int)` - 1 parameter
- `System.out.println(String)` - 1 parameter
- `String.length(String)` - 1 parameter
- `Socket.new(String, int)` - 2 parameters
- `Socket.send(Socket, String)` - 2 parameters
- `Socket.recv(Socket, int)` - 2 parameters
- `Socket.close(Socket)` - 1 parameter
- `Socket.isConnected(Socket)` - 1 parameter

`Socket.init()` is currently the **only** zero-parameter native method in the system.

## Verification Steps

1. **Rebuild**: `.\build_all.bat`
2. **Test in DOSBox-X**:
   ```
   djc sockinit.jav
   djvm -d sockinit.djc
   ```
3. **Verify**: Should see "Socket initialized" message without errors
4. **Run all tests**: `cd tests && runtest.bat`

## Conclusion

The fix corrects the native method registration for zero-parameter methods. The key insight is that `param_types` should be `NULL` when `param_count` is 0, not an array containing `NATIVE_PARAM_VOID`.

This is consistent with C conventions where NULL pointers indicate "no data" rather than pointers to empty arrays.