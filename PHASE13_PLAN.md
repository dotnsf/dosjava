# Phase 13: String Method Enhancement - 開発計画

## 概要

Phase 13では、DOSJavaのString機能を大幅に拡張し、一般的なJavaプログラミングで頻繁に使用される8つのメソッドを追加実装します。これにより、文字列操作の利便性が大幅に向上し、より実用的なプログラムの作成が可能になります。

## 目標

### 主要目標
1. **8つの新規Stringメソッドの実装**
   - charAt(int) - 指定位置の文字取得
   - isEmpty() - 空文字列チェック
   - trim() - 前後の空白削除
   - replace(char, char) - 文字置換
   - compareTo(String) - 辞書順比較
   - lastIndexOf(String) - 後方検索
   - contains(String) - 部分文字列の存在チェック
   - repeat(int) - 文字列の繰り返し

2. **標準Javaとの互換性維持**
   - 可能な限り標準Java APIと同じ動作
   - 同じメソッド名とシグネチャ

3. **16-bit DOS環境への最適化**
   - メモリ効率的な実装
   - Large memory modelでの動作保証

## 実装するメソッド詳細

### Phase 13.1: charAt(int index)

**シグネチャ**: `char charAt(int index)`

**機能**: 指定されたインデックス位置の文字を返す

**パラメータ**:
- `index`: 文字位置（0-based）

**戻り値**: 指定位置の文字（char型として扱うが、DOSJavaではint型で返す）

**例外**:
- `StringIndexOutOfBoundsException`: インデックスが範囲外の場合

**実装場所**:
- `src/runtime/string.c`: `string_charat()` は既に存在（内部使用）
- `src/vm/interpreter.c`: VM実装を追加
- `src/vm/native.c`: ネイティブメソッド登録

**使用例**:
```java
String s = "Hello";
int c = s.charAt(1);  // 'e' (101)
System.out.println(c);
```

**実装難易度**: 低（既存関数の活用）

---

### Phase 13.2: isEmpty()

**シグネチャ**: `boolean isEmpty()`

**機能**: 文字列が空（長さ0）かどうかを判定

**パラメータ**: なし

**戻り値**: 
- `true` (1): 文字列が空の場合
- `false` (0): 文字列が空でない場合

**例外**: なし

**実装場所**:
- `src/runtime/string.c`: `string_isempty()` を追加
- `src/vm/interpreter.c`: VM実装を追加
- `src/vm/native.c`: ネイティブメソッド登録

**使用例**:
```java
String s1 = "";
String s2 = "Hello";
boolean b1 = s1.isEmpty();  // true
boolean b2 = s2.isEmpty();  // false
```

**実装難易度**: 極低

---

### Phase 13.3: trim()

**シグネチャ**: `String trim()`

**機能**: 文字列の前後の空白文字（スペース、タブ、改行）を削除した新しい文字列を返す

**パラメータ**: なし

**戻り値**: 前後の空白を削除した新しい文字列

**例外**: なし

**実装場所**:
- `src/runtime/string.c`: `string_trim()` を追加
- `src/vm/interpreter.c`: VM実装を追加
- `src/vm/native.c`: ネイティブメソッド登録

**使用例**:
```java
String s = "  Hello World  ";
String trimmed = s.trim();  // "Hello World"
System.out.println(trimmed);
```

**実装詳細**:
- 空白文字の定義: スペース(' '), タブ('\t'), 改行('\n'), キャリッジリターン('\r')
- 前方から最初の非空白文字を探す
- 後方から最初の非空白文字を探す
- その範囲の部分文字列を返す

**実装難易度**: 低

---

### Phase 13.4: replace(char oldChar, char newChar)

**シグネチャ**: `String replace(char oldChar, char newChar)`

**機能**: 文字列内の全ての`oldChar`を`newChar`に置換した新しい文字列を返す

**パラメータ**:
- `oldChar`: 置換対象の文字（int型として渡される）
- `newChar`: 置換後の文字（int型として渡される）

**戻り値**: 置換後の新しい文字列

**例外**: なし

**実装場所**:
- `src/runtime/string.c`: `string_replace_char()` を追加
- `src/vm/interpreter.c`: VM実装を追加
- `src/vm/native.c`: ネイティブメソッド登録

**使用例**:
```java
String s = "Hello World";
String replaced = s.replace('o', 'a');  // "Hella Warld"
System.out.println(replaced);
```

**実装詳細**:
- 元の文字列と同じ長さの新しい文字列を作成
- 各文字をチェックし、oldCharならnewCharに置換
- 置換が1つもない場合でも新しい文字列を返す

**実装難易度**: 低

---

### Phase 13.5: compareTo(String anotherString)

**シグネチャ**: `int compareTo(String anotherString)`

**機能**: 2つの文字列を辞書順で比較

**パラメータ**:
- `anotherString`: 比較対象の文字列

**戻り値**:
- 負の値: この文字列が引数より小さい
- 0: 両方の文字列が等しい
- 正の値: この文字列が引数より大きい

**例外**:
- `NullPointerException`: 引数がnullの場合

**実装場所**:
- `src/runtime/string.c`: `string_compare()` は既に存在
- `src/vm/interpreter.c`: VM実装を追加
- `src/vm/native.c`: ネイティブメソッド登録

**使用例**:
```java
String s1 = "apple";
String s2 = "banana";
int result = s1.compareTo(s2);  // 負の値（"apple" < "banana"）
if (result < 0) {
    System.out.println("s1 is less than s2");
}
```

**実装難易度**: 低（既存関数の活用）

---

### Phase 13.6: lastIndexOf(String str)

**シグネチャ**: `int lastIndexOf(String str)`

**機能**: 指定された部分文字列が最後に出現する位置を返す

**パラメータ**:
- `str`: 検索する部分文字列

**戻り値**: 
- 最後に出現する位置（0-based）
- -1: 見つからない場合

**例外**: なし

**実装場所**:
- `src/runtime/string.c`: `string_lastindexof()` を追加
- `src/vm/interpreter.c`: VM実装を追加
- `src/vm/native.c`: ネイティブメソッド登録

**使用例**:
```java
String s = "Hello World Hello";
int pos = s.lastIndexOf("Hello");  // 12
System.out.println(pos);
```

**実装詳細**:
- 文字列の末尾から前方に向かって検索
- 最初に見つかった位置を返す
- 空文字列の検索は文字列の長さを返す

**実装難易度**: 中

---

### Phase 13.7: contains(String str)

**シグネチャ**: `boolean contains(String str)`

**機能**: 指定された部分文字列が含まれているかを判定

**パラメータ**:
- `str`: 検索する部分文字列

**戻り値**:
- `true` (1): 部分文字列が含まれる
- `false` (0): 部分文字列が含まれない

**例外**: なし

**実装場所**:
- `src/runtime/string.c`: `string_contains()` を追加（indexOfを使用）
- `src/vm/interpreter.c`: VM実装を追加
- `src/vm/native.c`: ネイティブメソッド登録

**使用例**:
```java
String s = "Hello World";
boolean b1 = s.contains("World");  // true
boolean b2 = s.contains("Java");   // false
```

**実装詳細**:
- 内部的に`indexOf(str) >= 0`を実行
- 既存のindexOf実装を活用

**実装難易度**: 極低

---

### Phase 13.8: repeat(int count)

**シグネチャ**: `String repeat(int count)`

**機能**: 文字列を指定回数繰り返した新しい文字列を返す

**パラメータ**:
- `count`: 繰り返し回数（0以上）

**戻り値**: 繰り返された文字列

**例外**:
- `IllegalArgumentException`: countが負の場合

**実装場所**:
- `src/runtime/string.c`: `string_repeat()` を追加
- `src/vm/interpreter.c`: VM実装を追加
- `src/vm/native.c`: ネイティブメソッド登録

**使用例**:
```java
String s = "Ha";
String repeated = s.repeat(3);  // "HaHaHa"
System.out.println(repeated);
```

**実装詳細**:
- count = 0 の場合は空文字列を返す
- count = 1 の場合は元の文字列のコピーを返す
- メモリ制約を考慮（最大長を制限）

**実装難易度**: 低

---

## 実装順序と依存関係

### フェーズ1: 基本メソッド（既存機能の活用）
1. **Phase 13.1: charAt()** - 既存の`string_charat()`を活用
2. **Phase 13.2: isEmpty()** - 単純な長さチェック
3. **Phase 13.5: compareTo()** - 既存の`string_compare()`を活用

### フェーズ2: 新規実装（単純）
4. **Phase 13.3: trim()** - 新規実装、依存なし
5. **Phase 13.4: replace()** - 新規実装、依存なし
6. **Phase 13.7: contains()** - 既存のindexOfを活用

### フェーズ3: 新規実装（やや複雑）
7. **Phase 13.6: lastIndexOf()** - 新規実装、後方検索
8. **Phase 13.8: repeat()** - 新規実装、メモリ管理が重要

### フェーズ4: テストと検証
9. **Phase 13.9: Testing and validation**
10. **Phase 13.10: Documentation**

## 技術的考慮事項

### メモリ管理
- **文字列バッファサイズ**: 最大256バイト（既存の制約）
- **repeat()の制限**: 結果が256バイトを超える場合はエラー
- **動的メモリ割り当て**: `memory_alloc()`を使用

### 例外処理
- Phase 11/12で実装済みの例外システムを活用
- 適切な例外タイプを使用:
  - `StringIndexOutOfBoundsException`: charAt()
  - `IllegalArgumentException`: repeat()（負の回数）
  - `NullPointerException`: compareTo()（null引数）

### 既存コードとの統合
- **string.c**: 基本関数を追加
- **interpreter.c**: VM実装を追加（OP_INVOKE_STATIC処理）
- **native.c**: ネイティブメソッド登録

### 16-bit DOS制約
- **int型**: 16-bit（-32768 ～ 32767）
- **char型**: 8-bit（0 ～ 255）
- **メモリセグメント**: Large memory model使用

## テスト計画

### 単体テスト（各メソッド）

#### charAt()
```java
String s = "Hello";
int c1 = s.charAt(0);   // 'H' (72)
int c2 = s.charAt(4);   // 'o' (111)
// int c3 = s.charAt(5);  // StringIndexOutOfBoundsException
```

#### isEmpty()
```java
String s1 = "";
String s2 = "Hello";
boolean b1 = s1.isEmpty();  // true
boolean b2 = s2.isEmpty();  // false
```

#### trim()
```java
String s1 = "  Hello  ";
String s2 = "\tWorld\n";
String t1 = s1.trim();  // "Hello"
String t2 = s2.trim();  // "World"
```

#### replace()
```java
String s = "Hello World";
String r1 = s.replace('o', 'a');  // "Hella Warld"
String r2 = s.replace('x', 'y');  // "Hello World"（変更なし）
```

#### compareTo()
```java
String s1 = "apple";
String s2 = "banana";
String s3 = "apple";
int c1 = s1.compareTo(s2);  // < 0
int c2 = s1.compareTo(s3);  // == 0
int c3 = s2.compareTo(s1);  // > 0
```

#### lastIndexOf()
```java
String s = "Hello World Hello";
int p1 = s.lastIndexOf("Hello");  // 12
int p2 = s.lastIndexOf("Java");   // -1
int p3 = s.lastIndexOf("o");      // 16
```

#### contains()
```java
String s = "Hello World";
boolean b1 = s.contains("World");  // true
boolean b2 = s.contains("Java");   // false
boolean b3 = s.contains("");       // true
```

#### repeat()
```java
String s = "Ha";
String r1 = s.repeat(0);  // ""
String r2 = s.repeat(1);  // "Ha"
String r3 = s.repeat(3);  // "HaHaHa"
// String r4 = s.repeat(-1);  // IllegalArgumentException
```

### 統合テスト

**tests/strext.jav**: 全メソッドの包括的テスト
```java
class StringExtensionTest {
    public static void main() {
        testCharAt();
        testIsEmpty();
        testTrim();
        testReplace();
        testCompareTo();
        testLastIndexOf();
        testContains();
        testRepeat();
        System.out.println("All tests passed!");
    }
    
    static void testCharAt() { /* ... */ }
    static void testIsEmpty() { /* ... */ }
    // ... 他のテストメソッド
}
```

**samples/strext.jav**: サンプルプログラム
```java
class StringExtensionDemo {
    public static void main() {
        String s = "  Hello World  ";
        
        // trim
        String trimmed = s.trim();
        System.out.println(trimmed);  // "Hello World"
        
        // replace
        String replaced = trimmed.replace('o', 'a');
        System.out.println(replaced);  // "Hella Warld"
        
        // contains
        boolean hasWorld = trimmed.contains("World");
        System.out.println(hasWorld);  // true
        
        // repeat
        String line = "-".repeat(10);
        System.out.println(line);  // "----------"
    }
}
```

## ファイル変更一覧

### 新規作成
- `dosjava/PHASE13_PLAN.md` - この計画書
- `dosjava/PHASE13_COMPLETION.md` - 完了報告（実装後）
- `dosjava/tests/strext.jav` - 統合テストプログラム
- `dosjava/samples/strext.jav` - サンプルプログラム

### 変更ファイル
- `dosjava/src/runtime/string.h` - 新規関数宣言を追加
- `dosjava/src/runtime/string.c` - 新規関数実装を追加
- `dosjava/src/vm/interpreter.c` - VM実装を追加
- `dosjava/src/vm/native.c` - ネイティブメソッド登録を追加
- `dosjava/README.md` - Phase 13の説明を追加

## マイルストーン

### Week 1: 基本メソッド実装
- [ ] Phase 13.1: charAt() 実装
- [ ] Phase 13.2: isEmpty() 実装
- [ ] Phase 13.5: compareTo() 実装
- [ ] 基本テスト実行

### Week 2: 新規メソッド実装（単純）
- [ ] Phase 13.3: trim() 実装
- [ ] Phase 13.4: replace() 実装
- [ ] Phase 13.7: contains() 実装
- [ ] 中間テスト実行

### Week 3: 新規メソッド実装（複雑）
- [ ] Phase 13.6: lastIndexOf() 実装
- [ ] Phase 13.8: repeat() 実装
- [ ] 全メソッドの統合テスト

### Week 4: テストと文書化
- [ ] Phase 13.9: 包括的テスト
- [ ] Phase 13.10: ドキュメント作成
- [ ] README.md更新
- [ ] サンプルプログラム作成

## 成功基準

### 機能要件
- ✅ 8つの新規Stringメソッドが正常に動作
- ✅ 標準Javaと同じ動作（可能な範囲で）
- ✅ 適切な例外処理

### 品質要件
- ✅ 全テストケースがパス
- ✅ メモリリークなし
- ✅ 16-bit DOS環境で安定動作

### ドキュメント要件
- ✅ 各メソッドの使用例を含むドキュメント
- ✅ README.mdの更新
- ✅ サンプルプログラムの提供

## リスク管理

### 技術的リスク

**リスク1: メモリ制約**
- **影響**: repeat()で大きな文字列を生成できない
- **対策**: 最大長を制限し、超過時はエラー

**リスク2: 16-bit int制約**
- **影響**: charAt()の戻り値がchar型ではなくint型
- **対策**: ドキュメントで明記、テストで検証

**リスク3: 既存コードとの互換性**
- **影響**: 新規メソッドが既存機能に影響
- **対策**: 段階的実装、各段階でテスト

### スケジュールリスク

**リスク4: 実装の複雑さ**
- **影響**: lastIndexOf()の実装に時間がかかる
- **対策**: 優先順位を設定、必要に応じて調整

## 次のステップ

1. **Phase 13.1の実装開始**: charAt()メソッドから着手
2. **継続的テスト**: 各メソッド実装後に単体テスト
3. **ドキュメント更新**: 実装と並行してドキュメント作成
4. **コードレビュー**: 各フェーズ完了時にレビュー

---

**作成日**: 2026-05-29  
**対象バージョン**: DOSJava Phase 13  
**ステータス**: 計画中