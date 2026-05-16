# Phase 3.5: Exception Handling and Date Support

## Overview

Phase 3.5では、Phase 4（ネットワーク機能）の実装に先立ち、以下の2つの重要な機能を実装します：

1. **Exception Handling**: try-catch-finally構文による例外処理
2. **Date Support**: 日付・時刻機能（java.util.Date相当）

これらの機能は、Phase 4のネットワーク処理において必須となる機能であり、先行実装することでPhase 4の品質と開発効率を大幅に向上させます。

## Goals

### Primary Goals
1. try-catch-finally構文の完全実装
2. Exception classの実装（単一の例外型）
3. Date classの実装（基本的な日時操作）
4. DOS時刻APIの統合
5. Phase 4で必要となる基盤の整備

### Success Criteria
- [ ] try-catch-finallyが正しく動作
- [ ] 例外のスロー・キャッチが動作
- [ ] finallyブロックが確実に実行される
- [ ] Date objectの作成・操作が動作
- [ ] DOS時刻APIから正確な日時を取得
- [ ] 全テストケースがパス
- [ ] メモリリークなし

## Phase 3.5.1: Exception Handling (Day 1-5)

### Duration: 5日間

### Architecture

```
┌─────────────────────────────────────┐
│   Java Source Code                  │
│   try { ... }                       │
│   catch (Exception e) { ... }       │
│   finally { ... }                   │
└─────────────────┬───────────────────┘
                  │ Parsing
┌─────────────────▼───────────────────┐
│   Compiler (parser.c)               │
│   - Parse try-catch-finally         │
│   - Generate exception table        │
│   - Generate bytecode               │
└─────────────────┬───────────────────┘
                  │ Bytecode
┌─────────────────▼───────────────────┐
│   Bytecode (.djc)                   │
│   - Exception table                 │
│   - OP_TRY, OP_CATCH, OP_FINALLY    │
│   - OP_THROW                        │
└─────────────────┬───────────────────┘
                  │ Execution
┌─────────────────▼───────────────────┐
│   VM (interpreter.c)                │
│   - Exception handling              │
│   - Stack unwinding                 │
│   - Finally execution               │
└─────────────────────────────────────┘
```

### Day 1-2: Compiler Support for Exception Syntax

#### Task 1.1: Lexer Updates
**File**: `tools/compiler/lexer.h`, `lexer.c`

新しいキーワードの追加:
```c
/* Exception handling keywords */
TOKEN_TRY,       /* try */
TOKEN_CATCH,     /* catch */
TOKEN_FINALLY,   /* finally */
TOKEN_THROW,     /* throw */
```

#### Task 1.2: AST Node Types
**File**: `tools/compiler/ast.h`

新しいノードタイプの追加:
```c
/* Exception handling nodes */
NODE_TRY_STMT,      /* try statement */
NODE_CATCH_CLAUSE,  /* catch clause */
NODE_FINALLY_BLOCK, /* finally block */
NODE_THROW_STMT,    /* throw statement */
```

AST構造体の拡張:
```c
typedef struct {
    uint16_t try_block;      /* try block node */
    uint16_t catch_clause;   /* catch clause node (optional) */
    uint16_t finally_block;  /* finally block node (optional) */
} TryStmtNode;

typedef struct {
    uint16_t exception_var;  /* Exception variable name offset */
    uint16_t catch_block;    /* catch block statements */
} CatchClauseNode;

typedef struct {
    uint16_t finally_block;  /* finally block statements */
} FinallyBlockNode;

typedef struct {
    uint16_t exception_expr; /* Exception expression to throw */
} ThrowStmtNode;
```

#### Task 1.3: Parser Implementation
**File**: `tools/compiler/parser.c`

新しいパース関数の実装:

```c
/**
 * Parse try-catch-finally statement
 * 
 * Grammar:
 *   try_stmt := 'try' block catch_clause? finally_block?
 *   catch_clause := 'catch' '(' 'Exception' ID ')' block
 *   finally_block := 'finally' block
 */
static uint16_t parse_try_stmt(Parser* parser) {
    uint16_t try_node;
    uint16_t try_block;
    uint16_t catch_clause = 0;
    uint16_t finally_block = 0;
    
    /* Expect 'try' keyword */
    if (!match(parser, TOKEN_TRY)) {
        parser_error(parser, "Expected 'try'");
        return 0;
    }
    
    /* Parse try block */
    try_block = parse_block(parser);
    if (try_block == 0) {
        return 0;
    }
    
    /* Parse optional catch clause */
    if (check(parser, TOKEN_CATCH)) {
        catch_clause = parse_catch_clause(parser);
        if (catch_clause == 0) {
            return 0;
        }
    }
    
    /* Parse optional finally block */
    if (check(parser, TOKEN_FINALLY)) {
        finally_block = parse_finally_block(parser);
        if (finally_block == 0) {
            return 0;
        }
    }
    
    /* At least one of catch or finally must be present */
    if (catch_clause == 0 && finally_block == 0) {
        parser_error(parser, "try statement must have catch or finally");
        return 0;
    }
    
    /* Create try statement node */
    try_node = ast_create_node(parser, NODE_TRY_STMT);
    /* Store try_block, catch_clause, finally_block in node */
    
    return try_node;
}

/**
 * Parse catch clause
 */
static uint16_t parse_catch_clause(Parser* parser) {
    uint16_t catch_node;
    uint16_t exception_var;
    uint16_t catch_block;
    
    /* Expect 'catch' keyword */
    if (!match(parser, TOKEN_CATCH)) {
        return 0;
    }
    
    /* Expect '(' */
    if (!match(parser, TOKEN_LPAREN)) {
        parser_error(parser, "Expected '(' after 'catch'");
        return 0;
    }
    
    /* Expect 'Exception' type */
    if (!match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected exception type");
        return 0;
    }
    /* TODO: Verify it's "Exception" */
    
    /* Expect exception variable name */
    if (!match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected exception variable name");
        return 0;
    }
    exception_var = parser->current.value;
    
    /* Expect ')' */
    if (!match(parser, TOKEN_RPAREN)) {
        parser_error(parser, "Expected ')' after exception variable");
        return 0;
    }
    
    /* Parse catch block */
    catch_block = parse_block(parser);
    if (catch_block == 0) {
        return 0;
    }
    
    /* Create catch clause node */
    catch_node = ast_create_node(parser, NODE_CATCH_CLAUSE);
    /* Store exception_var, catch_block in node */
    
    return catch_node;
}

/**
 * Parse finally block
 */
static uint16_t parse_finally_block(Parser* parser) {
    uint16_t finally_node;
    uint16_t finally_block;
    
    /* Expect 'finally' keyword */
    if (!match(parser, TOKEN_FINALLY)) {
        return 0;
    }
    
    /* Parse finally block */
    finally_block = parse_block(parser);
    if (finally_block == 0) {
        return 0;
    }
    
    /* Create finally block node */
    finally_node = ast_create_node(parser, NODE_FINALLY_BLOCK);
    /* Store finally_block in node */
    
    return finally_node;
}

/**
 * Parse throw statement
 */
static uint16_t parse_throw_stmt(Parser* parser) {
    uint16_t throw_node;
    uint16_t exception_expr;
    
    /* Expect 'throw' keyword */
    if (!match(parser, TOKEN_THROW)) {
        return 0;
    }
    
    /* Parse exception expression */
    exception_expr = parse_expression(parser);
    if (exception_expr == 0) {
        parser_error(parser, "Expected exception expression");
        return 0;
    }
    
    /* Expect ';' */
    if (!match(parser, TOKEN_SEMICOLON)) {
        parser_error(parser, "Expected ';' after throw statement");
        return 0;
    }
    
    /* Create throw statement node */
    throw_node = ast_create_node(parser, NODE_THROW_STMT);
    /* Store exception_expr in node */
    
    return throw_node;
}
```

#### Task 1.4: Exception Table Generation
**File**: `tools/compiler/codegen.c`

例外テーブルの構造:
```c
typedef struct {
    uint16_t try_start;      /* try block start PC */
    uint16_t try_end;        /* try block end PC */
    uint16_t catch_start;    /* catch block start PC (0 if no catch) */
    uint16_t finally_start;  /* finally block start PC (0 if no finally) */
    uint16_t exception_var;  /* Exception variable index */
} ExceptionHandler;
```

コード生成:
```c
/**
 * Generate code for try-catch-finally statement
 */
static void codegen_try_stmt(CodeGen* gen, uint16_t node) {
    uint16_t try_start, try_end;
    uint16_t catch_start = 0, catch_end = 0;
    uint16_t finally_start = 0, finally_end = 0;
    uint16_t end_label;
    
    /* Get try, catch, finally blocks from node */
    uint16_t try_block = /* ... */;
    uint16_t catch_clause = /* ... */;
    uint16_t finally_block = /* ... */;
    
    /* Generate try block */
    emit_opcode(gen, OP_TRY_BEGIN);
    try_start = gen->code_pos;
    codegen_block(gen, try_block);
    try_end = gen->code_pos;
    emit_opcode(gen, OP_TRY_END);
    
    /* Jump to finally or end */
    end_label = create_label(gen);
    emit_jump(gen, OP_GOTO, end_label);
    
    /* Generate catch block if present */
    if (catch_clause != 0) {
        catch_start = gen->code_pos;
        emit_opcode(gen, OP_CATCH_BEGIN);
        /* Store exception in variable */
        codegen_catch_clause(gen, catch_clause);
        catch_end = gen->code_pos;
        emit_opcode(gen, OP_CATCH_END);
        
        /* Jump to finally or end */
        emit_jump(gen, OP_GOTO, end_label);
    }
    
    /* Generate finally block if present */
    if (finally_block != 0) {
        finally_start = gen->code_pos;
        emit_opcode(gen, OP_FINALLY_BEGIN);
        codegen_block(gen, finally_block);
        emit_opcode(gen, OP_FINALLY_END);
    }
    
    /* End label */
    bind_label(gen, end_label);
    
    /* Add exception handler to table */
    add_exception_handler(gen, try_start, try_end, 
                         catch_start, finally_start, 
                         exception_var);
}
```

### Day 3-4: VM Exception Handling

#### Task 3.1: New Opcodes
**File**: `src/format/opcodes.h`

新しいオペコードの追加:
```c
/* Exception Handling (0x90-0x9F) */
#define OP_TRY_BEGIN     0x90  /* Mark try block start */
#define OP_TRY_END       0x91  /* Mark try block end */
#define OP_CATCH_BEGIN   0x92  /* Mark catch block start */
#define OP_CATCH_END     0x93  /* Mark catch block end */
#define OP_FINALLY_BEGIN 0x94  /* Mark finally block start */
#define OP_FINALLY_END   0x95  /* Mark finally block end */
#define OP_THROW         0x96  /* Throw exception */
```

#### Task 3.2: Exception Handler Structure
**File**: `src/vm/interpreter.c`

VM内部構造:
```c
typedef struct {
    uint16_t try_start;
    uint16_t try_end;
    uint16_t catch_start;
    uint16_t finally_start;
    uint16_t exception_var;
} ExceptionHandler;

typedef struct {
    ExceptionHandler handlers[MAX_EXCEPTION_HANDLERS];
    int handler_count;
    
    /* Current exception state */
    int has_exception;
    Value exception_value;
    uint16_t exception_pc;
} ExceptionContext;
```

#### Task 3.3: Exception Handling Implementation
**File**: `src/vm/interpreter.c`

```c
/**
 * Throw exception
 */
static void vm_throw_exception(VM* vm, Value exception) {
    ExceptionContext* ctx = &vm->exception_ctx;
    
    /* Set exception state */
    ctx->has_exception = 1;
    ctx->exception_value = exception;
    ctx->exception_pc = vm->pc;
    
    /* Find exception handler */
    ExceptionHandler* handler = find_exception_handler(vm, vm->pc);
    
    if (handler == NULL) {
        /* Unhandled exception - halt VM */
        fprintf(stderr, "Unhandled exception at PC %d\n", vm->pc);
        vm->halted = 1;
        return;
    }
    
    /* Execute finally block if present */
    if (handler->finally_start != 0) {
        vm->pc = handler->finally_start;
        /* Mark that we need to jump to catch after finally */
        vm->pending_catch_pc = handler->catch_start;
    } else if (handler->catch_start != 0) {
        /* Jump directly to catch */
        vm->pc = handler->catch_start;
        /* Store exception in catch variable */
        vm->locals[handler->exception_var] = exception;
    }
    
    /* Clear exception flag */
    ctx->has_exception = 0;
}

/**
 * Find exception handler for given PC
 */
static ExceptionHandler* find_exception_handler(VM* vm, uint16_t pc) {
    ExceptionContext* ctx = &vm->exception_ctx;
    
    for (int i = 0; i < ctx->handler_count; i++) {
        ExceptionHandler* handler = &ctx->handlers[i];
        
        if (pc >= handler->try_start && pc < handler->try_end) {
            return handler;
        }
    }
    
    return NULL;
}

/**
 * Execute OP_THROW
 */
case OP_THROW: {
    Value exception = vm_pop(vm);
    vm_throw_exception(vm, exception);
    break;
}

/**
 * Execute OP_TRY_BEGIN
 */
case OP_TRY_BEGIN: {
    /* Mark try block entry */
    vm->try_depth++;
    break;
}

/**
 * Execute OP_TRY_END
 */
case OP_TRY_END: {
    /* Mark try block exit */
    vm->try_depth--;
    
    /* If no exception, execute finally if present */
    ExceptionHandler* handler = find_exception_handler(vm, vm->pc);
    if (handler && handler->finally_start != 0) {
        vm->pc = handler->finally_start;
    }
    break;
}

/**
 * Execute OP_FINALLY_END
 */
case OP_FINALLY_END: {
    /* Finally block completed */
    
    /* If there was a pending catch, jump to it */
    if (vm->pending_catch_pc != 0) {
        vm->pc = vm->pending_catch_pc;
        vm->pending_catch_pc = 0;
    }
    break;
}
```

### Day 5: Exception Testing

#### Task 5.1: Basic Exception Tests
**File**: `tests/exception/test_basic.jav`

```java
class TestBasic {
    public static void main(String[] args) {
        // Test 1: Basic try-catch
        try {
            System.out.println("In try block");
            throw new Exception("Test exception");
            System.out.println("After throw (should not print)");
        } catch (Exception e) {
            System.out.println("Caught exception");
        }
        System.out.println("After try-catch");
        
        // Test 2: try-finally without exception
        try {
            System.out.println("Try block 2");
        } finally {
            System.out.println("Finally block 2");
        }
        
        // Test 3: try-catch-finally with exception
        try {
            System.out.println("Try block 3");
            throw new Exception("Test 3");
        } catch (Exception e) {
            System.out.println("Catch block 3");
        } finally {
            System.out.println("Finally block 3");
        }
    }
}
```

#### Task 5.2: Nested Exception Tests
**File**: `tests/exception/test_nested.jav`

```java
class TestNested {
    public static void main(String[] args) {
        // Nested try-catch
        try {
            System.out.println("Outer try");
            try {
                System.out.println("Inner try");
                throw new Exception("Inner exception");
            } catch (Exception e) {
                System.out.println("Inner catch");
            }
            System.out.println("After inner try-catch");
        } catch (Exception e) {
            System.out.println("Outer catch");
        } finally {
            System.out.println("Outer finally");
        }
    }
}
```

#### Task 5.3: Finally Guarantee Tests
**File**: `tests/exception/test_finally.jav`

```java
class TestFinally {
    public static void main(String[] args) {
        // Test that finally always executes
        int result = testFinally();
        System.out.println("Result: " + result);
    }
    
    public static int testFinally() {
        try {
            System.out.println("Try block");
            return 1;
        } finally {
            System.out.println("Finally block (should execute before return)");
        }
    }
}
```

## Phase 3.5.2: Date Support (Day 6-8)

### Duration: 3日間

### Architecture

```
┌─────────────────────────────────────┐
│   Java Code                         │
│   Date d = new Date();              │
│   d.getTime();                      │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│   VM Runtime (date.c)               │
│   - Date object management          │
│   - Native method implementations   │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│   DOS Time API (INT 21h)            │
│   - AH=2Ah: Get System Date         │
│   - AH=2Ch: Get System Time         │
└─────────────────────────────────────┘
```

### Day 6: DOS Time API Integration

#### Task 6.1: DOS Time Functions
**File**: `src/runtime/dostime.h`, `dostime.c`

```c
/**
 * DOS Time API Wrapper
 */

#ifndef DOSTIME_H
#define DOSTIME_H

#include "../types.h"

typedef struct {
    uint16_t year;
    uint8_t month;    /* 1-12 */
    uint8_t day;      /* 1-31 */
    uint8_t hour;     /* 0-23 */
    uint8_t minute;   /* 0-59 */
    uint8_t second;   /* 0-59 */
    uint8_t hundredths; /* 0-99 */
} DOSDateTime;

/**
 * Get current date and time from DOS
 */
void dos_get_datetime(DOSDateTime* dt);

/**
 * Convert DOSDateTime to Unix timestamp (milliseconds since 1970-01-01)
 */
int32_t dos_datetime_to_timestamp(const DOSDateTime* dt);

/**
 * Convert Unix timestamp to DOSDateTime
 */
void dos_timestamp_to_datetime(int32_t timestamp, DOSDateTime* dt);

#endif /* DOSTIME_H */
```

実装:
```c
#include "dostime.h"
#include <dos.h>

/**
 * Get current date and time from DOS
 */
void dos_get_datetime(DOSDateTime* dt) {
    union REGS regs;
    
    /* Get date (INT 21h, AH=2Ah) */
    regs.h.ah = 0x2A;
    int86(0x21, &regs, &regs);
    dt->year = regs.x.cx;
    dt->month = regs.h.dh;
    dt->day = regs.h.dl;
    
    /* Get time (INT 21h, AH=2Ch) */
    regs.h.ah = 0x2C;
    int86(0x21, &regs, &regs);
    dt->hour = regs.h.ch;
    dt->minute = regs.h.cl;
    dt->second = regs.h.dh;
    dt->hundredths = regs.h.dl;
}

/**
 * Convert DOSDateTime to Unix timestamp
 * Simplified calculation (not accounting for leap seconds, etc.)
 */
int32_t dos_datetime_to_timestamp(const DOSDateTime* dt) {
    int32_t days;
    int32_t seconds;
    
    /* Calculate days since 1970-01-01 */
    /* Simplified: assume 365.25 days per year */
    days = (dt->year - 1970) * 365 + (dt->year - 1970) / 4;
    
    /* Add days for months */
    static const int month_days[] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };
    days += month_days[dt->month - 1];
    days += dt->day - 1;
    
    /* Calculate seconds */
    seconds = days * 86400L;
    seconds += dt->hour * 3600L;
    seconds += dt->minute * 60L;
    seconds += dt->second;
    
    /* Convert to milliseconds */
    return seconds * 1000L + (dt->hundredths * 10L);
}
```

### Day 7: Date Class Implementation

#### Task 7.1: Date Object Structure
**File**: `src/runtime/date.h`, `date.c`

```c
/**
 * Date Class Implementation
 */

#ifndef DATE_H
#define DATE_H

#include "../types.h"
#include "dostime.h"

typedef struct {
    int32_t time_ms;  /* Milliseconds since 1970-01-01 00:00:00 UTC */
    DOSDateTime cached_dt;
    int cache_valid;
} DateObject;

/**
 * Create new Date object with current time
 */
DateObject* date_new(void);

/**
 * Create new Date object with specified time
 */
DateObject* date_new_with_time(int32_t time_ms);

/**
 * Get time in milliseconds
 */
int32_t date_get_time(DateObject* date);

/**
 * Set time in milliseconds
 */
void date_set_time(DateObject* date, int32_t time_ms);

/**
 * Get year (e.g., 2026)
 */
int date_get_full_year(DateObject* date);

/**
 * Get month (0-11)
 */
int date_get_month(DateObject* date);

/**
 * Get day of month (1-31)
 */
int date_get_date(DateObject* date);

/**
 * Get hours (0-23)
 */
int date_get_hours(DateObject* date);

/**
 * Get minutes (0-59)
 */
int date_get_minutes(DateObject* date);

/**
 * Get seconds (0-59)
 */
int date_get_seconds(DateObject* date);

/**
 * Convert to string
 */
void date_to_string(DateObject* date, char* buffer, int buffer_size);

#endif /* DATE_H */
```

実装:
```c
#include "date.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * Create new Date object with current time
 */
DateObject* date_new(void) {
    DateObject* date = (DateObject*)malloc(sizeof(DateObject));
    if (date == NULL) {
        return NULL;
    }
    
    DOSDateTime dt;
    dos_get_datetime(&dt);
    date->time_ms = dos_datetime_to_timestamp(&dt);
    date->cached_dt = dt;
    date->cache_valid = 1;
    
    return date;
}

/**
 * Create new Date object with specified time
 */
DateObject* date_new_with_time(int32_t time_ms) {
    DateObject* date = (DateObject*)malloc(sizeof(DateObject));
    if (date == NULL) {
        return NULL;
    }
    
    date->time_ms = time_ms;
    date->cache_valid = 0;
    
    return date;
}

/**
 * Get time in milliseconds
 */
int32_t date_get_time(DateObject* date) {
    return date->time_ms;
}

/**
 * Set time in milliseconds
 */
void date_set_time(DateObject* date, int32_t time_ms) {
    date->time_ms = time_ms;
    date->cache_valid = 0;
}

/**
 * Ensure cached datetime is valid
 */
static void ensure_cache(DateObject* date) {
    if (!date->cache_valid) {
        dos_timestamp_to_datetime(date->time_ms, &date->cached_dt);
        date->cache_valid = 1;
    }
}

/**
 * Get year
 */
int date_get_full_year(DateObject* date) {
    ensure_cache(date);
    return date->cached_dt.year;
}

/**
 * Get month (0-11)
 */
int date_get_month(DateObject* date) {
    ensure_cache(date);
    return date->cached_dt.month - 1;  /* Convert to 0-based */
}

/**
 * Get day of month (1-31)
 */
int date_get_date(DateObject* date) {
    ensure_cache(date);
    return date->cached_dt.day;
}

/**
 * Get hours (0-23)
 */
int date_get_hours(DateObject* date) {
    ensure_cache(date);
    return date->cached_dt.hour;
}

/**
 * Get minutes (0-59)
 */
int date_get_minutes(DateObject* date) {
    ensure_cache(date);
    return date->cached_dt.minute;
}

/**
 * Get seconds (0-59)
 */
int date_get_seconds(DateObject* date) {
    ensure_cache(date);
    return date->cached_dt.second;
}

/**
 * Convert to string
 */
void date_to_string(DateObject* date, char* buffer, int buffer_size) {
    ensure_cache(date);
    
    snprintf(buffer, buffer_size, 
             "%04d-%02d-%02d %02d:%02d:%02d",
             date->cached_dt.year,
             date->cached_dt.month,
             date->cached_dt.day,
             date->cached_dt.hour,
             date->cached_dt.minute,
             date->cached_dt.second);
}
```

### Day 8: Date Testing

#### Task 8.1: Date Tests
**File**: `tests/date/test_date.jav`

```java
class TestDate {
    public static void main(String[] args) {
        // Test 1: Create Date with current time
        Date now = new Date();
        System.out.println("Current date: " + now.toString());
        
        // Test 2: Get time components
        System.out.println("Year: " + now.getFullYear());
        System.out.println("Month: " + now.getMonth());
        System.out.println("Date: " + now.getDate());
        System.out.println("Hours: " + now.getHours());
        System.out.println("Minutes: " + now.getMinutes());
        System.out.println("Seconds: " + now.getSeconds());
        
        // Test 3: Get/Set time in milliseconds
        long time = now.getTime();
        System.out.println("Time (ms): " + time);
        
        Date date2 = new Date();
        date2.setTime(time);
        System.out.println("Date2: " + date2.toString());
        
        // Test 4: Time difference
        Date start = new Date();
        // Do some work...
        for (int i = 0; i < 1000; i++) {
            int x = i * i;
        }
        Date end = new Date();
        long elapsed = end.getTime() - start.getTime();
        System.out.println("Elapsed time: " + elapsed + " ms");
    }
}
```

## Memory Considerations

### Exception Handling Memory
- Exception table: ~100-200 bytes per method
- Exception objects: ~50 bytes each
- Stack frames for unwinding: existing stack space

### Date Object Memory
- DateObject structure: 16 bytes
- Cached datetime: included in structure
- Total per Date: ~20 bytes

### Total Additional Memory
- Exception handling: ~1-2KB
- Date support: ~1KB (code + data)
- **Total**: ~2-3KB additional memory usage

## Testing Strategy

### Exception Handling Tests
1. **Basic Tests**
   - Simple try-catch
   - try-finally
   - try-catch-finally
   
2. **Advanced Tests**
   - Nested try-catch
   - Multiple catch blocks (future)
   - Exception in finally block
   
3. **Edge Cases**
   - Unhandled exceptions
   - Return in try block
   - Exception in catch block

### Date Tests
1. **Basic Tests**
   - Create Date
   - Get current time
   - Get time components
   
2. **Advanced Tests**
   - Set time
   - Time arithmetic
   - Date comparison
   
3. **Integration Tests**
   - Date with exception handling
   - Date in loops
   - Date with I/O operations

## Implementation Schedule

### Week 1: Exception Handling (5 days)
- **Day 1-2**: Compiler support (lexer, parser, AST)
- **Day 3-4**: VM exception handling (opcodes, interpreter)
- **Day 5**: Testing and debugging

### Week 2: Date Support (3 days)
- **Day 6**: DOS time API integration
- **Day 7**: Date class implementation
- **Day 8**: Testing and documentation

### Total: 8 days (1.5 weeks)

## Deliverables

### Phase 3.5.1 Deliverables
- [ ] Updated lexer with exception keywords
- [ ] Updated parser with try-catch-finally support
- [ ] Exception table generation in codegen
- [ ] VM exception handling implementation
- [ ] Exception test suite
- [ ] Documentation

### Phase 3.5.2 Deliverables
- [ ] DOS time API wrapper (dostime.h/c)
- [ ] Date class implementation (date.h/c)
- [ ] Date test suite
- [ ] Documentation

### Final Deliverables
- [ ] PHASE3_5_PLAN.md (this document)
- [ ] PHASE3_5_TASKS.md (detailed task checklist)
- [ ] PHASE3_5_QUICKSTART.md (quick start guide)
- [ ] All tests passing
- [ ] Ready for Phase 4

## Success Metrics

### Exception Handling
- [ ] All exception tests pass
- [ ] Finally blocks always execute
- [ ] Stack unwinding works correctly
- [ ] No memory leaks

### Date Support
- [ ] Date objects created successfully
- [ ] Time components retrieved correctly
- [ ] DOS time API integration works
- [ ] No memory leaks

### Overall
- [ ] Memory usage < 3KB additional
- [ ] No regression in existing tests
- [ ] Documentation complete
- [ ] Ready for Phase 4 network implementation

## Risk Mitigation

### Identified Risks
1. **Exception handling complexity**
   - Risk: Stack unwinding may be complex
   - Mitigation: Start with simple cases, test incrementally

2. **DOS time API limitations**
   - Risk: DOS time may not be accurate
   - Mitigation: Document limitations, provide workarounds

3. **Memory constraints**
   - Risk: Additional memory usage
   - Mitigation: Optimize structures, profile memory usage

## Next Steps

After Phase 3.5 completion:
1. Review and test all functionality
2. Update documentation
3. Prepare for Phase 4 (Network with mTCP)
4. Integrate exception handling into Phase 4 code

---

**Created**: 2026-05-15
**Status**: Ready to begin implementation
**Estimated Duration**: 8 days (1.5 weeks)