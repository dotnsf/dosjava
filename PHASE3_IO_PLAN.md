# Phase 3: ストリームとI/O基盤 実装計画

## 概要

Phase 3では、ファイル入出力とストリーム処理の基盤を実装します。16-bit DOS環境の制約を考慮しながら、Javaの基本的なI/O APIのサブセットを提供します。

**期間**: 2週間  
**優先度**: ⭐中優先

## 目標

1. **基底クラスの実装**
   - `InputStream` - 入力ストリーム基底クラス
   - `OutputStream` - 出力ストリーム基底クラス

2. **ファイルストリームの実装**
   - `FileInputStream` - ファイル読み込み
   - `FileOutputStream` - ファイル書き込み

3. **バッファリング機能**
   - `BufferedReader` - バッファ付き読み込み
   - `BufferedWriter` - バッファ付き書き込み

## アーキテクチャ

### クラス階層

```
Object
  ├─ InputStream (abstract)
  │   └─ FileInputStream
  │
  └─ OutputStream (abstract)
      └─ FileOutputStream

Reader (abstract)
  └─ BufferedReader

Writer (abstract)
  └─ BufferedWriter
```

### メモリ制約（16-bit DOS）

- **ファイルハンドル**: 最大8個同時オープン
- **バッファサイズ**: 512バイト（DOSセクタサイズ）
- **パス長**: 最大80文字（DOS制限）
- **合計メモリ**: 約6KB（ランタイムライブラリ全体で20KB以内）

## 実装コンポーネント

### 1. InputStream 基底クラス

**ファイル**: `src/runtime/inputstream.h`, `src/runtime/inputstream.c`

#### データ構造

```c
typedef struct InputStream {
    Object base;              /* 基底オブジェクト */
    uint8_t is_open;          /* オープン状態 */
    uint16_t position;        /* 現在位置 */
} InputStream;
```

#### 主要メソッド

```c
/* 初期化・終了 */
int inputstream_init(InputStream* stream);
void inputstream_close(InputStream* stream);

/* 読み込み操作（サブクラスで実装） */
int inputstream_read(InputStream* stream);                    /* 1バイト読み込み */
int inputstream_read_bytes(InputStream* stream, uint8_t* buf, uint16_t len);  /* バッファ読み込み */
int inputstream_available(InputStream* stream);               /* 利用可能バイト数 */
int inputstream_skip(InputStream* stream, uint16_t n);        /* スキップ */
```

**推定サイズ**: ~200行

### 2. OutputStream 基底クラス

**ファイル**: `src/runtime/outputstream.h`, `src/runtime/outputstream.c`

#### データ構造

```c
typedef struct OutputStream {
    Object base;              /* 基底オブジェクト */
    uint8_t is_open;          /* オープン状態 */
    uint16_t position;        /* 現在位置 */
} OutputStream;
```

#### 主要メソッド

```c
/* 初期化・終了 */
int outputstream_init(OutputStream* stream);
void outputstream_close(OutputStream* stream);
void outputstream_flush(OutputStream* stream);

/* 書き込み操作（サブクラスで実装） */
int outputstream_write(OutputStream* stream, uint8_t byte);   /* 1バイト書き込み */
int outputstream_write_bytes(OutputStream* stream, const uint8_t* buf, uint16_t len);  /* バッファ書き込み */
```

**推定サイズ**: ~180行

### 3. FileInputStream

**ファイル**: `src/runtime/fileinputstream.h`, `src/runtime/fileinputstream.c`

#### データ構造

```c
typedef struct FileInputStream {
    InputStream base;         /* 基底ストリーム */
    int16_t file_handle;      /* DOSファイルハンドル */
    char filename[80];        /* ファイル名 */
    uint16_t file_size;       /* ファイルサイズ */
} FileInputStream;
```

#### 主要メソッド

```c
/* ファイルオープン */
FileInputStream* fileinputstream_new(const char* filename);
void fileinputstream_delete(FileInputStream* fis);

/* 読み込み操作 */
int fileinputstream_read(FileInputStream* fis);
int fileinputstream_read_bytes(FileInputStream* fis, uint8_t* buf, uint16_t len);
int fileinputstream_available(FileInputStream* fis);
void fileinputstream_close(FileInputStream* fis);
```

#### DOS API使用

```c
/* DOS INT 21h functions */
- AH=3Dh: ファイルオープン（読み込みモード）
- AH=3Fh: ファイル読み込み
- AH=42h: ファイルシーク
- AH=3Eh: ファイルクローズ
```

**推定サイズ**: ~250行

### 4. FileOutputStream

**ファイル**: `src/runtime/fileoutputstream.h`, `src/runtime/fileoutputstream.c`

#### データ構造

```c
typedef struct FileOutputStream {
    OutputStream base;        /* 基底ストリーム */
    int16_t file_handle;      /* DOSファイルハンドル */
    char filename[80];        /* ファイル名 */
    uint8_t append_mode;      /* 追記モード */
} FileOutputStream;
```

#### 主要メソッド

```c
/* ファイルオープン */
FileOutputStream* fileoutputstream_new(const char* filename);
FileOutputStream* fileoutputstream_new_append(const char* filename);
void fileoutputstream_delete(FileOutputStream* fos);

/* 書き込み操作 */
int fileoutputstream_write(FileOutputStream* fos, uint8_t byte);
int fileoutputstream_write_bytes(FileOutputStream* fos, const uint8_t* buf, uint16_t len);
void fileoutputstream_flush(FileOutputStream* fos);
void fileoutputstream_close(FileOutputStream* fos);
```

#### DOS API使用

```c
/* DOS INT 21h functions */
- AH=3Ch: ファイル作成
- AH=3Dh: ファイルオープン（書き込みモード）
- AH=40h: ファイル書き込み
- AH=42h: ファイルシーク（追記モード用）
- AH=3Eh: ファイルクローズ
```

**推定サイズ**: ~280行

### 5. BufferedReader

**ファイル**: `src/runtime/bufferedreader.h`, `src/runtime/bufferedreader.c`

#### データ構造

```c
#define BUFFER_SIZE 512

typedef struct BufferedReader {
    Object base;              /* 基底オブジェクト */
    InputStream* input;       /* 入力ストリーム */
    uint8_t buffer[BUFFER_SIZE];  /* 読み込みバッファ */
    uint16_t buffer_pos;      /* バッファ内位置 */
    uint16_t buffer_len;      /* バッファ内データ長 */
    uint8_t is_open;          /* オープン状態 */
} BufferedReader;
```

#### 主要メソッド

```c
/* 初期化 */
BufferedReader* bufferedreader_new(InputStream* input);
void bufferedreader_delete(BufferedReader* br);

/* 読み込み操作 */
int bufferedreader_read(BufferedReader* br);                  /* 1文字読み込み */
char* bufferedreader_readline(BufferedReader* br);            /* 1行読み込み */
int bufferedreader_ready(BufferedReader* br);                 /* 読み込み可能か */
void bufferedreader_close(BufferedReader* br);
```

**推定サイズ**: ~300行

### 6. BufferedWriter

**ファイル**: `src/runtime/bufferedwriter.h`, `src/runtime/bufferedwriter.c`

#### データ構造

```c
#define BUFFER_SIZE 512

typedef struct BufferedWriter {
    Object base;              /* 基底オブジェクト */
    OutputStream* output;     /* 出力ストリーム */
    uint8_t buffer[BUFFER_SIZE];  /* 書き込みバッファ */
    uint16_t buffer_pos;      /* バッファ内位置 */
    uint8_t is_open;          /* オープン状態 */
} BufferedWriter;
```

#### 主要メソッド

```c
/* 初期化 */
BufferedWriter* bufferedwriter_new(OutputStream* output);
void bufferedwriter_delete(BufferedWriter* bw);

/* 書き込み操作 */
int bufferedwriter_write(BufferedWriter* bw, char c);         /* 1文字書き込み */
int bufferedwriter_write_string(BufferedWriter* bw, const char* str);  /* 文字列書き込み */
int bufferedwriter_newline(BufferedWriter* bw);               /* 改行書き込み */
void bufferedwriter_flush(BufferedWriter* bw);                /* フラッシュ */
void bufferedwriter_close(BufferedWriter* bw);
```

**推定サイズ**: ~280行

## VM拡張

### 新しいオペコード

```c
/* ファイルI/O操作 */
OP_FILE_OPEN_READ    = 0x90,  /* ファイルを読み込みモードでオープン */
OP_FILE_OPEN_WRITE   = 0x91,  /* ファイルを書き込みモードでオープン */
OP_FILE_READ         = 0x92,  /* ファイルから読み込み */
OP_FILE_WRITE        = 0x93,  /* ファイルへ書き込み */
OP_FILE_CLOSE        = 0x94,  /* ファイルをクローズ */
OP_FILE_FLUSH        = 0x95,  /* バッファをフラッシュ */
```

### インタープリタ拡張

**ファイル**: `src/vm/interpreter.c`

```c
case OP_FILE_OPEN_READ: {
    /* スタックからファイル名を取得 */
    String* filename = (String*)stack_pop_ref(vm->stack);
    
    /* FileInputStreamを作成 */
    FileInputStream* fis = fileinputstream_new(string_get_cstr(filename));
    if (!fis) {
        vm_error(vm, "Failed to open file for reading");
        return -1;
    }
    
    /* スタックにプッシュ */
    stack_push_ref(vm->stack, (Object*)fis);
    break;
}

case OP_FILE_WRITE: {
    /* スタックからデータとストリームを取得 */
    String* data = (String*)stack_pop_ref(vm->stack);
    FileOutputStream* fos = (FileOutputStream*)stack_pop_ref(vm->stack);
    
    /* 書き込み */
    const char* cstr = string_get_cstr(data);
    fileoutputstream_write_bytes(fos, (const uint8_t*)cstr, strlen(cstr));
    
    /* ストリームを戻す */
    stack_push_ref(vm->stack, (Object*)fos);
    break;
}
```

**推定追加サイズ**: ~200行

## コンパイラ拡張

### 新しいネイティブメソッド認識

**ファイル**: `tools/compiler/codegen.c`

```c
/* FileOutputStream.write(String) */
if (strcmp(class_name, "FileOutputStream") == 0 && 
    strcmp(method_name, "write") == 0) {
    emit_opcode(codegen, OP_FILE_WRITE);
    return 0;
}

/* BufferedWriter.write(String) */
if (strcmp(class_name, "BufferedWriter") == 0 && 
    strcmp(method_name, "write") == 0) {
    emit_opcode(codegen, OP_FILE_WRITE);
    return 0;
}

/* close() メソッド */
if (strcmp(method_name, "close") == 0) {
    emit_opcode(codegen, OP_FILE_CLOSE);
    return 0;
}
```

**推定追加サイズ**: ~150行

## 実装ステップ

### Week 1: 基底クラスとファイルストリーム

#### Day 1-2: 基底クラス実装
- [ ] `InputStream` 基底クラス (inputstream.h/c)
- [ ] `OutputStream` 基底クラス (outputstream.h/c)
- [ ] 基本的なテスト

#### Day 3-4: FileInputStream実装
- [ ] `FileInputStream` 実装 (fileinputstream.h/c)
- [ ] DOS API統合（INT 21h）
- [ ] 読み込みテスト

#### Day 5-7: FileOutputStream実装
- [ ] `FileOutputStream` 実装 (fileoutputstream.h/c)
- [ ] DOS API統合（INT 21h）
- [ ] 書き込みテスト
- [ ] 追記モードテスト

### Week 2: バッファリングとVM統合

#### Day 8-10: BufferedReader/Writer実装
- [ ] `BufferedReader` 実装 (bufferedreader.h/c)
- [ ] `BufferedWriter` 実装 (bufferedwriter.h/c)
- [ ] バッファリングテスト

#### Day 11-12: VM拡張
- [ ] 新しいオペコード追加
- [ ] インタープリタ拡張
- [ ] コンパイラ拡張

#### Day 13-14: 統合テストとデバッグ
- [ ] エンドツーエンドテスト
- [ ] パフォーマンステスト
- [ ] バグ修正

## テストケース

### Test 1: 基本的なファイル書き込み

```java
class TestFileWrite {
    public static void main() {
        FileOutputStream fos = new FileOutputStream("TEST.TXT");
        fos.write("Hello File");
        fos.close();
    }
}
```

**期待される動作**:
- `TEST.TXT` ファイルが作成される
- ファイルに "Hello File" が書き込まれる

### Test 2: ファイル読み込み

```java
class TestFileRead {
    public static void main() {
        FileInputStream fis = new FileInputStream("TEST.TXT");
        int b = fis.read();
        while (b != -1) {
            System.out.print((char)b);
            b = fis.read();
        }
        fis.close();
    }
}
```

**期待される動作**:
- `TEST.TXT` の内容が画面に表示される

### Test 3: BufferedWriter使用

```java
class TestBufferedWrite {
    public static void main() {
        FileOutputStream fos = new FileOutputStream("BUFFER.TXT");
        BufferedWriter bw = new BufferedWriter(fos);
        bw.write("Line 1");
        bw.newLine();
        bw.write("Line 2");
        bw.close();
    }
}
```

**期待される動作**:
- `BUFFER.TXT` に2行のテキストが書き込まれる

### Test 4: BufferedReader使用

```java
class TestBufferedRead {
    public static void main() {
        FileInputStream fis = new FileInputStream("BUFFER.TXT");
        BufferedReader br = new BufferedReader(fis);
        String line = br.readLine();
        while (line != null) {
            System.out.println(line);
            line = br.readLine();
        }
        br.close();
    }
}
```

**期待される動作**:
- `BUFFER.TXT` の各行が画面に表示される

### Test 5: 追記モード

```java
class TestAppend {
    public static void main() {
        FileOutputStream fos = new FileOutputStream("APPEND.TXT", true);
        fos.write("Appended text");
        fos.close();
    }
}
```

**期待される動作**:
- 既存ファイルの末尾にテキストが追加される

## ビルドシステム統合

### Makefile更新

```makefile
# Runtime library - I/O streams
$(OBJ_DIR)/inputstream.obj: src/runtime/inputstream.c src/runtime/inputstream.h
	@echo Compiling inputstream.c...
	$(CC) $(CFLAGS) -fo=$@ src/runtime/inputstream.c

$(OBJ_DIR)/outputstream.obj: src/runtime/outputstream.c src/runtime/outputstream.h
	@echo Compiling outputstream.c...
	$(CC) $(CFLAGS) -fo=$@ src/runtime/outputstream.c

$(OBJ_DIR)/fileinputstream.obj: src/runtime/fileinputstream.c src/runtime/fileinputstream.h
	@echo Compiling fileinputstream.c...
	$(CC) $(CFLAGS) -fo=$@ src/runtime/fileinputstream.c

$(OBJ_DIR)/fileoutputstream.obj: src/runtime/fileoutputstream.c src/runtime/fileoutputstream.h
	@echo Compiling fileoutputstream.c...
	$(CC) $(CFLAGS) -fo=$@ src/runtime/fileoutputstream.c

$(OBJ_DIR)/bufferedreader.obj: src/runtime/bufferedreader.c src/runtime/bufferedreader.h
	@echo Compiling bufferedreader.c...
	$(CC) $(CFLAGS) -fo=$@ src/runtime/bufferedreader.c

$(OBJ_DIR)/bufferedwriter.obj: src/runtime/bufferedwriter.c src/runtime/bufferedwriter.h
	@echo Compiling bufferedwriter.c...
	$(CC) $(CFLAGS) -fo=$@ src/runtime/bufferedwriter.c

# Update djvm.exe dependencies
RUNTIME_OBJS = $(OBJ_DIR)/object.obj $(OBJ_DIR)/string.obj $(OBJ_DIR)/system.obj \
               $(OBJ_DIR)/integer.obj $(OBJ_DIR)/inputstream.obj $(OBJ_DIR)/outputstream.obj \
               $(OBJ_DIR)/fileinputstream.obj $(OBJ_DIR)/fileoutputstream.obj \
               $(OBJ_DIR)/bufferedreader.obj $(OBJ_DIR)/bufferedwriter.obj
```

## エラーハンドリング

### ファイル操作エラー

```c
/* エラーコード */
#define FILE_ERROR_NOT_FOUND    -1
#define FILE_ERROR_ACCESS       -2
#define FILE_ERROR_DISK_FULL    -3
#define FILE_ERROR_TOO_MANY     -4
#define FILE_ERROR_INVALID      -5

/* エラーメッセージ */
const char* file_error_message(int error_code) {
    switch (error_code) {
        case FILE_ERROR_NOT_FOUND:  return "File not found";
        case FILE_ERROR_ACCESS:     return "Access denied";
        case FILE_ERROR_DISK_FULL:  return "Disk full";
        case FILE_ERROR_TOO_MANY:   return "Too many open files";
        case FILE_ERROR_INVALID:    return "Invalid file handle";
        default:                    return "Unknown error";
    }
}
```

## パフォーマンス考慮事項

### バッファサイズ

- **512バイト**: DOSセクタサイズに合わせて最適化
- 小さすぎる: システムコール回数増加
- 大きすぎる: メモリ不足

### ファイルハンドル管理

- 最大8個まで同時オープン（DOS制限）
- 使用後は必ずクローズ
- リソースリーク防止

### メモリ使用量

```
InputStream:         8 bytes
OutputStream:        8 bytes
FileInputStream:     96 bytes (8 + 2 + 80 + 2 + padding)
FileOutputStream:    92 bytes (8 + 2 + 80 + 1 + padding)
BufferedReader:      528 bytes (8 + 4 + 512 + 2 + 2)
BufferedWriter:      528 bytes (8 + 4 + 512 + 2)
---
合計（最大8ストリーム）: ~5KB
```

## 成功基準

Phase 3 完了の条件:

1. ✓ すべての基底クラスが実装されている
2. ✓ FileInputStream/OutputStreamが動作する
3. ✓ BufferedReader/Writerが動作する
4. ✓ すべてのテストケースがパスする
5. ✓ メモリリークがない
6. ✓ ファイルハンドルが正しく管理されている
7. ✓ エラーハンドリングが適切
8. ✓ ドキュメントが完備されている

## 次のフェーズ

Phase 3 完了後:

- **Phase 4**: 配列サポートの安定化
- **Phase 5**: オブジェクト指向機能の拡張

## 注意事項

### DOS環境の制約

1. **ファイル名**: 8.3形式（例: `FILENAME.TXT`）
2. **パス区切り**: バックスラッシュ `\`
3. **改行コード**: CR+LF (`\r\n`)
4. **テキストモード**: 自動変換に注意

### メモリ管理

- すべてのストリームオブジェクトは参照カウント管理
- `close()` 呼び出し後も参照が残る可能性
- 明示的な `delete` は不要（参照カウントが0で自動削除）

### エラー処理

- ファイル操作は常に失敗する可能性がある
- すべてのファイル操作後にエラーチェック
- リソースリークを防ぐため、エラー時も `close()` を呼ぶ

---

**作成日**: 2026-05-11  
**最終更新**: 2026-05-11