# Date Class Test Documentation

## Overview

This directory contains test programs for the Date class implementation in the dosjava runtime library.

## Date Class Features

The Date class provides date and time functionality for DOS Java programs:

### Constructor Methods
- `date_new()` - Creates a Date object with the current system time
- `date_new_with_time(uint32_t time_sec)` - Creates a Date object with a specific Unix timestamp

### Getter Methods
- `date_get_time(Date* date)` - Returns the Unix timestamp (seconds since 1970-01-01 UTC)
- `date_get_full_year(Date* date)` - Returns the 4-digit year (1980-2099)
- `date_get_month(Date* date)` - Returns the month (0-11, where 0=January)
- `date_get_date(Date* date)` - Returns the day of month (1-31)
- `date_get_hours(Date* date)` - Returns the hour (0-23)
- `date_get_minutes(Date* date)` - Returns the minute (0-59)
- `date_get_seconds(Date* date)` - Returns the second (0-59)

### Setter Methods
- `date_set_time(Date* date, uint32_t time_sec)` - Sets the Date to a specific Unix timestamp

### Utility Methods
- `date_to_string(Date* date, char* buffer, size_t size)` - Formats the date as "YYYY-MM-DD HH:MM:SS"

## Implementation Details

### Caching Strategy
The Date class uses a lazy caching mechanism to optimize performance:
- Date/time components are only calculated when first accessed
- Cache is invalidated when `date_set_time()` is called
- Subsequent getter calls use cached values until invalidation

### DOS Time API Integration
The Date class uses the DOS Time API (dostime.h) for:
- Getting current system date/time via INT 21h
- Converting between DOS date/time and Unix timestamps
- Handling DOS epoch limitations (1980-2099)

### Month Convention
Following JavaScript Date API convention:
- Months are 0-based: 0=January, 1=February, ..., 11=December
- This differs from DOS API which uses 1-based months

## Test Programs

### C Test Programs

#### test_date.c
Comprehensive test program with 6 test suites:

1. **Test 1: date_new()** - Current time creation
   - Creates Date with current system time
   - Validates timestamp is reasonable (> year 2020)

2. **Test 2: date_new_with_time()** - Specific time creation
   - Creates Date with known timestamp (2024-01-15 12:30:45)
   - Validates all date/time components

3. **Test 3: Date component getters**
   - Tests all getter methods
   - Validates caching mechanism

4. **Test 4: date_set_time()** - Time modification
   - Changes Date to different timestamp
   - Validates cache invalidation
   - Confirms new values are correct

5. **Test 5: date_to_string()** - String formatting
   - Tests string output format
   - Validates "YYYY-MM-DD HH:MM:SS" format

6. **Test 6: Current time validation**
   - Creates Date with current time

### Java Test Programs

The following Java test programs validate the Date class integration with the dosjava compiler and VM:

#### dtest1.jav - Date Class Registration Test
- **Purpose**: Verify that the Date class is recognized by the compiler
- **Test**: Creates a Date object with `new Date()`
- **Expected**: Compiles successfully without errors
- **Run**: `test_d1.bat`

#### dtest2.jav - Date Constructor Test
- **Purpose**: Test Date object creation with both constructors
- **Tests**:
  - `Date()` - Creates Date with current time
  - `Date(int timestamp)` - Creates Date with specific timestamp
- **Expected**: Both constructors work correctly
- **Run**: `test_d2.bat`

#### dtest3.jav - Date Getter Methods Test
- **Purpose**: Test all Date getter methods with current time
- **Tests**:
  - `getFullYear()` - Returns current year
  - `getMonth()` - Returns current month (0-11)
  - `getDate()` - Returns current day (1-31)
  - `getHours()` - Returns current hour (0-23)
  - `getMinutes()` - Returns current minute (0-59)
  - `getSeconds()` - Returns current second (0-59)
- **Expected**: All getters return reasonable values for current time
- **Run**: `test_d3.bat`

#### dtest4.jav - Date setTime/getTime Test
- **Purpose**: Test time modification and retrieval
- **Tests**:
  - Create Date with timestamp 10000
  - Verify date components (1970-01-01 02:46:40)
  - Call `setTime(20000)`
  - Verify new date components (1970-01-01 05:33:20)
  - Call `getTime()` and verify it returns 20000
- **Expected**: setTime updates the date, getTime returns correct timestamp
- **Run**: `test_d4.bat`

#### dtest5.jav - Multiple Date Objects Test
- **Purpose**: Test creating and managing multiple Date objects
- **Tests**:
  - Create 3 Date objects with different timestamps
  - Verify each object maintains its own state
  - Modify one object with setTime()
  - Verify other objects are unaffected
- **Expected**: Each Date object is independent
- **Run**: `test_d5.bat`

#### dtest6.jav - Date Formatting Test
- **Purpose**: Demonstrate date formatting in Java
- **Tests**:
  - Get current date/time components
  - Format as "YYYY-MM-DD HH:MM:SS" using println
  - Display individual components with labels
- **Expected**: Formatted date output with proper zero-padding
- **Run**: `test_d6.bat`

#### run_all.bat - Run All Java Tests
- **Purpose**: Execute all Java Date tests in sequence
- **Tests**: Runs dtest1 through dtest6
- **Expected**: All tests pass
- **Run**: `run_all.bat`

### Java API Usage Examples

```java
// Example 1: Get current date and time
Date now = new Date();
int year = now.getFullYear();
int month = now.getMonth();  // 0-11
int day = now.getDate();     // 1-31

// Example 2: Create date with specific timestamp
Date d = new Date(10000);
int hours = d.getHours();
int minutes = d.getMinutes();
int seconds = d.getSeconds();

// Example 3: Modify date
Date d = new Date();
d.setTime(20000);
int timestamp = d.getTime();

// Example 4: Format date for display
Date d = new Date();
System.out.println("Year: ");
System.out.println(d.getFullYear());
System.out.println("Month: ");
System.out.println(d.getMonth() + 1);  // Display as 1-12
```

### Important Notes for Java Programs

1. **16-bit Integer Limitation**: dosjava only supports 16-bit integers (-32768 to 32767)
   - Large timestamps (like 1705320645) will overflow
   - Use small timestamps (< 32767) or current time constructor `Date()`
   - `getTime()` returns int, not long

2. **Month Convention**: Months are 0-based (0=January, 11=December)
   - To display as 1-12, add 1: `month + 1`

3. **No String Concatenation**: Use multiple `println()` calls for formatting
   - Cannot do: `"Year: " + year`
   - Must do: `println("Year: "); println(year);`

   - Validates year is reasonable (>= 2024)
   - Checks month and day ranges

## Building

The Date class and tests are built as part of the main dosjava build:

```batch
cd dosjava
build_all.bat
```

This creates:
- `build/bin/tdate.exe` - Date class test program

## Running Tests

### In DOSBox-X

1. Mount the dosjava directory:
   ```
   mount c c:\path\to\dosjava
   c:
   cd build\bin
   ```

2. Run the test batch file:
   ```
   tdate.bat
   ```

3. Or run directly:
   ```
   tdate.exe
   ```

### Expected Output

```
========================================
Date Class Test Program
========================================

Test 1: date_new() - Current time
  Created Date with current time
  Timestamp: 1715850000 (example)
  PASS

Test 2: date_new_with_time() - Specific time
  Created Date with timestamp: 1705320645
  Expected: 2024-01-15 12:30:45
  Year: 2024, Month: 0 (Jan), Day: 15
  Time: 12:30:45
  PASS

Test 3: Date component getters
  Testing all getter methods...
  get_time(): 1705320645
  get_full_year(): 2024
  get_month(): 0
  get_date(): 15
  get_hours(): 12
  get_minutes(): 30
  get_seconds(): 45
  PASS

Test 4: date_set_time() - Time modification
  Original timestamp: 1705320645
  New timestamp: 1609459200 (2021-01-01 00:00:00)
  After set_time():
  Year: 2021, Month: 0, Day: 1
  Time: 00:00:00
  PASS

Test 5: date_to_string() - String formatting
  Date string: "2024-01-15 12:30:45"
  PASS

Test 6: Current time validation
  Current Date created
  Year: 2024 (or later)
  Month: 0-11 range
  Day: 1-31 range
  PASS

========================================
All 6 tests passed!
========================================
```

## DOS Limitations

### Date Range
- Minimum: 1980-01-01 00:00:00
- Maximum: 2099-12-31 23:59:59
- Dates outside this range may produce undefined behavior

### Time Resolution
- DOS time has 2-second resolution for seconds
- Hundredths of seconds are available but not exposed in Date API

### Time Zone
- DOS time is local time (no timezone support)
- Unix timestamps are treated as UTC
- No automatic timezone conversion

## Integration with VM

The Date class is designed to be integrated with the dosjava VM:

### Native Method Mapping
```java
// Java code (future implementation)
Date d = new Date();
int year = d.getFullYear();
int month = d.getMonth();  // 0-11
String str = d.toString();
```

### VM Opcodes
- `OP_NEW` with Date class ID creates Date objects
- Native method calls invoke C functions via function pointers
- Date objects are managed by VM garbage collector

## Performance Considerations

### Caching Benefits
- First getter call: ~100 CPU cycles (DOS INT 21h + conversion)
- Cached getter calls: ~10 CPU cycles (memory access only)
- Cache invalidation: ~5 CPU cycles (flag reset)

### Memory Usage
- Date object: 16 bytes (base Object + timestamp + cache)
- Minimal overhead for caching mechanism

## Future Enhancements

Potential improvements for future phases:
1. Timezone support (if DOS timezone info available)
2. Date arithmetic (add/subtract days, months, years)
3. Date comparison methods (before, after, equals)
4. Locale-specific formatting
5. Parse date strings into Date objects

## Related Documentation

- `tests/dostime/README.md` - DOS Time API documentation
- `src/runtime/date.h` - Date class header
- `src/runtime/date.c` - Date class implementation
- `src/runtime/dostime.h` - DOS Time API header
- `src/runtime/dostime.c` - DOS Time API implementation

## Version History

- **2026-05-16**: Initial implementation
  - Basic Date class with current time and timestamp support
  - All getter/setter methods
  - String formatting
  - Comprehensive test suite
  - Caching optimization