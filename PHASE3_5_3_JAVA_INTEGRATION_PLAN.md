# Phase 3.5.3: Date Java Integration Plan

## Overview

Phase 3.5.3 integrates the C-level Date implementation (completed in Phase 3.5.2) with the dosjava compiler and VM, enabling Java programs to use Date functionality.

## Implementation Strategy

Following the existing pattern used for I/O classes (FileInputStream, BufferedWriter, etc.), we will:

1. Register Date as a built-in class in the semantic analyzer
2. Add Date object creation support in VM's OP_NEW handler
3. Add Date method calls in VM's OP_INVOKE_VIRTUAL handler
4. Create Java test programs
5. Document the Java API

## Day 1: Semantic Analyzer Update

### Goal
Register Date as a built-in class so the compiler recognizes it.

### Files to Modify
- `tools/compiler/semantic.c`

### Implementation Steps

#### 1. Add "Date" to builtin_classes array
Location: `register_builtin_classes()` function

```c
static int register_builtin_classes(SemanticAnalyzer* analyzer) {
    const char* builtin_classes[] = {
        "BufferedWriter",
        "BufferedReader",
        "FileOutputStream",
        "FileInputStream",
        "OutputStream",
        "InputStream",
        "Date",           // ADD THIS
        NULL
    };
    // ... rest of function
}
```

#### 2. Test Compilation
Create a simple test file:
```java
class DateTest {
    public static void main() {
        Date d = new Date();
        return;
    }
}
```

Compile with djc.exe and verify no semantic errors.

### Success Criteria
- ✅ Date class recognized by semantic analyzer
- ✅ No "undefined class" errors when compiling Date usage
- ✅ Compilation produces .djc file

## Day 2: VM OP_NEW Update (Date Constructor)

### Goal
Enable `new Date()` and `new Date(long)` in Java code.

### Files to Modify
- `src/vm/interpreter.c`

### Implementation Steps

#### 1. Add date.h include
At top of interpreter.c:
```c
#include "../runtime/date.h"
```

#### 2. Add Date() constructor handling in OP_NEW
Location: Inside `case OP_NEW:` block, after BufferedReader handling

```c
else if (strcmp(class_name, "Date") == 0) {
    Date* date_obj;
    
    if (arg_count == 0) {
        /* Date() - current time */
        date_obj = date_new();
    } else if (arg_count == 1) {
        /* Date(long timestamp) */
        uint32_t timestamp = (uint32_t)args[0];
        date_obj = date_new_with_time(timestamp);
    } else {
        printf("ERROR: Invalid argument count for Date constructor: %u\n", arg_count);
        return -1;
    }
    
    if (date_obj == NULL) {
        printf("ERROR: Failed to create Date object\n");
        return -1;
    }
    
    /* In Small memory model, pointer is already 16-bit offset */
    object_handle = (uint16_t)(uintptr_t)date_obj;
}
```

#### 3. Update Makefile
Ensure date.c is linked with djvm.exe:
```makefile
DJVM_OBJS = ... build/obj/date.obj build/obj/dostime.obj
```

#### 4. Test
Create test program:
```java
class DateNew {
    public static void main() {
        Date d1 = new Date();
        Date d2 = new Date(1705320645);
        return;
    }
}
```

Compile and run with djvm.exe.

### Success Criteria
- ✅ `new Date()` creates Date object
- ✅ `new Date(timestamp)` creates Date with specific time
- ✅ No crashes or memory errors
- ✅ Object handles returned correctly

## Day 3: VM OP_INVOKE_VIRTUAL Update (Date Methods)

### Goal
Enable calling Date methods from Java code.

### Files to Modify
- `src/vm/interpreter.c`

### Implementation Steps

#### 1. Add Date method handlers in OP_INVOKE_VIRTUAL
Location: Inside `case OP_INVOKE_VIRTUAL:` block, after close() handling

```c
else if (strcmp(method_name, "getTime") == 0) {
    /* Date.getTime() - returns long (as uint16_t for now) */
    Date* date_obj;
    uint32_t timestamp;
    
    /* Pop object reference */
    object_handle = stack_pop_shared(ctx);
    
    /* Call native method */
    date_obj = (Date*)(uintptr_t)object_handle;
    timestamp = date_get_time(date_obj);
    
    /* Push result (truncated to 16-bit for now) */
    stack_push_shared(ctx, (uint16_t)timestamp);
    break;
}
else if (strcmp(method_name, "setTime") == 0) {
    /* Date.setTime(long) */
    Date* date_obj;
    uint32_t timestamp;
    
    /* Pop timestamp argument */
    timestamp = (uint32_t)stack_pop_shared(ctx);
    
    /* Pop object reference */
    object_handle = stack_pop_shared(ctx);
    
    /* Call native method */
    date_obj = (Date*)(uintptr_t)object_handle;
    date_set_time(date_obj, timestamp);
    break;
}
else if (strcmp(method_name, "getFullYear") == 0) {
    /* Date.getFullYear() - returns int */
    Date* date_obj;
    uint16_t year;
    
    /* Pop object reference */
    object_handle = stack_pop_shared(ctx);
    
    /* Call native method */
    date_obj = (Date*)(uintptr_t)object_handle;
    year = date_get_full_year(date_obj);
    
    /* Push result */
    stack_push_shared(ctx, year);
    break;
}
else if (strcmp(method_name, "getMonth") == 0) {
    /* Date.getMonth() - returns int (0-11) */
    Date* date_obj;
    uint8_t month;
    
    /* Pop object reference */
    object_handle = stack_pop_shared(ctx);
    
    /* Call native method */
    date_obj = (Date*)(uintptr_t)object_handle;
    month = date_get_month(date_obj);
    
    /* Push result */
    stack_push_shared(ctx, (uint16_t)month);
    break;
}
else if (strcmp(method_name, "getDate") == 0) {
    /* Date.getDate() - returns int (1-31) */
    Date* date_obj;
    uint8_t day;
    
    /* Pop object reference */
    object_handle = stack_pop_shared(ctx);
    
    /* Call native method */
    date_obj = (Date*)(uintptr_t)object_handle;
    day = date_get_date(date_obj);
    
    /* Push result */
    stack_push_shared(ctx, (uint16_t)day);
    break;
}
else if (strcmp(method_name, "getHours") == 0) {
    /* Date.getHours() - returns int (0-23) */
    Date* date_obj;
    uint8_t hours;
    
    /* Pop object reference */
    object_handle = stack_pop_shared(ctx);
    
    /* Call native method */
    date_obj = (Date*)(uintptr_t)object_handle;
    hours = date_get_hours(date_obj);
    
    /* Push result */
    stack_push_shared(ctx, (uint16_t)hours);
    break;
}
else if (strcmp(method_name, "getMinutes") == 0) {
    /* Date.getMinutes() - returns int (0-59) */
    Date* date_obj;
    uint8_t minutes;
    
    /* Pop object reference */
    object_handle = stack_pop_shared(ctx);
    
    /* Call native method */
    date_obj = (Date*)(uintptr_t)object_handle;
    minutes = date_get_minutes(date_obj);
    
    /* Push result */
    stack_push_shared(ctx, (uint16_t)minutes);
    break;
}
else if (strcmp(method_name, "getSeconds") == 0) {
    /* Date.getSeconds() - returns int (0-59) */
    Date* date_obj;
    uint8_t seconds;
    
    /* Pop object reference */
    object_handle = stack_pop_shared(ctx);
    
    /* Call native method */
    date_obj = (Date*)(uintptr_t)object_handle;
    seconds = date_get_seconds(date_obj);
    
    /* Push result */
    stack_push_shared(ctx, (uint16_t)seconds);
    break;
}
```

#### 2. Test Each Method
Create comprehensive test:
```java
class DateMethods {
    public static void main() {
        Date d = new Date(1705320645);
        
        int year = d.getFullYear();
        int month = d.getMonth();
        int day = d.getDate();
        int hours = d.getHours();
        int minutes = d.getMinutes();
        int seconds = d.getSeconds();
        
        System.out.println("Year: ");
        System.out.println(year);
        System.out.println("Month: ");
        System.out.println(month);
        
        return;
    }
}
```

### Success Criteria
- ✅ All getter methods work correctly
- ✅ setTime() updates Date object
- ✅ Values match expected results
- ✅ No crashes or stack corruption

## Day 4: Java Test Programs

### Goal
Create comprehensive Java test programs to validate Date functionality.

### Test Files to Create

#### 1. tests/date/date1.jav - Basic Date Creation
```java
class date1 {
    public static void main() {
        Date d = new Date();
        int year = d.getFullYear();
        
        System.out.println("Current year: ");
        System.out.println(year);
        
        return;
    }
}
```

#### 2. tests/date/date2.jav - Date with Timestamp
```java
class date2 {
    public static void main() {
        Date d = new Date(1705320645);
        
        int year = d.getFullYear();
        int month = d.getMonth();
        int day = d.getDate();
        
        System.out.println("Year: ");
        System.out.println(year);
        System.out.println("Month: ");
        System.out.println(month);
        System.out.println("Day: ");
        System.out.println(day);
        
        return;
    }
}
```

#### 3. tests/date/date3.jav - All Getters
```java
class date3 {
    public static void main() {
        Date d = new Date(1705320645);
        
        int year = d.getFullYear();
        int month = d.getMonth();
        int day = d.getDate();
        int hours = d.getHours();
        int minutes = d.getMinutes();
        int seconds = d.getSeconds();
        
        System.out.println("Date: ");
        System.out.println(year);
        System.out.println(month);
        System.out.println(day);
        System.out.println("Time: ");
        System.out.println(hours);
        System.out.println(minutes);
        System.out.println(seconds);
        
        return;
    }
}
```

#### 4. tests/date/date4.jav - setTime Test
```java
class date4 {
    public static void main() {
        Date d = new Date();
        
        int year1 = d.getFullYear();
        System.out.println("Before: ");
        System.out.println(year1);
        
        d.setTime(1609459200);
        
        int year2 = d.getFullYear();
        System.out.println("After: ");
        System.out.println(year2);
        
        return;
    }
}
```

#### 5. tests/date/date5.jav - Multiple Date Objects
```java
class date5 {
    public static void main() {
        Date d1 = new Date(1705320645);
        Date d2 = new Date(1609459200);
        
        int year1 = d1.getFullYear();
        int year2 = d2.getFullYear();
        
        System.out.println("Date 1: ");
        System.out.println(year1);
        System.out.println("Date 2: ");
        System.out.println(year2);
        
        return;
    }
}
```

### Test Execution

#### Create test batch file: tests/date/rundate.bat
```batch
@echo off
echo ========================================
echo Date Java Integration Tests
echo ========================================
echo.

echo Test 1: Basic Date Creation
djc date1.jav
djvm date1.djc
echo.

echo Test 2: Date with Timestamp
djc date2.jav
djvm date2.djc
echo.

echo Test 3: All Getters
djc date3.jav
djvm date3.djc
echo.

echo Test 4: setTime Test
djc date4.jav
djvm date4.djc
echo.

echo Test 5: Multiple Date Objects
djc date5.jav
djvm date5.djc
echo.

echo ========================================
echo All tests completed
echo ========================================
```

### Success Criteria
- ✅ All test programs compile without errors
- ✅ All test programs run without crashes
- ✅ Output values are correct
- ✅ Multiple Date objects work independently

## Day 5: Integration Testing and Documentation

### Goal
Verify complete integration and document the Java API.

### Integration Tests

#### 1. Date with I/O Integration
Create `tests/date/dateio.jav`:
```java
class dateio {
    public static void main() {
        Date d = new Date();
        int year = d.getFullYear();
        int month = d.getMonth();
        int day = d.getDate();
        
        FileOutputStream fos = new FileOutputStream("date.txt");
        BufferedWriter bw = new BufferedWriter(fos);
        
        bw.write("Date: ");
        bw.write(year);
        bw.write("-");
        bw.write(month);
        bw.write("-");
        bw.write(day);
        bw.newLine();
        
        bw.close();
        fos.close();
        
        System.out.println("Date written to file");
        return;
    }
}
```

#### 2. Regression Testing
Run all existing tests to ensure Date integration doesn't break anything:
```batch
cd tests
runtest.bat
```

### Documentation

#### 1. Create PHASE3_5_3_JAVA_API.md
Document the Java API for Date class:
- Constructor syntax
- Method signatures
- Usage examples
- Limitations
- Best practices

#### 2. Update tests/date/README.md
Add Java integration section:
- Java test programs
- Expected output
- Running instructions

#### 3. Update PHASE3_5_SUMMARY.md
Add Phase 3.5.3 completion status.

#### 4. Update README.md
Add Date to supported features list.

### Success Criteria
- ✅ Date works with I/O classes
- ✅ All regression tests pass
- ✅ Documentation complete
- ✅ Examples work correctly

## Technical Considerations

### Memory Management
- Date objects allocated on heap via date_new()
- VM holds 16-bit pointer (Small memory model)
- No explicit garbage collection yet
- Objects persist until program exit

### Type Limitations
- Java `long` mapped to C `uint32_t` (32-bit)
- Timestamps limited to 2038 (Unix Y2038 problem)
- DOS date range: 1980-2099
- Month values: 0-11 (JavaScript convention)

### Performance
- First method call: ~100 CPU cycles (cache miss)
- Cached method calls: ~10 CPU cycles
- Acceptable for DOS environment

### Error Handling
- NULL checks on object creation
- Invalid timestamp handling
- Stack overflow protection
- Clear error messages

## Implementation Order

### Day 1 (Semantic Analyzer)
1. Modify semantic.c
2. Test compilation
3. Verify no errors

### Day 2 (OP_NEW)
1. Add date.h include
2. Implement Date constructors
3. Update Makefile
4. Test object creation

### Day 3 (OP_INVOKE_VIRTUAL)
1. Implement getTime/setTime
2. Implement getFullYear/getMonth/getDate
3. Implement getHours/getMinutes/getSeconds
4. Test each method

### Day 4 (Java Tests)
1. Create 5 test programs
2. Create rundate.bat
3. Run all tests
4. Verify output

### Day 5 (Integration & Docs)
1. Create dateio.jav
2. Run regression tests
3. Write documentation
4. Update existing docs

## Expected Timeline

- **Day 1**: 2-3 hours (semantic analyzer)
- **Day 2**: 3-4 hours (OP_NEW implementation)
- **Day 3**: 4-5 hours (OP_INVOKE_VIRTUAL implementation)
- **Day 4**: 2-3 hours (Java test programs)
- **Day 5**: 2-3 hours (integration and documentation)

**Total**: 13-18 hours over 5 days

## Deliverables

### Code
- ✅ Modified semantic.c
- ✅ Modified interpreter.c
- ✅ Updated Makefile
- ✅ 5 Java test programs
- ✅ Integration test program
- ✅ Test batch files

### Documentation
- ✅ PHASE3_5_3_JAVA_API.md
- ✅ Updated tests/date/README.md
- ✅ Updated PHASE3_5_SUMMARY.md
- ✅ Updated README.md

### Testing
- ✅ All Java tests passing
- ✅ Regression tests passing
- ✅ Integration tests passing
- ✅ DOSBox-X validation

## Risk Mitigation

### Potential Issues
1. **Stack corruption**: Careful argument handling
2. **Memory leaks**: Proper object lifecycle
3. **Type mismatches**: Explicit casting
4. **DOS limitations**: Document constraints

### Mitigation Strategies
1. Extensive testing at each step
2. Follow existing I/O class patterns
3. Clear error messages
4. Comprehensive documentation

## Success Metrics

- ✅ Date class usable from Java
- ✅ All methods functional
- ✅ No crashes or errors
- ✅ Performance acceptable
- ✅ Documentation complete
- ✅ Tests comprehensive

---

**Document Version**: 1.0  
**Created**: 2026-05-16  
**Status**: Ready for Implementation