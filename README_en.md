# DOS Java Compiler

![DOSJava](https://raw.githubusercontent.com/dotnsf/dosjava/refs/heads/main/dosjava.png)

Java compiler and runtime environment for 16-bit PC-DOS

## Overview

DOS Java Compiler is a complete toolchain for compiling and executing a subset of Java in 16-bit PC-DOS environments. Built with Open Watcom v2 C compiler and runs in Large memory model (multi-segment support).

## Key Features

- **Integrated Compiler (djc.exe)**: One-step compilation from Java source to .djc bytecode
- **Virtual Machine (djvm.exe)**: Stack-based bytecode interpreter
- **Runtime Library**: Basic Java classes (Object, String, System, Integer)
- **Toolchain**: .djc generation and .class conversion tools

## Supported Java Subset

### Data Types
- `int` - 16-bit signed integer (-32768 to 32767)
  - Array indices must be `int` type only
- `long` - 32-bit signed integer (added in Phase 5)
  - Literals: `123L` or `123l`
  - Operations: `+`, `-`, `*`, `/`, `%`
  - Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
  - `System.out.println(long)` supported
- `float` - 32-bit floating point (added in Phase 6.1)
  - Literals: `3.14f` or `3.14F`
  - Operations: `+`, `-`, `*`, `/`, `%`
  - Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
  - `System.out.println(float)` supported
  - IEEE 754 format
- `boolean` - Boolean value
  - Literals: `true`, `false` (added in Phase 8)
  - Logical operations: `&&`, `||`, `!`
- `void` - No return value
- `null` - Null literal (added in Phase 9.3)
  - Can be assigned to reference types (String, arrays)
  - Comparison operations with reference types (`==`, `!=`)
- `String` - String type
  - String literals
  - `String` local variables
  - `System.out.println(String)`
  - `str.length()` / `"abc".length()`
  - `String + String` concatenation
  - Chained concatenation (e.g., `a + b + "56"`)
  - **Methods added in Phase 13**:
    - `String charAt(int index)` - Returns character at specified position (as String)
    - `int isEmpty()` - Check if string is empty (1=true, 0=false)
    - `String trim()` - Remove leading and trailing whitespace
    - `String replace(String target, String replacement)` - String replacement
    - `int compareTo(String other)` - Lexicographic comparison
    - `int lastIndexOf(String str)` - Last occurrence position
    - `int contains(String str)` - Substring check (1=true, 0=false)
    - `String repeat(int count)` - String repetition

### Control Flow
- `if` / `else`
- `while`
- `for`
- `return`
- `switch` / `case` / `default` / `break` (added in Phase 7)
  - Switch statements for INT, LONG, and STRING types
  - Multiple case statements
  - Fall-through behavior (added in Phase 7.3)
  - Default case (at any position)
  - Nested switch statements

### Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`
- **Relational**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical**: `&&`, `||`, `!`
- **Assignment**: `=`
- **Type Cast**: `(type)expression` (added in Phase 9.1)
  - Mutual conversion between int ↔ long ↔ float
  - Examples: `(int)3.7f`, `(float)42`, `(long)999`

### Classes and Methods
- Class declaration (one class per file)
- `public static` methods
- Static methods with `int` parameters
- `int` / `void` return values
- Local variables
  - Initialization at declaration supported (added in Phase 10.1)
  - Examples: `int a = 42;`, `String s = "Hello";`, `float f = 3.14f;`
- Static method calls

### Currently Available Array Features
- `int[]` - Integer arrays
- `long[]` - Long integer arrays (added in Phase 5.4)
- `float[]` - Floating point arrays (added in Phase 6.2)
- `boolean[]` - Boolean arrays
- Array element access
- Array element assignment
- `array.length`

### Math Class (added in Phase 6.3, integer argument support in Phase 9.2)
Class providing mathematical functions

**Phase 9.2 Enhancement**: All Math functions now accept int/long arguments.
Integer arguments are automatically converted to float.

#### Basic Functions
- `float Math.abs(float x)` - Absolute value
  - `Math.abs(int)`, `Math.abs(long)` also available
- `float Math.min(float a, float b)` - Minimum value
  - `Math.min(int, int)`, `Math.min(long, long)` also available
- `float Math.max(float a, float b)` - Maximum value
  - `Math.max(int, int)`, `Math.max(long, long)` also available
- `float Math.sqrt(float x)` - Square root
  - `Math.sqrt(int)`, `Math.sqrt(long)` also available
- `float Math.floor(float x)` - Floor function (added in Phase 6.3)
  - `Math.floor(int)`, `Math.floor(long)` also available
- `float Math.ceil(float x)` - Ceiling function (added in Phase 6.3)
  - `Math.ceil(int)`, `Math.ceil(long)` also available

#### Trigonometric Functions
- `float Math.sin(float x)` - Sine (radians)
  - `Math.sin(int)`, `Math.sin(long)` also available
- `float Math.cos(float x)` - Cosine (radians)
  - `Math.cos(int)`, `Math.cos(long)` also available
- `float Math.tan(float x)` - Tangent (radians)
  - `Math.tan(int)`, `Math.tan(long)` also available

#### Exponential and Logarithmic Functions
- `float Math.pow(float base, float exp)` - Power
  - `Math.pow(int, int)`, `Math.pow(long, long)` also available
- `float Math.exp(float x)` - Natural exponential function (e^x)
  - `Math.exp(int)`, `Math.exp(long)` also available
- `float Math.log(float x)` - Natural logarithm (ln(x))
  - `Math.log(int)`, `Math.log(long)` also available

#### Usage Example
```java
class MathDemo {
    public static void main() {
        float x = 3.0f;
        float y = 4.0f;
        
        // Pythagorean theorem: c = √(x² + y²)
        float c = Math.sqrt(Math.pow(x, 2.0f) + Math.pow(y, 2.0f));
        System.out.println(c);  // 5.00
        
        // Circle area: A = π × r²
        float pi = 3.14f;
        float r = 5.0f;
        float area = pi * Math.pow(r, 2.0f);
        System.out.println(area);  // 78.50
        
        // Phase 9.2: Using integer arguments
        int a = -42;
        float absValue = Math.abs(a);  // Automatically converted to float
        System.out.println(absValue);  // 42.00
        
        int base = 2;
        float result = Math.pow(base, 3);  // 2^3 = 8
        System.out.println(result);  // 8.00
        
        return;
    }
}
```

### Date Class (added in Phase 3.5, long type support in Phase 5.5)
Class for handling date and time in DOS environment

#### Constructors
- `Date()` - Create Date object with current system time
- `Date(long timestamp)` - Create Date object with specified Unix timestamp (**in seconds**)
  - **Important**: `timestamp` is **in seconds** (Java standard API uses milliseconds, but dosjava uses seconds)
  - 32-bit long type supports range from 1970 to 2106

#### Methods
- `long getTime()` - Returns Unix timestamp (**in seconds**)
  - **Important**: Return value is **in seconds** (Java standard API uses milliseconds, but dosjava uses seconds)
- `void setTime(long timestamp)` - Set to specified timestamp (**in seconds**)
  - **Important**: `timestamp` is **in seconds** (Java standard API uses milliseconds, but dosjava uses seconds)
- `int getFullYear()` - Returns year (1970-2106)
- `int getMonth()` - Returns month (0-11, 0=January)
- `int getDate()` - Returns day (1-31)
- `int getHours()` - Returns hour (0-23)
- `int getMinutes()` - Returns minute (0-59)
- `int getSeconds()` - Returns second (0-59)

#### Usage Example
```java
class DateDemo {
    public static void main() {
        // Create Date object with current time
        Date now = new Date();
        
        // Get year, month, day
        int year = now.getFullYear();
        int month = now.getMonth() + 1;  // Display as 1-12
        int day = now.getDate();
        
        System.out.println("Date: ");
        System.out.println(year);
        System.out.println("-");
        System.out.println(month);
        System.out.println("-");
        System.out.println(day);
        
        // Get timestamp (in seconds)
        long timestamp = now.getTime();
        System.out.println("Timestamp (seconds): ");
        System.out.println(timestamp);
        
        // Create Date object with specific timestamp (in seconds)
        long oneDay = 86400L;  // 24 * 60 * 60 seconds
        Date tomorrow = new Date(timestamp + oneDay);
        
        return;
    }
}
```

#### Important Constraints

##### ⚠️ Timestamps are in seconds (not milliseconds)
Due to 32-bit long type limitations, dosjava's Date class handles timestamps **in seconds**.
This differs from standard Java's Date API:

| API | dosjava | Standard Java |
|-----|---------|---------------|
| `Date(long)` | **seconds** | milliseconds |
| `getTime()` | **seconds** | milliseconds |
| `setTime(long)` | **seconds** | milliseconds |

**Reason**:
- 32-bit long max value: 4,294,967,295
- 2026 timestamp in milliseconds: ~1,779,468,000,000 (exceeds 32-bit)
- 2026 timestamp in seconds: ~1,779,468,000 (within 32-bit range)

**Example**:
```java
// dosjava (seconds)
long timestamp = 1779468000L;  // May 22, 2026 (seconds)
Date d = new Date(timestamp);

// Standard Java (milliseconds) would be:
// long timestamp = 1779468000000L;  // May 22, 2026 (milliseconds)
```

##### Other Limitations
- **32-bit long type**: Timestamps range from 0 to 4,294,967,295 (1970 to 2106)
- **Month convention**: 0-based (0=January, 11=December), same as JavaScript Date API
- **Time zone**: Local time only (no timezone conversion)
- **Precision**: Second precision (no millisecond precision)

### Http Class (added in Phase 4)
Class providing HTTP client functionality in DOS environment

#### Prerequisites
- External program `doscurl.exe` is required (must be in PATH environment variable)
- Network connection must be available

#### Methods

**HTTP Request Methods:**
- `String Http.get(String url)` - HTTP GET request (basic)
- `String Http.get(String url, String headers)` - HTTP GET request (with custom headers)
  - `headers`: Newline-separated header string (e.g., `"User-Agent: DOSJava\nAccept: */*"`)
- `String Http.post(String url, String data)` - HTTP POST request
- `String Http.post(String url, String data, String headers)` - HTTP POST request (with custom headers)
- `String Http.put(String url, String data)` - HTTP PUT request
- `String Http.put(String url, String data, String headers)` - HTTP PUT request (with custom headers)
- `String Http.delete(String url)` - HTTP DELETE request
- `String Http.delete(String url, String headers)` - HTTP DELETE request (with custom headers)

**Proxy Configuration Methods (Phase 4.8):**
- `void Http.setProxy(String proxyUrl)` - Set proxy server URL
  - Example: `Http.setProxy("http://proxy.example.com:8080")`
  - With authentication: `Http.setProxy("http://user:pass@proxy.example.com:8080")`
  - Pass empty string to clear proxy: `Http.setProxy("")`
- `void Http.clearProxy()` - Clear proxy server setting
- `String Http.getProxy()` - Get current proxy server URL (returns empty string if not set)

#### Usage Example
```java
class HttpDemo {
    public static void main() {
        try {
            // Basic GET request
            String response1 = Http.get("http://example.com/api");
            System.out.println(response1);
            
            // GET request with custom headers
            String headers = "User-Agent: DOSJava/1.0\nAccept: application/json";
            String response2 = Http.get("http://example.com/api", headers);
            System.out.println(response2);
            
            // POST request
            String postData = "name=test&value=123";
            String response3 = Http.post("http://example.com/api", postData);
            System.out.println(response3);
            
            // Using proxy server (Phase 4.8)
            Http.setProxy("http://proxy.company.com:8080");
            String response4 = Http.get("http://example.com/api");
            System.out.println(response4);
            
            // Clear proxy for direct connection
            Http.clearProxy();
            
        } catch (Exception e) {
            System.out.println("Network error occurred");
            System.out.println(e.getMessage());
        }
        
        return;
    }
}
```

#### Important Notes
- All HTTP methods may throw `NetworkException`
- Responses are returned as strings
- Timeout is 10 seconds (doscurl.exe setting)
- For consecutive requests, it is recommended to add appropriate delays between requests

### Exception Class (added in Phase 11, extended in Phase 12)
Class for detecting and handling runtime exceptions

#### Automatically Detected Exceptions
DOSJava VM automatically detects and throws the following 6 types of runtime exceptions:

| Exception Type | Type Code | Detection Condition |
|----------------|-----------|---------------------|
| NullPointerException | 1 | Access to null array/object |
| ArrayIndexOutOfBoundsException | 2 | Array index out of bounds |
| NumberFormatException | 3 | Invalid string in Integer.parseInt() |
| IllegalArgumentException | 4 | Negative array size, invalid substr range |
| StringIndexOutOfBoundsException | 5 | substr() out of bounds access |
| NetworkException | 6 | HTTP request failure |

#### Methods
- `int getType()` - Returns exception type code (0-6)
  - 0: General exception (when explicitly thrown with throw statement)
  - 1-6: Above automatically detected exceptions
- `String getMessage()` - Returns exception message
  - **Phase 12 Enhancement**: Message now includes line number where exception occurred
  - Format: `"<exception type> (line <line number>)"`
  - Example: `"NullPointerException (line 42)"`

#### Usage Example
```java
class ExceptionDemo {
    public static void main() {
        try {
            // Attempt array out of bounds access
            int[] arr = new int[5];
            int x = arr[10];  // ArrayIndexOutOfBoundsException
        } catch (Exception e) {
            System.out.println("Exception caught!");
            
            // Get exception type
            int type = e.getType();
            System.out.println("Type: ");
            System.out.println(type);  // 2
            
            // Get exception message (Phase 12: with line number)
            String msg = e.getMessage();
            System.out.println("Message: ");
            System.out.println(msg);  // "Array index out of bounds (line 7)"
        }
        
        return;
    }
}
```

#### Exception Handling Syntax
```java
try {
    // Code that may throw exceptions
    int result = Integer.parseInt("abc");  // NumberFormatException
} catch (Exception e) {
    // Exception handling
    System.out.println("Error occurred");
}
```

#### Important Notes
- All exceptions are caught as `Exception` type
- Exception type is determined by `getType()` method
- Exceptions can also be explicitly thrown with `throw` statement
- Nested try-catch blocks are also supported

### Limitations
- No instance methods (except Date and Exception classes)
- No method overloading
- No `String` parameters
- No `String` return values
- No `String + int`
- No general object creation (except Date and Exception classes)
- No inheritance/interfaces
- No packages/imports

## System Requirements

### Runtime Environment
- 16-bit PC-DOS 3.0 or higher
- 640KB RAM or more
- DOSBox (recommended)

### Build Environment
- Open Watcom v2 C Compiler
- Windows/Linux/Mac (cross-compilation)

## Installation

### Pre-built Binaries

The `build/bin/` directory contains:

- [djc.exe](https://github.com/dotnsf/dosjava/raw/refs/heads/main/build/bin/djc.exe)         - Integrated compiler
- [djvm.exe](https://github.com/dotnsf/dosjava/raw/refs/heads/main/build/bin/djvm.exe)        - Virtual machine
- [linechk.exe](https://github.com/dotnsf/dosjava/raw/refs/heads/main/build/bin/linechk.exe)        - File checker for testing (used in tests\runtest.bat)


### Build from Source
```batch
# Set environment variables
set WATCOM=C:\WATCOM
set PATH=C:\WATCOM\binnt;C:\WATCOM\binw;%PATH%
set INCLUDE=C:\WATCOM\h;C:\WATCOM\h\nt

# Build
cd dosjava
wmake all
```

### Example: Clone from git repository and run in DOSBox

- 1. Install [DOSBox](http://www.dosbox.com/)

- 2. git clone
  - `> git clone https://github.com/dotnsf/dosjava.git`

- 3. Edit `dosbox.conf`
  - On Windows, by default there should be a file `$HOME\AppData\Local\DOSBox\dosbox-x.xx-x.conf` (where `x.xx-x` is the DOSBox version), edit this file with a text editor like Notepad
  - Add the following under `[autoexec]`

```
[autoexec]
mount c "C:\Users\username\dosjava"
set PATH=%PATH%;c:\build\bin
c:
```

- 4. Start DOSBox

- 5. `djc.exe` and `djvm.exe` exist in `c:\build\bin` in DOSBox, so run them within DOSBox



## Quick Start

### 1. Hello World Program

Create `hello.jav`:
```java
class hello {
    public static void main() {
        System.out.println("Hello, DOS!");
    }
}
```

### 2. Compilation

```batch
djc hello.jav
```

Output: `hello.djc`

### 3. Execution

```batch
djvm hello.djc
```

## Usage

### djc (Integrated Compiler)

```batch
djc [options] <source.jav>
```

#### Options
- `-o <file>` - Specify output file (default: `<source>.djc`)
- `-k` - Keep intermediate files (.tok, .ast, .sym)
- `-v` - Verbose output
- `-h, --help` - Show help
- `--version` - Show version information

#### Examples
```batch
# Basic usage
djc hello.jav

# Specify output file
djc -o test.djc test.jav

# Verbose output with intermediate files
djc -k -v hello.jav
```

### Compilation Process

```
source.jav
    ↓
[Lexer] → source.tok (tokens)
    ↓
[Parser] → source.ast (abstract syntax tree)
    ↓
[Semantic] → source.sym (symbol table)
    ↓
[CodeGen] → source.djc (bytecode)
```

Note: Since the runtime environment is 16-bit PC-DOS, source/output filenames should generally use DOS 8.3 format (filename 8 characters or less + extension 3 characters or less).
Example: Use `hello.jav` instead of `hello.java`
```

## Sample Programs

### Arithmetic Operations
```java
class Arithmetic {
    public static void main() {
        int a = 10;
        int b = 20;
        int sum = a + b;
        int product = a * b;
        return;
    }
}
```

### Conditional Branching
```java
class Conditional {
    public static void main() {
        int x = 10;
        int result = 0;
        
        if (x > 5) {
            result = 100;
        }
        
        return;
    }
}
```

### Loops
```java
class Loop {
    public static void main() {
        int i = 0;
        int sum = 0;
        
        while (i < 10) {
            sum = sum + i;
            i = i + 1;
        }
        
        return;
    }
}
```

### Fibonacci Sequence
```java
class Fibonacci {
    public static void main() {
        int n = 10;
        int a = 0;
        int b = 1;
        int i = 0;
        
        while (i < n) {
            int temp = a + b;
            a = b;
            b = temp;
            i = i + 1;
        }
        
        return;
    }
}
```

## Project Structure

```
dosjava/
├── src/                    # Source code
│   ├── vm/                # Virtual machine
│   ├── runtime/           # Runtime library
│   └── format/            # .djc format
├── tools/                 # Tools
│   └── compiler/          # Compiler
├── tests/                 # Tests
│   ├── e2e/              # End-to-end tests
│   ├── lexer/            # Lexer tests
│   ├── parser/           # Parser tests
│   └── semantic/         # Semantic analysis tests
├── build/                 # Build output
│   ├── bin/              # Executables
│   └── obj/              # Object files
├── Makefile              # Build script
└── README.md             # This file
```

## Testing

### Run End-to-End Tests
```batch
cd tests\e2e
run_tests.bat
```

### Test Categories
- **Basic**: Hello World, minimal programs
- **Arithmetic**: Addition, complex expressions
- **Control**: if/else, while, for
- **Methods**: static calls, arguments, int return values
- **Strings**: `println(String)`, `length()`, `String + String`
- **Arrays**: `int[]`, `array.length`
- **Complex**: Fibonacci, factorial

## Documentation

- [API_REFERENCE.md](API_REFERENCE.md) - **API Reference (Complete guide for all classes and methods)**
- [QUICKSTART.md](QUICKSTART.md) - 5-minute getting started guide
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture
- [BUILD.md](BUILD.md) - Build instructions
- [TECHNICAL_SPEC.md](TECHNICAL_SPEC.md) - Technical specification
- [PHASE3_5_SUMMARY.md](PHASE3_5_SUMMARY.md) - Phase 3.5 implementation summary (Date Support completed)
- [PHASE4_COMPLETION.md](PHASE4_COMPLETION.md) - Phase 4 completion report (HTTP Client Functionality)
- [PHASE5_PLAN.md](PHASE5_PLAN.md) - Phase 5 implementation plan
- [PHASE11_COMPLETION.md](PHASE11_COMPLETION.md) - Phase 11 completion report (Runtime Exception Detection)
- [PHASE12_COMPLETION.md](PHASE12_COMPLETION.md) - Phase 12 completion report (Exception Line Numbers)
- [PHASE13_COMPLETION.md](PHASE13_COMPLETION.md) - Phase 13 completion report (String Method Enhancement)

## Troubleshooting

### Compilation Errors
```
Error: Input file not found: Hello.java
```
→ Check filename and path

### Syntax Errors
```
Compilation failed in phase: Parsing
Error: Parse error at line 3, column 16
```
→ Check syntax (missing semicolons, etc.)

### Type Errors
```
Compilation failed in phase: Semantic analysis
Error: Type mismatch
```
→ Check type compatibility

## Limitations

### Memory Limitations
- Code: 64KB
- Data: 64KB
- Stack: 4-8KB
- Heap: ~40KB

### Language Limitations
- No instance-based object-oriented features (except Date class)
- `String` has limited support from Phase 1
- `String + int`, `String` parameters, `String` return values not supported
- No `double` type (`float` added in Phase 6.1)

## Development Phases

### Phase 1: Basic Features ✅ Completed
- Virtual machine (djvm.exe) implementation
- String literal support
- `String.length()` support
- `String + String` concatenation support

### Phase 2: String Feature Enhancement ✅ Completed
- Complete String class implementation
- String manipulation methods (charAt, substring, indexOf, etc.)
- String comparison and utilities

### Phase 3: I/O System ✅ Completed
- InputStream/OutputStream base classes
- FileInputStream/FileOutputStream
- BufferedReader/BufferedWriter
- Constructor argument support

### Phase 3.5: Exception Handling and Date Support ✅ Completed
- **Phase 3.5.1**: Exception Handling ✅ Completed
  - try-catch-finally-throw syntax implementation
  - Basic exception handling functionality
- **Phase 3.5.2**: Date Support ✅ Completed
  - DOS Time API implementation (dostime.h/dostime.c)
  - Date class implementation (date.h/date.c)
  - All 48 tests passed
  - Details: [PHASE3_5_SUMMARY.md](PHASE3_5_SUMMARY.md)

### Phase 4: HTTP Client Functionality ✅ Completed
- **Phase 4.1**: HTTP Client Basic Implementation
  - HTTP client functionality via doscurl.exe integration
  - Http.get(String url) - Basic GET request
  - Http.post(String url, String data) - POST request
  - Http.put(String url, String data) - PUT request
  - Http.delete(String url) - DELETE request
  - NetworkException (type code 6) added
- **Phase 4.5**: Custom Header Support (GET)
  - Http.get(String url, String headers) - GET request with headers
  - Newline-separated header format support
  - DOS 127-character command line limitation workaround (batch file method)
- **Phase 4.6**: Testing and Samples
  - Comprehensive test programs (tests/httphead.jav, tests/httpsimpl.jav)
  - Sample program (samples/http.jav)
  - Consecutive request support (with delay functionality)
- **Phase 4.7**: POST/PUT/DELETE Custom Header Support ✅ Completed
  - Http.post(String url, String data, String headers) - POST with headers
  - Http.put(String url, String data, String headers) - PUT with headers
  - Http.delete(String url, String headers) - DELETE with headers
  - Custom header support completed for all HTTP methods
  - Symbol table size expansion (256→512 symbols, 2KB→4KB string pool)
- **Limitations**:
  - HTTP status code retrieval not implemented (doscurl limitation)
- Details: [PHASE4_COMPLETION.md](PHASE4_COMPLETION.md)

### Phase 5: Long Type Support ✅ Completed
- **Phase 5.1**: 32-bit long type basic implementation
  - Literals, operators, comparison operations
- **Phase 5.2**: long type code generation and VM implementation
  - Stack operations, arithmetic operations, comparison operations
- **Phase 5.3**: long type output support
  - `System.out.println(long)` implementation
- **Phase 5.4**: long array support
  - `long[]` array creation, access, assignment
- **Phase 5.5**: Date class long type support
  - Handle timestamps as long type in seconds

### Phase 6: Float Type and Math Support ✅ Completed
- **Phase 6.1**: 32-bit float type implementation
  - Literals, operators, comparison operations
  - IEEE 754 format
  - `System.out.println(float)` support
- **Phase 6.2**: float array support
  - `float[]` array creation, access, assignment
- **Phase 6.3**: Math class implementation
  - Basic functions: abs, min, max, sqrt
  - Trigonometric functions: sin, cos, tan
  - Exponential/logarithmic: pow, exp, log
  - Details: [PHASE6_3_MATH_COMPLETION.md](PHASE6_3_MATH_COMPLETION.md)

### Phase 7: Switch Statement Support ✅ Completed
- **Phase 7.1**: Basic switch statement support
  - INT, LONG, STRING type switches
  - case and default statements
- **Phase 7.2**: String switch optimization
  - Efficient string comparison
- **Phase 7.3**: Fall-through behavior implementation
  - Continue to next case without break
  - Details: [PHASE7_3_SWITCH_FALLTHROUGH_FIX.md](PHASE7_3_SWITCH_FALLTHROUGH_FIX.md)

### Phase 8: Boolean Literal Support ✅ Completed
- `true` / `false` literal support
- Parser, semantic analyzer, code generator extensions
- Use in loop conditions (`while (true)`, etc.)
- Details: [PHASE8_BOOLEAN_LITERAL_PLAN.md](PHASE8_BOOLEAN_LITERAL_PLAN.md)

### Phase 9: Type System Enhancements ✅ Completed
- **Phase 9.1**: Type cast support
  - `(type)expression` syntax implementation
  - Mutual conversion between int ↔ long ↔ float
  - Math function return value casting support
- **Phase 9.2**: Math function integer argument support
  - All Math functions accept int/long arguments
  - Automatic conversion to float type
- **Phase 9.3**: null literal support
  - `null` keyword addition
  - Assignment to reference types (String, arrays)
  - null comparison operations (`==`, `!=`)
- Details: [PHASE9_COMPLETION.md](PHASE9_COMPLETION.md)

### Phase 10: Language Feature Enhancements ✅ Completed
- **Phase 10.1**: Variable declaration initialization support
  - Initialization with literals (`int a = 42;`)
  - Initialization with expressions (`int sum = a + b;`)
  - Support for all primitive types and String type
  - Details: [PHASE10_1_COMPLETION.md](PHASE10_1_COMPLETION.md)

### Phase 11: Runtime Exception Detection and Auto-Throw ✅ Completed
- **Phase 11.1**: Exception Type System Enhancement
  - Definition of 5 exception type codes (0-5)
  - Exception information fields added to ExecutionContext
- **Phase 11.2**: NullPointerException Detection
  - Automatic detection of null array/object access
- **Phase 11.3**: ArrayIndexOutOfBoundsException Detection
  - Automatic detection of array out of bounds access
- **Phase 11.4**: NumberFormatException Detection
  - Detection of invalid strings in Integer.parseInt()
- **Phase 11.5**: IllegalArgumentException Detection
  - Detection of negative array size, invalid substr range
- **Phase 11.6**: StringIndexOutOfBoundsException Detection
  - Detection of substr() out of bounds access
- **Phase 11.7**: Exception Message Retrieval
  - Exception.getType() method implementation
  - Exception.getMessage() method implementation
- **Phase 11.8**: Comprehensive Testing
  - 10 comprehensive test cases (tests/excall.jav)
  - Sample program (samples/excauto.jav)
- **Phase 11.9**: Documentation
  - Details: [PHASE11_COMPLETION.md](PHASE11_COMPLETION.md)

### Phase 12: Exception Line Number Information ✅ Completed
- **Phase 12.1**: DJC Format Extension
  - Extended DJC file format from v0x0001 to v0x0002
  - Added line number table (LineNumberEntry array)
  - Fast line number lookup via binary search
- **Phase 12.2**: Compiler Line Number Tracking
  - Line number tracking implementation in compiler
  - Global PC (Program Counter) calculation fix
  - Recording of line numbers and bytecode offsets for each statement
- **Phase 12.3**: VM Line Number Lookup
  - Exception PC saving functionality in VM
  - PC protection via exception_pending flag
  - Line number addition to exception messages
- **Phase 12.4**: Testing and Validation
  - Detailed test program (tests/excline.jav)
  - Sample program (samples/exctest.jav)
  - Confirmed accurate line number reporting for all 6 exception types
- **Phase 12.5**: Documentation
  - Exception message format: `"<exception type> (line <line number>)"`
  - Backward compatibility: Old format (v0x0001) DJC files can still run
  - Details: [PHASE12_COMPLETION.md](PHASE12_COMPLETION.md)

### Phase 13: String Method Enhancement ✅ Completed
- **Phase 13.1-13.8**: Implementation of 8 new String methods
  - `charAt(int)` - Returns character at specified position (StringIndexOutOfBoundsException support)
  - `isEmpty()` - Empty string check
  - `trim()` - Remove leading/trailing whitespace (space, tab, newline, CR)
  - `replace(String, String)` - Replace all occurrences
  - `compareTo(String)` - Lexicographic comparison (NullPointerException support)
  - `lastIndexOf(String)` - Last occurrence position
  - `contains(String)` - Substring check
  - `repeat(int)` - String repetition (IllegalArgumentException support)
- **Phase 13.9**: Comprehensive testing and validation
  - Basic functionality tests (strext1-3.jav)
  - Exception handling tests (strexc.jav)
  - Edge case tests (stredge1-2.jav)
  - Sample program (samples/strext.jav)
- **Phase 13.10**: Documentation
  - Details: [PHASE13_COMPLETION.md](PHASE13_COMPLETION.md)

### Future Plans
- [ ] Phase 4 extension (POST/PUT/DELETE custom header support)
- [ ] Optimization
- [ ] Debugger

## License

This code is licensed under the MIT License.

This project is provided as-is without warranty. The MIT License permits use for any purpose, including commercial use.

## Copyright

2026 [K.Kimura @ Juge.Me](https://github.com/dotnsf) all rights reserved.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## Author

[K.Kimura](https://github.com/dotnsf), developed for 16-bit DOS environments using Open Watcom C/C++.

## Acknowledgments

- Open Watcom Project
- Java Language Specification
- DOS development community
