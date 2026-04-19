# DOS Java Compiler - サンプルプログラム集

このディレクトリには、DOS Java Compilerで動作するサンプルプログラムが含まれています。

## サンプル一覧

### 1. Hello World (`01_hello.java`)
最も基本的なプログラム。変数の宣言と代入を学びます。

```batch
djc 01_hello.java
```

**学習内容**:
- クラス宣言
- mainメソッド
- 変数宣言
- return文

---

### 2. 算術演算 (`02_arithmetic.java`)
算術演算子の使用方法を学びます。

```batch
djc 02_arithmetic.java
```

**学習内容**:
- 加算（`+`）
- 減算（`-`）
- 乗算（`*`）
- 除算（`/`）
- 剰余（`%`）
- 複雑な式

---

### 3. 条件分岐 (`03_conditional.java`)
if文を使った条件分岐を学びます。

```batch
djc 03_conditional.java
```

**学習内容**:
- if文
- 比較演算子（`>`, `<`, `==`, `!=`）
- 論理演算子（`&&`, `||`）
- ネストしたif文

---

### 4. ループ (`04_loop.java`)
whileループを使った繰り返し処理を学びます。

```batch
djc 04_loop.java
```

**学習内容**:
- whileループ
- ループカウンタ
- ネストしたループ
- 累積計算

---

### 5. フィボナッチ数列 (`05_fibonacci.java`)
フィボナッチ数列を計算します。

```batch
djc 05_fibonacci.java
```

**学習内容**:
- 反復的アルゴリズム
- 変数の交換
- ループの活用

**出力**: F(10) = 55

---

### 6. 階乗 (`06_factorial.java`)
階乗（n!）を計算します。

```batch
djc 06_factorial.java
```

**学習内容**:
- 累積乗算
- ループ条件
- 数学的計算

**出力**: 5! = 120

---

### 7. 最大公約数 (`07_gcd.java`)
ユークリッドの互除法で最大公約数を計算します。

```batch
djc 07_gcd.java
```

**学習内容**:
- ユークリッドの互除法
- 剰余演算の活用
- アルゴリズムの実装

**出力**: GCD(48, 18) = 6

---

## 全サンプルのコンパイル

### Windowsバッチファイル

`compile_all.bat`を作成:
```batch
@echo off
echo Compiling all examples...
for %%f in (*.java) do (
    echo Compiling %%f...
    djc %%f
)
echo Done!
```

実行:
```batch
compile_all.bat
```

### DOSコマンド

```batch
djc 01_hello.java
djc 02_arithmetic.java
djc 03_conditional.java
djc 04_loop.java
djc 05_fibonacci.java
djc 06_factorial.java
djc 07_gcd.java
```

## 学習の進め方

### ステップ1: 基礎を学ぶ
1. `01_hello.java` - 基本構文
2. `02_arithmetic.java` - 演算子
3. `03_conditional.java` - 条件分岐

### ステップ2: 制御構造を学ぶ
4. `04_loop.java` - ループ

### ステップ3: アルゴリズムを学ぶ
5. `05_fibonacci.java` - 数列
6. `06_factorial.java` - 階乗
7. `07_gcd.java` - 最大公約数

## カスタマイズのヒント

### 値を変更してみる

例: フィボナッチ数列の項数を変更
```java
int n = 15;  // 10から15に変更
```

### 新しい計算を追加

例: 平均値の計算
```java
int sum = a + b + c;
int average = sum / 3;
```

### ループ条件を変更

例: 100までの合計
```java
while (i < 100) {  // 10から100に変更
    sum = sum + i;
    i = i + 1;
}
```

## トラブルシューティング

### コンパイルエラー

**セミコロン忘れ**:
```
Error: Parse error at line 5, column 20
```
→ 各文の最後に`;`を付けてください

**括弧の不一致**:
```
Error: Parse error at line 10
```
→ `{`と`}`の数を確認してください

### 論理エラー

**無限ループ**:
```java
while (i < 10) {
    sum = sum + i;
    // i = i + 1; を忘れた！
}
```
→ ループカウンタの更新を忘れずに

**整数オーバーフロー**:
```java
int n = 10;
int result = 1;
while (i <= n) {
    result = result * i;  // 大きな値でオーバーフロー
    i = i + 1;
}
```
→ 16-bit整数の範囲（-32768 ～ 32767）に注意

## さらに学ぶ

### 推奨リソース
- [QUICKSTART.md](../QUICKSTART.md) - クイックスタートガイド
- [README.md](../README.md) - 完全なドキュメント
- [tests/e2e/](../tests/e2e/) - テストケース

### 次のステップ
1. サンプルを改造してみる
2. 独自のアルゴリズムを実装
3. より複雑なプログラムに挑戦

## 貢献

新しいサンプルプログラムのアイデアがあれば、ぜひ追加してください！

---

Happy coding on DOS! 🎉