# Phase 3.5: Exception Handling and Date Support - Quick Start Guide

## Overview

このガイドでは、Phase 3.5で実装する例外処理（try-catch-finally）とDate機能の使い方を説明します。

## Phase 3.5.1: Exception Handling

### 基本的な使い方

#### 1. Simple try-catch

```java
class Example1 {
    public static void main(String[] args) {
        try {
            System.out.println("Processing...");
            throw new Exception("Something went wrong");
            System.out.println("This will not be printed");
        } catch (Exception e) {
            System.out.println("Error caught!");
        }
        System.out.println("Program continues");
    }
}
```

**期待される出力:**
```
Processing...
Error caught!
Program continues
```

#### 2. try-finally (例外なし)

```java
class Example2 {
    public static void main(String[] args) {
        try {
            System.out.println("Opening resource");
            System.out.println("Using resource");
        } finally {
            System.out.println("Closing resource");
        }
        System.out.println("Done");
    }
}
```

**期待される出力:**
```
Opening resource
Using resource
Closing resource
Done
```

#### 3. try-catch-finally (例外あり)

```java
class Example3 {
    public static void main(String[] args) {
        try {
            System.out.println("Opening file");
            throw new Exception("File not found");
        } catch (Exception e) {
            System.out.println("Error: File not found");
        } finally {
            System.out.println("Cleanup");
        }
        System.out.println("Done");
    }
}
```

**期待される出力:**
```
Opening file
Error: File not found
Cleanup
Done
```

### 実用例: ファイル処理

```java
class FileProcessor {
    public static void main(String[] args) {
        FileInputStream fis = null;
        
        try {
            fis = new FileInputStream("data.txt");
            
            int data;
            while ((data = fis.read()) != -1) {
                System.out.print((char)data);
            }
        } catch (Exception e) {
            System.out.println("Error reading file");
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (Exception e) {
                    System.out.println("Error closing file");
                }
            }
        }
    }
}
```

### ネストした例外処理

```java
class NestedExample {
    public static void main(String[] args) {
        try {
            System.out.println("Outer try");
            
            try {
                System.out.println("Inner try");
                throw new Exception("Inner error");
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

**期待される出力:**
```
Outer try
Inner try
Inner catch
After inner try-catch
Outer finally
```

## Phase 3.5.2: Date Support

### 基本的な使い方

#### 1. 現在時刻の取得

```java
class DateExample1 {
    public static void main(String[] args) {
        Date now = new Date();
        System.out.println("Current date: " + now.toString());
    }
}
```

**期待される出力:**
```
Current date: 2026-05-15 12:34:56
```

#### 2. 時刻コンポーネントの取得

```java
class DateExample2 {
    public static void main(String[] args) {
        Date now = new Date();
        
        System.out.println("Year: " + now.getFullYear());
        System.out.println("Month: " + now.getMonth());      // 0-11
        System.out.println("Date: " + now.getDate());        // 1-31
        System.out.println("Hours: " + now.getHours());      // 0-23
        System.out.println("Minutes: " + now.getMinutes());  // 0-59
        System.out.println("Seconds: " + now.getSeconds());  // 0-59
    }
}
```

**期待される出力:**
```
Year: 2026
Month: 4
Date: 15
Hours: 12
Minutes: 34
Seconds: 56
```

#### 3. タイムスタンプの取得と設定

```java
class DateExample3 {
    public static void main(String[] args) {
        Date date1 = new Date();
        long time = date1.getTime();
        
        System.out.println("Timestamp: " + time);
        
        Date date2 = new Date();
        date2.setTime(time);
        
        System.out.println("Date1: " + date1.toString());
        System.out.println("Date2: " + date2.toString());
    }
}
```

### 実用例: 経過時間の測定

```java
class TimerExample {
    public static void main(String[] args) {
        Date start = new Date();
        System.out.println("Start: " + start.toString());
        
        // 何か処理を実行
        int sum = 0;
        for (int i = 0; i < 10000; i++) {
            sum += i;
        }
        
        Date end = new Date();
        System.out.println("End: " + end.toString());
        
        long elapsed = end.getTime() - start.getTime();
        System.out.println("Elapsed: " + elapsed + " ms");
        System.out.println("Sum: " + sum);
    }
}
```

### 実用例: タイムスタンプ付きログ

```java
class Logger {
    public static void log(String message) {
        Date now = new Date();
        System.out.println("[" + now.toString() + "] " + message);
    }
    
    public static void main(String[] args) {
        log("Application started");
        log("Processing data");
        log("Application finished");
    }
}
```

**期待される出力:**
```
[2026-05-15 12:34:56] Application started
[2026-05-15 12:34:56] Processing data
[2026-05-15 12:34:56] Application finished
```

## 統合例: 例外処理とDate

### タイムアウト付き処理

```java
class TimeoutExample {
    public static void main(String[] args) {
        Date start = new Date();
        long timeout = 5000; // 5秒
        
        try {
            System.out.println("Processing started");
            
            // 長時間かかる処理のシミュレーション
            for (int i = 0; i < 1000000; i++) {
                Date now = new Date();
                long elapsed = now.getTime() - start.getTime();
                
                if (elapsed > timeout) {
                    throw new Exception("Timeout");
                }
                
                // 何か処理
                int x = i * i;
            }
            
            System.out.println("Processing completed");
        } catch (Exception e) {
            System.out.println("Error: Timeout occurred");
        } finally {
            Date end = new Date();
            long total = end.getTime() - start.getTime();
            System.out.println("Total time: " + total + " ms");
        }
    }
}
```

### ファイル処理のログ記録

```java
class FileLogger {
    public static void main(String[] args) {
        Date start = new Date();
        System.out.println("[" + start.toString() + "] File processing started");
        
        FileInputStream fis = null;
        
        try {
            fis = new FileInputStream("data.txt");
            System.out.println("[" + new Date().toString() + "] File opened");
            
            int count = 0;
            int data;
            while ((data = fis.read()) != -1) {
                count++;
            }
            
            System.out.println("[" + new Date().toString() + "] Read " + count + " bytes");
        } catch (Exception e) {
            System.out.println("[" + new Date().toString() + "] Error: " + e.getMessage());
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                    System.out.println("[" + new Date().toString() + "] File closed");
                } catch (Exception e) {
                    System.out.println("[" + new Date().toString() + "] Error closing file");
                }
            }
            
            Date end = new Date();
            long elapsed = end.getTime() - start.getTime();
            System.out.println("[" + end.toString() + "] Processing finished (" + elapsed + " ms)");
        }
    }
}
```

## コンパイルと実行

### コンパイル

```batch
REM Windows上で
cd dosjava
djc.exe examples\exception_example.jav
```

### DOSBox-Xでの実行

```batch
REM DOSBox-X内で
D:
CD \DOSJAVA\BUILD\BIN
DJVM.EXE EXCEPTION_EXAMPLE.DJC
```

## テスト

### 例外処理のテスト

```batch
REM Phase 3.5.1のテストを実行
cd dosjava\tests\exception
djc.exe test_basic.jav
djvm.exe test_basic.djc

djc.exe test_nested.jav
djvm.exe test_nested.djc

djc.exe test_finally.jav
djvm.exe test_finally.djc
```

### Dateのテスト

```batch
REM Phase 3.5.2のテストを実行
cd dosjava\tests\date
djc.exe test_date.jav
djvm.exe test_date.djc

djc.exe test_date_ops.jav
djvm.exe test_date_ops.djc
```

## トラブルシューティング

### 問題: "Unknown keyword: try"

**原因**: Lexerが更新されていない

**解決策**:
```batch
REM コンパイラを再ビルド
cd dosjava
wmake clean
wmake all
```

### 問題: "OP_THROW: Unknown opcode"

**原因**: VMが更新されていない

**解決策**:
```batch
REM VMを再ビルド
cd dosjava
wmake clean
wmake djvm
```

### 問題: Date objectが作成できない

**原因**: Date runtimeが組み込まれていない

**解決策**:
```batch
REM date.cとdostime.cがリンクされているか確認
wmake clean
wmake all
```

### 問題: finallyブロックが実行されない

**原因**: 例外ハンドラが正しく設定されていない

**解決策**:
- コンパイラのcodegen.cを確認
- 例外テーブルが正しく生成されているか確認
- VMのexception handlingコードを確認

## API Reference

### Exception Handling

#### try-catch-finally構文

```java
try {
    // 例外が発生する可能性のあるコード
} catch (Exception e) {
    // 例外処理
} finally {
    // 常に実行されるコード
}
```

#### throw文

```java
throw new Exception("Error message");
```

### Date Class

#### コンストラクタ

```java
Date date = new Date();              // 現在時刻
Date date = new Date(timestamp);     // 指定時刻（未実装）
```

#### メソッド

```java
long getTime()           // タイムスタンプ（ミリ秒）を取得
void setTime(long time)  // タイムスタンプを設定
int getFullYear()        // 年を取得（例: 2026）
int getMonth()           // 月を取得（0-11）
int getDate()            // 日を取得（1-31）
int getHours()           // 時を取得（0-23）
int getMinutes()         // 分を取得（0-59）
int getSeconds()         // 秒を取得（0-59）
String toString()        // 文字列表現を取得
```

## 制限事項

### Exception Handling
- 例外型は`Exception`のみサポート
- `IOException`等の特定例外型は未サポート
- 複数のcatchブロックは未サポート
- 例外メッセージの取得は未実装

### Date
- タイムゾーンは未サポート（ローカル時刻のみ）
- ミリ秒精度はDOS時刻APIに依存（実際は秒単位）
- 閏秒は未考慮
- 1970年以前の日付は未サポート
- Date演算（加算・減算）は手動で実装が必要

## 次のステップ

Phase 3.5完了後:
1. Phase 4（ネットワーク機能）の実装
2. 例外処理をネットワークエラーハンドリングに活用
3. DateをHTTPヘッダー処理やタイムアウト管理に活用

## 参考資料

- `PHASE3_5_PLAN.md` - 詳細な実装計画
- `PHASE3_5_TASKS.md` - タスクチェックリスト
- `tests/exception/README.md` - 例外処理テストガイド
- `tests/date/README.md` - Dateテストガイド

---

**Last Updated**: 2026-05-15
**Status**: Ready for implementation