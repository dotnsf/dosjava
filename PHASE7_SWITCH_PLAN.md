# Phase 7: Switch Statement Implementation Plan

## Overview
Implement switch-case statements in dosjava, supporting int, long, and String types. This phase adds switch statement support to the compiler and VM, enabling multi-way branching with case labels and default handling.

## Date
Start: 2026-05-23

## Goals
1. Implement switch statement parsing and AST representation
2. Add switch statement semantic analysis (type checking)
3. Implement code generation using if-else chain strategy
4. Support int, long, and String switch expressions
5. Support break statements within switch
6. Support default case handling
7. Create comprehensive test suite

## Technical Approach

### Implementation Strategy: If-Else Chain

Instead of jump tables (complex, memory-intensive), we use if-else chains:

```java
switch (expr) {
    case value1: stmt1; break;
    case value2: stmt2; break;
    default: stmt3;
}

↓ Compiled to bytecode equivalent of:

temp = expr;
if (temp == value1) goto case1;
if (temp == value2) goto case2;
goto default;
case1: stmt1; goto end;
case2: stmt2; goto end;
default: stmt3;
end:
```

### String Switch Strategy

For String type, use `String.equals()` method (already implemented in Phase 2):

```java
switch (str) {
    case "hello": stmt1; break;
    case "world": stmt2; break;
}

↓ Compiled to:

temp = str;
if (temp.equals("hello")) goto case1;
if (temp.equals("world")) goto case2;
goto end;
case1: stmt1; goto end;
case2: stmt2; goto end;
end:
```

### Memory Considerations

- **No jump table**: Saves memory, simpler implementation
- **Linear search**: O(n) case matching, acceptable for typical switch statements
- **Reuse existing opcodes**: GOTO, IF_EQ, INVOKE_VIRTUAL (for String.equals)
- **Stack usage**: Minimal - only stores switch expression value temporarily

---

## Phase 7.1: Basic Switch Support (Days 1-3)

### Duration: 3 days

### Objectives
- Implement switch statement parsing
- Add switch AST nodes
- Implement semantic analysis for int/long switch
- Generate bytecode for int/long switch
- Support break statements

---

### Day 1: Lexer and Parser Updates

**Duration:** 1 day

#### Tasks

1. **Add switch/case/default/break keywords to lexer** (30 min)
   - File: [`tools/compiler/lexer.h`](tools/compiler/lexer.h)
   - Add token types:
     ```c
     TOK_SWITCH,    /* switch */
     TOK_CASE,      /* case */
     TOK_DEFAULT,   /* default */
     TOK_BREAK,     /* break */
     TOK_COLON      /* : */
     ```
   - File: [`tools/compiler/lexer.c`](tools/compiler/lexer.c)
   - Add keyword recognition in `lexer_next_token()`

2. **Add switch AST node types** (30 min)
   - File: [`tools/compiler/ast.h`](tools/compiler/ast.h)
   - Add node types:
     ```c
     NODE_SWITCH,        /* Switch statement */
     NODE_CASE,          /* Case label */
     NODE_DEFAULT,       /* Default label */
     NODE_BREAK          /* Break statement */
     ```
   - Add AST node structures:
     ```c
     /* Switch statement */
     struct {
         uint16_t expr;          /* Switch expression */
         uint16_t case_count;    /* Number of cases */
         uint16_t first_case;    /* Index of first case */
         uint16_t has_default;   /* 1 if has default case */
         uint16_t default_stmt;  /* Index of default statement */
     } switch_stmt;
     
     /* Case label */
     struct {
         uint16_t value;         /* Case value expression */
         uint16_t stmt;          /* Case statement */
         uint16_t next_case;     /* Index of next case (linked list) */
     } case_label;
     
     /* Break statement */
     struct {
         uint16_t unused;        /* Reserved for future use */
     } break_stmt;
     ```

3. **Implement switch statement parser** (2 hours)
   - File: [`tools/compiler/parser.c`](tools/compiler/parser.c)
   - Add `parse_switch_stmt()` function:
     ```c
     static uint16_t parse_switch_stmt(Parser* parser) {
         // switch (expr) { case value: stmt; ... default: stmt; }
         // 1. Parse switch keyword and (
         // 2. Parse switch expression
         // 3. Parse )
         // 4. Parse {
         // 5. Parse case labels and statements
         // 6. Parse optional default label
         // 7. Parse }
         // 8. Build switch AST node with linked case list
     }
     ```
   - Add `parse_case_label()` function
   - Add `parse_break_stmt()` function
   - Update `parse_statement()` to handle TOK_SWITCH and TOK_BREAK

4. **Test parser with simple switch** (30 min)
   - Create test file: [`tests/swsimple.jav`](tests/swsimple.jav)
   - Test basic int switch with 2-3 cases
   - Verify AST generation

**Deliverables:**
- Updated lexer with switch keywords
- Switch AST node definitions
- Switch statement parser
- Basic test file

---

### Day 2: Semantic Analysis

**Duration:** 1 day

#### Tasks

1. **Implement switch expression type checking** (1 hour)
   - File: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
   - Add `check_switch_stmt()` function:
     ```c
     static int check_switch_stmt(SemanticAnalyzer* analyzer, uint16_t switch_node) {
         // 1. Check switch expression type (must be int, long, or String)
         // 2. Store expression type for case value checking
         // 3. Check all case labels
         // 4. Check default statement if present
         // 5. Verify no duplicate case values
     }
     ```
   - Validate switch expression type (int, long only for Phase 7.1)
   - Store expression type in symbol table context

2. **Implement case label type checking** (1 hour)
   - Add `check_case_label()` function:
     ```c
     static int check_case_label(SemanticAnalyzer* analyzer, uint16_t case_node, TypeInfo switch_type) {
         // 1. Check case value is constant expression
         // 2. Check case value type matches switch expression type
         // 3. Check case statement
         // 4. Verify case value is unique (no duplicates)
     }
     ```
   - Verify case values are compile-time constants
   - Verify case value types match switch expression type
   - Detect duplicate case values (error)

3. **Implement break statement checking** (30 min)
   - Add `check_break_stmt()` function
   - Verify break is inside switch or loop
   - Track switch/loop nesting level in analyzer context

4. **Add switch context tracking** (1 hour)
   - Add to SemanticAnalyzer structure:
     ```c
     uint16_t in_switch;         /* 1 if inside switch statement */
     TypeInfo switch_expr_type;  /* Type of current switch expression */
     ```
   - Update context when entering/exiting switch
   - Use for break statement validation

5. **Test semantic analysis** (30 min)
   - Test valid switch statements
   - Test invalid cases:
     - Non-constant case values
     - Type mismatch (int switch with long case)
     - Duplicate case values
     - Break outside switch/loop

**Deliverables:**
- Switch semantic analysis functions
- Case label type checking
- Break statement validation
- Context tracking for switch statements

---

### Day 3: Code Generation for Int/Long Switch

**Duration:** 1 day

#### Tasks

1. **Implement switch code generation** (2 hours)
   - File: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
   - Add `generate_switch_stmt()` function:
     ```c
     static int generate_switch_stmt(CodeGen* codegen, uint16_t switch_node) {
         // Strategy: If-else chain
         // 1. Generate code for switch expression
         // 2. Store result in temporary variable (DUP for comparisons)
         // 3. For each case:
         //    - Generate comparison (expr == case_value)
         //    - Generate conditional jump to case body
         // 4. Generate jump to default (or end if no default)
         // 5. Generate case bodies with labels
         // 6. Generate default body if present
         // 7. Generate end label
     }
     ```
   - Use if-else chain strategy (not jump table)
   - Generate labels for each case and end

2. **Implement case label code generation** (1 hour)
   - Generate comparison code for each case
   - Generate conditional jump to case body
   - Handle both int and long comparisons:
     - int: Use OP_EQ + OP_IF_TRUE
     - long: Use OP_LEQ + OP_IF_TRUE

3. **Implement break statement code generation** (30 min)
   - Add `generate_break_stmt()` function
   - Generate GOTO to switch end label
   - Track switch end label in codegen context:
     ```c
     uint16_t switch_end_label;  /* Label for switch end */
     ```

4. **Add label management** (1 hour)
   - Enhance label generation for switch:
     ```c
     uint16_t codegen_alloc_label(CodeGen* codegen);
     int codegen_emit_label(CodeGen* codegen, uint16_t label);
     int codegen_emit_goto_label(CodeGen* codegen, uint16_t label);
     ```
   - Track label positions for backpatching

5. **Test code generation** (30 min)
   - Test int switch with 2-3 cases
   - Test long switch
   - Test switch with default
   - Test switch with break statements
   - Verify generated bytecode

**Deliverables:**
- Switch code generation (if-else chain)
- Case label code generation
- Break statement code generation
- Label management for switch

---

## Phase 7.2: String Switch Support (Days 4-5)

### Duration: 2 days

### Objectives
- Extend semantic analysis for String switch
- Implement String comparison in switch
- Test String switch statements

---

### Day 4: String Switch Semantic Analysis

**Duration:** 1 day

#### Tasks

1. **Update switch expression type checking** (1 hour)
   - File: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
   - Modify `check_switch_stmt()` to accept String type:
     ```c
     // Allow: TYPE_INT, TYPE_LONG, TYPE_CLASS (String only)
     if (expr_type.kind == TYPE_CLASS) {
         const char* class_name = get_string(expr_type.class_name);
         if (strcmp(class_name, "String") != 0) {
             semantic_error("Switch expression must be int, long, or String");
             return -1;
         }
     }
     ```

2. **Update case label type checking** (1 hour)
   - Modify `check_case_label()` for String literals:
     ```c
     // For String switch, case values must be string literals
     if (switch_type.kind == TYPE_CLASS) {
         if (case_value_node->type != NODE_LITERAL_STRING) {
             semantic_error("String switch requires string literal case values");
             return -1;
         }
     }
     ```
   - Verify case values are string literals (not variables)

3. **Test String switch semantic analysis** (1 hour)
   - Test valid String switch
   - Test invalid cases:
     - Non-String class in switch expression
     - Non-literal case values
     - Duplicate string case values

**Deliverables:**
- String type support in switch semantic analysis
- String literal validation for case labels
- Test cases for String switch

---

### Day 5: String Switch Code Generation

**Duration:** 1 day

#### Tasks

1. **Implement String comparison in switch** (2 hours)
   - File: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
   - Modify `generate_switch_stmt()` for String type:
     ```c
     if (switch_expr_type.kind == TYPE_CLASS) {
         // String switch: use String.equals() for comparison
         // For each case:
         // 1. DUP switch expression (keep on stack)
         // 2. PUSH case string literal
         // 3. INVOKE_VIRTUAL String.equals(String)
         // 4. IF_TRUE jump to case body
     }
     ```
   - Use `String.equals()` method (already implemented)
   - Generate INVOKE_VIRTUAL for equals() call

2. **Handle String literals in case labels** (1 hour)
   - Load string literals from constant pool
   - Generate PUSH_STRING for case values
   - Ensure string literals are added to constant pool

3. **Optimize String switch** (optional, 30 min)
   - Consider caching switch expression in local variable
   - Avoid repeated DUP operations if possible

4. **Test String switch code generation** (1 hour)
   - Create test file: [`tests/swstring.jav`](tests/swstring.jav)
   - Test String switch with 3-4 cases
   - Test with default case
   - Test with break statements
   - Verify correct execution in DOSBox-X

**Deliverables:**
- String switch code generation
- String.equals() integration
- String switch test file
- Verified execution

---

## Phase 7.3: Advanced Features (Day 6)

### Duration: 1 day

### Objectives
- Implement fall-through behavior (optional)
- Add switch statement optimizations
- Comprehensive testing

---

### Day 6: Fall-through and Testing

**Duration:** 1 day

#### Tasks

1. **Implement fall-through behavior** (1 hour)
   - Modify code generation to support fall-through:
     ```java
     switch (x) {
         case 1:
         case 2:
             System.out.println("1 or 2");
             break;
         case 3:
             System.out.println("3");
             // fall through
         case 4:
             System.out.println("3 or 4");
             break;
     }
     ```
   - Generate code without break for fall-through cases
   - Test fall-through behavior

2. **Create comprehensive test suite** (2 hours)
   - Create test files:
     - [`tests/swint.jav`](tests/swint.jav) - Int switch with multiple cases
     - [`tests/swlong.jav`](tests/swlong.jav) - Long switch
     - [`tests/swstring.jav`](tests/swstring.jav) - String switch
     - [`tests/swdefault.jav`](tests/swdefault.jav) - Default case handling
     - [`tests/swfall.jav`](tests/swfall.jav) - Fall-through behavior
     - [`tests/swnested.jav`](tests/swnested.jav) - Nested switch statements
   - Test edge cases:
     - Empty switch (no cases)
     - Single case
     - Many cases (10+)
     - No default case
     - Break in nested loops

3. **Performance testing** (30 min)
   - Test switch with many cases (10-20)
   - Compare with equivalent if-else chain
   - Measure execution time in DOSBox-X

4. **Documentation** (30 min)
   - Document switch statement syntax
   - Document limitations (if any)
   - Update README.md with Phase 7 completion

**Deliverables:**
- Fall-through support
- Comprehensive test suite (6+ test files)
- Performance measurements
- Documentation updates

---

## Success Criteria

### Phase 7.1 (Days 1-3)
- ✅ Switch statement parsing works for int/long
- ✅ Semantic analysis validates switch expressions and case labels
- ✅ Code generation produces correct bytecode (if-else chain)
- ✅ Break statements work correctly
- ✅ Basic int switch test passes in DOSBox-X

### Phase 7.2 (Days 4-5)
- ✅ String switch semantic analysis works
- ✅ String.equals() integration in switch works
- ✅ String switch test passes in DOSBox-X
- ✅ All three types (int, long, String) work correctly

### Phase 7.3 (Day 6)
- ✅ Fall-through behavior works (optional)
- ✅ All test files pass in DOSBox-X
- ✅ Nested switch statements work
- ✅ Documentation complete

---

## Build Configuration

### Makefile Changes

No changes required - switch statements use existing opcodes (GOTO, IF_TRUE, INVOKE_VIRTUAL).

### Compiler Build

```bash
# Build compiler with switch support
cd dosjava
build_djc.bat
```

### VM Build

No changes required - VM already supports all necessary opcodes.

---

## Test Files

### Test File Structure

```
dosjava/tests/
├── swsimple.jav    # Simple int switch (Day 1)
├── swint.jav       # Int switch with multiple cases (Day 6)
├── swlong.jav      # Long switch (Day 6)
├── swstring.jav    # String switch (Day 5)
├── swdefault.jav   # Default case handling (Day 6)
├── swfall.jav      # Fall-through behavior (Day 6)
└── swnested.jav    # Nested switch statements (Day 6)
```

### Example Test: swsimple.jav

```java
class SwitchSimple {
    public static void main() {
        int x;
        
        x = 1;
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        return;
    }
}
```

### Example Test: swstring.jav

```java
class SwitchString {
    public static void main() {
        String cmd;
        
        cmd = "help";
        switch (cmd) {
            case "help":
                System.out.println("Show help");
                break;
            case "quit":
                System.out.println("Goodbye");
                break;
            case "run":
                System.out.println("Running");
                break;
            default:
                System.out.println("Unknown command");
                break;
        }
        
        return;
    }
}
```

---

## Documentation

### Files to Create/Update

1. **PHASE7_SWITCH_PLAN.md** (this file)
   - Complete implementation plan
   - Technical approach
   - Day-by-day tasks

2. **PHASE7_SWITCH_COMPLETION.md** (after completion)
   - Implementation summary
   - Test results
   - Known limitations
   - Performance measurements

3. **README.md**
   - Add Phase 7 to feature list
   - Update supported language features
   - Add switch statement examples

4. **COMPILER_DESIGN.md**
   - Add switch statement to AST node types
   - Document switch code generation strategy
   - Add switch statement examples

---

## Performance Considerations

### Expected Performance

- **If-else chain**: O(n) case matching
  - Acceptable for typical switch statements (< 10 cases)
  - First case is fastest, last case is slowest
  
- **Memory usage**: Minimal
  - No jump table allocation
  - Only temporary storage for switch expression
  
- **Code size**: Moderate
  - Each case adds ~10-15 bytes of bytecode
  - Comparable to equivalent if-else chain

### Optimization Opportunities

1. **Case ordering**: Place most common cases first
2. **Range optimization**: Group consecutive cases
3. **String interning**: Cache string literals (future)

---

## Risk Mitigation

### Potential Issues

1. **Complex fall-through**: May be difficult to implement correctly
   - **Mitigation**: Start with simple cases, add fall-through later
   
2. **String comparison overhead**: String.equals() is slower than int comparison
   - **Mitigation**: Document performance characteristics, acceptable for typical use
   
3. **Many cases**: Linear search may be slow for 20+ cases
   - **Mitigation**: Document limitation, suggest if-else for many cases

### Contingency Plans

1. **If fall-through is too complex**: Skip it, require explicit break
2. **If String switch is too slow**: Document as limitation, suggest alternatives
3. **If memory is insufficient**: Reduce maximum case count

---

## Timeline Summary

| Phase | Duration | Description |
|-------|----------|-------------|
| 7.1 Day 1 | 1 day | Lexer and parser updates |
| 7.1 Day 2 | 1 day | Semantic analysis |
| 7.1 Day 3 | 1 day | Code generation (int/long) |
| 7.2 Day 4 | 1 day | String switch semantic analysis |
| 7.2 Day 5 | 1 day | String switch code generation |
| 7.3 Day 6 | 1 day | Fall-through and testing |
| **Total** | **6 days** | **Complete switch implementation** |

---

## Next Steps After Phase 7

1. **Phase 8**: Enhanced exception handling
   - Multiple catch blocks
   - Exception hierarchy
   - Finally block execution

2. **Phase 9**: Additional data types
   - double type (64-bit float)
   - char type (16-bit character)
   - byte/short types

3. **Phase 10**: Standard library expansion
   - StringBuilder class
   - ArrayList class
   - HashMap class (simple)

---

## References

### Java Language Specification
- JLS §14.11: Switch Statements
- JLS §14.15: Break Statement

### Implementation References
- Phase 6 Float Plan: [`PHASE6_FLOAT_PLAN.md`](PHASE6_FLOAT_PLAN.md)
- Compiler Design: [`COMPILER_DESIGN.md`](COMPILER_DESIGN.md)
- AST Definitions: [`tools/compiler/ast.h`](tools/compiler/ast.h)

---

**End of Phase 7 Plan**