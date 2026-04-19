/*
 * Example 3: Conditional Statements
 * 
 * if文の使用例。
 * 条件に基づいて異なる処理を実行します。
 */

class Conditional {
    public static void main() {
        int x = 10;
        int y = 20;
        int result = 0;
        
        // 単純なif文
        if (x > 5) {
            result = 100;
        }
        
        // 複数の条件
        if (x < y) {
            result = result + 50;
        }
        
        // ネストしたif文
        if (x > 0) {
            if (y > 15) {
                result = result + 25;
            }
        }
        
        // 論理演算子
        if (x > 5 && y < 30) {
            result = result + 10;
        }
        
        return;
    }
}

// Made with Bob
