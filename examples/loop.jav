/*
 * Example 4: While Loop
 * 
 * whileループの使用例。
 * 繰り返し処理を実行します。
 */

class Loop {
    public static void main() {
        int i = 0;
        int sum = 0;
        
        // 0から9までの合計を計算
        while (i < 10) {
            sum = sum + i;
            i = i + 1;
        }
        // sum = 45
        
        // ネストしたループ
        int x = 0;
        while (x < 3) {
            int y = 0;
            while (y < 3) {
                int product = x * y;
                y = y + 1;
            }
            x = x + 1;
        }
        
        return;
    }
}


