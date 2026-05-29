# Phase 12 Implementation Plan: Exception Line Number Information

## Overview
Add source code line number information to runtime exceptions to improve debugging experience. This builds on Phase 11's exception handling foundation.

**Goal**: Display error messages like `Exception: Array index out of bounds (line 42)` instead of just `Exception: Array index out of bounds`

## Current Status (After Phase 11)
- ✅ Phase 11 completed: 5 types of runtime exceptions with auto-detection
- ✅ Exception.getType() and Exception.getMessage() implemented
- ✅ All exception types working correctly
- 🎯 Need to add: Source line number information to exception messages

## Phase 12 Sub-phases

### Phase 12.1: DJC Format Extension ⏳
**Goal**: Extend .djc file format to include line number table

**Tasks**:
1. Update DJC_VERSION from 0x0001 to 0x0002
2. Add line_number_table_count to DJCHeader
3. Define LineNumberEntry structure (pc:2, line_no:2)
4. Implement line number table reading/writing
5. Maintain backward compatibility with version 0x0001

**Files to modify**:
- `src/format/djc.h` - Add structures and constants
- `src/format/djc.c` - Implement read/write functions

**Success criteria**:
- DJC format version 0x0002 defined
- Line number table can be written and read
- Old format (0x0001) files still work

### Phase 12.2: Compiler Line Number Tracking ⏳
**Goal**: Generate line number table during compilation

**Tasks**:
1. Add line number tracking to CodeGenerator structure
2. Extract line numbers from AST nodes
3. Generate line number entries during code generation
4. Write line number table to output file
5. Ensure line numbers are accurate for all statement types

**Files to modify**:
- `tools/compiler/codegen.h` - Add line tracking structures
- `tools/compiler/codegen.c` - Implement line number tracking
- `tools/compiler/ast.h` - Verify line number fields in AST nodes

**Success criteria**:
- Line number table generated for all compiled files
- Line numbers match source code accurately
- No compilation errors or warnings

### Phase 12.3: VM Line Number Lookup ⏳
**Goal**: Look up source line numbers during exception handling

**Tasks**:
1. Implement djc_get_source_line() function
2. Use binary search for efficient lookup
3. Update throw_runtime_exception() to include line numbers
4. Format exception messages with line numbers
5. Handle cases where line number is unavailable (old format)

**Files to modify**:
- `src/format/djc.h` - Add djc_get_source_line() declaration
- `src/format/djc.c` - Implement line number lookup
- `src/vm/interpreter.c` - Update exception throwing

**Success criteria**:
- Line numbers correctly retrieved from PC
- Exception messages include line numbers
- Graceful handling of missing line info

### Phase 12.4: Testing and Validation ⏳
**Goal**: Verify line numbers work for all exception types

**Tasks**:
1. Create test files for each exception type with line numbers
2. Verify line numbers are correct in exception messages
3. Test backward compatibility with old .djc files
4. Test edge cases (first line, last line, nested blocks)
5. Update existing test suite

**Test files to create**:
- `tests/excline1.jav` - NullPointerException with line number
- `tests/excline2.jav` - ArrayIndexOutOfBoundsException with line number
- `tests/excline3.jav` - NumberFormatException with line number
- `tests/excline4.jav` - IllegalArgumentException with line number
- `tests/excline5.jav` - StringIndexOutOfBoundsException with line number
- `tests/exclineall.jav` - All exception types with line numbers

**Success criteria**:
- All 5 exception types show correct line numbers
- Backward compatibility maintained
- All tests pass in DOSBox-X

### Phase 12.5: Documentation ⏳
**Goal**: Document the line number feature

**Tasks**:
1. Create PHASE12_COMPLETION.md
2. Update README.md with line number information
3. Document DJC format version 0x0002
4. Add usage examples

**Success criteria**:
- Complete documentation of implementation
- README.md updated with examples
- Format specification documented

## Technical Design

### 1. DJC File Format Version 0x0002

```
[Header: 14 bytes]
  - magic: 2 bytes (0x444A)
  - version: 2 bytes (0x0002)  <-- Changed from 0x0001
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

### 3. Line Number Lookup Algorithm

```c
uint16_t djc_get_source_line(DJCFile* file, uint16_t pc) {
    /* Binary search in line number table */
    int left = 0;
    int right = file->header.line_number_table_count - 1;
    uint16_t result = 0;
    
    while (left <= right) {
        int mid = (left + right) / 2;
        if (file->line_numbers[mid].pc <= pc) {
            result = file->line_numbers[mid].line_no;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return result;
}
```

### 4. Exception Message Format

```c
/* In throw_runtime_exception() */
uint16_t line_no = djc_get_source_line(ctx->djc_file, 
                                       (uint16_t)(ctx->pc - ctx->djc_file->bytecode));

if (line_no > 0) {
    snprintf(full_message, sizeof(full_message), 
             "%s (line %u)", message, line_no);
} else {
    strncpy(full_message, message, sizeof(full_message) - 1);
}
```

### 5. Backward Compatibility Strategy

**Version 0x0001 (old format)**:
- No line_number_table_count field
- No line number table
- djc_get_source_line() returns 0
- Exception messages without line numbers

**Version 0x0002 (new format)**:
- Has line_number_table_count field
- Has line number table
- djc_get_source_line() returns actual line number
- Exception messages with line numbers

**VM behavior**:
```c
if (file->header.version == 0x0001) {
    /* Old format - skip line number table */
    file->line_numbers = NULL;
    file->header.line_number_table_count = 0;
} else if (file->header.version == 0x0002) {
    /* New format - read line number table */
    read_line_number_table(file);
}
```

## Memory Impact Analysis

### File Size Impact
- Small program (100 lines, ~50 bytecode instructions): +200 bytes
- Medium program (500 lines, ~250 bytecode instructions): +1KB
- Large program (2000 lines, ~1000 bytecode instructions): +4KB

### Runtime Memory Impact
- Line number table loaded into memory
- 4 bytes per entry
- Typical: 200-400 bytes for small programs
- Acceptable for 16-bit DOS environment

### Performance Impact
- **Normal execution**: Zero overhead (line lookup only on exception)
- **Exception handling**: ~10-20 binary search iterations (negligible)
- **File loading**: Minimal increase (reading line table)

## Integration with Phase 11

### Exception Types with Line Numbers

All 5 Phase 11 exception types will show line numbers:

1. **NullPointerException (type 1)**
   ```
   Before: Exception: Null pointer access
   After:  Exception: Null pointer access (line 42)
   ```

2. **ArrayIndexOutOfBoundsException (type 2)**
   ```
   Before: Exception: Array index out of bounds
   After:  Exception: Array index out of bounds (line 58)
   ```

3. **NumberFormatException (type 3)**
   ```
   Before: Exception: Invalid number format
   After:  Exception: Invalid number format (line 23)
   ```

4. **IllegalArgumentException (type 4)**
   ```
   Before: Exception: Illegal argument
   After:  Exception: Illegal argument (line 67)
   ```

5. **StringIndexOutOfBoundsException (type 5)**
   ```
   Before: Exception: String index out of bounds
   After:  Exception: String index out of bounds (line 91)
   ```

### Exception.getMessage() Enhancement

The `Exception.getMessage()` method will automatically return messages with line numbers:

```java
try {
    int[] arr = new int[5];
    int x = arr[10];  // Line 42
} catch (Exception e) {
    String msg = e.getMessage();
    System.out.println(msg);  // "Array index out of bounds (line 42)"
}
```

## Test Plan

### Test 1: NullPointerException with Line Number
```java
// tests/excline1.jav
class ExcLine1 {
    public static void main() {
        try {
            int[] arr = null;
            int x = arr[0];  // Line 6 - Should show this line
        } catch (Exception e) {
            String msg = e.getMessage();
            System.out.println(msg);
        }
    }
}
```
Expected output: `Null pointer access (line 6)`

### Test 2: ArrayIndexOutOfBoundsException with Line Number
```java
// tests/excline2.jav
class ExcLine2 {
    public static void main() {
        try {
            int[] arr = new int[5];
            int x = arr[10];  // Line 6 - Should show this line
        } catch (Exception e) {
            String msg = e.getMessage();
            System.out.println(msg);
        }
    }
}
```
Expected output: `Array index out of bounds (line 6)`

### Test 3: Multiple Exceptions with Different Line Numbers
```java
// tests/exclineall.jav
class ExcLineAll {
    public static void main() {
        int pass = 0;
        int total = 5;
        
        // Test 1: NullPointerException
        try {
            int[] arr = null;
            int x = arr[0];  // Line 10
        } catch (Exception e) {
            String msg = e.getMessage();
            if (msg.indexOf("line 10") >= 0) {
                pass = pass + 1;
            }
        }
        
        // Test 2: ArrayIndexOutOfBoundsException
        try {
            int[] arr = new int[3];
            int x = arr[5];  // Line 21
        } catch (Exception e) {
            String msg = e.getMessage();
            if (msg.indexOf("line 21") >= 0) {
                pass = pass + 1;
            }
        }
        
        // Test 3: NumberFormatException
        try {
            int x = Integer.parseInt("abc");  // Line 31
        } catch (Exception e) {
            String msg = e.getMessage();
            if (msg.indexOf("line 31") >= 0) {
                pass = pass + 1;
            }
        }
        
        // Test 4: IllegalArgumentException
        try {
            int[] arr = new int[-5];  // Line 41
        } catch (Exception e) {
            String msg = e.getMessage();
            if (msg.indexOf("line 41") >= 0) {
                pass = pass + 1;
            }
        }
        
        // Test 5: StringIndexOutOfBoundsException
        try {
            String s = "hello";
            String sub = s.substr(10, 15);  // Line 51
        } catch (Exception e) {
            String msg = e.getMessage();
            if (msg.indexOf("line 51") >= 0) {
                pass = pass + 1;
            }
        }
        
        System.out.println("Result:");
        System.out.println(pass);
        System.out.println(total);
    }
}
```
Expected output:
```
Result:
5
5
```

### Test 4: Backward Compatibility
1. Compile a file with old compiler (version 0x0001)
2. Run with new VM
3. Should work without line numbers
4. Exception messages should not have "(line X)"

## Implementation Order

1. ✅ **Design phase** (this document)
2. ⏳ **Phase 12.1**: DJC format extension
3. ⏳ **Phase 12.2**: Compiler line number tracking
4. ⏳ **Phase 12.3**: VM line number lookup
5. ⏳ **Phase 12.4**: Testing and validation
6. ⏳ **Phase 12.5**: Documentation

## Potential Issues and Solutions

### Issue 1: AST nodes may not have line numbers
**Solution**: 
- Check parser.c to verify line number tracking
- If missing, add line number field to ASTNode structure
- Update parser to populate line numbers

### Issue 2: Binary search overhead
**Solution**: 
- Cache last lookup result (most exceptions occur at same line)
- Binary search is O(log n), very fast even for large tables

### Issue 3: Inlined code (multiple source lines -> same PC)
**Solution**: 
- Use first line number for that PC range
- Document this behavior

### Issue 4: Memory constraints in DOS
**Solution**: 
- Line table is optional, can be omitted for release builds
- Typical overhead is <1KB, acceptable

### Issue 5: Line numbers in nested try-catch blocks
**Solution**: 
- PC always points to current instruction
- Line lookup works correctly regardless of nesting

## Success Criteria

- ✅ DJC format version incremented to 0x0002
- ✅ Line number table written by compiler
- ✅ Line number table read by VM
- ✅ All 5 exception types show correct line numbers
- ✅ Exception.getMessage() returns messages with line numbers
- ✅ Backward compatibility maintained (version 0x0001 files work)
- ✅ All tests pass in DOSBox-X
- ✅ Memory usage acceptable (<1KB overhead)
- ✅ Documentation complete

## Timeline Estimate

- Phase 12.1 (DJC format): 45 minutes
- Phase 12.2 (Compiler): 60 minutes
- Phase 12.3 (VM): 45 minutes
- Phase 12.4 (Testing): 45 minutes
- Phase 12.5 (Documentation): 30 minutes
- **Total**: ~3.5 hours

## Benefits

1. **Improved debugging**: Programmers can immediately identify where exceptions occur
2. **Better error messages**: More informative than Phase 11 alone
3. **Professional quality**: Matches behavior of modern Java environments
4. **Minimal overhead**: Small file size increase, zero runtime overhead
5. **Backward compatible**: Old .djc files continue to work

## Next Steps

Start with Phase 12.1: Update DJC file format (djc.h and djc.c)