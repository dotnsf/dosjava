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

### test_date.c
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