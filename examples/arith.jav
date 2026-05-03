/*
 * Example 2: Arithmetic Operations
 * 
 * 算術演算子の使用例。
 * 加算、減算、乗算、除算、剰余を実行します。
 */

class Arithmetic {
    public static void main() {
        int a = 10;
        int b = 20;
        
        // 加算
        int sum = a + b;        // 30
        System.out.println( sum );
        
        // 減算
        int diff = b - a;       // 10
        System.out.println( diff );
        
        // 乗算
        int product = a * b;    // 200
        System.out.println( product );
        
        // 除算
        int quotient = b / a;   // 2
        System.out.println( quotient );
        
        // 剰余
        int remainder = b % a;  // 0
        System.out.println( remainder );
        
        // 複雑な式
        int complex = (a + b) * 2 - a / 2;  // 55
        System.out.println( complex );
        
        return;
    }
}


