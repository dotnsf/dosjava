# DOS Java - Lightweight Java for 16-bit PC-DOS

A minimal Java runtime environment designed to run on 16-bit PC-DOS systems, compiled with Open Watcom v2 C compiler.

## Overview

DOS Java is a lightweight implementation of a Java subset that can execute on 16-bit DOS systems with limited memory (640KB). It features:

- **Custom bytecode format** (.djc) optimized for 16-bit architecture
- **Minimal java.lang package** (Object, String, System, Integer, Boolean)
- **Small memory footprint** (Small memory model: 64KB code + 64KB data)
- **Reference counting GC** suitable for constrained environments
- **Preprocessor tool** to convert standard .class files to .djc format

## System Requirements

### Target System (DOS)
- **OS**: PC-DOS, MS-DOS 3.0 or higher
- **CPU**: 8086/8088 or higher
- **Memory**: 640KB RAM (minimum 256KB recommended)
- **Storage**: Floppy disk or hard drive

### Development System
- **Compiler**: Open Watcom v2 C compiler
- **Java**: JDK 8 or higher (for compiling test programs)
- **OS**: Windows, Linux, or DOS
- **Emulator** (optional): DOSBox, QEMU, or VirtualBox

## Quick Start

### 1. Write a Java Program

```java
// HelloWorld.java
class HelloWorld {
    public static void main() {
        System.out.println("Hello, DOS!");
    }
}
```

### 2. Compile with javac

```bash
javac HelloWorld.java
```

### 3. Convert to .djc Format

```bash
djpreproc HelloWorld.class HelloWorld.djc
```

### 4. Run on DOS

```bash
dosjava HelloWorld.djc
```

Output:
```
Hello, DOS!
```

## Project Structure

```
dosjava/
├── README.md                    # This file
├── TECHNICAL_SPEC.md           # Detailed technical specification
├── PROJECT_STRUCTURE.md        # Project organization
├── IMPLEMENTATION_ROADMAP.md   # Step-by-step implementation guide
├── src/                        # VM source code
├── preprocessor/               # .class to .djc converter
├── tests/                      # Test programs
├── examples/                   # Example programs
├── docs/                       # Additional documentation
└── Makefile                    # Build system
```

## Supported Java Features

### Data Types
- **Primitives**: `int` (16-bit), `boolean`, `char`
- **Objects**: `Object`, `String`, `Integer`, `Boolean`
- **Arrays**: Basic array support (limited)

### Language Constructs
- Classes (single inheritance)
- Methods (static and instance)
- Fields (static and instance)
- Control flow: `if/else`, `while`, `for`
- Operators: arithmetic, comparison, logical

### Standard Library
- `java.lang.Object` - Base class
- `java.lang.String` - String operations
- `java.lang.System` - System.out.print/println
- `java.lang.Integer` - Integer wrapper
- `java.lang.Boolean` - Boolean wrapper

## NOT Supported

- `long`, `float`, `double` types
- Interfaces and abstract classes
- Exception handling (try/catch)
- Multithreading
- Reflection
- Inner classes
- Generics
- Annotations

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    DOS Java System                       │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  ┌──────────────┐         ┌──────────────────────────┐  │
│  │ Preprocessor │────────>│  Lightweight Bytecode    │  │
│  │ (.class →    │         │  Format (.djc)           │  │
│  │  .djc)       │         └──────────────────────────┘  │
│  └──────────────┘                    │                   │
│                                      ▼                   │
│  ┌───────────────────────────────────────────────────┐  │
│  │           DOS Java Virtual Machine                │  │
│  ├───────────────────────────────────────────────────┤  │
│  │  Class Loader  │  Bytecode Interpreter            │  │
│  │  Memory Mgr    │  Method Invocation               │  │
│  │  GC (Simple)   │  Stack Management                │  │
│  └───────────────────────────────────────────────────┘  │
│                          │                               │
│                          ▼                               │
│  ┌───────────────────────────────────────────────────┐  │
│  │        Minimal java.lang Package (C impl)         │  │
│  │  Object, String, System, Integer, Boolean, etc.   │  │
│  └───────────────────────────────────────────────────┘  │
│                                                           │
└─────────────────────────────────────────────────────────┘
```

## Building from Source

### Prerequisites

1. Install Open Watcom v2:
   - Download from: https://github.com/open-watcom/open-watcom-v2
   - Add to PATH

2. Verify installation:
   ```bash
   wcc -?
   ```

### Build VM

```bash
# On DOS or with Open Watcom cross-compiler
cd dosjava
wmake
```

This produces:
- `dosjava.exe` - DOS Java VM
- `djpreproc.exe` - Preprocessor tool

### Build Preprocessor (on host system)

```bash
cd preprocessor
make
```

## Usage

### Basic Usage

```bash
dosjava <program.djc>
```

### Preprocessor Usage

```bash
djpreproc <input.class> [-o <output.djc>]
```

Options:
- `-o <file>` - Specify output file (default: same name with .djc extension)
- `-v` - Verbose output
- `-d` - Debug mode (show translation details)

### Examples

```bash
# Convert and run
djpreproc HelloWorld.class
dosjava HelloWorld.djc

# With custom output name
djpreproc MyApp.class -o app.djc
dosjava app.djc

# Verbose mode
djpreproc Complex.class -v -o complex.djc
```

## Example Programs

### Hello World

```java
class HelloWorld {
    public static void main() {
        System.out.println("Hello, DOS!");
    }
}
```

### Arithmetic

```java
class Calculator {
    public static void main() {
        int a = 10;
        int b = 20;
        int sum = a + b;
        int product = a * b;
        
        System.out.print("Sum: ");
        System.out.println(sum);
        System.out.print("Product: ");
        System.out.println(product);
    }
}
```

### Loops

```java
class Counter {
    public static void main() {
        int i = 1;
        while (i <= 10) {
            System.out.println(i);
            i = i + 1;
        }
    }
}
```

### Objects

```java
class Point {
    int x;
    int y;
    
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
    
    public int getX() {
        return this.x;
    }
    
    public int getY() {
        return this.y;
    }
}

class TestPoint {
    public static void main() {
        Point p = new Point(10, 20);
        System.out.println(p.getX());
        System.out.println(p.getY());
    }
}
```

## Memory Constraints

### Small Memory Model
- **Code Segment**: 64KB maximum
- **Data Segment**: 64KB maximum
- **Heap**: ~10-14KB for objects
- **Stack**: ~20KB for method calls

### Recommendations
- Keep classes small and focused
- Minimize object creation
- Reuse objects when possible
- Avoid deep recursion
- Use primitives instead of wrappers when possible

## Performance Tips

1. **Minimize Object Creation**
   - Objects are expensive in limited memory
   - Reuse objects when possible

2. **Use Static Methods**
   - Static methods have less overhead
   - No object allocation needed

3. **Avoid String Concatenation in Loops**
   - Each concatenation creates a new string
   - Build strings outside loops when possible

4. **Keep Methods Small**
   - Smaller methods use less stack space
   - Easier for the interpreter to optimize

## Troubleshooting

### "Out of memory" Error
- Reduce object creation
- Simplify data structures
- Consider breaking program into smaller parts

### "Class not found" Error
- Verify .djc file exists
- Check file path
- Ensure preprocessor completed successfully

### "Invalid bytecode" Error
- Recompile Java source
- Re-run preprocessor
- Check for unsupported Java features

### Program Crashes
- Check for stack overflow (deep recursion)
- Verify array bounds
- Enable debug mode for more information

## Development Status

This is an educational/experimental project. Current status:

- [x] Technical specification complete
- [x] Architecture designed
- [x] Implementation roadmap created
- [ ] Core VM implementation
- [ ] Runtime library implementation
- [ ] Preprocessor implementation
- [ ] Testing and optimization
- [ ] Documentation completion

## Contributing

This project is designed as a learning exercise for understanding:
- Virtual machine implementation
- Bytecode interpretation
- Memory management in constrained environments
- 16-bit DOS programming
- Cross-compilation with Open Watcom

Contributions, suggestions, and improvements are welcome!

## License

[To be determined - suggest MIT or similar permissive license]

## References

- [Java Virtual Machine Specification](https://docs.oracle.com/javase/specs/jvms/se8/html/)
- [Open Watcom v2 Documentation](https://github.com/open-watcom/open-watcom-v2/wiki)
- [DOS Programming Reference](http://www.ctyme.com/intr/int.htm)
- [8086 Assembly Language](https://en.wikipedia.org/wiki/X86_assembly_language)

## Acknowledgments

Inspired by:
- Classic Java implementations (Java 1.0/1.1)
- Embedded Java (Java ME/J2ME)
- Retro computing enthusiasts
- DOS programming community

## Contact

[Project maintainer information to be added]

---

**Note**: This is a minimal Java implementation designed for educational purposes and retro computing. It is not intended to be a full Java runtime or to run production applications.