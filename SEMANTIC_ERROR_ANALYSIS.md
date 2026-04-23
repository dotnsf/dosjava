# Semantic Analyzer Compilation Error Analysis

## Error Report

```
Compiling semantic.c...
tools\compiler\semantic.c(220): Error! E1176: Parameter 2, pointer type mismatch
tools\compiler\semantic.c(220): Note! N2003: source conversion type is 'struct __iobuf *'
tools\compiler\semantic.c(220): Note! N2004: target conversion type is 'char const *'
tools\compiler\semantic.c(220): Note! N2002: 'symtable_write' defined in: tools\compiler\symtable.h(159)
```

## Root Cause

**Type mismatch in SemanticAnalyzer structure definition**

### Current (Incorrect) Definition
In [`semantic.h`](dosjava/tools/compiler/semantic.h:27):
```c
typedef struct {
    FILE* ast_file;             /* AST input file */
    const char* symbol_file;    /* Symbol table output file path */  // ← WRONG TYPE
    // ...
} SemanticAnalyzer;
```

### Actual Usage
In [`semantic.c`](dosjava/tools/compiler/semantic.c:28):
```c
/* Save symbol file path */
analyzer->symbol_file = symbol_file;  // symbol_file is const char*
```

In [`semantic.c`](dosjava/tools/compiler/semantic.c:218):
```c
symtable_write(analyzer->symtable, analyzer->symbol_file);
```

### Function Signature
In [`symtable.h`](dosjava/tools/compiler/symtable.h:159):
```c
int symtable_write(SymbolTable* table, const char* filename);
```

## Problem

The struct definition declares `symbol_file` as `const char*` (correct), but somewhere it was changed to `FILE*` (incorrect). The code at line 28 stores a string pointer, and line 218 passes it to a function expecting a string pointer, so the struct definition must be wrong.

## Solution

Change the struct definition in `semantic.h` from:
```c
FILE* ast_file;             /* AST input file */
const char* symbol_file;    /* Symbol table output file path */
```

To:
```c
FILE* ast_file;             /* AST input file */
const char* symbol_file;    /* Symbol table output file path */
```

Wait - checking the actual struct definition again...

Actually, looking at the error message more carefully:
- Line 218 passes `analyzer->symbol_file` (which the compiler thinks is `FILE*`)
- To `symtable_write()` which expects `const char*`

This means the struct definition currently has `FILE* symbol_file` but it should be `const char* symbol_file`.

## Fix

In [`semantic.h`](dosjava/tools/compiler/semantic.h:27), change:
```c
FILE* ast_file;             /* AST input file */
FILE* symbol_file;          /* ← WRONG: Should be const char* */
```

To:
```c
FILE* ast_file;             /* AST input file */
const char* symbol_file;    /* Symbol table output file path */
```

This matches:
1. How it's assigned at line 28: `analyzer->symbol_file = symbol_file;` (string)
2. How it's used at line 218: `symtable_write(..., analyzer->symbol_file);` (expects string)

---

*Analysis created: 2026-04-20*
*Status: Ready to fix*