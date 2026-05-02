/*
 * Example 7: Greatest Common Divisor (GCD)
 * 
 * 最大公約数の計算（ユークリッドの互除法）。
 * GCD(a, b) = GCD(b, a mod b)
 */

class GCD {
    public static void main() {
        int a = 48;
        int b = 18;
        
        // ユークリッドの互除法
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        
        // a に最大公約数が格納される
        // GCD(48, 18) = 6
        
        return;
    }
}


