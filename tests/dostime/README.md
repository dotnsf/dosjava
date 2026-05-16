# DOS Time API Tests

このディレクトリには、DOS Time API（dostime.c/dostime.h）の機能をテストするプログラムが含まれています。

## テストプログラム

### test_dostime.c (tdtime.exe)
DOS Time APIの包括的なテストプログラムです。

**テスト内容:**
1. **Test 1: dos_get_datetime()** - 現在の日時取得
   - DOS INT 21h経由で現在の日時を取得
   - 年、月、日、時、分、秒、1/100秒を表示
   - 値の妥当性を検証

2. **Test 2: dos_datetime_to_timestamp()** - 日時→タイムスタンプ変換
   - 既知の日時をUnixタイムスタンプに変換
   - テストケース:
     - 1970-01-01 00:00:00 → 0
     - 2000-01-01 00:00:00 → 946684800
     - 2024-01-01 12:00:00 → 1704110400

3. **Test 3: dos_timestamp_to_datetime()** - タイムスタンプ→日時変換
   - Unixタイムスタンプを日時に逆変換
   - Test 2と同じ値を使用して検証

4. **Test 4: Round-trip conversion** - 往復変換テスト
   - 現在時刻を取得
   - タイムスタンプに変換
   - 再度日時に変換
   - 元の値と一致することを確認（秒単位）

## ビルド方法

```batch
cd dosjava
wmake test_dostime
```

または

```batch
cd dosjava
.\build_all.bat
```

## 実行方法

### DOSBox-Xでの実行

1. DOSBox-Xを起動
2. dosjavaディレクトリにマウント:
   ```
   mount c c:\Users\dotns\src\dosjava
   c:
   cd build\bin
   ```
3. テストバッチファイルを実行:
   ```
   tdtime.bat
   ```

または直接実行:
```
tdtime.exe
```

## 期待される出力

```
========================================
DOS Time API Test Suite
========================================

Test 1: dos_get_datetime()
Current date/time: 2026-05-16 17:20:30.45
  Year: 2026
  Month: 5
  Day: 16
  Hour: 17
  Minute: 20
  Second: 30
  Hundredths: 45
Test 1: PASSED

Test 2: dos_datetime_to_timestamp()
  1970-01-01 00:00:00 -> 0 (expected 0)
  2000-01-01 00:00:00 -> 946684800 (expected 946684800)
  2024-01-01 12:00:00 -> 1704110400 (expected 1704110400)
Test 2: PASSED

Test 3: dos_timestamp_to_datetime()
  0 -> 1970-01-01 00:00:00
  946684800 -> 2000-01-01 00:00:00
  1704110400 -> 2024-01-01 12:00:00
Test 3: PASSED

Test 4: Round-trip conversion
  Original: 2026-05-16 17:20:30
  Timestamp: 1778877630
  Converted back: 2026-05-16 17:20:30
Test 4: PASSED

========================================
All tests passed!
========================================
```

## 注意事項

1. **DOS時刻の制約**
   - DOSの日付は1980年1月1日から2099年12月31日まで
   - 1980年以前の日付は正しく処理されない可能性があります

2. **タイムゾーン**
   - DOS時刻はローカル時刻
   - Unixタイムスタンプ変換時はUTCとして扱われます
   - タイムゾーンの考慮が必要な場合は注意してください

3. **精度**
   - DOS時刻の最小単位は1/100秒（10ミリ秒）
   - Unixタイムスタンプは秒単位

## トラブルシューティング

### テストが失敗する場合

1. **日時取得の失敗**
   - DOSBox-Xの時刻設定を確認
   - BIOSエミュレーションが正しく動作しているか確認

2. **タイムスタンプ変換の失敗**
   - 閏年計算のロジックを確認
   - 月の日数計算を確認

3. **往復変換の失敗**
   - 秒未満の精度は失われるため、秒単位で比較
   - 1/100秒の値は無視される

## 関連ファイル

- `src/runtime/dostime.h` - DOS Time APIヘッダー
- `src/runtime/dostime.c` - DOS Time API実装
- `tests/dostime/test_dostime.c` - テストプログラム
- `build/bin/tdtime.exe` - ビルド済み実行ファイル
- `build/bin/tdtime.bat` - テスト実行バッチファイル（8.3形式）