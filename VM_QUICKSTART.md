# DOS Java VM Quick Start Guide

## Building the VM

### Compile djvm.exe

```batch
wmake djvm
```

This will:
1. Compile `src/vm/djvm.c` to `build/obj/djvm.obj`
2. Link with VM, format, and runtime libraries
3. Create `build/bin/djvm.exe`

### Build All Components

```batch
wmake all
```

This builds the complete toolchain:
- `djc.exe` - Compiler
- `djvm.exe` - Virtual Machine
- Test programs

## Running Programs

### Basic Execution

```batch
djvm hello.djc
```

Executes the `main` method in `hello.djc`.

### Specify Method

```batch
djvm hello.djc myMethod
```

Executes a specific method.

### Verbose Mode

```batch
djvm -v hello.djc
```

Shows detailed execution information:
- DJC file information
- Method details
- Execution status

### Debug Mode

```batch
djvm -d hello.djc
```

Traces execution step-by-step:
- Shows each instruction
- Displays PC (program counter)
- Shows stack and local variables

## Complete Workflow

### 1. Write Java Code

Create `hello.java`:
```java
public class Hello {
    public static void main() {
        System.out.println("Hello, DOS!");
    }
}
```

### 2. Compile to .djc

```batch
djc hello.java
```

Creates `hello.djc` bytecode file.

### 3. Run with VM

```batch
djvm hello.djc
```

Output:
```
Hello, DOS!
```

## Command-Line Options

### Options

- `-v, --verbose` - Verbose output (shows file info, method details)
- `-d, --debug` - Debug mode (trace each instruction)
- `-h, --help` - Show help message
- `--version` - Show version information

### Arguments

- `<file.djc>` - Bytecode file to execute (required)
- `[method]` - Method name to execute (default: "main")

## Examples

### Example 1: Hello World

```batch
djvm hello.djc
```

### Example 2: Arithmetic

```batch
djvm arithmetic.djc
```

### Example 3: Verbose Execution

```batch
djvm -v factorial.djc
```

Output:
```
DOS Java Virtual Machine v1.0
Loading: factorial.djc

DJC File Information:
  Magic:     0x444A
  Version:   0x0001
  Constants: 5
  Methods:   2
  Fields:    0
  Code size: 128 bytes

Executing method: main
  Code offset: 0
  Code length: 64
  Max stack:   4
  Max locals:  2

Execution completed successfully
```

### Example 4: Debug Mode

```batch
djvm -d hello.djc
```

Output:
```
DOS Java Virtual Machine v1.0
Loading: hello.djc

=== Debug Mode ===
[PC=0000] Opcode=0x02
[PC=0003] Opcode=0x0A
[PC=0004] Opcode=0x80
42
[PC=0005] Opcode=0x42

Execution completed successfully
```

## Error Messages

### File Not Found

```
ERROR: Failed to open file: hello.djc
```

**Solution:** Check that the .djc file exists in the current directory.

### Method Not Found

```
ERROR: Method not found: myMethod
```

**Solution:** Verify the method name is correct and exists in the .djc file.

### Invalid DJC File

```
ERROR: Failed to open file: hello.djc
```

**Solution:** Ensure the file is a valid .djc bytecode file compiled with djc.

### Execution Error

```
ERROR: Execution failed at PC=0042
```

**Solution:** Use debug mode (`-d`) to trace execution and identify the problem.

### Division by Zero

```
ERROR: Division by zero
```

**Solution:** Check your Java code for division operations with zero divisor.

### Stack Overflow

```
ERROR: Stack overflow
```

**Solution:** Reduce recursion depth or increase max_stack in the method.

## Supported Opcodes

### Stack Operations
- `PUSH_CONST` - Push constant from pool
- `PUSH_INT` - Push immediate integer
- `POP` - Pop and discard
- `DUP` - Duplicate top
- `LOAD_LOCAL` - Load local variable
- `STORE_LOCAL` - Store to local variable
- `LOAD_0/1/2` - Load local 0/1/2 (optimized)
- `STORE_0/1/2` - Store to local 0/1/2 (optimized)

### Arithmetic Operations
- `ADD` - Integer addition
- `SUB` - Integer subtraction
- `MUL` - Integer multiplication
- `DIV` - Integer division
- `MOD` - Integer modulo
- `NEG` - Negate
- `INC_LOCAL` - Increment local variable

### Comparison Operations
- `CMP_EQ` - Compare equal
- `CMP_NE` - Compare not equal
- `CMP_LT` - Compare less than
- `CMP_LE` - Compare less or equal
- `CMP_GT` - Compare greater than
- `CMP_GE` - Compare greater or equal

### Control Flow
- `GOTO` - Unconditional jump
- `IF_TRUE` - Jump if true
- `IF_FALSE` - Jump if false
- `IF_EQ` - Jump if equal
- `IF_NE` - Jump if not equal
- `IF_LT` - Jump if less than
- `IF_LE` - Jump if less or equal
- `IF_GT` - Jump if greater than
- `IF_GE` - Jump if greater or equal

### Method Invocation
- `RETURN` - Return void
- `RETURN_VALUE` - Return with value

### Special Operations
- `PRINT_INT` - Print integer (debug)
- `PRINT_CHAR` - Print character (debug)
- `HALT` - Halt execution

## Memory Limits

### DOS Small Memory Model
- Code segment: 64KB
- Data segment: 64KB
- Stack: ~4KB
- Heap: ~30KB

### VM Memory Usage
- Interpreter: ~8KB
- Runtime library: ~6KB
- DJC file: ~10KB
- Execution stack: ~4KB
- Total: ~28KB (within limits)

## Performance Tips

### 1. Use Optimized Opcodes
Use `LOAD_0`, `LOAD_1`, `LOAD_2` instead of `LOAD_LOCAL` for first three locals.

### 2. Minimize Stack Depth
Keep max_stack as small as possible to reduce memory usage.

### 3. Avoid Deep Recursion
DOS has limited stack space. Use iteration when possible.

### 4. Reduce Code Size
Smaller .djc files load faster and use less memory.

## Troubleshooting

### VM Crashes

**Problem:** djvm.exe crashes or hangs

**Solutions:**
1. Use debug mode to identify the problem instruction
2. Check for infinite loops in your code
3. Verify the .djc file is not corrupted
4. Ensure sufficient DOS memory is available

### Slow Execution

**Problem:** Program runs slowly

**Solutions:**
1. 8086 CPU is inherently slow
2. Reduce loop iterations
3. Optimize algorithm
4. Use faster DOS machine or emulator

### Out of Memory

**Problem:** Memory allocation fails

**Solutions:**
1. Reduce heap usage
2. Use smaller data structures
3. Free unused objects
4. Increase DOS memory (if possible)

## Next Steps

1. **Try Examples** - Run the example programs in `examples/`
2. **Write Programs** - Create your own Java programs
3. **Debug Issues** - Use debug mode to understand execution
4. **Optimize** - Profile and improve performance
5. **Extend** - Add new opcodes or features

## Additional Resources

- [VM Implementation Plan](VM_IMPLEMENTATION_PLAN.md) - Development roadmap
- [Architecture](ARCHITECTURE.md) - System design
- [Compiler Guide](COMPILER_QUICKSTART.md) - Using djc compiler
- [Technical Spec](TECHNICAL_SPEC.md) - Detailed specifications

## Support

For issues or questions:
1. Check error messages carefully
2. Use debug mode to trace execution
3. Review example programs
4. Consult documentation

---

**DOS Java VM v1.0** - 16-bit DOS Bytecode Interpreter