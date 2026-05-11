# Phase 3: ストリームとI/O基盤 - クイックスタートガイド

## 概要

このガイドでは、Phase 3で実装するファイルI/O機能の基本的な使い方を説明します。

## 前提条件

- Phase 2（String型の完全サポート）が完了していること
- djc.exe と djvm.exe がビルドされていること
- DOSBox または実機DOS環境

## 基本的なファイル操作

### 1. ファイルへの書き込み

最もシンプルなファイル書き込みの例：

```java
class SimpleWrite {
    public static void main() {
        // ファイルを作成してオープン
        FileOutputStream fos = new FileOutputStream("HELLO.TXT");
        
        // 文字列を書き込み
        fos.write("Hello, DOS File!");
        
        // ファイルをクローズ
        fos.close();
        
        System.out.println("File written successfully");
    }
}
```

**コンパイルと実行**:
```batch
djc SimpleWrite.jav
djvm SimpleWrite.djc
```

**結果**: `HELLO.TXT` ファイルが作成され、"Hello, DOS File!" が書き込まれます。

### 2. ファイルからの読み込み

ファイルの内容を読み込んで表示：

```java
class SimpleRead {
    public static void main() {
        // ファイルをオープン
        FileInputStream fis = new FileInputStream("HELLO.TXT");
        
        // 1バイトずつ読み込み
        int b = fis.read();
        while (b != -1) {
            System.out.print((char)b);
            b = fis.read();
        }
        
        // ファイルをクローズ
        fis.close();
        
        System.out.println();
        System.out.println("File read successfully");
    }
}
```

**実行**:
```batch
djc SimpleRead.jav
djvm SimpleRead.djc
```

**結果**: `HELLO.TXT` の内容が画面に表示されます。

## バッファリングを使った効率的なI/O

### 3. BufferedWriter で行単位の書き込み

```java
class BufferedWrite {
    public static void main() {
        // FileOutputStreamを作成
        FileOutputStream fos = new FileOutputStream("LINES.TXT");
        
        // BufferedWriterでラップ
        BufferedWriter bw = new BufferedWriter(fos);
        
        // 複数行を書き込み
        bw.write("First line");
        bw.newLine();
        bw.write("Second line");
        bw.newLine();
        bw.write("Third line");
        bw.newLine();
        
        // クローズ（自動的にフラッシュされる）
        bw.close();
        
        System.out.println("Lines written successfully");
    }
}
```

### 4. BufferedReader で行単位の読み込み

```java
class BufferedRead {
    public static void main() {
        // FileInputStreamを作成
        FileInputStream fis = new FileInputStream("LINES.TXT");
        
        // BufferedReaderでラップ
        BufferedReader br = new BufferedReader(fis);
        
        // 行単位で読み込み
        String line = br.readLine();
        int lineNum = 1;
        
        while (line != null) {
            System.out.print(lineNum);
            System.out.print(": ");
            System.out.println(line);
            
            lineNum = lineNum + 1;
            line = br.readLine();
        }
        
        // クローズ
        br.close();
        
        System.out.println("File read successfully");
    }
}
```

## 実用的な例

### 5. ファイルコピー

```java
class FileCopy {
    public static void main() {
        // 入力ファイルをオープン
        FileInputStream fis = new FileInputStream("SOURCE.TXT");
        
        // 出力ファイルを作成
        FileOutputStream fos = new FileOutputStream("DEST.TXT");
        
        // バイト単位でコピー
        int b = fis.read();
        int count = 0;
        
        while (b != -1) {
            fos.write(b);
            count = count + 1;
            b = fis.read();
        }
        
        // 両方のファイルをクローズ
        fis.close();
        fos.close();
        
        System.out.print("Copied ");
        System.out.print(count);
        System.out.println(" bytes");
    }
}
```

### 6. テキストファイルの行数カウント

```java
class LineCount {
    public static void main() {
        FileInputStream fis = new FileInputStream("INPUT.TXT");
        BufferedReader br = new BufferedReader(fis);
        
        int count = 0;
        String line = br.readLine();
        
        while (line != null) {
            count = count + 1;
            line = br.readLine();
        }
        
        br.close();
        
        System.out.print("Total lines: ");
        System.out.println(count);
    }
}
```

### 7. ファイルへの追記

```java
class AppendFile {
    public static void main() {
        // 追記モードでオープン（第2引数にtrue）
        FileOutputStream fos = new FileOutputStream("LOG.TXT", true);
        BufferedWriter bw = new BufferedWriter(fos);
        
        // 新しい行を追加
        bw.write("New log entry");
        bw.newLine();
        
        bw.close();
        
        System.out.println("Log entry added");
    }
}
```

## エラーハンドリング

### 8. ファイルが存在しない場合の処理

```java
class SafeRead {
    public static void main() {
        // ファイルオープンを試みる
        FileInputStream fis = new FileInputStream("NOFILE.TXT");
        
        // fis が null の場合、ファイルが存在しない
        if (fis == null) {
            System.out.println("Error: File not found");
            return;
        }
        
        // ファイル処理
        int b = fis.read();
        while (b != -1) {
            System.out.print((char)b);
            b = fis.read();
        }
        
        fis.close();
    }
}
```

## ベストプラクティス

### 1. 必ずファイルをクローズする

```java
// 良い例
FileOutputStream fos = new FileOutputStream("FILE.TXT");
fos.write("data");
fos.close();  // 必ずクローズ

// 悪い例
FileOutputStream fos = new FileOutputStream("FILE.TXT");
fos.write("data");
// クローズし忘れ → ファイルハンドルリーク
```

### 2. バッファリングを活用する

```java
// 効率的（バッファリングあり）
FileOutputStream fos = new FileOutputStream("FILE.TXT");
BufferedWriter bw = new BufferedWriter(fos);
bw.write("line 1");
bw.newLine();
bw.write("line 2");
bw.close();

// 非効率的（バッファリングなし）
FileOutputStream fos = new FileOutputStream("FILE.TXT");
fos.write("line 1\r\n");
fos.write("line 2\r\n");
fos.close();
```

### 3. エラーチェックを行う

```java
FileInputStream fis = new FileInputStream("FILE.TXT");
if (fis == null) {
    System.out.println("Error: Cannot open file");
    return;
}
// 正常処理
fis.close();
```

## DOS環境での注意事項

### ファイル名の制約

DOS環境では8.3形式のファイル名を使用してください：

```java
// OK
FileOutputStream fos = new FileOutputStream("OUTPUT.TXT");
FileOutputStream fos = new FileOutputStream("DATA.DAT");
FileOutputStream fos = new FileOutputStream("LOG.LOG");

// NG（長すぎる）
FileOutputStream fos = new FileOutputStream("VERYLONGFILENAME.TXT");
```

### 改行コード

DOS環境では改行は CR+LF (`\r\n`) です。`BufferedWriter.newLine()` を使用すると自動的に正しい改行コードが挿入されます。

```java
// 推奨
bw.write("line");
bw.newLine();  // 自動的に \r\n が挿入される

// 手動で指定する場合
fos.write("line\r\n");
```

### ファイルハンドルの制限

DOS環境では同時にオープンできるファイル数に制限があります（通常8個）。使い終わったファイルは必ずクローズしてください。

## テスト方法

### DOSBox でのテスト

1. DOSBox を起動
2. dosjava ディレクトリに移動
3. テストプログラムをコンパイル・実行

```batch
C:\> cd dosjava\build\bin
C:\dosjava\build\bin> djc ..\..\tests\io\filewrite.jav
C:\dosjava\build\bin> djvm ..\..\tests\io\filewrite.djc
```

### 結果の確認

```batch
C:\dosjava\build\bin> type OUTPUT.TXT
Hello, DOS File!
```

## トラブルシューティング

### 問題: ファイルが作成されない

**原因**: ディスクが書き込み禁止、またはディスク容量不足

**解決策**:
- ディスクの書き込み権限を確認
- ディスク容量を確認
- 別のディレクトリで試す

### 問題: ファイルが読めない

**原因**: ファイルが存在しない、またはパスが間違っている

**解決策**:
- ファイル名を確認（8.3形式）
- パスを確認
- `dir` コマンドでファイルの存在を確認

### 問題: "Too many open files" エラー

**原因**: ファイルをクローズし忘れている

**解決策**:
- すべてのファイル操作後に `close()` を呼ぶ
- 不要になったファイルはすぐにクローズする

## 次のステップ

Phase 3 の実装が完了したら：

1. より複雑なファイル操作を試す
2. 複数ファイルの同時処理
3. バイナリファイルの処理
4. Phase 4（配列サポートの安定化）へ進む

## 参考資料

- `PHASE3_IO_PLAN.md` - 詳細な実装計画
- `PHASE3_IO_TASKS.md` - 実装タスクリスト
- `docs/API_IO.md` - API リファレンス（実装後）

---

**作成日**: 2026-05-11  
**最終更新**: 2026-05-11