# Phase 5: CLI and Final Integration Plan

## Overview
Phase 5では、全てのコンパイラコンポーネントを統合し、使いやすいCLIツール（djc.exe）を作成します。これにより、Javaソースファイルから.djcバイトコードまでを一つのコマンドで生成できるようになります。

## Goals
1. 統合コンパイラCLI（djc.exe）の作成
2. 全パイプラインの自動実行
3. エンドツーエンドテスト
4. 完全なドキュメント
5. 使用例とサンプルプログラム

## Architecture

### Compiler Pipeline
```
Input: source.java
    ↓
[Lexer] → source.tok (tokens)
    ↓
[Parser] → source.ast (AST)
    ↓
[Semantic] → source.sym (symbol table)
    ↓
[CodeGen] → source.djc (bytecode)
    ↓
Output: source.djc
```

### CLI Design

#### Command-Line Interface
```
djc [options] <source.java>

Options:
  -o <file>     Output file (default: <source>.djc)
  -k            Keep intermediate files (.tok, .ast, .sym)
  -v            Verbose output
  -h, --help    Show help message
  
Examples:
  djc Hello.java              # Compile to Hello.djc
  djc -o test.djc Test.java   # Compile to test.djc
  djc -k -v Hello.java        # Keep intermediates, verbose
```

#### Error Handling
- Clear error messages with file, line, and column information
- Stop on first error in each phase
- Report which phase failed
- Suggest fixes when possible

## Implementation Tasks

### Phase 5.1: Design Integrated Compiler CLI

**File**: `tools/compiler/djc.h`

```c
typedef struct {
    /* Input */
    const char* source_file;    /* Input .java file */
    const char* output_file;    /* Output .djc file */
    
    /* Options */
    int keep_intermediates;     /* Keep .tok, .ast, .sym files */
    int verbose;                /* Verbose output */
    
    /* Intermediate files */
    char tok_file[256];         /* Token file path */
    char ast_file[256];         /* AST file path */
    char sym_file[256];         /* Symbol table file path */
    
    /* Statistics */
    uint16_t line_count;
    uint16_t token_count;
    uint16_t node_count;
    uint16_t symbol_count;
    uint16_t bytecode_size;
    
    /* Error tracking */
    int has_error;
    int error_phase;            /* 0=none, 1=lex, 2=parse, 3=semantic, 4=codegen */
} CompilerContext;
```

**Functions**:
- `compiler_init()` - Initialize compiler context
- `compiler_compile()` - Run full compilation pipeline
- `compiler_cleanup()` - Cleanup and remove intermediate files
- `run_lexer()` - Execute lexer phase
- `run_parser()` - Execute parser phase
- `run_semantic()` - Execute semantic analysis phase
- `run_codegen()` - Execute code generation phase
- `print_statistics()` - Print compilation statistics

### Phase 5.2: Implement djc.c (Main Compiler Driver)

**File**: `tools/compiler/djc.c` (約500行)

**Main Components**:

1. **Command-line parsing**
   ```c
   int parse_arguments(int argc, char* argv[], CompilerContext* ctx);
   void print_usage(const char* program_name);
   void print_version();
   ```

2. **Pipeline execution**
   ```c
   int run_lexer(CompilerContext* ctx);
   int run_parser(CompilerContext* ctx);
   int run_semantic(CompilerContext* ctx);
   int run_codegen(CompilerContext* ctx);
   ```

3. **File management**
   ```c
   void generate_intermediate_filenames(CompilerContext* ctx);
   void cleanup_intermediate_files(CompilerContext* ctx);
   int check_file_exists(const char* filename);
   ```

4. **Error reporting**
   ```c
   void report_phase_error(CompilerContext* ctx, const char* phase);
   void print_statistics(CompilerContext* ctx);
   ```

5. **Main function**
   ```c
   int main(int argc, char* argv[]) {
       CompilerContext ctx;
       
       /* Parse arguments */
       if (parse_arguments(argc, argv, &ctx) != 0) {
           return 1;
       }
       
       /* Run compilation pipeline */
       if (compiler_compile(&ctx) != 0) {
           return 1;
       }
       
       /* Print statistics */
       if (ctx.verbose) {
           print_statistics(&ctx);
       }
       
       /* Cleanup */
       compiler_cleanup(&ctx);
       
       return 0;
   }
   ```

### Phase 5.3: Create End-to-End Test Cases

**Test Programs**:

1. **tests/e2e/hello.java** - Simple Hello World
   ```java
   class Hello {
       public static void main() {
           int x = 42;
           return;
       }
   }
   ```

2. **tests/e2e/arithmetic.java** - Arithmetic operations
   ```java
   class Arithmetic {
       public static void main() {
           int a = 10;
           int b = 20;
           int c = a + b * 2;
           int d = (a + b) / 2;
           return;
       }
   }
   ```

3. **tests/e2e/control.java** - Control flow
   ```java
   class Control {
       public static void main() {
           int i = 0;
           while (i < 10) {
               i = i + 1;
           }
           
           if (i == 10) {
               int result = 100;
           }
           
           return;
       }
   }
   ```

4. **tests/e2e/fibonacci.java** - Fibonacci (complex example)
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

**Test Script**: `tests/e2e/run_tests.bat`
```batch
@echo off
echo Running end-to-end compiler tests...
echo.

set TESTS=hello arithmetic control fibonacci

for %%t in (%TESTS%) do (
    echo Testing %%t.java...
    ..\..\build\bin\djc.exe -v %%t.java
    if errorlevel 1 (
        echo FAILED: %%t.java
        exit /b 1
    )
    echo PASSED: %%t.java
    echo.
)

echo All tests passed!
```

### Phase 5.4: Write Final Documentation

**Documents to create/update**:

1. **README.md** - Project overview and quick start
2. **COMPILER_USAGE.md** - Detailed compiler usage guide
3. **LANGUAGE_SPEC.md** - Supported Java subset specification
4. **BYTECODE_SPEC.md** - .djc bytecode format specification
5. **EXAMPLES.md** - Example programs and use cases

**README.md Structure**:
```markdown
# DOS Java Compiler

16-bit PC-DOS上で動作するJavaコンパイラ

## Features
- Java subset to .djc bytecode compilation
- Stack-based bytecode format
- Optimized for 16-bit DOS environment
- Complete compilation pipeline

## Quick Start
```batch
djc Hello.java
djvm Hello.djc
```

## Supported Java Subset
- Primitive types: int, boolean
- Control flow: if, while, return
- Operators: arithmetic, relational, logical
- Methods: static methods only
- Classes: single class per file

## Requirements
- Open Watcom v2 C Compiler
- 16-bit DOS or DOSBox
- 640KB RAM minimum

## Building
```batch
wmake all
```

## Usage
```batch
djc [options] <source.java>
```

## Examples
See EXAMPLES.md for sample programs.

## License
MIT License
```

### Phase 5.5: Create Usage Examples

**EXAMPLES.md Content**:

1. **Hello World**
2. **Calculator**
3. **Factorial**
4. **Prime Numbers**
5. **Sorting**

Each example includes:
- Java source code
- Compilation command
- Expected output
- Explanation

## Build System Integration

### Makefile Updates

Add djc.exe target:
```makefile
# Integrated compiler
$(OBJ_DIR)/djc.obj: tools/compiler/djc.c tools/compiler/djc.h
	@echo Compiling djc.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/djc.c

$(BIN_DIR)/djc.exe: $(OBJ_DIR)/djc.obj $(OBJ_DIR)/codegen.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj $(FORMAT_OBJS) $(VM_OBJS) $(RUNTIME_OBJS)
	@echo Linking djc.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/djc.obj $(OBJ_DIR)/codegen.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj $(FORMAT_OBJS) $(VM_OBJS) $(RUNTIME_OBJS) }

djc: $(BIN_DIR)/djc.exe
```

Update all target:
```makefile
all: test_memory test_interpreter mkdjc java2djc test_lexer test_parser test_semantic test_codegen djc
```

## Testing Strategy

### Unit Tests
- Each phase tested independently (already done)

### Integration Tests
- Pipeline execution
- Intermediate file generation
- Error propagation

### End-to-End Tests
- Complete compilation from .java to .djc
- Verify bytecode correctness
- Test with VM interpreter

### Regression Tests
- Ensure existing functionality still works
- Test edge cases
- Test error handling

## Success Criteria

Phase 5 is complete when:
1. ✓ djc.exe compiles and links successfully
2. ✓ Can compile simple Java programs to .djc
3. ✓ All end-to-end tests pass
4. ✓ Documentation is complete
5. ✓ Examples work correctly
6. ✓ Error messages are clear and helpful
7. ✓ Performance is acceptable for 16-bit DOS

## Deliverables

### Code
- [ ] `tools/compiler/djc.h` - Compiler driver header
- [ ] `tools/compiler/djc.c` - Compiler driver implementation
- [ ] Updated makefile with djc target

### Tests
- [ ] `tests/e2e/hello.java`
- [ ] `tests/e2e/arithmetic.java`
- [ ] `tests/e2e/control.java`
- [ ] `tests/e2e/fibonacci.java`
- [ ] `tests/e2e/run_tests.bat`

### Documentation
- [ ] `README.md` - Updated project overview
- [ ] `COMPILER_USAGE.md` - Compiler usage guide
- [ ] `LANGUAGE_SPEC.md` - Language specification
- [ ] `BYTECODE_SPEC.md` - Bytecode specification
- [ ] `EXAMPLES.md` - Example programs

## Timeline

- **Day 1**: Design and implement djc.h/djc.c
- **Day 2**: Create end-to-end tests
- **Day 3**: Write documentation
- **Day 4**: Testing and refinement
- **Day 5**: Final polish and release

## Next Steps

After Phase 5 completion:
1. Performance optimization
2. Additional language features
3. Better error messages
4. IDE integration
5. Debugger support

## Notes

- Keep CLI simple and intuitive
- Follow DOS conventions for file paths
- Provide clear error messages
- Make compilation fast (important for 16-bit DOS)
- Document all limitations clearly