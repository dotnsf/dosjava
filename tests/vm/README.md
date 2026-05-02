# VM Method Invocation Tests

This directory contains test programs for verifying the method invocation functionality of the DOS Java VM.

## Test Programs

### test_method_call.java
Tests basic method invocation with no parameters and no return value.
- Calls a helper method from main
- Verifies call frame management
- Tests INVOKE_STATIC and RETURN opcodes

### test_method_return.java
Tests method invocation with return value.
- Calls a method that returns an integer
- Verifies return value handling
- Tests INVOKE_STATIC and RETURN_VALUE opcodes

## Running Tests

### In DOSBox or DOS Environment

1. Navigate to this directory:
   ```
   cd \dosjava\tests\vm
   ```

2. Run the test script:
   ```
   run_method_tests.bat
   ```

### Manual Testing

1. Compile a test program:
   ```
   ..\..\build\bin\djc.exe test_method_call.java
   ```

2. Run the compiled bytecode:
   ```
   ..\..\build\bin\djvm.exe test_method_call.djc
   ```

3. For debug output:
   ```
   ..\..\build\bin\djvm.exe -d test_method_call.djc
   ```

## Expected Results

Both test programs should:
- Compile without errors
- Execute without errors
- Complete successfully with exit code 0

## Implementation Status

✅ **Phase 3.1 Complete**: Method Invocation
- Shared stack design (4KB stack, 256 bytes locals)
- INVOKE_STATIC opcode implementation
- Enhanced RETURN and RETURN_VALUE opcodes
- Call frame management (max 4 levels)
- djc_find_method helper function

## Notes

- These are DOS 16-bit executables and require a DOS-compatible environment
- Windows 11 PowerShell cannot directly execute DOS programs
- Use DOSBox, FreeDOS, or a DOS emulator to run the tests
- The VM uses a shared stack design to minimize memory usage in the DOS environment

## Next Steps

- Phase 4.1: Implement native method support (System.out.println)
- Add more complex test cases (nested calls, multiple parameters)
- Test call depth limits
- Verify stack and local variable management