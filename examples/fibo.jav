/*
 * Example 5: Fibonacci Sequence
 * 
 * フィボナッチ数列の計算。
 * 反復的なアプローチで最初のn項を計算します。
 */

class Fibonacci {
    public static void main() {
        int n = 10;     // 計算する項数
        int a = 0;      // F(0)
        int b = 1;      // F(1)
        int i = 0;
        
        // 最初のn項を計算
        // i =    0, 1, 2, 3, 4,  5,  6,  7,  8,  9, 10
        // t =    1, 2, 3, 5, 8, 13, 21, 34, 55, 89
        // a = 0, 1, 1, 2, 3, 5,  8, 13, 21, 34, 55
        // b = 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89
        while (i < n) {
            int temp = a + b;
            a = b;
            b = temp;
            i = i + 1;
        }
        
        // 最終的にbにはF(n)が格納される
        // n=10の場合、b=89
        System.out.println( b );  // 89
        
        return;
    }
}


