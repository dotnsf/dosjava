# Phase 6.3: Math Class Implementation Plan

**Target**: Implement Math class with basic mathematical functions  
**Duration**: Days 13-16 (4 days)  
**Status**: Planning

## Overview

Implement a Math class with essential mathematical functions as native methods. All methods will be static and work with float types.

## Implementation Strategy

### 1. Native Method Approach

Math functions will be implemented as native methods in C, similar to System.out.println(). This approach:
- Provides efficient implementation using C math library
- Avoids complex bytecode for mathematical operations
- Maintains consistency with existing native method pattern

### 2. Function Categories

#### Day 13: Basic Functions
- `Math.abs(float)` - Absolute value
- `Math.min(float, float)` - Minimum of two values
- `Math.max(float, float)` - Maximum of two values
- `Math.sqrt(float)` - Square root

#### Day 14: Trigonometric Functions
- `Math.sin(float)` - Sine (radians)
- `Math.cos(float)` - Cosine (radians)
- `Math.tan(float)` - Tangent (radians)

#### Day 15: Exponential Functions
- `Math.pow(float, float)` - Power (base^exponent)
- `Math.exp(float)` - Exponential (e^x)
- `Math.log(float)` - Natural logarithm

#### Day 16: Testing & Documentation
- Comprehensive test suite
- Documentation
- Performance verification

## Technical Design

### Native Method Signature

All Math methods follow this pattern:
```c
static int native_math_function(
    ExecutionContext* ctx,
    uint16_t* args,
    uint8_t arg_count,
    uint16_t* result
)
```

### Float Parameter Handling

Float values are passed as 2 words (high, low):
```c
uint32_t float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
float value;
memcpy(&value, &float_bits, sizeof(float));
```

### Float Return Value Handling

Float results are returned as 2 words:
```c
uint32_t result_bits;
memcpy(&result_bits, &result_value, sizeof(float));
result[0] = (uint16_t)(result_bits >> 16);  /* high word */
result[1] = (uint16_t)(result_bits & 0xFFFF); /* low word */
```

### Method Registration

Each Math method must be registered in `native_register_builtins()`:
```c
native_register(
    "Math",
    "sqrt",
    "(F)F",
    native_math_sqrt,
    2,  /* 2 words for float parameter */
    param_types,
    NATIVE_RETURN_FLOAT
);
```

## Implementation Steps

### Step 1: Update native.h
- Add `NATIVE_PARAM_FLOAT` enum value
- Add `NATIVE_RETURN_FLOAT` enum value
- Update documentation

### Step 2: Implement Math Functions in native.c
- Add math.h include
- Implement each Math function as native method
- Handle float parameter conversion
- Handle float return value conversion

### Step 3: Register Math Methods
- Update `native_register_builtins()` to register all Math methods
- Ensure correct descriptors (e.g., "(F)F", "(FF)F")

### Step 4: Compiler Support
- Verify Math class is recognized in semantic analysis
- Ensure static method calls work correctly
- Test float parameter passing

### Step 5: Create Test Files
- `math_basic.jav` - Test abs, min, max, sqrt
- `math_trig.jav` - Test sin, cos, tan
- `math_exp.jav` - Test pow, exp, log
- `math_all.jav` - Comprehensive test

## Method Specifications

### Math.abs(float)
```java
float x = -3.5f;
float result = Math.abs(x);  // 3.5
```
Implementation: `fabs()` from math.h

### Math.min(float, float)
```java
float result = Math.min(3.5f, 2.1f);  // 2.1
```
Implementation: `fminf()` or conditional

### Math.max(float, float)
```java
float result = Math.max(3.5f, 2.1f);  // 3.5
```
Implementation: `fmaxf()` or conditional

### Math.sqrt(float)
```java
float result = Math.sqrt(9.0f);  // 3.0
```
Implementation: `sqrtf()` from math.h

### Math.sin(float)
```java
float result = Math.sin(0.0f);  // 0.0
```
Implementation: `sinf()` from math.h

### Math.cos(float)
```java
float result = Math.cos(0.0f);  // 1.0
```
Implementation: `cosf()` from math.h

### Math.tan(float)
```java
float result = Math.tan(0.0f);  // 0.0
```
Implementation: `tanf()` from math.h

### Math.pow(float, float)
```java
float result = Math.pow(2.0f, 3.0f);  // 8.0
```
Implementation: `powf()` from math.h

### Math.exp(float)
```java
float result = Math.exp(1.0f);  // 2.718...
```
Implementation: `expf()` from math.h

### Math.log(float)
```java
float result = Math.log(2.718f);  // 1.0
```
Implementation: `logf()` from math.h

## Error Handling

### Invalid Input
- `sqrt(-1.0f)` → Return NaN
- `log(-1.0f)` → Return NaN
- `pow(0.0f, -1.0f)` → Return Inf

C math library handles these cases automatically.

## Testing Strategy

### Unit Tests
Each function will have dedicated tests:
- Normal cases
- Edge cases (0, negative, very large/small)
- Special values (NaN, Inf)

### Integration Tests
Test combinations of Math functions:
```java
float result = Math.sqrt(Math.pow(3.0f, 2.0f) + Math.pow(4.0f, 2.0f));
// Should be 5.0 (Pythagorean theorem)
```

## Build System Updates

### Makefile
Ensure math library is linked:
```makefile
LIBS = -lm
```

### Watcom Compiler
May need to specify math library explicitly.

## Known Limitations

1. **Single Precision Only**: All functions use float (32-bit), not double
2. **Radians Only**: Trigonometric functions use radians, not degrees
3. **No Constants**: Math.PI, Math.E not implemented (can be added later)
4. **Limited Functions**: Only essential functions implemented

## Future Enhancements

- Math.PI, Math.E constants
- Additional functions: asin, acos, atan, atan2
- Degree/radian conversion helpers
- Hyperbolic functions: sinh, cosh, tanh
- Rounding functions: floor, ceil, round

## Success Criteria

- ✅ All 10 Math functions implemented
- ✅ All functions pass unit tests
- ✅ Integration tests pass
- ✅ Documentation complete
- ✅ No memory leaks or crashes
- ✅ Reasonable performance (< 1ms per call)

## Timeline

- **Day 13**: Implement basic functions (abs, min, max, sqrt)
- **Day 14**: Implement trigonometric functions (sin, cos, tan)
- **Day 15**: Implement exponential functions (pow, exp, log)
- **Day 16**: Testing, debugging, and documentation

---
**Phase 6.3 Start Date**: 2026-05-23