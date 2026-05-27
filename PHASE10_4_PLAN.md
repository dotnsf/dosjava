# Phase 10.4 Implementation Plan: Source Line Number Information

## Overview
Add source code line number information to runtime exception error messages to improve debugging experience.

**Goal**: Display error messages like `Exception: Division by zero (line 65)` instead of just `Exception: Division by zero`

## Current Status
- Phase 10.3 completed: Runtime exceptions show meaningful error messages
- Need to add: Source line number information

## Technical Approach

### 1. DJC File Format Extension

#### Current Format (Version 0x0001)
```
[Header: 12 bytes]
  - magic: 2 bytes (0x444A)
  - version: 2 bytes (0x0001)
  - constant_pool_count: 2 bytes
  - method_count: 2 bytes
  - field_count: 2 bytes
  - code_size: 2 bytes

[Constant Pool]
[Methods]
[Fields]
[Bytecode]
```

#### New Format (Version 0x0002)
```
[Header: 14 bytes]
  - magic: 2 bytes (0x444A)
  - version: 2 bytes (0x0002)
  - constant_pool_count: 2 bytes
  - method_count: 2 bytes
  - field_count: 2 bytes
  - code_size: 2 bytes
  - line_number_table_count: 2 bytes  <-- NEW

[Constant Pool]
[Methods]
[Fields]
[Bytecode]
[Line Number Table]  <-- NEW
  - Entry format: [pc:2][line:2] repeated
```

### 2. Line Number Table Structure

```c
typedef struct {
    uint16_t pc;        /* Program counter (bytecode offset) */
    uint16_t line_no;   /* Source line number */
} LineNumberEntry;
```

**Properties**:
- Sorted by PC for binary search
- One entry per source line that generates bytecode
- Compact: 4 bytes per entry

**Example**:
```
Source:          Bytecode PC:    Line Number Entry:
line 10: int a;  -> PC 0         [0, 10]
line 11: int b;  -> PC 2         [2, 11]
line 12: a / b;  -> PC 4         [4, 12]
```

### 3. Implementation Steps

#### Step 1: Update DJC Format (djc.h, djc.c)

**djc.h changes**:
```c
#define DJC_VERSION 0x0002  // Increment version

typedef struct {
    uint16_t magic;
    uint16_t version;
    uint16_t constant_pool_count;
    uint16_t method_count;
    uint16_t field_count;
    uint16_t code_size;
    uint16_t line_number_table_count;  // NEW
} DJCHeader;

typedef struct {
    uint16_t pc;
    uint16_t line_no;
} LineNumberEntry;

typedef struct {
    DJCHeader header;
    DJCConstant* constants;
    DJCMethod* methods;
    DJCField* fields;
    uint8_t* bytecode;
    LineNumberEntry* line_numbers;  // NEW
} DJCFile;

// NEW function
uint16_t djc_get_source_line(DJCFile* file, uint16_t pc);
```

**djc.c changes**:
- Update `read_header()` to read line_number_table_count
- Add `read_line_numbers()` function
- Update `djc_open()` to read line number table
- Add `djc_get_source_line()` for binary search
- Update `djc_close()` to free line number table

#### Step 2: Compiler Changes (codegen.c)

**Add line number tracking**:
```c
typedef struct {
    uint16_t pc;
    uint16_t line_no;
} CodeGenLineEntry;

typedef struct {
    // ... existing fields ...
    CodeGenLineEntry* line_entries;
    uint16_t line_entry_count;
    uint16_t line_entry_capacity;
    uint16_t current_line;  // Track current source line
} CodeGen;

// NEW function
void codegen_add_line_entry(CodeGen* cg, uint16_t line_no);
```

**Modify emit functions**:
- Track current line number from AST nodes
- Add line entry when line changes
- Write line number table to output file

#### Step 3: VM Changes (interpreter.c)

**Update throw_runtime_exception()**:
```c
static int throw_runtime_exception(ExecutionContext* ctx, const char* message) {
    uint16_t line_no;
    char full_message[64];
    
    /* Get source line number from current PC */
    line_no = djc_get_source_line(ctx->djc_file, 
                                   (uint16_t)(ctx->pc - ctx->djc_file->bytecode));
    
    /* Format message with line number */
    if (line_no > 0) {
        snprintf(full_message, sizeof(full_message), 
                 "%s (line %u)", message, line_no);
    } else {
        strncpy(full_message, message, sizeof(full_message) - 1);
    }
    full_message[sizeof(full_message) - 1] = '\0';
    
    /* Save to context */
    strncpy(ctx->exception_message, full_message, 
            sizeof(ctx->exception_message) - 1);
    ctx->exception_message[sizeof(ctx->exception_message) - 1] = '\0';
    
    // ... rest of function ...
}
```

#### Step 4: Backward Compatibility

**Version handling**:
- Version 0x0001: No line number table (old format)
- Version 0x0002: With line number table (new format)
- VM reads version and handles both formats
- If version 0x0001, `djc_get_source_line()` returns 0 (no line info)

### 4. Memory Impact Analysis

**File Size Impact**:
- Small program (100 lines, ~50 bytecode instructions): +200 bytes
- Medium program (500 lines, ~250 bytecode instructions): +1KB
- Large program (2000 lines, ~1000 bytecode instructions): +4KB

**Runtime Memory Impact**:
- Line number table loaded into memory
- 4 bytes per entry
- Typical: 200-400 bytes for small programs
- Acceptable for 16-bit DOS environment

### 5. Test Plan

#### Test Cases

**Test 1: Division by zero with line number**
```java
// line 5
class Test {
    public static void main() {
        int a = 10;     // line 8
        int b = 0;      // line 9
        int c = a / b;  // line 10 <- Error here
    }
}
```
Expected: `Exception: Division by zero (line 10)`

**Test 2: File not found with line number**
```java
// line 5
class Test {
    public static void main() {
        String line;
        File.open("missing.txt");  // line 9 <- Error here
        line = File.readLine();
    }
}
```
Expected: `Exception: Cannot open file: missing.txt (line 9)`

**Test 3: Backward compatibility**
- Compile with old compiler (version 0x0001)
- Run with new VM
- Should work without line numbers

**Test 4: Multiple exceptions**
```java
class Test {
    public static void main() {
        try {
            int x = 1 / 0;  // line 5
        } catch (Exception e) {
            System.out.println(e);
        }
        
        try {
            File.open("x.txt");  // line 11
        } catch (Exception e) {
            System.out.println(e);
        }
    }
}
```
Expected:
```
Exception: Division by zero (line 5)
Exception: Cannot open file: x.txt (line 11)
```

### 6. Implementation Order

1. ✅ **Design phase** (this document)
2. **DJC format update** (djc.h, djc.c)
3. **Compiler line tracking** (codegen.c)
4. **VM line lookup** (interpreter.c)
5. **Testing** (create test files)
6. **Documentation** (completion report)

### 7. Potential Issues and Solutions

**Issue 1**: Binary search overhead
- **Solution**: Cache last lookup result, most exceptions occur at same line

**Issue 2**: Inlined code (multiple source lines -> same PC)
- **Solution**: Use first line number for that PC range

**Issue 3**: Optimized code (PC doesn't match source)
- **Solution**: Compiler should emit line entries even for optimized code

**Issue 4**: Memory constraints
- **Solution**: Line table is optional, can be omitted for release builds

### 8. Future Enhancements (Not in Phase 10.4)

1. Full stack trace with multiple line numbers
2. Column number information
3. Source file name in error messages
4. Debug mode with more detailed information

## Success Criteria

- ✅ DJC format version incremented to 0x0002
- ✅ Line number table written by compiler
- ✅ Line number table read by VM
- ✅ Error messages include line numbers
- ✅ Backward compatibility maintained
- ✅ All tests pass in DOSBox-X
- ✅ Memory usage acceptable (<1KB overhead)

## Timeline Estimate

- Step 1 (DJC format): 30 minutes
- Step 2 (Compiler): 45 minutes
- Step 3 (VM): 30 minutes
- Step 4 (Testing): 30 minutes
- Step 5 (Documentation): 15 minutes
- **Total**: ~2.5 hours

## Next Steps

Start with Step 1: Update DJC file format (djc.h and djc.c)