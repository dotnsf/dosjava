# AGENTS.md - AI Assistant Guidelines for DOSJava Project

## Project Overview

**DOSJava** is a Java compiler and runtime environment for 16-bit PC-DOS systems. It compiles a subset of Java to custom bytecode (.djc) and executes it on a stack-based virtual machine.

## Key Information for AI Assistants

### 1. Project Structure

```
dosjava/
├── src/                    # Source code
│   ├── vm/                # Virtual machine (djvm.exe)
│   ├── runtime/           # Runtime library (Object, String, System, Date, Math)
│   └── format/            # .djc format definitions
├── tools/                 # Development tools
│   └── compiler/          # Compiler (djc.exe)
│       ├── lexer.c        # Lexical analyzer
│       ├── parser.c       # Parser (AST generation)
│       ├── semantic.c     # Semantic analyzer
│       └── codegen.c      # Code generator
├── tests/                 # Test files
├── samples/               # Sample programs
├── build/                 # Build output
│   ├── bin/              # Executables (djc.exe, djvm.exe)
│   └── obj/              # Object files
└── docs/                  # Documentation (various PHASE*.md files)
```

### 2. Build System

**Compiler**: Open Watcom v2 C Compiler
**Target**: 16-bit DOS (8086+)
**Memory Model**: Large (multiple segments, far pointers)

**Build Commands**:
```batch
wmake all           # Build everything
build_all.bat       # Windows batch script
build_djc.bat       # Build compiler only
build_vm.bat        # Build VM only
```

### 3. Supported Java Subset

#### Data Types
- `int` (16-bit signed)
- `long` (32-bit signed, Phase 5)
- `float` (32-bit IEEE 754, Phase 6)
- `boolean` (true/false literals, Phase 8)
- `String` (limited support)
- Arrays: `int[]`, `long[]`, `float[]`, `boolean[]`

#### Control Flow
- `if` / `else`
- `while`
- `for`
- `switch` / `case` / `default` / `break` (Phase 7, with fall-through support)
- `return`

#### Classes and Methods
- Single class per file
- `public static` methods only (except Date class)
- No inheritance, interfaces, or packages
- Date class supports instance methods

#### Runtime Library
- `System.out.println()` - Multiple overloads
- `String` - Basic operations (length, charAt, substring, concat, etc.)
- `Date` - Date/time support (Phase 3.5, uses seconds not milliseconds)
- `Math` - Mathematical functions (Phase 6.3)

### 4. Development Phases

- **Phase 1-2**: Foundation, String support ✅
- **Phase 3**: I/O System ✅
- **Phase 3.5**: Date Support ✅
- **Phase 4**: Network (in progress)
- **Phase 5**: Long type support ✅
- **Phase 6**: Float type and Math library ✅
- **Phase 7**: Switch statement support ✅
- **Phase 8**: Boolean literal support ✅
- **Phase 9**: Future enhancements

### 5. Important Constraints

#### Memory Limitations
- Code segment: 64KB
- Data segment: 64KB
- Stack: 4-8KB
- Heap: ~40KB

#### DOS Limitations
- File names: 8.3 format (e.g., `hello.jav` not `hello.java`)
- Line endings: CRLF (Windows style)
- No long file paths

#### Language Limitations
- No instance methods (except Date class)
- No method overloading
- No `String` parameters or return values (except in runtime library)
- No general object creation (except Date)
- No exceptions (planned for future)

### 6. Testing

**Test Locations**:
- `tests/` - Unit tests and specific feature tests
- `samples/` - Sample programs demonstrating features
- `samples/runtest.bat` - Automated test runner

**Test Execution**:
```batch
cd tests
djc.exe testfile.jav
djvm.exe testfile.djc
```

**DOSBox Testing**:
```batch
test_compile_dosbox.bat
run_array_tests_dosbox.bat
```

### 7. Common Tasks

#### Adding a New Feature

1. **Plan**: Create `PHASE_X_FEATURE_PLAN.md`
2. **Implement**:
   - Update lexer if new keywords needed
   - Update parser for new syntax
   - Update semantic analyzer for type checking
   - Update code generator for bytecode emission
   - Update VM if new opcodes needed
3. **Test**: Create test files in `tests/` or `samples/`
4. **Document**: Create `PHASE_X_COMPLETION.md`
5. **Update**: Update `README.md` and `PROGRESS.md`

#### Fixing a Bug

1. **Reproduce**: Create minimal test case
2. **Analyze**: Identify root cause (lexer/parser/semantic/codegen/VM)
3. **Fix**: Make targeted changes
4. **Test**: Verify fix with test case
5. **Document**: Update relevant PHASE*.md or create bug fix document
6. **Regression**: Run existing tests to ensure no breakage

#### Building and Testing

```batch
# Build
cd dosjava
build_all.bat
```

```Test
dosjava works under 16bit PC-DOS.
You can't execute djc.exe/djvm.exe directly.

So I would work for test in DOSBox-X, and I will feedback result to you.
```

### 8. Code Style

- **C Code**: K&R style, 4-space indentation
- **Comments**: Comprehensive, explain "why" not just "what"
- **Error Handling**: Check return values, provide meaningful error messages
- **Memory**: Always free allocated memory, check for leaks

### 9. Documentation Standards

#### File Naming
- Plans: `PHASE_X_FEATURE_PLAN.md`
- Completion: `PHASE_X_COMPLETION.md`
- Technical: `FEATURE_TECHNICAL_SPEC.md`

#### Document Structure
1. Overview
2. Problem/Goal
3. Solution/Implementation
4. Testing
5. Results
6. Future Work

### 10. Important Files to Reference

- `README.md` - Main project documentation
- `TECHNICAL_SPEC.md` - Technical specifications
- `COMPILER_DESIGN.md` - Compiler architecture
- `VM_IMPLEMENTATION_PLAN.md` - VM design
- `PROGRESS.md` - Development progress tracking
- `PHASE*.md` - Phase-specific documentation

### 11. Common Pitfalls

1. **DOS File Names**: Always use 8.3 format
2. **Memory Model**: Remember far pointers in Large model
3. **Stack Overflow**: Limited stack space (4-8KB)
4. **Type Sizes**: `int` is 16-bit, not 32-bit
5. **String Handling**: Limited support, no String parameters
6. **Date Timestamps**: Uses seconds, not milliseconds
7. **Line Endings**: Must be CRLF for DOS

### 12. Debugging Tips

1. **Compiler Issues**: Use `-k` flag to keep intermediate files (.tok, .ast, .sym)
2. **VM Issues**: Add debug output in interpreter.c
3. **Memory Issues**: Use memory statistics and integrity checks
4. **Bytecode Issues**: Examine .djc file with hex editor

### 13. When Making Changes

**Always**:
- ✅ Read related PHASE*.md documents first
- ✅ Understand the existing implementation
- ✅ Create test cases before implementing
- ✅ Build and test after changes
- ✅ Update documentation
- ✅ Check for regressions

**Never**:
- ❌ Make changes without understanding context
- ❌ Skip testing
- ❌ Leave undocumented changes
- ❌ Break existing functionality
- ❌ Ignore memory constraints

### 14. Communication Style

When working on this project:
- Be direct and technical
- Reference specific files and line numbers
- Explain root causes, not just symptoms
- Provide complete solutions, not partial fixes
- Document decisions and trade-offs

### 15. Success Criteria

A task is complete when:
1. ✅ Code compiles without errors
2. ✅ All tests pass
3. ✅ Documentation is updated
4. ✅ No regressions introduced
5. ✅ Changes are explained clearly

---

## Quick Reference

**Build**: `build_all.bat`
**Test**: `cd samples; runtest.bat`
**Compiler**: `djc.exe source.jav`
**VM**: `djvm.exe program.djc`
**Docs**: `README.md`, `PHASE*.md`

---

**Last Updated**: 2026-05-25
**Project Status**: Phase 8 Complete
**Next Phase**: Phase 9 - Additional Runtime Libraries