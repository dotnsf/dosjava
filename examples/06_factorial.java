/*
 * Example 6: Factorial
 * 
 * 階乗の計算。
 * n! = n × (n-1) × (n-2) × ... × 2 × 1
 */

class Factorial {
    public static void main() {
        int n = 5;          // 5の階乗を計算
        int result = 1;
        int i = 1;
        
        // 1からnまでの積を計算
        while (i <= n) {
            result = result * i;
            i = i + 1;
        }
        
        // result = 5! = 120
        
        return;
    }
}

// Made with Bob
